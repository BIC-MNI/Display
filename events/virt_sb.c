
#include  <def_graphics.h>

public  void  initialize_virtual_spaceball( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_virtual_spaceball );
    DECL_EVENT_FUNCTION( turn_off_virtual_spaceball );
    void                 add_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               start_virtual_spaceball );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               turn_off_virtual_spaceball );
}

private  DEF_EVENT_FUNCTION( turn_off_virtual_spaceball )
    /* ARGSUSED */
{
    void         remove_action_table_function();

    remove_action_table_function( &graphics->action_table,
                                  LEFT_MOUSE_DOWN_EVENT );

    remove_action_table_function( &graphics->action_table, TERMINATE_EVENT );
}

private  DEF_EVENT_FUNCTION( start_virtual_spaceball )
    /* ARGSUSED */
{
    void                  add_action_table_function();
    DECL_EVENT_FUNCTION(  handle_update );
    DECL_EVENT_FUNCTION(  handle_mouse_movement );
    DECL_EVENT_FUNCTION(  terminate_virtual_spaceball );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               handle_update );

    add_action_table_function( &graphics->action_table,
                               MOUSE_MOVEMENT_EVENT,
                               handle_mouse_movement );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_UP_EVENT,
                               terminate_virtual_spaceball );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               terminate_virtual_spaceball );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_virtual_spaceball )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   perform_rotation();
    void   update_view();

    perform_rotation( graphics );

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
    void   perform_rotation();

    perform_rotation( graphics );

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

private  void  perform_rotation( graphics )
    graphics_struct  *graphics;
{
    static  Point  centre = { 0.5, 0.5, 0.0 };
    Real           x_radius, y_radius;
    Real           aspect;
    Real           G_get_window_aspect();
    Transform      transform;
    Boolean        make_spaceball_transform();
    void           apply_transform();

    aspect = G_get_window_aspect( &graphics->window );

    if( aspect < 1.0 )
    {
        x_radius = 0.5 * aspect;
        y_radius = 0.5;
    }
    else
    {
        x_radius = 0.5;
        y_radius = 0.5 / aspect;
    }

    if( make_spaceball_transform( &graphics->prev_mouse_position,
                                  &graphics->mouse_position,
                                  &centre, x_radius, y_radius,
                                  &transform ) )
    {
        apply_transform( graphics, &transform );
    }

    graphics->prev_mouse_position = graphics->mouse_position;
}

private  Boolean  make_spaceball_transform( old, new, centre,
                                            x_radius, y_radius,
                                            transform )
    Point      *old;
    Point      *new;
    Point      *centre;
    Real       x_radius;
    Real       y_radius;
    Transform  *transform;
{
    Boolean  transform_created;
    Real     x_old, y_old, x_new, y_new, z_old, z_new;
    Real     dist_old, dist_new;
    Real     angle, sin_angle;
    Vector   v0, v1;
    Vector   axis_of_rotation;
    void     make_rotation_about_axis();

    x_old = (Point_x(*old) - Point_x(*centre)) / x_radius;
    y_old = (Point_y(*old) - Point_y(*centre)) / y_radius;

    x_new = (Point_x(*new) - Point_x(*centre)) / x_radius;
    y_new = (Point_y(*new) - Point_y(*centre)) / y_radius;

    dist_old = x_old * x_old + y_old * y_old;
    dist_new = x_new * x_new + y_new * y_new;

    transform_created = FALSE;

    if( (x_old != x_new || y_old != y_new) &&
        dist_old <= 1.0 && dist_new <= 1.0 )
    {
        z_old = 1.0 - sqrt( dist_old );
        z_new = 1.0 - sqrt( dist_new );

        fill_Vector( v0, x_old, y_old, z_old );
        fill_Vector( v1, x_new, y_new, z_new );

        CROSS_VECTORS( axis_of_rotation, v0, v1 );

        sin_angle = MAGNITUDE( axis_of_rotation );

        if( sin_angle > 0.0 )
        {
            SCALE_VECTOR( axis_of_rotation, axis_of_rotation, 1.0 / sin_angle );

            angle = asin( (double) sin_angle );
            if( DOT_VECTORS( v0, v1 ) < 0.0 )
            {
                angle += PI / 2.0;
            }

            make_rotation_about_axis( &axis_of_rotation, angle, transform );

            transform_created = TRUE;
        }
    }

    return( transform_created );
}

private  void  apply_transform( graphics, transform )
    graphics_struct   *graphics;
    Transform         *transform;
{
    void    make_transform_in_coordinate_system();
    Vector  z_axis;
    Point   centre;
    void    get_view_z_axis();
    void    get_view_centre();
    void    transform_model();

    get_view_z_axis( &graphics->three_d.view, &z_axis );
    get_view_centre( &graphics->three_d.view, &centre );

    make_transform_in_coordinate_system( &centre,
                                         &graphics->three_d.view.x_axis,
                                         &graphics->three_d.view.y_axis,
                                         &z_axis,
                                         transform, transform );

    transform_model( graphics, transform );

    graphics->update_required = TRUE;
}
