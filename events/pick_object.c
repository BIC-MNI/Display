/**
 * \file pick_object.c
 * \brief Handling picking (selecting) objects based on clicks in the 
 * 3D window.
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

static    DEF_EVENT_FUNCTION( update_picked_object );
static    DEF_EVENT_FUNCTION( terminate_picking_object );
static    DEF_EVENT_FUNCTION( start_picking_object );
static  void  pick_point_under_mouse(
    display_struct    *display );

/**
 * Configures the left mouse button to set the current object in the
 * object list.
 */
void  initialize_picking_object(
    display_struct    *display )
{
    add_action_table_function( &display->action_table,
                               LEFT_MOUSE_DOWN_EVENT, start_picking_object );
                               
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( start_picking_object )
{
    push_action_table( &display->action_table, LEFT_MOUSE_UP_EVENT );

    push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               update_picked_object );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_picking_object );

    add_action_table_function( &display->action_table,
                               LEFT_MOUSE_UP_EVENT,
                               terminate_picking_object );

    return( VIO_OK );
}

static  void  remove_events(
    action_table_struct  *action_table )
{
    pop_action_table( action_table, LEFT_MOUSE_UP_EVENT );
    pop_action_table( action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( action_table, NO_EVENT,
                                  update_picked_object );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( terminate_picking_object )
{
    remove_events( &display->action_table );

    pick_point_under_mouse( display );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( update_picked_object )
{
    pick_point_under_mouse( display );

    return( VIO_OK );
}

/**
 * Find the object that intersects with the current mouse point.
 * \param display The display_struct of the 3D window.
 * \param desired_object_type The type of object we are looking for, or -1
 * if any object should be considered.
 * \param object The object_struct of the object that was found.
 * \param object_index The index of the found object.
 * \param intersection The position of the intersection in model space.
 */
VIO_BOOL  get_mouse_scene_intersection(
    display_struct    *display,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *object_index,
    VIO_Point             *intersection )
{
    VIO_BOOL          found;
    VIO_Real             x, y;
    VIO_Point            origin, transformed_origin;
    VIO_Vector           direction, transformed_direction;

    found = FALSE;

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) )
    {
        convert_screen_to_ray( &display->three_d.view, x, y,
                               &origin, &direction );

        transform_world_to_model( &display->three_d.view, &origin,
                                  &transformed_origin );
        transform_world_to_model_vector( &display->three_d.view, &direction,
                                         &transformed_direction );

        found = intersect_ray_with_objects_hierarchy(
                      display, &transformed_origin,
                      &transformed_direction, desired_object_type,
                      object, object_index, intersection);
    }
    return( found );
}

/**
 * Find the object that intersects with the current cursor.
 * \param display The display_struct of the 3D window.
 * \param desired_object_type The type of object we are looking for, or -1
 * if any object should be considered.
 * \param object The object_struct of the object that was found.
 * \param object_index The index of the found object.
 * \param intersection The position of the intersection in model space.
 */
VIO_BOOL  get_cursor_scene_intersection(
    display_struct    *display,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *object_index,
    VIO_Point         *intersection )
{
    VIO_Point        origin, transformed_origin;
    VIO_Vector       direction, transformed_direction;

    get_cursor_origin(display, &origin);

    /* This is based on examining what is consistently returned by
     * convert_screen_to_ray().
     */
    Vector_x(direction) = 0;
    Vector_y(direction) = 0;
    Vector_z(direction) = -1;

    transform_world_to_model( &display->three_d.view, &origin,
                              &transformed_origin );
    transform_world_to_model_vector( &display->three_d.view, &direction,
                                     &transformed_direction );

    return intersect_ray_with_objects_hierarchy(display,
                                                &transformed_origin,
                                                &transformed_direction,
                                                desired_object_type,
                                                object,
                                                object_index,
                                                intersection);
}

/**
 * Get the polygon object under the mouse, if any.
 * \param display The display_struct of the 3D window.
 * \param polygons The polygons_struct of the found object.
 * \param poly_index The index of the polygons.
 * \param intersection The position of the intersection between the
 * mouse and the polygons.
 */
VIO_BOOL  get_polygon_under_mouse(
    display_struct    *display,
    polygons_struct   **polygons,
    int               *poly_index,
    VIO_Point             *intersection )
{
    VIO_BOOL          found;
    object_struct    *object;

    found = get_mouse_scene_intersection( display, POLYGONS,
                                          &object, poly_index,
                                          intersection );

    if( found && object->object_type == POLYGONS )
        *polygons = get_polygons_ptr( object );
    else
        found = FALSE;

    return( found );
}

/**
 * Identify the 3D object under the mouse cursor, and select it in the 
 * object list if found. Also updates the 3D and slice window cursors.
 * \param display The display_struct of the 3D window.
 */
static void
pick_point_under_mouse(display_struct    *display)
{
    VIO_Point            intersection_point;
    object_struct    *object;
    int              object_index;
    object_struct    *current;

    if( get_mouse_scene_intersection( display, (Object_types) -1, &object,
                                      &object_index, &intersection_point ) )
    {
        display->three_d.cursor.origin = intersection_point;
        update_cursor( display );

        if( !get_current_object( display, &current ) || current != object )
        {
            set_current_object( display, object );
            rebuild_selected_list( display,
                                   display->associated[MARKER_WINDOW] );
        }

        set_update_required( display, get_cursor_bitplanes() );

        if( update_voxel_from_cursor( display->associated[SLICE_WINDOW] ) )
        {
            set_update_required( display->associated[SLICE_WINDOW],
                                 NORMAL_PLANES );
        }

        update_all_menu_text( display );
    }
}
