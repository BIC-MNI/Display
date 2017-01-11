/**
 * \file rotate_slice.c
 *
 * \brief Support rotation of the oblique slice orientation in the 3D window.
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

static DEF_EVENT_FUNCTION( start_rotating_slice );
static DEF_EVENT_FUNCTION( update_rotating_slice );
static DEF_EVENT_FUNCTION( stop_rotating_slice );
static DEF_EVENT_FUNCTION( terminate_rotating_slice );

static void perform_rotation( display_struct *display );

/**
 * Tell the program that the next middle mouse down event should
 * trigger the rotation of the oblique slice view.
 *
 * This is the only public entry point for this file.
 *
 * \param display A pointer to the 3D view window.
 */
void
initialize_rotating_slice( display_struct *display )
{
    if( get_n_volumes(display) == 0 )
        return;

    set_volume_cross_section_visibility( display, TRUE );

    terminate_any_interactions( display );

    push_action_table( &display->action_table, MIDDLE_MOUSE_DOWN_EVENT );
    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_rotating_slice );

    push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );
    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_rotating_slice );
}


/**
 * Puts an end to the entire oblique slice rotation process.
 *
 * \param display A pointer to a top-level window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 * \returns VIO_OK if event was processed.
 */
static DEF_EVENT_FUNCTION( terminate_rotating_slice )
{
    pop_action_table( &display->action_table, MIDDLE_MOUSE_DOWN_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );
    return( VIO_OK );
}


/**
 * \brief Start rotating the oblique slice plane.
 *
 * Generally called when the middle mouse button is pressed.
 *
 * \param display A pointer to a top-level window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 * \returns VIO_OK if event was processed.
 */
static  DEF_EVENT_FUNCTION( start_rotating_slice )
{
    push_action_table( &display->action_table, NO_EVENT );
    add_action_table_function( &display->action_table,
                               NO_EVENT, update_rotating_slice );

    push_action_table(&display->action_table, MIDDLE_MOUSE_UP_EVENT );
    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT, stop_rotating_slice );

    push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );
    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               stop_rotating_slice );
    record_mouse_position( display );
    return( VIO_OK );
}

/**
 * \brief Update the rotation of the arbitrary view plane.
 *
 * Called in response to NO_EVENT messages (e.g. mouse movement) to
 * update the arbitrary view plane orientation according to the change
 * in the mouse position.
 *
 * \param display A pointer to a top-level window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 * \returns VIO_OK if event was processed.
 */
static  DEF_EVENT_FUNCTION( update_rotating_slice )
{
    perform_rotation( display );
    return( VIO_OK );
}

/**
 * \brief Stop rotating the arbitrary view plane.
 *
 * Generally called in response to a middle mouse up event.
 *
 * \param display A pointer to a top-level window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 * \returns VIO_OK if event was processed.
 */
static  DEF_EVENT_FUNCTION( stop_rotating_slice )
{
    perform_rotation( display );
    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, MIDDLE_MOUSE_UP_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );
    terminate_any_interactions( display );
    return( VIO_OK );
}

/**
 * Apply the given transform to the axes of the arbitrary (oblique) view plane.
 * \param slice_window A pointer to the slice view window.
 * \param transform A pointer to the transform to apply to the axes.
 */
static void
transform_slice_axes( display_struct *slice_window, VIO_Transform *transform )
{
    VIO_Volume  volume;
    VIO_Real    origin[VIO_MAX_DIMENSIONS];
    VIO_Real    x_axis[VIO_MAX_DIMENSIONS];
    VIO_Real    y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real    wx_x, wx_y, wx_z;
    VIO_Real    wy_x, wy_y, wy_z;
    int         volume_index;
    int         view_index;

    volume_index = get_current_volume_index( slice_window );
    view_index   = get_arbitrary_view_index( slice_window );

    volume = get_volume( slice_window );

    get_slice_plane( slice_window, volume_index, view_index,
                     origin, x_axis, y_axis );

    convert_voxel_vector_to_world( volume, x_axis, &wx_x, &wx_y, &wx_z );
    convert_voxel_vector_to_world( volume, y_axis, &wy_x, &wy_y, &wy_z );

    transform_vector( transform, wx_x, wx_y, wx_z, &wx_x, &wx_y, &wx_z );
    transform_vector( transform, wy_x, wy_y, wy_z, &wy_x, &wy_y, &wy_z );

    convert_world_vector_to_voxel( volume, wx_x, wx_y, wx_z, x_axis );
    convert_world_vector_to_voxel( volume, wy_x, wy_y, wy_z, y_axis );

    array_normalize( x_axis, VIO_N_DIMENSIONS );
    array_normalize( y_axis, VIO_N_DIMENSIONS );

    set_slice_plane( slice_window, volume_index, view_index, x_axis, y_axis );
}

/**
 * Translate mouse movements in the 3D view window into rotations of the
 * arbitrary view plane.
 * \param display A pointer to the 3D view window.
 */
static void
perform_rotation( display_struct *display )
{
    VIO_Real       x, y;        /* Mouse coordinates */
    VIO_Transform  transform, inverse, transform_in_space;
    display_struct *slice_window;

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) &&
        get_spaceball_transform( display,
                                 Point_x( display->prev_mouse_position ),
                                 Point_y( display->prev_mouse_position ),
                                 x, y, &transform ) &&
        get_slice_window( display, &slice_window) )
    {
        compute_transform_inverse( &display->three_d.view.modeling_transform,
                                   &inverse );
        concat_transforms( &transform_in_space,
                           &display->three_d.view.modeling_transform,
                           &transform );
        concat_transforms( &transform_in_space,
                           &transform_in_space, &inverse );
        transform_slice_axes( slice_window, &transform_in_space );
        record_mouse_position( display );
        reset_slice_view( slice_window,
                          get_arbitrary_view_index( slice_window ) );
    }
}
