
#include  <def_graphics.h>

#define  EVENT_TIMEOUT  0.1

public  Status   main_event_loop()
{
    Status   status;
    Status   process_events();
    void     update_all_required_windows();

    status = OK;

    while( status == OK )
    {
        status = process_events();

        update_all_required_windows();
    }

    return( status );
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
        if( windows[i]->update_required )
        {
            update_graphics( windows[i] );
        }
    }
}

Status  process_events()
{
    Status            status;
    Status            perform_action();
    Real              current_realtime_seconds();
    Real              stop_time;
    event_struct      event;
    graphics_struct   *graphics;
    graphics_struct   *lookup_window();
    void              G_get_event();

    status = OK;

    stop_time = current_realtime_seconds() + EVENT_TIMEOUT;

    do
    {
        G_get_event( &event );

        graphics = lookup_window( event.window_id );

        if( graphics != (graphics_struct *) 0 )
        {
            status = perform_action( graphics, &event );
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
