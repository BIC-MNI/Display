
#include  <display.h>

static    DEF_EVENT_FUNCTION( start_rotating_slice );
static    DEF_EVENT_FUNCTION( turn_off_rotating_slice );
static    DEF_EVENT_FUNCTION( handle_update_rotation );
static    DEF_EVENT_FUNCTION( terminate_rotation );
static    DEF_EVENT_FUNCTION( terminate_rotating_slice );

private  BOOLEAN  perform_rotation(
    display_struct   *display );

public  void  initialize_rotating_slice(
    display_struct   *display )
{
    display_struct  *slice_window;

    if( !get_slice_window( display, &slice_window ) )
        return;

    set_volume_cross_section_visibility( display, ON );

    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_rotating_slice );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_rotating_slice );
}

private  DEF_EVENT_FUNCTION( terminate_rotating_slice )    /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_rotating_slice );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_rotating_slice );
}

private  DEF_EVENT_FUNCTION( turn_off_rotating_slice )    /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_rotating_slice );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_rotating_slice );
}

private  DEF_EVENT_FUNCTION( start_rotating_slice )     /* ARGSUSED */
{
    add_action_table_function( &display->action_table,
                               NO_EVENT, handle_update_rotation );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT, terminate_rotation );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_rotation );

    record_mouse_position( display );

    return( OK );
}

private  void  update_rotation(
    display_struct   *display )
{
    display_struct   *slice_window;

    if( perform_rotation( display ) &&
        get_slice_window( display, &slice_window ) )
    {
        reset_slice_view( slice_window, OBLIQUE_VIEW_INDEX );
        set_slice_window_update( slice_window, OBLIQUE_VIEW_INDEX );
    }
}

private  DEF_EVENT_FUNCTION( handle_update_rotation )      /* ARGSUSED */
{
    update_rotation( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_rotation )     /* ARGSUSED */
{
    update_rotation( display );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_rotation );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT, terminate_rotation );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_rotation );

    return( OK );
}

private  void  transform_slice_axes(
    display_struct   *slice_window,
    Transform        *transform )
{
    Volume  volume;
    Real    x0, y0, z0;
    Real    len;
    Real    separations[MAX_DIMENSIONS];
    Real    origin[MAX_DIMENSIONS];
    Real    x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    Real    world_x_axis[MAX_DIMENSIONS], world_y_axis[MAX_DIMENSIONS];

    volume = get_volume( slice_window );

    get_slice_plane( slice_window, OBLIQUE_VIEW_INDEX, origin, x_axis, y_axis );

    get_volume_separations( volume, separations );

    x_axis[X] /= ABS( separations[X] );
    x_axis[Y] /= ABS( separations[Y] );
    x_axis[Z] /= ABS( separations[Z] );
    y_axis[X] /= ABS( separations[X] );
    y_axis[Y] /= ABS( separations[Y] );
    y_axis[Z] /= ABS( separations[Z] );

    origin[X] = 0.0;
    origin[Y] = 0.0;
    origin[Z] = 0.0;
    convert_voxel_to_world( volume, origin, &x0, &y0, &z0 );

    convert_voxel_to_world( volume, x_axis,
                        &world_x_axis[X], &world_x_axis[Y], &world_x_axis[Z] );
    convert_voxel_to_world( volume, y_axis,
                        &world_y_axis[X], &world_y_axis[Y], &world_y_axis[Z] );

    world_x_axis[X] -= x0;
    world_x_axis[Y] -= y0;
    world_x_axis[Z] -= z0;

    world_y_axis[X] -= x0;
    world_y_axis[Y] -= y0;
    world_y_axis[Z] -= z0;

    transform_vector( transform,
                      world_x_axis[X], world_x_axis[Y], world_x_axis[Z],
                      &world_x_axis[X], &world_x_axis[Y], &world_x_axis[Z] );
    transform_vector( transform,
                      world_y_axis[X], world_y_axis[Y], world_y_axis[Z],
                      &world_y_axis[X], &world_y_axis[Y], &world_y_axis[Z] );

    convert_world_to_voxel( volume,
                            world_x_axis[X], world_x_axis[Y], world_x_axis[Z],
                            x_axis );
    convert_world_to_voxel( volume,
                            world_y_axis[X], world_y_axis[Y], world_y_axis[Z],
                            y_axis );
    convert_world_to_voxel( volume, 0.0, 0.0, 0.0, origin );

    x_axis[X] -= origin[X];
    x_axis[Y] -= origin[Y];
    x_axis[Z] -= origin[Z];
    y_axis[X] -= origin[X];
    y_axis[Y] -= origin[Y];
    y_axis[Z] -= origin[Z];

    x_axis[X] *= ABS( separations[X] );
    x_axis[Y] *= ABS( separations[Y] );
    x_axis[Z] *= ABS( separations[Z] );
    y_axis[X] *= ABS( separations[X] );
    y_axis[Y] *= ABS( separations[Y] );
    y_axis[Z] *= ABS( separations[Z] );

    len = sqrt(x_axis[X]*x_axis[X] + x_axis[Y]*x_axis[Y] + x_axis[Z]*x_axis[Z]);
    if( len > 0.0 )
    {
        x_axis[X] /= len;
        x_axis[Y] /= len;
        x_axis[Z] /= len;
    }

    len = sqrt(y_axis[X]*y_axis[X] + y_axis[Y]*y_axis[Y] + y_axis[Z]*y_axis[Z]);
    if( len > 0.0 )
    {
        y_axis[X] /= len;
        y_axis[Y] /= len;
        y_axis[Z] /= len;
    }

    set_slice_plane( slice_window, OBLIQUE_VIEW_INDEX, x_axis, y_axis );
}

private  BOOLEAN  perform_rotation(
    display_struct   *display )
{
    display_struct  *slice_window;
    Real            x, y;
    Transform       transform, inverse, transform_in_space;
    BOOLEAN         moved;

    moved = FALSE;

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) &&
        get_spaceball_transform( display,
                                 Point_x(display->prev_mouse_position),
                                 Point_y(display->prev_mouse_position),
                                 x, y, &transform ) &&
        get_slice_window( display, &slice_window ) )
    {
        (void) compute_transform_inverse(
                      &display->three_d.view.modeling_transform,
                      &inverse );

        concat_transforms( &transform_in_space,
                           &display->three_d.view.modeling_transform,
                           &transform );
        concat_transforms( &transform_in_space,
                           &transform_in_space, &inverse );

        transform_slice_axes( slice_window, &transform_in_space );
        record_mouse_position( display );
        moved = TRUE;
    }

    return( moved );
}
