
#include  <def_display.h>

static    DEF_EVENT_FUNCTION( start_magnification );
static    DEF_EVENT_FUNCTION( turn_off_magnification );
static    DEF_EVENT_FUNCTION( handle_update );
static    DEF_EVENT_FUNCTION( handle_mouse_movement );
static    DEF_EVENT_FUNCTION( terminate_magnification );
private  void  perform_magnification(
    display_struct   *display );

public  void  initialize_magnification(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_magnification );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_magnification );
}

private  DEF_EVENT_FUNCTION( turn_off_magnification ) /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_magnification );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_magnification );

    return( OK );
}

private  DEF_EVENT_FUNCTION( start_magnification ) /* ARGSUSED */
{
    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update );

    add_action_table_function( &display->action_table,
                               MOUSE_MOVEMENT_EVENT,
                               handle_mouse_movement );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_magnification );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_magnification );

    record_mouse_position( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_magnification )    /* ARGSUSED */
{
    perform_magnification( display );

    if( graphics_update_required( display ) )
        update_view( display );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update );
    remove_action_table_function( &display->action_table,
                                  MOUSE_MOVEMENT_EVENT,
                                  handle_mouse_movement );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_magnification );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_magnification );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_mouse_movement )      /* ARGSUSED */
{
    perform_magnification( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update )      /* ARGSUSED */
{
    if( graphics_update_required( display ) )
    {
        update_view( display );
    }

    return( OK );
}

private  void  perform_magnification(
    display_struct   *display )
{
    Real      x, y, x_prev, y_prev, delta, factor;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) )
    {
        delta = x - x_prev;

        factor = exp( -delta * log( 2.0 ) );

        magnify_view_size( &display->three_d.view, factor );

        set_update_required( display, NORMAL_PLANES );
    }
}
