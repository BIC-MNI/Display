
#include  <def_graphics.h>

#define  EVENT_TIMEOUT  0.1

public  Status   main_event_loop( graphics )
    graphics_struct  *graphics;
{
    Status   status;
    Status   process_events();
    void     update_graphics();

    status = OK;

    while( status == OK )
    {
        status = process_events( graphics );

        if( graphics->update_required )
        {
            update_graphics( graphics );
        }
    }

    return( status );
}

Status  process_events( graphics )
    graphics_struct  *graphics;
{
    Status        status;
    Status        perform_action();
    Real          current_realtime_seconds();
    Real          stop_time;
    event_struct  event;
    void          G_get_event();

    status = OK;

    stop_time = current_realtime_seconds() + EVENT_TIMEOUT;

    do
    {
        G_get_event( &event );

        if( event.window_id == graphics->window.window_id )
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
