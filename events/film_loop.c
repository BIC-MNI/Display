/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

static    DEF_EVENT_FUNCTION( check_updated );

static  VIO_Status  create_film_loop_header(
    VIO_STR   base_filename,
    int      window_width,
    int      window_height,
    int      n_steps );
static  VIO_STR  create_frame_filename(
    VIO_STR  base_filename,
    int     step );
static  VIO_Status  save_image_to_file(
    display_struct    *display );
static  void  display_next_frame(
    display_struct   *display );

  VIO_Status  start_film_loop(
    display_struct   *display,
    VIO_STR           base_filename,
    int              axis_index,
    int              n_steps )
{
    VIO_Status    status;
    VIO_Real      angle;
    int       x_size, y_size;

    add_action_table_function( &display->action_table, NO_EVENT,
                               check_updated );

    angle = 2.0 * PI / (VIO_Real) n_steps;

    make_rotation_transform( angle, axis_index,
                             &display->three_d.film_loop.transform );

    display->three_d.film_loop.n_steps = n_steps;
    display->three_d.film_loop.current_step = 1;

    G_get_window_size( display->window, &x_size, &y_size );

    if( (x_size & 1) == 1 ) --x_size;
    if( (y_size & 1) == 1 ) --y_size;

    display->three_d.film_loop.x_size = x_size;
    display->three_d.film_loop.y_size = y_size;
    display->three_d.film_loop.base_filename = create_string( base_filename );

    status = create_film_loop_header( base_filename, x_size, y_size, n_steps );

    return( status );
}

static  void  end_film_loop(
    display_struct   *display )
{
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  check_updated );

    delete_string( display->three_d.film_loop.base_filename );

    print( "Done film loop.\n" );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( check_updated )
{
    VIO_Status    status;

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

static  VIO_Status  create_film_loop_header(
    VIO_STR   base_filename,
    int      window_width,
    int      window_height,
    int      n_steps )
{
    VIO_Status  status;
    int     i;
    FILE    *file;
    VIO_STR  header_name;
    VIO_STR  frame_filename;
    VIO_STR  no_dirs;

    header_name = concat_strings( base_filename, ".flm" );

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
        frame_filename = create_frame_filename( base_filename, i+1 );

        no_dirs = remove_directories_from_filename( frame_filename );

        if( status == OK )
            status = output_string( file, no_dirs );

        if( status == OK )
            status = io_newline( file, WRITE_FILE, ASCII_FORMAT );

        delete_string( frame_filename );
        delete_string( no_dirs );
    }

    if( status == OK )
        status = close_file( file );

    delete_string( header_name );

    return( status );
}

static  VIO_STR  create_frame_filename(
    VIO_STR   base_filename,
    int      step )
{
    char     buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];

    (void) sprintf( buffer, "%s_%d.rgb", base_filename, step );

    return( create_string( buffer ) );
}

static  VIO_Status  save_image_to_file(
    display_struct    *display )
{
    VIO_Status         status;
    VIO_STR         frame_filename;

    frame_filename = create_frame_filename(
                           display->three_d.film_loop.base_filename,
                           display->three_d.film_loop.current_step );

    status = save_window_to_file( display, frame_filename, 0, -1, 0, -1 );

    delete_string( frame_filename );

    return( status );
}

static  void  display_next_frame(
    display_struct   *display )
{
    apply_transform_in_view_space( display,
                                   &display->three_d.film_loop.transform );
}
