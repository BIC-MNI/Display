/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

static    DEF_EVENT_FUNCTION( start_rotating_slice );
static    DEF_EVENT_FUNCTION( turn_off_rotating_slice );
static    DEF_EVENT_FUNCTION( handle_update_rotation );
static    DEF_EVENT_FUNCTION( terminate_rotation );
static    DEF_EVENT_FUNCTION( terminate_rotating_slice );

private  VIO_BOOL  perform_rotation(
    display_struct   *display );

public  void  initialize_rotating_slice(
    display_struct   *display )
{
    if( get_n_volumes(display) == 0 )
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

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_rotating_slice )
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_rotating_slice );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_rotating_slice );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( turn_off_rotating_slice )
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_rotating_slice );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_rotating_slice );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_rotating_slice )
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
        reset_slice_view( slice_window,
                          get_arbitrary_view_index(slice_window) );
    }
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_update_rotation )
{
    update_rotation( display );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_rotation )
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
    Real    len;
    Real    separations[MAX_DIMENSIONS];
    Real    origin[MAX_DIMENSIONS];
    Real    x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    Real    world_x_axis[MAX_DIMENSIONS], world_y_axis[MAX_DIMENSIONS];

    volume = get_volume( slice_window );

    get_slice_plane( slice_window,get_current_volume_index( slice_window ),
                     get_arbitrary_view_index(slice_window),
                     origin, x_axis, y_axis );

    get_volume_separations( volume, separations );

    convert_voxel_vector_to_world( volume, x_axis,
                        &world_x_axis[X], &world_x_axis[Y], &world_x_axis[Z] );

    convert_voxel_vector_to_world( volume, y_axis,
                        &world_y_axis[X], &world_y_axis[Y], &world_y_axis[Z] );

    transform_vector( transform,
                      world_x_axis[X], world_x_axis[Y], world_x_axis[Z],
                      &world_x_axis[X], &world_x_axis[Y], &world_x_axis[Z] );
    transform_vector( transform,
                      world_y_axis[X], world_y_axis[Y], world_y_axis[Z],
                      &world_y_axis[X], &world_y_axis[Y], &world_y_axis[Z] );

    convert_world_vector_to_voxel( volume,
                      world_x_axis[X], world_x_axis[Y], world_x_axis[Z],
                      x_axis );

    convert_world_vector_to_voxel( volume,
                      world_y_axis[X], world_y_axis[Y], world_y_axis[Z],
                      y_axis );

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

    set_slice_plane( slice_window,get_current_volume_index( slice_window ),
                     get_arbitrary_view_index(slice_window),
                     x_axis, y_axis );
}

private  VIO_BOOL  perform_rotation(
    display_struct   *display )
{
    display_struct  *slice_window;
    Real            x, y;
    Transform       transform, inverse, transform_in_space;
    VIO_BOOL         moved;

    moved = FALSE;

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) &&
        get_spaceball_transform( display,
                                 (Real) Point_x(display->prev_mouse_position),
                                 (Real) Point_y(display->prev_mouse_position),
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
