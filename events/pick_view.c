
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  start_picking_viewport( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( pick_first_corner_point );
    DECL_EVENT_FUNCTION( show_rectangle_at_mouse );
    DECL_EVENT_FUNCTION( terminate_picking_viewport );
    void                 push_action_table();
    void                 add_action_table_function();
    void                 terminate_any_interactions();
    void                 set_update_required();

    push_action_table( &graphics->action_table, MIDDLE_MOUSE_DOWN_EVENT );
    push_action_table( &graphics->action_table, MIDDLE_MOUSE_UP_EVENT );
    push_action_table( &graphics->action_table, TERMINATE_EVENT );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               terminate_picking_viewport );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               pick_first_corner_point );


    add_action_table_function( &graphics->action_table, NO_EVENT,
                               show_rectangle_at_mouse );

    graphics->prev_mouse_position = graphics->mouse_position;
    set_update_required( graphics, NORMAL_PLANES );
}

private  void  remove_events( action_table )
    action_table_struct  *action_table;
{
    void   remove_action_table_function();
    void   pop_action_table();

    remove_action_table_function( action_table, NO_EVENT );

    pop_action_table( action_table, MIDDLE_MOUSE_DOWN_EVENT );
    pop_action_table( action_table, MIDDLE_MOUSE_UP_EVENT );
    pop_action_table( action_table, TERMINATE_EVENT );
}

private  DEF_EVENT_FUNCTION( terminate_picking_viewport )
    /* ARGSUSED */
{
    void   remove_events();

    remove_events( &graphics->action_table );

    return( OK );
}

private  get_coordinates( p1, p2, x_min, y_min, x_max, y_max )
    Point   *p1;
    Point   *p2;
    Real    *x_min, *y_min;
    Real    *x_max, *y_max;
{
    Real   dx, dy, x1, y1, x2, y2;

    x1 = Point_x(*p1);
    y1 = Point_y(*p1);
    x2 = Point_x(*p2);
    y2 = Point_y(*p2);

    dx = ABS( x2 - x1 );
    dy = ABS( y2 - y1 );

    if( dx < Viewport_min_x_size )
    {
        if( x1 <= x2 )
            x2 = x1 + Viewport_min_x_size;
        else
            x2 = x1 - Viewport_min_x_size;
    }

    if( dy < Viewport_min_y_size )
    {
        if( y1 < y2 )
            y2 = y1 + Viewport_min_y_size;
        else
            y2 = y1 - Viewport_min_y_size;
    }

    *x_min = MIN( x1, x2 );
    *x_max = MAX( x1, x2 );
    *y_min = MIN( y1, y2 );
    *y_max = MAX( y1, y2 );
}

private  DEF_EVENT_FUNCTION( show_rectangle_at_mouse )
    /* ARGSUSED */
{
    Boolean  mouse_moved();
    Real     x1, y1, x2, y2;
    void     draw_2d_rectangle();
    void     set_update_required();

    if( graphics_update_required( graphics ) || mouse_moved(graphics) )
    {
        get_coordinates( &graphics->mouse_position, &graphics->mouse_position,
                         &x1, &y1, &x2, &y2 );

        draw_2d_rectangle( graphics, SCREEN_VIEW, &Viewport_feedback_colour,
                           x1, y1, x2, y2 );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( OK );
}

private  DEF_EVENT_FUNCTION( pick_first_corner_point )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION( done_picking_viewport );
    DECL_EVENT_FUNCTION( show_picked_viewport );
    void                 remove_action_table_function();
    void                 add_action_table_function();
    void                 set_update_required();

    graphics->viewport_picking.first_corner = graphics->mouse_position;

    graphics->prev_mouse_position = graphics->mouse_position;

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               done_picking_viewport );

    remove_action_table_function( &graphics->action_table, NO_EVENT );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               show_picked_viewport );

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( show_picked_viewport )
    /* ARGSUSED */
{
    Real   x1, y1, x2, y2;
    void   draw_2d_rectangle();
    void   set_update_required();

    if( graphics_update_required( graphics ) || mouse_moved(graphics) )
    {
        get_coordinates( &graphics->viewport_picking.first_corner,
                         &graphics->mouse_position,
                         &x1, &y1, &x2, &y2 );

        draw_2d_rectangle( graphics, SCREEN_VIEW,
                           &Viewport_feedback_colour, x1, y1, x2, y2 );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( OK );
}

private  DEF_EVENT_FUNCTION( done_picking_viewport )
    /* ARGSUSED */
{
    void   set_view_rectangle();
    void   adjust_view_for_aspect();
    void   update_view();
    Real   x_min, y_min, x_max, y_max;
    void   set_update_required();

    remove_events( &graphics->action_table );

    get_coordinates( &graphics->viewport_picking.first_corner,
                     &graphics->mouse_position,
                     &x_min, &y_min, &x_max, &y_max );

    set_view_rectangle( &graphics->three_d.view, x_min, x_max, y_min, y_max );

    adjust_view_for_aspect( &graphics->three_d.view, &graphics->window );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );
    
    return( OK );
}
