
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_virtual_spaceball( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_virtual_spaceball );
    DECL_EVENT_FUNCTION( turn_off_virtual_spaceball );
    void                 add_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
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
                                  MIDDLE_MOUSE_DOWN_EVENT );

    remove_action_table_function( &graphics->action_table, TERMINATE_EVENT );
}

private  DEF_EVENT_FUNCTION( start_virtual_spaceball )
    /* ARGSUSED */
{
    void                  add_action_table_function();
    DECL_EVENT_FUNCTION(  handle_update_rotation );
    DECL_EVENT_FUNCTION(  terminate_rotation );
    DECL_EVENT_FUNCTION(  handle_update_translation );
    DECL_EVENT_FUNCTION(  terminate_translation );
    Boolean               mouse_close_to_cursor();

    graphics->prev_mouse_position = graphics->mouse_position;

    if( mouse_close_to_cursor( graphics ) )
    {
        add_action_table_function( &graphics->action_table,
                                   NO_EVENT, handle_update_translation );

        add_action_table_function( &graphics->action_table,
                                   MIDDLE_MOUSE_UP_EVENT,
                                   terminate_translation );

        add_action_table_function( &graphics->action_table,
                                   TERMINATE_EVENT, terminate_translation );
    }
    else
    {
        add_action_table_function( &graphics->action_table,
                                   NO_EVENT, handle_update_rotation );

        add_action_table_function( &graphics->action_table,
                                   MIDDLE_MOUSE_UP_EVENT, terminate_rotation );

        add_action_table_function( &graphics->action_table,
                                   TERMINATE_EVENT, terminate_rotation );
    }

    return( OK );
}

private  void  update_rotation( graphics )
    graphics_struct  *graphics;
{
    void      update_view();
    Boolean   perform_rotation();

    if( perform_rotation( graphics ) )
    {
        update_view( graphics );

        graphics->update_required = TRUE;
    }
}

private  DEF_EVENT_FUNCTION( handle_update_rotation )      /* ARGSUSED */
{
    void      update_rotation();

    update_rotation( graphics );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_rotation )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   update_rotation();

    update_rotation( graphics );
    
    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_UP_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT );

    return( OK );
}

private  Boolean  perform_rotation( graphics )
    graphics_struct  *graphics;
{
    static  Point  centre = { 0.5, 0.5, 0.0 };
    Real           x_radius, y_radius;
    Real           aspect;
    Real           G_get_window_aspect();
    Transform      transform;
    Boolean        make_spaceball_transform();
    void           apply_transform_in_view_space();
    Boolean        moved;

    moved = FALSE;

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
        apply_transform_in_view_space( graphics, &transform );
        moved = TRUE;
    }

    graphics->prev_mouse_position = graphics->mouse_position;

    return( moved );
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

private  void  update_translation( graphics )
    graphics_struct  *graphics;
{
    void      update_view();
    Boolean   perform_cursor_translation();

    if( perform_cursor_translation( graphics ) )
    {
        graphics->update_required = TRUE;
    }
}

private  DEF_EVENT_FUNCTION( handle_update_translation )      /* ARGSUSED */
{
    void      update_translation();

    update_translation( graphics );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_translation )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   update_translation();

    update_translation( graphics );
    
    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_UP_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT );

    return( OK );
}

private  Boolean  perform_cursor_translation( graphics )
    graphics_struct  *graphics;
{
    Vector       mouse_dir, offset, axis_direction;
    Vector       ray_direction, transformed_direction;
    Point        ray_origin, transformed_origin;
    int          axis_index, best_axis, second_best_axis, a1, a2;
    Point        pt, pt_screen, cursor_screen, new_screen_origin, new_cursor;
    Boolean      moved;
    Boolean      intersect_lines_3d();
    Real         mag_mouse, mag_axis[N_DIMENSIONS], dot_prod[N_DIMENSIONS];
    Real         angle[N_DIMENSIONS], dist, mouse_dist, prev_mouse_dist;
    Vector       axis_screen[N_DIMENSIONS], prev_offset;
    void         transform_point_to_screen();
    void         convert_mouse_to_ray();
    void         transform_world_to_model();
    void         transform_world_to_model_vector();
    void         update_cursor();

    moved = FALSE;

    if( !EQUAL_POINTS(graphics->mouse_position,graphics->prev_mouse_position) )
    {
        SUB_POINTS( mouse_dir, graphics->mouse_position,
                               graphics->prev_mouse_position );

        mag_mouse = MAGNITUDE( mouse_dir );

        if( mag_mouse > Cursor_mouse_threshold )
        {
            moved = TRUE;
        }
    }

    if( moved )
    {
        pt = graphics->three_d.cursor.origin;
        transform_point_to_screen( &graphics->three_d.view, &pt,
                                   &cursor_screen );

        for_less( axis_index, 0, N_DIMENSIONS )
        {
            pt = graphics->three_d.cursor.origin;
            Point_coord(pt,axis_index) += 1.0;
            transform_point_to_screen( &graphics->three_d.view, &pt,
                                       &pt_screen );
            SUB_POINTS( axis_screen[axis_index], pt_screen, cursor_screen );
            mag_axis[axis_index] = MAGNITUDE( axis_screen[axis_index] );

            if( mag_axis[axis_index] == 0.0 )
            {
                dot_prod[axis_index] = 0.0;
                angle[axis_index] = 90.0;
            }
            else
            {
                dot_prod[axis_index] =
                         DOT_VECTORS( mouse_dir,axis_screen[axis_index]) /
                         mag_mouse / mag_axis[axis_index];
                angle[axis_index] = acos( (double) ABS(dot_prod[axis_index]) )
                                    * RAD_TO_DEG;
            }
        }

        best_axis = X_AXIS;
        for_inclusive( axis_index, Y_AXIS, Z_AXIS )
        {
            if( ABS(dot_prod[axis_index]) > ABS(dot_prod[best_axis]) )
            {
                best_axis = axis_index;
            }
        }

        a1 = (best_axis + 1) % N_DIMENSIONS;
        a2 = (best_axis + 2) % N_DIMENSIONS;

        if( ABS(dot_prod[a1]) > ABS(dot_prod[a2]) )
            second_best_axis = a1;
        else
            second_best_axis = a2;

        if( angle[best_axis] > Max_cursor_angle ||
            angle[second_best_axis] - angle[best_axis] < Min_cursor_angle_diff )
        {
            moved = FALSE;
        }
    }

    if( moved )
    {
        mouse_dist = dot_prod[best_axis] * mag_mouse;
        SUB_POINTS( prev_offset, graphics->prev_mouse_position, cursor_screen );
        prev_mouse_dist = DOT_VECTORS( prev_offset, axis_screen[best_axis] ) /
                          mag_axis[best_axis];
        dist = mouse_dist - prev_mouse_dist;
        dist = mouse_dist;
        SCALE_VECTOR( offset, axis_screen[best_axis], dist );
        ADD_POINT_VECTOR( new_screen_origin, cursor_screen, offset );

        fill_Vector( axis_direction, 0.0, 0.0, 0.0 );
        Vector_coord( axis_direction, best_axis ) = 1.0;

        convert_mouse_to_ray( &graphics->three_d.view, &new_screen_origin,
                              &ray_origin, &ray_direction );
        transform_world_to_model( &graphics->three_d.view, &ray_origin,
                                  &transformed_origin );
        transform_world_to_model_vector( &graphics->three_d.view,
                                         &ray_direction,
                                         &transformed_direction );
        moved = intersect_lines_3d(
                     &graphics->three_d.cursor.origin,
                     &axis_direction,
                     &transformed_origin, &transformed_direction, &new_cursor );
    }

    if( moved )
    {
        graphics->three_d.cursor.origin = new_cursor;

        update_cursor( graphics );

        graphics->prev_mouse_position = graphics->mouse_position;
    }

    return( moved );
}

private  Boolean  mouse_close_to_cursor( graphics )
    graphics_struct   *graphics;
{
    Boolean  close;
    void     transform_point_to_screen();
    Point    cursor_screen;
    Vector   diff_vector;
    Real     diff;

    transform_point_to_screen( &graphics->three_d.view,
                               &graphics->three_d.cursor.origin,
                               &cursor_screen );

    SUB_POINTS( diff_vector, cursor_screen, graphics->mouse_position );

    diff = MAGNITUDE( diff_vector );

    close = (diff < Cursor_pick_distance);

    return( close );
}
