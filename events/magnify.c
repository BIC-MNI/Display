
#include  <def_graphics.h>

public  void  initialize_magnification( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_magnification );
    DECL_EVENT_FUNCTION( turn_off_magnification );
    void                 add_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               turn_off_magnification );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_magnification );
}

private  DEF_EVENT_FUNCTION( turn_off_magnification )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION( start_magnification );
    void    remove_action_table_function();

    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT, turn_off_magnification );

    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_magnification );

    return( OK );
}

private  DEF_EVENT_FUNCTION( start_magnification )
    /* ARGSUSED */
{
    void                  add_action_table_function();
    DECL_EVENT_FUNCTION(  handle_update );
    DECL_EVENT_FUNCTION(  handle_mouse_movement );
    DECL_EVENT_FUNCTION(  terminate_magnification );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               handle_update );

    add_action_table_function( &graphics->action_table,
                               MOUSE_MOVEMENT_EVENT,
                               handle_mouse_movement );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_magnification );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               terminate_magnification );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_magnification )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION(  handle_update );
    DECL_EVENT_FUNCTION(  handle_mouse_movement );
    void   remove_action_table_function();
    void   perform_magnification();
    void   update_view();

    perform_magnification( graphics );

    if( graphics_update_required( graphics ) )
    {
        update_view( graphics );
    }
    
    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT, handle_update );
    remove_action_table_function( &graphics->action_table,
                                  MOUSE_MOVEMENT_EVENT,
                                  handle_mouse_movement );
    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_magnification );
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT,
                                  terminate_magnification );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_mouse_movement )      /* ARGSUSED */
{
    void   perform_magnification();

    perform_magnification( graphics );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update )      /* ARGSUSED */
{
    void   update_view();

    if( graphics_update_required( graphics ) )
    {
        update_view( graphics );
    }

    return( OK );
}

private  void  perform_magnification( graphics )
    graphics_struct  *graphics;
{
    Real      delta, factor;
    void      magnify_view_size();
    void      set_update_required();

    delta = Point_x(graphics->mouse_position) -
            Point_x(graphics->prev_mouse_position);

    factor = exp( -delta * log( 2.0 ) );

    magnify_view_size( &graphics->three_d.view, factor );

    set_update_required( graphics, NORMAL_PLANES );

    graphics->prev_mouse_position = graphics->mouse_position;
}
