
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_front_clipping( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_front_clipping );
    DECL_EVENT_FUNCTION( turn_off_front_clipping );
    void                 add_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               turn_off_front_clipping );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               start_front_clipping );
}

private  DEF_EVENT_FUNCTION( turn_off_front_clipping )
    /* ARGSUSED */
{
    void    remove_action_table_function();

    remove_action_table_function( &graphics->action_table,
                                  LEFT_MOUSE_DOWN_EVENT );

    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT );

    return( OK );
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

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
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
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT );

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

public  void  initialize_back_clipping( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_back_clipping );
    DECL_EVENT_FUNCTION( turn_off_back_clipping );
    void                 add_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               turn_off_back_clipping );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               start_back_clipping );
}

private  DEF_EVENT_FUNCTION( turn_off_back_clipping )
    /* ARGSUSED */
{
    void   remove_action_table_function();

    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT );

    remove_action_table_function( &graphics->action_table,
                                  LEFT_MOUSE_DOWN_EVENT );

    return( OK );
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

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
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
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT );

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

    dist = delta * graphics->three_d.view.back_distance;

    if( front_flag )
    {
        graphics->three_d.view.front_distance += dist;

        if( graphics->three_d.view.front_distance < Closest_front_plane )
        {
            graphics->three_d.view.front_distance = Closest_front_plane;
        }

        if( graphics->three_d.view.front_distance >
            graphics->three_d.view.back_distance )
        {
            graphics->three_d.view.front_distance =
            graphics->three_d.view.back_distance;
        }
    }
    else
    {
        graphics->three_d.view.back_distance += dist;

        if( graphics->three_d.view.back_distance <
            graphics->three_d.view.front_distance )
        {
            graphics->three_d.view.back_distance =
            graphics->three_d.view.front_distance;
        }
    }
    
    graphics->update_required = TRUE;

    graphics->prev_mouse_position = graphics->mouse_position;
}
