
#include  <display.h>

public  void   terminate_any_interactions(
    display_struct   *display )
{
    Status               status;
    event_function_type  *actions;
    int                  i, n_actions;

    n_actions = get_event_actions( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   &actions );

    status = OK;

    for_less( i, 0, n_actions )
    {
        status = (*actions[i]) ( display, TERMINATE_INTERACTION_EVENT, 0 );
        if( status != OK )
        {
            break;
        }
    }
}
