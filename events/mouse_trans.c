
#include  <def_graphics.h>

public  void  initialize_translation( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_translation );
    DECL_EVENT_FUNCTION( turn_off_translation );
    void                 add_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               turn_off_translation );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_translation );
}

private  DEF_EVENT_FUNCTION( turn_off_translation )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION( start_translation );
    void   remove_action_table_function();

    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT, turn_off_translation );

    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_translation );

    return( OK );
}

private  DEF_EVENT_FUNCTION( start_translation )
    /* ARGSUSED */
{
    void                  add_action_table_function();
    DECL_EVENT_FUNCTION(  handle_update );
    DECL_EVENT_FUNCTION(  handle_mouse_movement );
    DECL_EVENT_FUNCTION(  terminate_translation );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               handle_update );

    add_action_table_function( &graphics->action_table,
                               MOUSE_MOVEMENT_EVENT,
                               handle_mouse_movement );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_translation );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               terminate_translation );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_translation )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION(  handle_update );
    DECL_EVENT_FUNCTION(  handle_mouse_movement );
    void   remove_action_table_function();
    void   perform_translation();
    void   update_view();

    perform_translation( graphics );

    if( graphics_update_required( graphics ) )
    {
        update_view( graphics );
    }
    
    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT, handle_update );
    remove_action_table_function( &graphics->action_table,
                                  MOUSE_MOVEMENT_EVENT, handle_mouse_movement );
    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_translation );
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT, terminate_translation );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_mouse_movement )      /* ARGSUSED */
{
    void   perform_translation();

    perform_translation( graphics );

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

private  void  perform_translation( graphics )
    graphics_struct  *graphics;
{
    Vector         delta, hor, vert;
    Transform      transform;
    void           transform_model();
    void           get_screen_axes();
    void           make_translation_transform();
    void           set_update_required();

    SUB_POINTS( delta, graphics->mouse_position,
                       graphics->prev_mouse_position );

    get_screen_axes( &graphics->three_d.view, &hor, &vert );

    SCALE_VECTOR( hor, hor, Point_x(delta) );
    SCALE_VECTOR( vert, vert, Point_y(delta) );

    ADD_VECTORS( delta, hor, vert );

    make_translation_transform( &delta, &transform );

    transform_model( graphics, &transform );

    set_update_required( graphics, NORMAL_PLANES );

    graphics->prev_mouse_position = graphics->mouse_position;
}
