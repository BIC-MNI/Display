#include  <def_files.h>
#include  <def_string.h>
#include  <def_objects.h>

public  Status   input_landmark_file( volume,
                                      filename, n_objects, object_list,
                                      marker_colour, default_size )
    volume_struct  *volume;
    char           filename[];
    int            *n_objects;
    object_struct  ***object_list;
    Colour         *marker_colour;
    Real           default_size;
{
    Status                  status;
    Status                  add_object_to_list();
    object_struct           *object;
    Status                  create_object();
    Status                  io_tag_point();
    marker_struct           marker;
    FILE                    *file;

    *n_objects = 0;

    status = open_file_with_default_suffix( filename, "lmk", READ_FILE,
                                            ASCII_FORMAT, &file );

    if( status == OK )
    {
        while( io_tag_point( file, READ_FILE, volume, default_size,
                             &marker ) == OK )
        {
            status = create_object( &object, MARKER );

            if( status == OK )
            {
                marker.colour = *marker_colour;
                *(object->ptr.marker) = marker;

                status = add_object_to_list( n_objects, object_list, object );
            }
        }
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}

public  Status  io_tag_point( file, io_direction, volume, default_size, marker )
    FILE            *file;
    IO_types        io_direction;
    volume_struct   *volume;
    Real            default_size;
    marker_struct   *marker;
{
    Status   status;
    Status   io_point();
    Status   io_newline();
    Status   io_int();
    Status   io_real();
    Status   io_quoted_string();
    String   line;
    void     strip_blanks();
    Point    position;
    void     convert_point_to_voxel();
    void     convert_voxel_to_point();
    void     convert_voxel_to_talairach();
    void     convert_talairach_to_voxel();
    void     get_volume_size();
    int      nx, ny, nz;
    int      len, offset;

    status = OK;

    if( io_direction == WRITE_FILE )
    {
        if( volume == (volume_struct *) 0 )
        {
            position = marker->position;
        }
        else
        {
            convert_point_to_voxel( volume,
                                    Point_x(marker->position),
                                    Point_y(marker->position),
                                    Point_z(marker->position),
                                    &Point_x(position),
                                    &Point_y(position),
                                    &Point_z(position) );

            get_volume_size( volume, &nx, &ny, &nz );

            convert_voxel_to_talairach( Point_x(position),
                                        Point_y(position),
                                        Point_z(position),
                                        nx, ny, nz,
                                        &Point_x(position),
                                        &Point_y(position),
                                        &Point_z(position) );
        }
    }

    if( status == OK )
        status = io_point( file, io_direction, ASCII_FORMAT, &position );

    if( io_direction == READ_FILE )
    {
        if( volume == (volume_struct *) 0 )
        {
            marker->position = position;
        }
        else
        {
            get_volume_size( volume, &nx, &ny, &nz );

            convert_talairach_to_voxel( Point_x(position),
                                        Point_y(position),
                                        Point_z(position),
                                        nx, ny, nz,
                                        &Point_x(position),
                                        &Point_y(position),
                                        &Point_z(position) );

            convert_voxel_to_point( volume,
                                    Point_x(position),
                                    Point_y(position),
                                    Point_z(position),
                                    &marker->position );
        }
    }

#define USE_X_POSITION_FOR_WEIGHT
#ifdef  USE_X_POSITION_FOR_WEIGHT
    if( status == OK )
    {
        if( io_direction == WRITE_FILE )
            status = io_real( file, io_direction, ASCII_FORMAT,
                              &Point_x(position));
        else
        {
            status = io_real( file, io_direction, ASCII_FORMAT, &marker->size );
            marker->size = default_size;
        }
    }
#else
    if( status == OK )
        status = io_real( file, io_direction, ASCII_FORMAT, &marker->size );
#endif

    if( status == OK )
        status = io_int( file, io_direction, ASCII_FORMAT,
                         &marker->structure_id );

    if( status == OK )
        status = io_int( file, io_direction, ASCII_FORMAT,
                         &marker->patient_id );

    if( io_direction == WRITE_FILE )
    {
        if( status == OK && strlen(marker->label) > 0 )
            status = io_quoted_string( file, io_direction, ASCII_FORMAT,
                                       marker->label, MAX_STRING_LENGTH );
    }
    else
    {
        if( status == OK )
            status = input_line( file, line, MAX_STRING_LENGTH );

        if( status == OK )
        {
            strip_blanks( line, line );

            if( line[0] == '"' )
                offset = 1;
            else
                offset = 0;

            (void) strcpy( marker->label, &line[offset] );

            len = strlen( marker->label );

            if( len > 0 && marker->label[len-1] == '"' )
                 marker->label[len-1] = (char) 0;
        }
    }

    if( status == OK )
        status = io_newline( file, io_direction, ASCII_FORMAT );

    return( status );
}
