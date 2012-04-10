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

private  Status   process_no_events_for_three_d_windows( void );
private  void     update_all_three_d_windows( void );
private  Status   process_no_events_for_slice_windows( void );
private  void     update_all_slice_windows( void );
private  Status   perform_action(
    display_struct   *display,
    Event_types      event_type,
    int              key_pressed );
private  void  update_this_type_of_windows(
    window_types   window_type );

private  void  quit_program( void )
{
    print( "Quitting Display\n" );

    exit( 0 );
}

private  void  handle_event(
    Event_types   event,
    Gwindow       window,
    int           key_pressed )
{
    Status          status;
    display_struct  *display;

    display = lookup_window( window );

    if( display == NULL )
        return;

    status = perform_action( display, event, key_pressed );

    if( status == QUIT )
        quit_program();
}

private  void  update_callback(
    Gwindow   window,
    void      *data )
{
    handle_event( WINDOW_REDRAW_EVENT, window, 0 );
}

private  void  resize_callback(
    Gwindow   window,
    int       x,
    int       y,
    int       x_size,
    int       y_size,
    void      *data )
{
    handle_event( WINDOW_RESIZE_EVENT, window, 0 );
}

private  void  key_down_callback(
    Gwindow   window,
    int       key,
    void      *data )
{
    handle_event( KEY_DOWN_EVENT, window, key );
}

private  void  mouse_movement_callback(
    Gwindow   window,
    int       x,
    int       y,
    void      *data )
{
    handle_event( MOUSE_MOVEMENT_EVENT, window, 0 );
}

private  void  left_down_callback(
    Gwindow   window,
    int       x,
    int       y,
    void      *data )
{
    handle_event( LEFT_MOUSE_DOWN_EVENT, window, 0 );
}

private  void  left_up_callback(
    Gwindow   window,
    int       x,
    int       y,
    void      *data )
{
    handle_event( LEFT_MOUSE_UP_EVENT, window, 0 );
}

private  void  middle_down_callback(
    Gwindow   window,
    int       x,
    int       y,
    void      *data )
{
    handle_event( MIDDLE_MOUSE_DOWN_EVENT, window, 0 );
}

private  void  middle_up_callback(
    Gwindow   window,
    int       x,
    int       y,
    void      *data )
{
    handle_event( MIDDLE_MOUSE_UP_EVENT, window, 0 );
}

private  void  right_down_callback( Gwindow   window,
    int       x,
    int       y,
    void      *data )
{
    handle_event( RIGHT_MOUSE_DOWN_EVENT, window, 0 );
}

private  void  right_up_callback(
    Gwindow   window,
    int       x,
    int       y,
    void      *data )
{
    handle_event( RIGHT_MOUSE_UP_EVENT, window, 0 );
}

private  void  iconify_callback(
    Gwindow   window,
    void      *data )
{
    handle_event( WINDOW_ICONIZED_EVENT, window, 0 );
}

private  void  deiconify_callback(
    Gwindow   window,
    void      *data )
{
    handle_event( WINDOW_DEICONIZED_EVENT, window, 0 );
}

private  void  enter_callback(
    Gwindow   window,
    void      *data )
{
    handle_event( WINDOW_ENTER_EVENT, window, 0 );
}

private  void  leave_callback(
    Gwindow   window,
    void      *data )
{
    handle_event( WINDOW_LEAVE_EVENT, window, 0 );
}

private  void  quit_callback(
    Gwindow   window,
    void      *data )
{
    handle_event( WINDOW_QUIT_EVENT, window, 0 );
}

public  void  initialize_window_callbacks(
    display_struct    *display_window )
{
    Gwindow   window;

    window = display_window->window;

    G_set_update_function( window, update_callback, NULL );
    G_set_resize_function( window, resize_callback, NULL);
    G_set_key_down_function( window, key_down_callback, NULL);
    G_set_mouse_movement_function( window, mouse_movement_callback, NULL);
    G_set_left_mouse_down_function( window, left_down_callback, NULL);
    G_set_left_mouse_up_function( window, left_up_callback, NULL);
    G_set_middle_mouse_down_function( window, middle_down_callback, NULL);
    G_set_middle_mouse_up_function( window, middle_up_callback, NULL);
    G_set_right_mouse_down_function( window, right_down_callback, NULL);
    G_set_right_mouse_up_function( window, right_up_callback, NULL);
    G_set_iconify_function( window, iconify_callback, NULL);
    G_set_deiconify_function( window, deiconify_callback, NULL);
    G_set_window_enter_function( window, enter_callback, NULL);
    G_set_window_leave_function( window, leave_callback, NULL);
    G_set_window_quit_function( window, quit_callback, NULL);
}

private  void  update_all_three_d( void )
{
    Status   status;

    status = process_no_events_for_three_d_windows();

    if( status == QUIT )
        quit_program();

    update_all_three_d_windows();
}

private  void  update_all_slice( void )
{
    Status   status;

    status = process_no_events_for_slice_windows();

    if( status == QUIT )
        quit_program();

    update_all_slice_windows();
}

private  void  update_all(
    void   *void_ptr )
{
    update_all_three_d();
    update_all_slice();
    G_add_timer_function( Min_interval_between_updates, update_all, NULL );
}

public  Status   main_event_loop( void )
{
    G_add_timer_function( Min_interval_between_updates, update_all, NULL );

    G_main_loop();

    return( OK );
}

public  BOOLEAN  window_is_up_to_date(
    display_struct   *display )
{
    return( !graphics_update_required( display ) &&
            !display->update_interrupted.last_was_interrupted );
}

private  void  update_all_three_d_windows( void )
{
    update_this_type_of_windows( MENU_WINDOW );
    update_this_type_of_windows( MARKER_WINDOW );
    update_this_type_of_windows( THREE_D_WINDOW );
    update_this_type_of_windows( MARKER_WINDOW );
}

private  void  update_all_slice_windows( void )
{
    update_this_type_of_windows( SLICE_WINDOW );
}

#ifdef DEBUG
private  void  debug_update( void )
{
    static  BOOLEAN  first = TRUE;
    static  Real  start_time;
    static  int   count;
    Real  end_time;

    if( first )
    {
        first = FALSE;
        start_time = current_realtime_seconds();
        count = 0;
    }

    ++count;
    if( count == 20 )
    {
        end_time = current_realtime_seconds();
        print( "FPS: %6.2g\n", (Real) count / (end_time - start_time) );
        count = 0;
        start_time = end_time;
    }
}
#endif

private  void  update_this_type_of_windows(
    window_types   window_type )
{
    int               i, n_windows;
    display_struct    **windows;

    n_windows = get_list_of_windows( &windows );

    for_less( i, 0, n_windows )
    {
        if( windows[i]->window_type == window_type )
        {
            if( window_type == SLICE_WINDOW )
                update_slice_window( windows[i] );

#ifdef DEBUG
            if( window_type == SLICE_WINDOW )
                debug_update();
#endif

            update_graphics( windows[i], &windows[i]->update_interrupted );
        }
    }
}

private  Status  process_no_events_for_three_d_windows( void )
{
    Status            status;
    int               i, n_windows;
    display_struct    **windows;

    status = OK;

    n_windows = get_list_of_windows( &windows );

    for_less( i, 0, n_windows )
        if( windows[i]->window_type != SLICE_WINDOW )
            status = perform_action( windows[i], NO_EVENT, 0 );

    return( status );
}

private  Status  process_no_events_for_slice_windows( void )
{
    Status            status;
    int               i, n_windows;
    display_struct    **windows;

    status = OK;

    n_windows = get_list_of_windows( &windows );

    for_less( i, 0, n_windows )
        if( windows[i]->window_type == SLICE_WINDOW )
            status = perform_action( windows[i], NO_EVENT, 0 );

    return( status );
}

private  Status   perform_action(
    display_struct   *display,
    Event_types      event_type,
    int              key_pressed )
{
    Status               status;
    event_function_type  *actions;
    int                  i, n_actions;

    n_actions = get_event_actions( &display->action_table, event_type,
                                   &actions );

    status = OK;

    for_less( i, 0, n_actions )
    {
        status = (*actions[i]) ( display, event_type, key_pressed );
        if( status != OK )
            break;
    }

    return( status );
}

public  BOOLEAN  is_shift_key_pressed( void )
{
    return( G_get_shift_key_state() ||
            G_get_ctrl_key_state() ||
            G_get_alt_key_state() );
}

