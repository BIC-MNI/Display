#include  <def_display.h>

public  Status   input_landmark_file(
    Volume         volume,
    char           filename[],
    model_struct   *model,
    Colour         marker_colour,
    VIO_Real           default_size,
    Marker_types   default_type )
{
    Status                  status;
    object_struct           *object;
    marker_struct           marker;
    FILE                    *file;

    status = open_file_with_default_suffix( filename, "lmk", READ_FILE,
                                            ASCII_FORMAT, &file );

    if( status == VIO_OK )
    {
        while( io_tag_point( file, READ_FILE, volume, default_size,
                             &marker ) == VIO_OK )
        {
            marker.colour = marker_colour;
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
    Volume          volume,
    VIO_Real            default_size,
    marker_struct   *marker )
{
    Status   status;
    String   line;
    Point    position;
    int      sizes[N_DIMENSIONS];
    int      len, offset;
    VIO_Real     x, y, z;
    VIO_Real     x_w, y_w, z_w;

    status = VIO_OK;

    if( io_direction == WRITE_FILE )
    {
        if( volume == (Volume) NULL )
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

            get_volume_sizes( volume, sizes );

            convert_voxel_to_talairach( x, y, z, sizes[VIO_X], sizes[VIO_Y], sizes[VIO_Z],
                                        &x, &y, &z );

            fill_Point( position, x, y, z );
        }
    }

    if( status == VIO_OK )
        status = io_point( file, io_direction, ASCII_FORMAT, &position );

    if( io_direction == READ_FILE )
    {
        if( volume == (Volume) NULL )
        {
            marker->position = position;
        }
        else
        {
            get_volume_sizes( volume, sizes );

            convert_talairach_to_voxel( Point_x(position),
                                        Point_y(position),
                                        Point_z(position),
                                        sizes[VIO_X], sizes[VIO_Y], sizes[VIO_Z],
                                        &x, &y, &z );

            convert_voxel_to_world( volume, x, y, z, &x_w, &y_w, &z_w );
            fill_Point( marker->position, x_w, y_w, z_w );
        }
    }

#define USE_X_POSITION_FOR_WEIGHT
#ifdef  USE_X_POSITION_FOR_WEIGHT
    if( status == VIO_OK )
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
    if( status == VIO_OK )
        status = io_real( file, io_direction, ASCII_FORMAT, &marker->size );
#endif

    if( status == VIO_OK )
        status = io_int( file, io_direction, ASCII_FORMAT,
                         &marker->structure_id );

    if( status == VIO_OK )
        status = io_int( file, io_direction, ASCII_FORMAT,
                         &marker->patient_id );

    if( io_direction == WRITE_FILE )
    {
        if( status == VIO_OK && strlen(marker->label) > 0 )
            status = io_quoted_string( file, io_direction, ASCII_FORMAT,
                                       marker->label, MAX_STRING_LENGTH );
    }
    else
    {
        if( status == VIO_OK )
            status = input_line( file, line, MAX_STRING_LENGTH );

        if( status == VIO_OK )
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

    if( status == VIO_OK )
        status = io_newline( file, io_direction, ASCII_FORMAT );

    return( status );
}
