
#include  <def_graphics.h>
#include  <def_globals.h>

public  Status   main_event_loop()
{
    Status   status;
    Status   process_events();
    Status   process_no_events_for_all_windows();
    void     update_all_required_windows();
    Real     update_time;
    Real     current_realtime_seconds();

    status = OK;

    update_time = 0.0;

    while( status != QUIT )
    {
        status = process_events( update_time );

        if( status != QUIT )
        {
            status = process_no_events_for_all_windows();
        }

        update_time = current_realtime_seconds();

        update_all_required_windows();

        update_time = current_realtime_seconds() - update_time;
    }

    return( OK );
}

public  Boolean  window_is_up_to_date( graphics )
    graphics_struct   *graphics;
{
    return( !graphics_update_required( graphics ) &&
            !graphics->update_interrupted.last_was_interrupted );
}

private  void  update_all_required_windows()
{
    int               i, n_windows;
    int               get_list_of_windows();
    graphics_struct   **windows;
    void              update_graphics();

    n_windows = get_list_of_windows( &windows );

    for_less( i, 0, n_windows )
    {
        if( graphics_update_required( windows[i] ) )
        {
            windows[i]->update_interrupted.last_was_interrupted = FALSE;
        }

        if( !window_is_up_to_date( windows[i] ) )
        {
            update_graphics( windows[i], &windows[i]->update_interrupted );
        }
    }
}

private  Status  process_no_events_for_all_windows()
{
    Status            status;
    Status            perform_action();
    int               i, n_windows;
    int               get_list_of_windows();
    graphics_struct   **windows;
    event_struct      event;

    status = OK;

    n_windows = get_list_of_windows( &windows );

    event.event_type = NO_EVENT;

    for_less( i, 0, n_windows )
    {
        status = perform_action( windows[i], &event );
    }

    return( status );
}

Status  process_events( update_time )
    Real   update_time;
{
    Status            status;
    Status            perform_action();
    Real              current_realtime_seconds();
    Real              stop_time, event_time;
    event_struct      event;
    graphics_struct   *graphics;
    graphics_struct   *lookup_window();
    void              G_get_event();

    status = OK;

    event_time = update_time * Event_timeout_factor;

    if( event_time < Event_timeout_min )
    {
        event_time = Event_timeout_min;
    }

    stop_time = current_realtime_seconds() + event_time;

    do
    {
        G_get_event( &event );

        if( event.event_type != NO_EVENT )
        {
            graphics = lookup_window( event.window_id );

            if( graphics != (graphics_struct *) 0 )
            {
                status = perform_action( graphics, &event );
            }
        }
    }
    while( status == OK &&
           event.event_type != NO_EVENT &&
           current_realtime_seconds() < stop_time );

    return( status );
}

private  Status   perform_action( graphics, event )
    graphics_struct  *graphics;
    event_struct     *event;
{
    Status               status;
    event_function_type  *actions;
    int                  i, n_actions;
    int                  get_event_actions();

    n_actions = get_event_actions( &graphics->action_table, event->event_type,
                                   &actions );

    status = OK;

    for_less( i, 0, n_actions )
    {
        status = (*actions[i]) ( graphics, event );
        if( status != OK )
        {
            break;
        }
    }

    return( status );
}
