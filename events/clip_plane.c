
#include  <display.h>

private    DEF_EVENT_FUNCTION( start_front_clipping );
private    DEF_EVENT_FUNCTION( turn_off_front_clipping );
private    DEF_EVENT_FUNCTION( handle_update_front );
private    DEF_EVENT_FUNCTION( terminate_front_clipping );
private    DEF_EVENT_FUNCTION( start_back_clipping );
private    DEF_EVENT_FUNCTION( turn_off_back_clipping );
private    DEF_EVENT_FUNCTION( handle_update_back );
private    DEF_EVENT_FUNCTION( terminate_back_clipping );
private  BOOLEAN  perform_clipping(
    display_struct   *display,
    BOOLEAN          front_flag );

public  void  initialize_front_clipping(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_front_clipping );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_front_clipping );
}

private  DEF_EVENT_FUNCTION( turn_off_front_clipping )
    /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_front_clipping );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_front_clipping );

    return( OK );
}

private  DEF_EVENT_FUNCTION( start_front_clipping )
    /* ARGSUSED */
{
    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update_front );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_front_clipping );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_front_clipping );

    record_mouse_position( display );

    return( OK );
}

private  void  update_clipping(
    display_struct   *display,
    BOOLEAN          front_flag )
{
    if( perform_clipping( display, front_flag ) )
    {
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
}

private  DEF_EVENT_FUNCTION( terminate_front_clipping )
    /* ARGSUSED */
{
    update_clipping( display, TRUE );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, turn_off_front_clipping );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_front_clipping );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_front_clipping );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_front )      /* ARGSUSED */
{
    update_clipping( display, TRUE );

    return( OK );
}

public  void  initialize_back_clipping(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_back_clipping );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_back_clipping );
}

private  DEF_EVENT_FUNCTION( turn_off_back_clipping )
    /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_back_clipping );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_back_clipping );

    return( OK );
}

private  DEF_EVENT_FUNCTION( start_back_clipping )
    /* ARGSUSED */
{
    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update_back );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_back_clipping );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_back_clipping );

    record_mouse_position( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_back_clipping )
    /* ARGSUSED */
{
    update_clipping( display, FALSE );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_back );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_back_clipping );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_back_clipping );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_back )      /* ARGSUSED */
{
    update_clipping( display, FALSE );

    return( OK );
}

private  BOOLEAN  perform_clipping(
    display_struct   *display,
    BOOLEAN          front_flag )
{
    BOOLEAN   moved;
    Real      x, y, x_prev, y_prev;
    Real      delta, dist;

    moved = FALSE;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) )
    {
        delta = x - x_prev;

        dist = delta * display->three_d.view.back_distance;

        if( front_flag )
        {
            display->three_d.view.front_distance += dist;

            if( display->three_d.view.front_distance < Closest_front_plane )
                display->three_d.view.front_distance = Closest_front_plane;

            if( display->three_d.view.front_distance >
                display->three_d.view.back_distance )
            {
                display->three_d.view.front_distance =
                display->three_d.view.back_distance;
            }
        }
        else
        {
            display->three_d.view.back_distance += dist;

            if( display->three_d.view.back_distance <
                display->three_d.view.front_distance )
            {
                display->three_d.view.back_distance =
                display->three_d.view.front_distance;
            }
        }
    
        moved = TRUE;
    }

    return( moved );
}
