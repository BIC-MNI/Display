
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  start_picking_viewport( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( pick_first_corner_point );
    DECL_EVENT_FUNCTION( show_rectangle_at_mouse );
    void                 install_action_table_function();
    void                 push_action_table();

    push_action_table( &graphics->action_table, NO_EVENT );
    push_action_table( &graphics->action_table, LEFT_MOUSE_DOWN_EVENT );
    push_action_table( &graphics->action_table, LEFT_MOUSE_UP_EVENT );
    push_action_table( &graphics->action_table, TERMINATE_EVENT );

    install_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_DOWN_EVENT,
                                   pick_first_corner_point );

    install_action_table_function( &graphics->action_table,
                                   NO_EVENT,
                                   show_rectangle_at_mouse );

    graphics->prev_mouse_position = graphics->mouse_position;
    graphics->update_required = TRUE;
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

    if( graphics->update_required || mouse_moved(graphics) )
    {
        get_coordinates( &graphics->mouse_position, &graphics->mouse_position,
                         &x1, &y1, &x2, &y2 );

        draw_2d_rectangle( graphics, SCREEN_VIEW, &Viewport_feedback_colour,
                           x1, y1, x2, y2 );

        graphics->update_required = TRUE;
    }

    return( OK );
}

private  DEF_EVENT_FUNCTION( pick_first_corner_point )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION( done_picking_viewport );
    DECL_EVENT_FUNCTION( show_picked_viewport );

    graphics->viewport_picking.first_corner = graphics->mouse_position;

    graphics->prev_mouse_position = graphics->mouse_position;

    install_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   done_picking_viewport );

    install_action_table_function( &graphics->action_table,
                                   NO_EVENT,
                                   show_picked_viewport );

    graphics->update_required = TRUE;

    return( OK );
}

private  DEF_EVENT_FUNCTION( show_picked_viewport )
    /* ARGSUSED */
{
    Real   x1, y1, x2, y2;
    void   draw_2d_rectangle();

    if( graphics->update_required || mouse_moved(graphics) )
    {
        get_coordinates( &graphics->viewport_picking.first_corner,
                         &graphics->mouse_position,
                         &x1, &y1, &x2, &y2 );

        draw_2d_rectangle( graphics, SCREEN_VIEW,
                           &Viewport_feedback_colour, x1, y1, x2, y2 );

        graphics->update_required = TRUE;
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
    void   pop_action_table();

    pop_action_table( &graphics->action_table, NO_EVENT );
    pop_action_table( &graphics->action_table, LEFT_MOUSE_DOWN_EVENT );
    pop_action_table( &graphics->action_table, LEFT_MOUSE_UP_EVENT );
    pop_action_table( &graphics->action_table, TERMINATE_EVENT );

    get_coordinates( &graphics->viewport_picking.first_corner,
                     &graphics->mouse_position,
                     &x_min, &y_min, &x_max, &y_max );

    set_view_rectangle( &graphics->three_d.view, x_min, x_max, y_min, y_max );

    adjust_view_for_aspect( &graphics->three_d.view, &graphics->window );

    update_view( graphics );

    graphics->update_required = TRUE;
    
    return( OK );
}
