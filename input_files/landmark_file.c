#include  <def_mni.h>

public  Status  io_tag_point(
    FILE            *file,
    IO_types        io_direction,
    volume_struct   *volume,
    Real            default_size,
    marker_struct   *marker );

public  Status   input_landmark_file(
    volume_struct  *volume,
    char           filename[],
    model_struct   *model,
    Colour         *marker_colour,
    Real           default_size,
    Marker_types   default_type )
{
    Status                  status;
    object_struct           *object;
    marker_struct           marker;
    FILE                    *file;

    status = open_file_with_default_suffix( filename, "lmk", READ_FILE,
                                            ASCII_FORMAT, &file );

    if( status == OK )
    {
        while( io_tag_point( file, READ_FILE, volume, default_size,
                             &marker ) == OK )
        {
            marker.colour = *marker_colour;
            marker.type = default_type;

            object = create_object( MARKER );
            *(get_marker_ptr(object)) = marker;

            add_object_to_model( model, object );
        }

        status = close_file( file );
    }

    return( status );
}

public  Status  io_tag_point(
    FILE            *file,
    IO_types        io_direction,
    volume_struct   *volume,
    Real            default_size,
    marker_struct   *marker )
{
    Status   status;
    String   line;
    Point    position;
    int      nx, ny, nz;
    int      len, offset;
    Real     x, y, z;
    Real     x_w, y_w, z_w;

    status = OK;

    if( io_direction == WRITE_FILE )
    {
        if( volume == (volume_struct *) 0 )
        {
            position = marker->position;
        }
        else
        {
            convert_world_to_voxel( volume,
                                    Point_x(marker->position),
                                    Point_y(marker->position),
                                    Point_z(marker->position),
                                    &x, &y, &z );

            get_volume_size( volume, &nx, &ny, &nz );

            convert_voxel_to_talairach( x, y, z, nx, ny, nz, &x, &y, &z );

            fill_Point( position, x, y, z );
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
                                        nx, ny, nz, &x, &y, &z );

            convert_voxel_to_world( volume, x, y, z, &x_w, &y_w, &z_w );
            fill_Point( marker->position, x_w, y_w, z_w );
        }
    }

#define USE_X_POSITION_FOR_WEIGHT
#ifdef  USE_X_POSITION_FOR_WEIGHT
    if( status == OK )
    {
        if( io_direction == WRITE_FILE )
            status = io_float( file, io_direction, ASCII_FORMAT,
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
