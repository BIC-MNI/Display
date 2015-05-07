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

static    DEF_EVENT_FUNCTION( left_mouse_press );
static    DEF_EVENT_FUNCTION( middle_mouse_press );

static  VIO_Status  handle_mouse_press_in_marker(
    display_struct      *marker_window,
    VIO_Real                x,
    VIO_Real                y );


  VIO_BOOL  update_current_marker(
    display_struct   *display,
    int              volume_index,
    VIO_Real         voxel[] )
{
    object_traverse_struct  object_traverse;
    VIO_BOOL                found;
    object_struct           *object, *closest_marker;
    VIO_Volume              volume;
    VIO_Point               voxel_pos;
    VIO_Real                x_w, y_w, z_w;
    VIO_Real                dist, closest_dist;

    initialize_object_traverse( &object_traverse, FALSE, 1,
                                &display->models[THREED_MODEL] );

    volume = get_nth_volume( display, volume_index );

    convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
    fill_Point( voxel_pos, x_w, y_w, z_w );

    found = FALSE;
    closest_dist = 0.0;

    while( get_next_object_traverse( &object_traverse, &object ) )
    {
        if( object->object_type == MARKER &&
            points_within_distance( &voxel_pos,
                                    &get_marker_ptr(object)->position,
                                    Marker_pick_size ) )
        {
            dist = distance_between_points( &voxel_pos,
                                            &get_marker_ptr(object)->position );

            if( !found || dist < closest_dist )
            {
                found = TRUE;
                closest_dist = dist;
                closest_marker = object;
            }
        }
    }

    if( found && (!get_current_object(display,&object) ||
                  object != closest_marker) )
    {
        set_current_object( display, closest_marker );
    }

    return( found );
}

static  void  initialize_marker_parameters(
    display_struct    *marker_window )
{
    int                 x_size, y_size;
    VIO_Real            x_scale, y_scale, scale;
    marker_window_struct  *marker;

    marker = &marker_window->marker;

    G_get_window_size( marker_window->window, &x_size, &y_size );

    x_scale = (VIO_Real) x_size / (VIO_Real) marker->default_x_size;
    y_scale = (VIO_Real) y_size / (VIO_Real) marker->default_y_size;

    scale = MIN( x_scale, y_scale );

    marker->character_width = scale * Menu_character_width;
    marker->character_height = scale * Menu_character_height;
    marker->character_offset = scale * Menu_key_character_offset;
    marker->selected_x_origin = scale * Selected_x_origin;
    marker->selected_y_origin = scale * Selected_y_origin;
    marker->selected_x_offset = scale * Selected_box_x_offset;
    marker->selected_y_offset = scale * Selected_box_y_offset;
    marker->selected_box_height = scale * Character_height_in_pixels;
    marker->font_size = scale * Menu_window_font_size;
}

static  DEF_EVENT_FUNCTION( handle_marker_resize )
{
    display_struct  *marker_window, *three_d;

    three_d = get_three_d_window(display);
    marker_window = three_d->associated[MARKER_WINDOW];

    initialize_marker_parameters( marker_window );
    rebuild_cursor_position_model( three_d );
    rebuild_selected_list( three_d, marker_window );

    return( VIO_OK );
}


  VIO_Status  initialize_marker_window(
    display_struct    *marker_window)
{
    VIO_Status           status;
    marker_window_struct *marker;
    VIO_Point            position;
    model_struct         *model;
    int                  ch, i, dir, len;
    VIO_BOOL             found;

    status = VIO_OK;
    marker = &marker_window->marker;

    G_set_transparency_state( marker_window->window, FALSE );

    initialize_resize_events( marker_window );
    add_action_table_function( &marker_window->action_table, WINDOW_RESIZE_EVENT,
                               handle_marker_resize );

    marker->default_x_size = Canonical_marker_window_width;
    marker->default_y_size = Canonical_marker_window_height;

    initialize_marker_parameters( marker_window );
    
    add_action_table_function( &marker_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT, left_mouse_press );
    add_action_table_function( &marker_window->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT, middle_mouse_press );
    return( status );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_character_down )
{
  return VIO_OK;
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_leaving_window )
{
    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_character_up )
{
    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( left_mouse_press )
{
    VIO_Status  status;
    int     x, y;

    status = VIO_OK;

    if( G_get_mouse_position( display->window, &x, &y ) )
    {
        status = handle_mouse_press_in_marker( display, (VIO_Real) x, (VIO_Real) y );
    }

    return( status );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( middle_mouse_press )
{
    pop_menu_one_level( display );

    return( VIO_OK );
}

static  VIO_Status  handle_mouse_press_in_marker(
    display_struct      *marker_window,
    VIO_Real                x,
    VIO_Real                y )
{
    display_struct      *three_d;
    VIO_Status          status;
    int                 key;
    object_struct       *object, *current;

    status = VIO_OK;

    three_d = get_three_d_window( marker_window );

    if( mouse_is_on_object_name( three_d, VIO_ROUND(x), VIO_ROUND(y), &object ) )
    {
        if( get_current_object( three_d, &current ) &&
            current == object && get_object_type(object) == MODEL )
        {
            push_current_object( three_d );
        }
        else
            set_current_object( three_d, object );

        rebuild_selected_list( three_d, three_d->associated[MARKER_WINDOW]);
        update_all_menu_text( three_d );
    }
    return( status );
}

