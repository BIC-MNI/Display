
#include  <def_graphics.h>

public  void  initialize_magnification( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_magnification );
    void                 install_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );
    install_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_DOWN_EVENT,
                                   start_magnification );
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
                               LEFT_MOUSE_UP_EVENT,
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
    void   remove_action_table_function();
    void   perform_magnification();
    void   update_view();

    perform_magnification( graphics );

    if( graphics->update_required )
    {
        update_view( graphics );
    }
    
    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  MOUSE_MOVEMENT_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  LEFT_MOUSE_UP_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT );

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

    if( graphics->update_required )
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

    delta = Point_x(graphics->mouse_position) -
            Point_x(graphics->prev_mouse_position);

    factor = exp( -delta * log( 2.0 ) );

    magnify_view_size( &graphics->view, factor );

    graphics->update_required = TRUE;

    graphics->prev_mouse_position = graphics->mouse_position;
}
