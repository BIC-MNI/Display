
#include  <def_graphics.h>

public  void   terminate_any_interactions( graphics )
    graphics_struct  *graphics;
{
    Status               status;
    event_function_type  *actions;
    int                  i, n_actions;
    int                  get_event_actions();
    event_struct         event;

    n_actions = get_event_actions( &graphics->action_table, TERMINATE_EVENT,
                                   &actions );

    status = OK;

    event.event_type = TERMINATE_EVENT;

    for_less( i, 0, n_actions )
    {
        status = (*actions[i]) ( graphics, &event );
        if( status != OK )
        {
            break;
        }
    }
}
