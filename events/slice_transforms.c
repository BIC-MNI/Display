
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  start_translating_slice( graphics )
    graphics_struct  *graphics;
{
}

public  void  start_rotating_slice( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_rotating );
    DECL_EVENT_FUNCTION( end_rotating );
    void                 add_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_rotating );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               end_rotating );
}

private  DEF_EVENT_FUNCTION( end_rotating )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION( start_rotating );
    void                 remove_action_table_function();

    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_rotating );

    remove_action_table_function( &graphics->action_table, TERMINATE_EVENT,
                                  end_rotating );
}

private  DEF_EVENT_FUNCTION( start_rotating )
    /* ARGSUSED */
{
    void                  add_action_table_function();
    void                  push_action_table();
    DECL_EVENT_FUNCTION(  handle_update_rotation );
    DECL_EVENT_FUNCTION(  terminate_rotation );

    graphics->prev_mouse_position = graphics->mouse_position;

    push_action_table( &graphics->action_table, NO_EVENT );
    push_action_table( &graphics->action_table, MIDDLE_MOUSE_UP_EVENT );
    push_action_table( &graphics->action_table, TERMINATE_EVENT );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT, handle_update_rotation );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_rotation );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT, terminate_rotation );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_rotation )
    /* ARGSUSED */
{
    void   pop_action_table();
    void   update_rotation();

    update_rotation( graphics );
    
    pop_action_table( &graphics->action_table, NO_EVENT );
    pop_action_table( &graphics->action_table, MIDDLE_MOUSE_UP_EVENT );
    pop_action_table( &graphics->action_table, TERMINATE_EVENT );

    return( OK );
}

private  void  modify_slice_transform( graphics, transform )
    graphics_struct  *graphics;
    Transform        *transform;
{
    volume_struct  *volume;
    int            nx, ny, nz;
    Point          centre_of_rotation;
    void           set_update_required();
    void           make_transform_relative_to_point();
    void           concat_transforms();

    if( get_current_volume( graphics, &volume ) )
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

        set_update_required( graphics, NORMAL_PLANES );
    }
}

private  void  update_rotation( graphics )
    graphics_struct  *graphics;
{
    Transform      transform;
    Boolean        compute_rotation();
    void           modify_slice_transform();

    if( compute_rotation( graphics, &transform ) )
    {
        modify_slice_transform( graphics, &transform );
    }
}

private  DEF_EVENT_FUNCTION( handle_update_rotation )      /* ARGSUSED */
{
    void      update_rotation();

    update_rotation( graphics );

    return( OK );
}

private  Boolean  compute_rotation( graphics, transform )
    graphics_struct  *graphics;
    Transform        *transform;
{
    static  Point  centre = { 0.5, 0.5, 0.0 };
    Real           x1, x2, y1, y2, curr_angle, prev_angle;
    Real           compute_clockwise_rotation();
    Boolean        moved;
    void           make_rotation_about_axis();

    moved = FALSE;

    x1 = Point_x(graphics->prev_mouse_position);
    y1 = Point_y(graphics->prev_mouse_position);
    x2 = Point_x(graphics->mouse_position);
    y2 = Point_y(graphics->mouse_position);

    prev_angle = compute_clockwise_rotation( x1 - Point_x(centre),
                                             y1 - Point_y(centre) );

    curr_angle = compute_clockwise_rotation( x2 - Point_x(centre),
                                             y2 - Point_y(centre) );

    if( prev_angle != curr_angle )
    {
        make_rotation_about_axis( Z_AXIS, curr_angle - prev_angle, transform );
        moved = TRUE;
    }
    else
        moved = FALSE;

    graphics->prev_mouse_position = graphics->mouse_position;

    return( moved );
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
