
#include  <def_graphics.h>
#include  <def_stdio.h>
#include  <def_string.h>
#include  <def_globals.h>

public  Status  start_film_loop( graphics, base_filename, axis_index, n_steps )
    graphics_struct  *graphics;
    char             base_filename[];
    int              axis_index;
    int              n_steps;
{
    DECL_EVENT_FUNCTION( check_updated );
    void                 add_action_table_function();
    Real                 angle;
    int                  x_size, y_size;
    void                 make_rotation_transform();
    Status               status;
    Status               create_film_loop_header();

    add_action_table_function( &graphics->action_table, NO_EVENT,
                               check_updated );

    angle = 2.0 * PI / n_steps;

    make_rotation_transform( angle, axis_index,
                             &graphics->three_d.film_loop.transform );

    graphics->three_d.film_loop.n_steps = n_steps;
    graphics->three_d.film_loop.current_step = 1;
    x_size = graphics->window.x_size;
    y_size = graphics->window.y_size;

    if( x_size & 1 == 1 ) --x_size;
    if( y_size & 1 == 1 ) --y_size;

    graphics->three_d.film_loop.x_size = x_size;
    graphics->three_d.film_loop.y_size = y_size;
    (void) strcpy( graphics->three_d.film_loop.base_filename, base_filename );

    CALLOC1( status, graphics->three_d.film_loop.image_storage,
             x_size * y_size, Pixel_colour );

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

    remove_action_table_function( &graphics->action_table, NO_EVENT );

    FREE1( status, graphics->three_d.film_loop.image_storage );

    PRINT( "Done film loop.\n" );

    return( status );
}

private  DEF_EVENT_FUNCTION( check_updated )
    /* ARGSUSED */
{
    Status    status;
    Boolean   window_is_up_to_date();
    Status    end_film_loop();
    Status    save_image_to_file();
    void      display_next_frame();
    void      update_view();

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

                graphics->update_required = TRUE;
            }
            else
            {
                status = end_film_loop( graphics );
            }
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
    Status  open_output_file();
    Status  io_int();
    Status  output_string();
    Status  io_newline();
    Status  close_file();
    String  header_name;
    String  frame_filename;
    String  no_dirs;
    void    strip_off_directories();
    void    create_frame_filename();

    (void) strcpy( header_name, base_filename );   
    (void) strcat( header_name, ".flm" );   

    status = open_output_file( header_name, &file );

    if( status == OK )
    {
        status = io_int( file, OUTPUTTING, ASCII_FORMAT, &window_width );
    }

    if( status == OK )
    {
        status = io_int( file, OUTPUTTING, ASCII_FORMAT, &window_height );
    }

    if( status == OK )
    {
        status = io_newline( file, OUTPUTTING, ASCII_FORMAT );
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
            status = io_newline( file, OUTPUTTING, ASCII_FORMAT );
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
    Status  status;
    Status  open_output_file();
    Status  io_binary_data();
    Status  close_file();
    void    G_read_pixels();
    FILE    *file;
    String  frame_filename;
    void    create_frame_filename();

    G_read_pixels( &graphics->window,
                   0, graphics->three_d.film_loop.x_size-1,
                   0, graphics->three_d.film_loop.y_size-1,
                   graphics->three_d.film_loop.image_storage );

    create_frame_filename( graphics->three_d.film_loop.base_filename,
                           graphics->three_d.film_loop.current_step,
                           frame_filename );

    status = open_output_file( frame_filename, &file );

    if( status == OK )
    {
        status = io_binary_data( file, OUTPUTTING,
                         (char *) graphics->three_d.film_loop.image_storage,
                         sizeof( graphics->three_d.film_loop.image_storage[0] ),
                         graphics->three_d.film_loop.x_size *
                         graphics->three_d.film_loop.y_size );
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
