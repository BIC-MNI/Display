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

static    DEF_EVENT_FUNCTION( update_picked_object );
static    DEF_EVENT_FUNCTION( terminate_picking_object );
static    DEF_EVENT_FUNCTION( start_picking_object );
private  void  pick_point_under_mouse(
    display_struct    *display );

public  void  initialize_picking_object(
    display_struct    *display )
{
    add_action_table_function( &display->action_table,
                               LEFT_MOUSE_DOWN_EVENT, start_picking_object );
                               
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_picking_object )
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

    return( OK );
}

private  void  remove_events(
    action_table_struct  *action_table )
{
    pop_action_table( action_table, LEFT_MOUSE_UP_EVENT );
    pop_action_table( action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( action_table, NO_EVENT,
                                  update_picked_object );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_picking_object )
{
    remove_events( &display->action_table );

    pick_point_under_mouse( display );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( update_picked_object )
{
    pick_point_under_mouse( display );

    return( OK );
}

public  BOOLEAN  get_mouse_scene_intersection(
    display_struct    *display,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *object_index,
    Point             *intersection )
{
    BOOLEAN          found;
    Real             x, y;
    Point            origin, transformed_origin;
    Vector           direction, transformed_direction;

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

public  BOOLEAN  get_polygon_under_mouse(
    display_struct    *display,
    polygons_struct   **polygons,
    int               *poly_index,
    Point             *intersection )
{
    BOOLEAN          found;
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

private  void  pick_point_under_mouse(
    display_struct    *display )
{
    Point            intersection_point;
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
                                   display->associated[MENU_WINDOW] );
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
