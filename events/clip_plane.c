
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_front_clipping( action_table )
    action_table_struct  *action_table;
{
    DECL_EVENT_FUNCTION( start_front_clipping );
    void                 install_action_table_function();

    install_action_table_function( action_table, LEFT_MOUSE_DOWN_EVENT,
                                   start_front_clipping );
}

private  DEF_EVENT_FUNCTION( start_front_clipping )
    /* ARGSUSED */
{
    void                  add_action_table_function();
    DECL_EVENT_FUNCTION(  handle_update_front );
    DECL_EVENT_FUNCTION(  handle_mouse_movement_front );
    DECL_EVENT_FUNCTION(  terminate_front_clipping );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               handle_update_front );

    add_action_table_function( &graphics->action_table,
                               MOUSE_MOVEMENT_EVENT,
                               handle_mouse_movement_front );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_UP_EVENT,
                               terminate_front_clipping );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_front_clipping )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   perform_clipping();
    void   update_view();

    perform_clipping( graphics, TRUE );

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

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_mouse_movement_front )      /* ARGSUSED */
{
    void   perform_clipping();

    perform_clipping( graphics, TRUE );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_front )      /* ARGSUSED */
{
    void   update_view();

    if( graphics->update_required )
    {
        update_view( graphics );
    }

    return( OK );
}

public  void  initialize_back_clipping( action_table )
    action_table_struct  *action_table;
{
    DECL_EVENT_FUNCTION( start_back_clipping );
    void                 install_action_table_function();

    install_action_table_function( action_table, LEFT_MOUSE_DOWN_EVENT,
                                   start_back_clipping );
}

private  DEF_EVENT_FUNCTION( start_back_clipping )
    /* ARGSUSED */
{
    void                  add_action_table_function();
    DECL_EVENT_FUNCTION(  handle_update_back );
    DECL_EVENT_FUNCTION(  handle_mouse_movement_back );
    DECL_EVENT_FUNCTION(  terminate_back_clipping );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               handle_update_back );

    add_action_table_function( &graphics->action_table,
                               MOUSE_MOVEMENT_EVENT,
                               handle_mouse_movement_back );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_UP_EVENT,
                               terminate_back_clipping );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_back_clipping )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   perform_clipping();
    void   update_view();

    perform_clipping( graphics, FALSE );

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

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_mouse_movement_back )      /* ARGSUSED */
{
    void   perform_clipping();

    perform_clipping( graphics, FALSE );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_back )      /* ARGSUSED */
{
    void   update_view();

    if( graphics->update_required )
    {
        update_view( graphics );
    }

    return( OK );
}

private  void  perform_clipping( graphics, front_flag )
    graphics_struct  *graphics;
    Boolean          front_flag;
{
    Real      delta, dist;

    delta = Point_x(graphics->mouse_position) -
            Point_x(graphics->prev_mouse_position);

    dist = delta * graphics->view.back_distance;

    if( front_flag )
    {
        graphics->view.front_distance += dist;

        if( graphics->view.front_distance < Closest_front_plane )
        {
            graphics->view.front_distance = Closest_front_plane;
        }

        if( graphics->view.front_distance > graphics->view.back_distance )
        {
            graphics->view.front_distance = graphics->view.back_distance;
        }
    }
    else
    {
        graphics->view.back_distance += dist;

        if( graphics->view.back_distance < graphics->view.front_distance )
        {
            graphics->view.back_distance = graphics->view.front_distance;
        }
    }
    
    graphics->update_required = TRUE;

    graphics->prev_mouse_position = graphics->mouse_position;
}
