
#include  <def_graphics.h>
#include  <def_files.h>
#include  <def_string.h>
#include  <def_globals.h>

static    DECL_EVENT_FUNCTION( check_updated );
static    Status               create_film_loop_header();
static    Status               save_image_to_file();
static    void                 display_next_frame();
static    void                 create_frame_filename();
static    Status               output_frame();
static    void                 get_pixel_bounds();

public  Status  start_film_loop( graphics, base_filename, axis_index, n_steps )
    graphics_struct  *graphics;
    char             base_filename[];
    int              axis_index;
    int              n_steps;
{
    void                 add_action_table_function();
    Real                 angle;
    int                  x_size, y_size;
    void                 make_rotation_transform();
    Status               status;

    add_action_table_function( &graphics->action_table, NO_EVENT,
                               check_updated );

    angle = 2.0 * PI / n_steps;

    make_rotation_transform( angle, axis_index,
                             &graphics->three_d.film_loop.transform );

    graphics->three_d.film_loop.n_steps = n_steps;
    graphics->three_d.film_loop.current_step = 1;
    x_size = graphics->window.x_size;
    y_size = graphics->window.y_size;

    if( (x_size & 1) == 1 ) --x_size;
    if( (y_size & 1) == 1 ) --y_size;

    graphics->three_d.film_loop.x_size = x_size;
    graphics->three_d.film_loop.y_size = y_size;
    (void) strcpy( graphics->three_d.film_loop.base_filename, base_filename );

    ALLOC( status, graphics->three_d.film_loop.image_storage, x_size * y_size );

    if( status == OK )
    {
        status = create_film_loop_header( base_filename,
                                          x_size, y_size, n_steps );
    }

    return( status );
}

private  Status  end_film_loop( graphics )
    graphics_struct  *graphics;
{
    Status   status;
    void     remove_action_table_function();

    remove_action_table_function( &graphics->action_table, NO_EVENT,
                                  check_updated );

    FREE( status, graphics->three_d.film_loop.image_storage );

    PRINT( "Done film loop.\n" );

    return( status );
}

private  DEF_EVENT_FUNCTION( check_updated )
    /* ARGSUSED */
{
    Status    status;
    Boolean   window_is_up_to_date();
    Status    end_film_loop();
    void      update_view();
    void      set_update_required();

    status = OK;

    if( window_is_up_to_date( graphics ) )
    {
        PRINT( "Frame %d/%d\n", graphics->three_d.film_loop.current_step,
               graphics->three_d.film_loop.n_steps );

        status = save_image_to_file( graphics );

        if( status == OK )
        {
            ++graphics->three_d.film_loop.current_step;

            if( graphics->three_d.film_loop.current_step <= 
                graphics->three_d.film_loop.n_steps )
            {
                display_next_frame( graphics );

                update_view( graphics );

                set_update_required( graphics, NORMAL_PLANES );
            }
            else
            {
                status = end_film_loop( graphics );
            }
        }
        else
        {
            status = end_film_loop( graphics );
        }
    }

    return( OK );
}

private  Status  create_film_loop_header( base_filename, window_width,
                                          window_height, n_steps )
    char   base_filename[];
    int    window_width;
    int    window_height;
    int    n_steps;
{
    Status  status;
    int     i;
    FILE    *file;
    Status  io_colour();
    String  header_name;
    String  frame_filename;
    String  no_dirs;
    void    strip_off_directories();

    (void) strcpy( header_name, base_filename );   
    (void) strcat( header_name, ".flm" );   

    status = open_file( header_name, WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        status = io_int( file, WRITE_FILE, ASCII_FORMAT, &window_width );
    }

    if( status == OK )
    {
        status = io_int( file, WRITE_FILE, ASCII_FORMAT, &window_height );
    }

    if( status == OK )
    {
        status = io_colour( file, WRITE_FILE, ASCII_FORMAT,
                            &Initial_background_colour );
    }

    if( status == OK )
    {
        status = io_newline( file, WRITE_FILE, ASCII_FORMAT );
    }

    for_less( i, 0, n_steps )
    {
        create_frame_filename( base_filename, i+1, frame_filename );

        strip_off_directories( frame_filename, no_dirs );

        if( status == OK )
        {
            status = output_string( file, no_dirs );
        }

        if( status == OK )
        {
            status = io_newline( file, WRITE_FILE, ASCII_FORMAT );
        }
    }

    if( status == OK )
    {
        status = close_file( file );
    }

    return( status );
}

private  void  create_frame_filename( base_filename, step, frame_filename )
    char   base_filename[];
    int    step;
    char   frame_filename[];
{
    (void) sprintf( frame_filename, "%s_%d.img", base_filename, step );
}

private  Status  save_image_to_file( graphics )
    graphics_struct   *graphics;
{
    Status         status;
    void           G_read_pixels();
    FILE           *file;
    String         frame_filename;
    int            x_min, x_max, y_min, y_max;

    G_read_pixels( &graphics->window,
                   0, graphics->three_d.film_loop.x_size-1,
                   0, graphics->three_d.film_loop.y_size-1,
                   graphics->three_d.film_loop.image_storage );

    get_pixel_bounds( graphics->three_d.film_loop.x_size,
                      graphics->three_d.film_loop.y_size,
                      graphics->three_d.film_loop.image_storage,
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

    create_frame_filename( graphics->three_d.film_loop.base_filename,
                           graphics->three_d.film_loop.current_step,
                           frame_filename );

    status = open_file_with_default_suffix( frame_filename, "frm",
                                            WRITE_FILE, BINARY_FORMAT, &file );

    if( status == OK )
    {
        status = output_frame( file,
                               graphics->three_d.film_loop.image_storage,
                               graphics->three_d.film_loop.x_size,
                               x_min, x_max, y_min, y_max );
    }

    if( status == OK )
    {
        status = close_file( file );
    }

    return( status );
}

private  void  display_next_frame( graphics )
    graphics_struct  *graphics;
{
    void   apply_transform_in_view_space();

    apply_transform_in_view_space( graphics,
                                   &graphics->three_d.film_loop.transform );
}

private  void  get_pixel_bounds( x_size, y_size, pixels,
                                 x_min, x_max, y_min, y_max )
    int           x_size, y_size;
    Pixel_colour  *pixels;
    int           *x_min;
    int           *x_max;
    int           *y_min;
    int           *y_max;
{
    int            x, y;
    Boolean        found_a_pixel;
    Pixel_colour   background;

    background = ACCESS_PIXEL( pixels, 0, 0, x_size );

    *x_min = -1;
    found_a_pixel = FALSE;

    do
    {
        ++(*x_min);

        for_less( y, 0, y_size )
        {
            if( ACCESS_PIXEL( pixels, *x_min, y, x_size ) != background )
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
            if( ACCESS_PIXEL( pixels, *x_max, y, x_size ) != background )
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
            if( ACCESS_PIXEL( pixels, x, *y_min, x_size ) != background )
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
            if( ACCESS_PIXEL( pixels, x, *y_max, x_size ) != background )
            {
                found_a_pixel = TRUE;
                break;
            }
        }

    } while( !found_a_pixel );
}

private  Status  output_frame( file, pixels, x_size, x_min, x_max, y_min,
                               y_max )
    FILE           *file;
    Pixel_colour   pixels[];
    int            x_size;
    int            x_min;
    int            x_max;
    int            y_min;
    int            y_max;
{
    Status        status;
    Status        io_int();
    Status        io_binary_data();
    int           *start, *end;
    int           y;
    int           n_pixels;
    Pixel_colour  background;

    background = ACCESS_PIXEL( pixels, 0, 0, x_size );

    status = io_int( file, WRITE_FILE, BINARY_FORMAT, &x_min );

    if( status == OK )
        status = io_int( file, WRITE_FILE, BINARY_FORMAT, &x_max );

    if( status == OK )
        status = io_int( file, WRITE_FILE, BINARY_FORMAT, &y_min );

    if( status == OK )
        status = io_int( file, WRITE_FILE, BINARY_FORMAT, &y_max );

    if( status == OK )
        ALLOC( status, start, y_max - y_min + 1 );

    if( status == OK )
        ALLOC( status, end, y_max - y_min + 1 );

    for_inclusive( y, y_min, y_max )
    {
        start[y-y_min] = x_min;
        while( start[y-y_min] < x_max &&
               ACCESS_PIXEL( pixels, start[y-y_min], y, x_size ) == background )
        {
            ++start[y-y_min];
        }

        end[y-y_min] = x_max;
        while( end[y-y_min] > x_min &&
               ACCESS_PIXEL( pixels, end[y-y_min], y, x_size ) == background )
        {
            --end[y-y_min];
        }
    }

    for_inclusive( y, y_min, y_max )
    {
        if( status == OK )
        {
            status = io_int( file, WRITE_FILE, BINARY_FORMAT, &start[y-y_min] );
        }

        n_pixels = end[y-y_min] - start[y-y_min] + 1;

        if( n_pixels < 0 )
        {
            n_pixels = 0;
        }

        if( status == OK )
        {
            status = io_int( file, WRITE_FILE, BINARY_FORMAT, &n_pixels );
        }
    }

    for_inclusive( y, y_min, y_max )
    {
        n_pixels = end[y-y_min] - start[y-y_min] + 1;

        if( status == OK && n_pixels > 0 )
        {
            status = io_binary_data( file, WRITE_FILE,
                        (char *) &ACCESS_PIXEL(pixels,start[y-y_min],y,x_size),
                        sizeof( pixels[0] ), n_pixels );
        }
    }

    return( status );
}
