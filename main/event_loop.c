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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/main/event_loop.c,v 1.23 1995-08-21 17:15:12 david Exp $";
#endif


#include  <display.h>

private  Status   process_no_events_for_all_windows( void );
private  void     update_all_required_windows( void );
private  Status   perform_action(
    display_struct   *display,
    Event_types      event_type,
    int              key_pressed );
private  void  update_this_type_of_windows(
    window_types   window_type );

public  Status   main_event_loop( void )
{
    Status          status;
    Event_types     event_type;
    int             key_pressed;
    window_struct   *window;
    display_struct  *display;

    status = OK;

    while( status != QUIT )
    {
        event_type = G_get_event( &window, &key_pressed );

        if( event_type != NO_EVENT )
        {
            display = lookup_window( window );

            if( display != (display_struct *) 0 )
                status = perform_action( display, event_type, key_pressed );
  
        }
        else
        {
            if( status != QUIT )
                status = process_no_events_for_all_windows();

            update_all_required_windows();
        }
    }

    return( OK );
}

public  BOOLEAN  window_is_up_to_date(
    display_struct   *display )
{
    return( !graphics_update_required( display ) &&
            !display->update_interrupted.last_was_interrupted );
}

private  void  update_all_required_windows( void )
{
    update_this_type_of_windows( MENU_WINDOW );
    update_this_type_of_windows( SLICE_WINDOW );
    update_this_type_of_windows( THREE_D_WINDOW );
}

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

            update_graphics( windows[i], &windows[i]->update_interrupted );
        }
    }
}

private  Status  process_no_events_for_all_windows( void )
{
    Status            status;
    int               i, n_windows;
    display_struct    **windows;

    status = OK;

    n_windows = get_list_of_windows( &windows );

    for_less( i, 0, n_windows )
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
