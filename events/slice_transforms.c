
#include  <def_display.h>

static    DECL_EVENT_FUNCTION( start_rotating );
static    DECL_EVENT_FUNCTION( end_rotating );
static    DECL_EVENT_FUNCTION( handle_update_rotation );
static    DECL_EVENT_FUNCTION( terminate_rotation );

public  void  start_translating_slice(
    display_struct   *display   /* ARGSUSED */ )
{
}

public  void  start_rotating_slice(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_rotating );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               end_rotating );
}

private  DEF_EVENT_FUNCTION( end_rotating )     /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_rotating );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  end_rotating );
}

private  DEF_EVENT_FUNCTION( start_rotating )     /* ARGSUSED */
{
    push_action_table( &display->action_table, NO_EVENT );
    push_action_table( &display->action_table, MIDDLE_MOUSE_UP_EVENT );
    push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    add_action_table_function( &display->action_table,
                               NO_EVENT, handle_update_rotation );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_rotation );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_rotation );

    record_mouse_position( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_rotation )     /* ARGSUSED */
{
    update_rotation( display );
    
    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, MIDDLE_MOUSE_UP_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    return( OK );
}

private  void  modify_slice_transform(
    display_struct   *display,
    Transform        *transform )
{
    volume_struct  *volume;
    int            nx, ny, nz;
    Point          centre_of_rotation;

    if( get_current_volume( display, &volume ) )
    {
        get_volume_size( volume, &nx, &ny, &nz );
        Point_x(centre_of_rotation) = (Real) nx / 2.0;
        Point_y(centre_of_rotation) = (Real) ny / 2.0;
        Point_z(centre_of_rotation) = (Real) nz / 2.0;

        make_transform_relative_to_point( &centre_of_rotation, transform,
                                          transform );

        concat_transforms( &volume->slice_transforms[volume->current_slice],
                           &volume->slice_transforms[volume->current_slice],
                           transform );

        set_update_required( display, NORMAL_PLANES );
    }
}

private  void  update_rotation(
    display_struct   *display )
{
    Transform      transform;

    if( compute_rotation( display, &transform ) )
        modify_slice_transform( display, &transform );
}

private  DEF_EVENT_FUNCTION( handle_update_rotation )      /* ARGSUSED */
{
    update_rotation( display );

    return( OK );
}

private  Boolean  compute_rotation(
    display_struct   *display,
    Transform        *transform )
{
    static  Point  centre = { 0.5, 0.5, 0.0 };
    Real           x1, x2, y1, y2, curr_angle, prev_angle;
    Boolean        moved;

    moved = FALSE;

    if( G_get_mouse_position_0_1( display->window, &x2, &y2 ) )
    {
        x2 = Point_x(display->mouse_position);
        y2 = Point_y(display->mouse_position);

        prev_angle = compute_clockwise_rotation( x1 - Point_x(centre),
                                                 y1 - Point_y(centre) );

        curr_angle = compute_clockwise_rotation( x2 - Point_x(centre),
                                                 y2 - Point_y(centre) );

        if( prev_angle != curr_angle )
        {
            make_rotation_transform( curr_angle - prev_angle, Z, transform );
            moved = TRUE;
        }
        else
            moved = FALSE;

        record_mouse_position( display );
    }

    return( moved );
}


#ifdef NOT_YET
private  void  perform_translation(
    display_struct   *display )
{
    Vector         delta, hor, vert;
    Transform      transform;

    SUB_POINTS( delta, display->mouse_position,
                       display->prev_mouse_position );

    get_screen_axes( &display->three_d.view, &hor, &vert );
    SCALE_VECTOR( hor, hor, Point_x(delta) );
    SCALE_VECTOR( vert, vert, Point_y(delta) );
    ADD_VECTORS( delta, hor, vert );
    make_translation_transform( &delta, &transform );
    transform_model( display, &transform );
    set_update_required( display, NORMAL_PLANES );
    display->prev_mouse_position = display->mouse_position;
}
#endif
