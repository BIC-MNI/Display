
#include  <def_display.h>

private    DEF_EVENT_FUNCTION( check_updated );

private  Status  create_film_loop_header(
    char   base_filename[],
    int    window_width,
    int    window_height,
    int    n_steps );
private  void  create_frame_filename(
    char   base_filename[],
    int    step,
    char   frame_filename[] );
private  Status  save_image_to_file(
    display_struct    *display );
private  void  display_next_frame(
    display_struct   *display );
private  void  get_pixel_bounds(
    int           x_size,
    int           y_size,
    Colour        *pixels,
    int           *x_min,
    int           *x_max,
    int           *y_min,
    int           *y_max );
private  Status  output_frame(
    FILE           *file,
    Colour         pixels[],
    int            x_size,
    int            x_min,
    int            x_max,
    int            y_min,
    int            y_max );

public  Status  start_film_loop(
    display_struct   *display,
    char             base_filename[],
    int              axis_index,
    int              n_steps )
{
    Status    status;
    Real      angle;
    int       x_size, y_size;

    add_action_table_function( &display->action_table, NO_EVENT,
                               check_updated );

    angle = 2.0 * PI / n_steps;

    make_rotation_transform( angle, axis_index,
                             &display->three_d.film_loop.transform );

    display->three_d.film_loop.n_steps = n_steps;
    display->three_d.film_loop.current_step = 1;

    G_get_window_size( display->window, &x_size, &y_size );

    if( (x_size & 1) == 1 ) --x_size;
    if( (y_size & 1) == 1 ) --y_size;

    display->three_d.film_loop.x_size = x_size;
    display->three_d.film_loop.y_size = y_size;
    (void) strcpy( display->three_d.film_loop.base_filename, base_filename );

    ALLOC( display->three_d.film_loop.image_storage, x_size * y_size );

    status = create_film_loop_header( base_filename, x_size, y_size, n_steps );

    return( status );
}

private  void  end_film_loop(
    display_struct   *display )
{
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  check_updated );

    FREE( display->three_d.film_loop.image_storage );

    print( "Done film loop.\n" );
}

private  DEF_EVENT_FUNCTION( check_updated )
    /* ARGSUSED */
{
    Status    status;

    status = OK;

    if( window_is_up_to_date( display ) )
    {
        print( "Frame %d/%d\n", display->three_d.film_loop.current_step,
               display->three_d.film_loop.n_steps );

        status = save_image_to_file( display );

        if( status == OK )
        {
            ++display->three_d.film_loop.current_step;

            if( display->three_d.film_loop.current_step <= 
                display->three_d.film_loop.n_steps )
            {
                display_next_frame( display );

                update_view( display );

                set_update_required( display, NORMAL_PLANES );
            }
            else
                end_film_loop( display );
        }
        else
            end_film_loop( display );
    }

    return( OK );
}

private  Status  create_film_loop_header(
    char   base_filename[],
    int    window_width,
    int    window_height,
    int    n_steps )
{
    Status  status;
    int     i;
    FILE    *file;
    String  header_name;
    String  frame_filename;
    String  no_dirs;

    (void) strcpy( header_name, base_filename );   
    (void) strcat( header_name, ".flm" );   

    status = open_file( header_name, WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
        status = io_int( file, WRITE_FILE, ASCII_FORMAT, &window_width );

    if( status == OK )
        status = io_int( file, WRITE_FILE, ASCII_FORMAT, &window_height );

    if( status == OK )
        status = io_colour( file, WRITE_FILE, ASCII_FORMAT,
                            &Initial_background_colour );

    if( status == OK )
        status = io_newline( file, WRITE_FILE, ASCII_FORMAT );

    for_less( i, 0, n_steps )
    {
        create_frame_filename( base_filename, i+1, frame_filename );

        strip_off_directories( frame_filename, no_dirs );

        if( status == OK )
            status = output_string( file, no_dirs );

        if( status == OK )
            status = io_newline( file, WRITE_FILE, ASCII_FORMAT );
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}

private  void  create_frame_filename(
    char   base_filename[],
    int    step,
    char   frame_filename[] )
{
    (void) sprintf( frame_filename, "%s_%d.img", base_filename, step );
}

private  Status  save_image_to_file(
    display_struct    *display )
{
    Status         status;
    FILE           *file;
    String         frame_filename;
    int            x_min, x_max, y_min, y_max;

    G_read_pixels( display->window,
                   0, display->three_d.film_loop.x_size-1,
                   0, display->three_d.film_loop.y_size-1,
                   display->three_d.film_loop.image_storage );

    get_pixel_bounds( display->three_d.film_loop.x_size,
                      display->three_d.film_loop.y_size,
                      display->three_d.film_loop.image_storage,
                      &x_min, &x_max, &y_min, &y_max );

    if( ((x_max - x_min) & 1) == 0 )
    {
        if( x_min > 0 )
            --x_min;
        else
            ++x_max;
    }

    if( ((y_max - y_min) & 1) == 0 )
    {
        if( y_min > 0 )
            --y_min;
        else
            ++y_max;
    }

    create_frame_filename( display->three_d.film_loop.base_filename,
                           display->three_d.film_loop.current_step,
                           frame_filename );

    status = open_file_with_default_suffix( frame_filename, "frm",
                                            WRITE_FILE, BINARY_FORMAT, &file );

    if( status == OK )
    {
        status = output_frame( file,
                               display->three_d.film_loop.image_storage,
                               display->three_d.film_loop.x_size,
                               x_min, x_max, y_min, y_max );
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}

private  void  display_next_frame(
    display_struct   *display )
{
    apply_transform_in_view_space( display,
                                   &display->three_d.film_loop.transform );
}

private  void  get_pixel_bounds(
    int           x_size,
    int           y_size,
    Colour        *pixels,
    int           *x_min,
    int           *x_max,
    int           *y_min,
    int           *y_max )
{
    int            x, y;
    Boolean        found_a_pixel;
    Colour         background;

    background = pixels[IJ(0,0,x_size)];

    *x_min = -1;
    found_a_pixel = FALSE;

    do
    {
        ++(*x_min);

        for_less( y, 0, y_size )
        {
            if( pixels[IJ(*x_min,y,x_size)] != background )
            {
                found_a_pixel = TRUE;
                break;
            }
        }

    } while( !found_a_pixel );

    *x_max = x_size;
    found_a_pixel = FALSE;

    do
    {
        --(*x_min);

        for_less( y, 0, y_size )
        {
            if( pixels[IJ(*x_max,y,x_size)] != background )
            {
                found_a_pixel = TRUE;
                break;
            }
        }

    } while( !found_a_pixel );

    *y_min = -1;
    found_a_pixel = FALSE;

    do
    {
        ++(*y_min);

        for_less( x, 0, x_size )
        {
            if( pixels[IJ(x,*y_min,x_size)] != background )
            {
                found_a_pixel = TRUE;
                break;
            }
        }

    } while( !found_a_pixel );

    *y_max = y_size;
    found_a_pixel = FALSE;

    do
    {
        --(*y_max);

        for_less( x, 0, x_size )
        {
            if( pixels[IJ(x,*y_max,x_size)] != background )
            {
                found_a_pixel = TRUE;
                break;
            }
        }

    } while( !found_a_pixel );
}

private  Status  output_frame(
    FILE           *file,
    Colour         pixels[],
    int            x_size,
    int            x_min,
    int            x_max,
    int            y_min,
    int            y_max )
{
    Status        status;
    int           *start, *end;
    int           y;
    int           n_pixels;
    Colour        background;

    background = pixels[IJ(0,0,x_size)];

    status = io_int( file, WRITE_FILE, BINARY_FORMAT, &x_min );

    if( status == OK )
        status = io_int( file, WRITE_FILE, BINARY_FORMAT, &x_max );

    if( status == OK )
        status = io_int( file, WRITE_FILE, BINARY_FORMAT, &y_min );

    if( status == OK )
        status = io_int( file, WRITE_FILE, BINARY_FORMAT, &y_max );

    if( status == OK )
    {
        ALLOC( start, y_max - y_min + 1 );
        ALLOC( end, y_max - y_min + 1 );
    }

    for_inclusive( y, y_min, y_max )
    {
        start[y-y_min] = x_min;
        while( start[y-y_min] < x_max &&
               pixels[IJ(start[y-y_min],y,x_size)] == background )
        {
            ++start[y-y_min];
        }

        end[y-y_min] = x_max;
        while( end[y-y_min] > x_min &&
               pixels[IJ(end[y-y_min],y,x_size)] == background )
        {
            --end[y-y_min];
        }
    }

    for_inclusive( y, y_min, y_max )
    {
        if( status == OK )
            status = io_int( file, WRITE_FILE, BINARY_FORMAT, &start[y-y_min] );

        n_pixels = end[y-y_min] - start[y-y_min] + 1;

        if( n_pixels < 0 )
            n_pixels = 0;

        if( status == OK )
            status = io_int( file, WRITE_FILE, BINARY_FORMAT, &n_pixels );
    }

    for_inclusive( y, y_min, y_max )
    {
        n_pixels = end[y-y_min] - start[y-y_min] + 1;

        if( status == OK && n_pixels > 0 )
        {
            status = io_binary_data( file, WRITE_FILE,
                        (void *) &pixels[IJ(start[y-y_min],y,x_size)],
                        sizeof( pixels[0] ), n_pixels );
        }
    }

    return( status );
}
