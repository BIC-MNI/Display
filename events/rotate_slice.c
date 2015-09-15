/**
 * \file rotate_slice.c
 *
 * \brief Support rotation of the oblique slice in the 3D window.
 *
 * \copyright
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
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

static    DEF_EVENT_FUNCTION( start_rotating_slice );
static    DEF_EVENT_FUNCTION( handle_update_rotation );
static    DEF_EVENT_FUNCTION( terminate_rotation );
static    DEF_EVENT_FUNCTION( terminate_rotating_slice );

static  VIO_BOOL  perform_rotation(
    display_struct   *display );

void  initialize_rotating_slice(
    display_struct   *display )
{
    if( get_n_volumes(display) == 0 )
        return;

    set_volume_cross_section_visibility( display, TRUE );

    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_rotating_slice );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_rotating_slice );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( terminate_rotating_slice )
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_rotating_slice );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_rotating_slice );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( start_rotating_slice )
{
    add_action_table_function( &display->action_table,
                               NO_EVENT, handle_update_rotation );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT, terminate_rotation );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_rotation );

    record_mouse_position( display );

    return( VIO_OK );
}

static  void  update_rotation(
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

static  DEF_EVENT_FUNCTION( handle_update_rotation )
{
    update_rotation( display );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( terminate_rotation )
{
    update_rotation( display );

    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_rotation );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT, terminate_rotation );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_rotation );

    return( VIO_OK );
}

static  void  transform_slice_axes(
    display_struct   *slice_window,
    VIO_Transform        *transform )
{
    VIO_Volume  volume;
    VIO_Real    len;
    VIO_Real    separations[VIO_MAX_DIMENSIONS];
    VIO_Real    origin[VIO_MAX_DIMENSIONS];
    VIO_Real    x_axis[VIO_MAX_DIMENSIONS], y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real    world_x_axis[VIO_MAX_DIMENSIONS], world_y_axis[VIO_MAX_DIMENSIONS];

    volume = get_volume( slice_window );

    get_slice_plane( slice_window,get_current_volume_index( slice_window ),
                     get_arbitrary_view_index(slice_window),
                     origin, x_axis, y_axis );

    get_volume_separations( volume, separations );

    convert_voxel_vector_to_world( volume, x_axis,
                        &world_x_axis[VIO_X], &world_x_axis[VIO_Y], &world_x_axis[VIO_Z] );

    convert_voxel_vector_to_world( volume, y_axis,
                        &world_y_axis[VIO_X], &world_y_axis[VIO_Y], &world_y_axis[VIO_Z] );

    transform_vector( transform,
                      world_x_axis[VIO_X], world_x_axis[VIO_Y], world_x_axis[VIO_Z],
                      &world_x_axis[VIO_X], &world_x_axis[VIO_Y], &world_x_axis[VIO_Z] );
    transform_vector( transform,
                      world_y_axis[VIO_X], world_y_axis[VIO_Y], world_y_axis[VIO_Z],
                      &world_y_axis[VIO_X], &world_y_axis[VIO_Y], &world_y_axis[VIO_Z] );

    convert_world_vector_to_voxel( volume,
                      world_x_axis[VIO_X], world_x_axis[VIO_Y], world_x_axis[VIO_Z],
                      x_axis );

    convert_world_vector_to_voxel( volume,
                      world_y_axis[VIO_X], world_y_axis[VIO_Y], world_y_axis[VIO_Z],
                      y_axis );

    len = sqrt(x_axis[VIO_X]*x_axis[VIO_X] + x_axis[VIO_Y]*x_axis[VIO_Y] + x_axis[VIO_Z]*x_axis[VIO_Z]);
    if( len > 0.0 )
    {
        x_axis[VIO_X] /= len;
        x_axis[VIO_Y] /= len;
        x_axis[VIO_Z] /= len;
    }

    len = sqrt(y_axis[VIO_X]*y_axis[VIO_X] + y_axis[VIO_Y]*y_axis[VIO_Y] + y_axis[VIO_Z]*y_axis[VIO_Z]);
    if( len > 0.0 )
    {
        y_axis[VIO_X] /= len;
        y_axis[VIO_Y] /= len;
        y_axis[VIO_Z] /= len;
    }

    set_slice_plane( slice_window,get_current_volume_index( slice_window ),
                     get_arbitrary_view_index(slice_window),
                     x_axis, y_axis );
}

static  VIO_BOOL  perform_rotation(
    display_struct   *display )
{
    display_struct  *slice_window;
    VIO_Real            x, y;
    VIO_Transform       transform, inverse, transform_in_space;
    VIO_BOOL         moved;

    moved = FALSE;

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) &&
        get_spaceball_transform( display,
                                 (VIO_Real) Point_x(display->prev_mouse_position),
                                 (VIO_Real) Point_y(display->prev_mouse_position),
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
