/**
 * \file markers.c
 * \brief Functions to create and implement the object list window.
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

#include <assert.h>

#include  <display.h>

static    DEF_EVENT_FUNCTION( left_mouse_press );
static    DEF_EVENT_FUNCTION( right_mouse_press );
static    DEF_EVENT_FUNCTION( middle_mouse_press );

/**
 * Select the marker that is closest to the given voxel position.
 * \param display The display_struct of the 3D view window.
 * \param volume_index The index of the active volume.
 * \param voxel The voxel position.
 * \returns TRUE if a marker was found that was within the 
 * Marker_pick_size of the voxel position.
 */
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

    assert(display->window_type == THREE_D_WINDOW);

    initialize_object_traverse( &object_traverse, FALSE, 1,
                                &display->models[THREED_MODEL] );

    volume = get_nth_volume( display, volume_index );

    convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
    fill_Point( voxel_pos, x_w, y_w, z_w );

    found = FALSE;
    closest_dist = 0.0;

    while( get_next_object_traverse( &object_traverse, &object ) )
    {
        marker_struct *marker_ptr;
        if( object->object_type == MARKER &&
            (marker_ptr = get_marker_ptr( object )) != NULL &&
            points_within_distance( &voxel_pos,
                                    &marker_ptr->position,
                                    Marker_pick_size ) )
        {
            dist = distance_between_points( &voxel_pos,
                                            &marker_ptr->position );

            if( !found || dist < closest_dist )
            {
                found = TRUE;
                closest_dist = dist;
                closest_marker = object;
            }
        }
    }

    if( found && (!get_current_object( display, &object ) ||
                  object != closest_marker) )
    {
        set_current_object( display, closest_marker );
    }

    return( found );
}

static  void  initialize_marker_parameters(
    display_struct    *marker_window )
{
    int                  x_size, y_size;
    marker_window_struct *marker;

    marker = &marker_window->marker;

    G_get_window_size( marker_window->window, &x_size, &y_size );

    marker->selected_x_offset = Selected_box_x_offset;
    marker->selected_y_offset = Selected_box_y_offset;
    marker->font_size = Object_window_font_size;
    marker->selected_x_origin = Selected_x_origin;
    marker->selected_y_origin = y_size - (marker->font_size * 2.0);
}

static  DEF_EVENT_FUNCTION( handle_marker_resize )
{
    display_struct *three_d;

    three_d = get_three_d_window(display);
    initialize_marker_parameters( display );
    rebuild_cursor_position_model( three_d );
    rebuild_selected_list( three_d, display );

    return( VIO_OK );
}


/**
 * Initialize the object window and its data structures.
 * \param marker_window The display_struct for the object (marker) window.
 * \returns VIO_OK if all goes well.
 */
VIO_Status  initialize_marker_window(
    display_struct    *marker_window)
{
    VIO_Status           status;
    marker_window_struct *marker;

    status = VIO_OK;
    marker = &marker_window->marker;

    G_set_transparency_state( marker_window->window, FALSE );

    initialize_resize_events( marker_window );
    add_action_table_function( &marker_window->action_table, 
                               WINDOW_RESIZE_EVENT, handle_marker_resize );

    marker->default_x_size = Canonical_marker_window_width;
    marker->default_y_size = Canonical_marker_window_height;

    initialize_marker_parameters( marker_window );
    
    add_action_table_function( &marker_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT, left_mouse_press );

    add_action_table_function( &marker_window->action_table,
                               RIGHT_MOUSE_DOWN_EVENT, right_mouse_press ); 

    add_action_table_function( &marker_window->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT, middle_mouse_press );

    return( status );
}

static DEF_EVENT_FUNCTION( left_mouse_press )
{
  VIO_Status status = VIO_OK;
  int        x, y;

  if( G_get_mouse_position( display->window, &x, &y ) )
  {
    display_struct      *three_d;
    object_struct       *object, *current;

    three_d = get_three_d_window( display );

    if( mouse_is_on_object_name( three_d, x, y, &object ) )
    {
      if ( get_current_object( three_d, &current ) && current != object )
      {
        set_current_object( three_d, object );
      }

      if (is_shift_key_pressed())
      {
        set_current_object_colour( three_d, NULL, NULL );
      }
      rebuild_selected_list( three_d, display );
      update_all_menu_text( three_d );
    }
  }
  return( status );
}

static DEF_EVENT_FUNCTION( middle_mouse_press )
{
  VIO_Status status = VIO_OK;
  int        x, y;

  if( G_get_mouse_position( display->window, &x, &y ) )
  {
    display_struct      *three_d = get_three_d_window( display );
    object_struct       *object, *current;

    if( mouse_is_on_object_name( three_d, x, y, &object ) )
    {
      if ( get_current_object( three_d, &current ) && current != object )
      {
        set_current_object( three_d, object );
      }

      if (is_shift_key_pressed())
      {
        toggle_render_mode( three_d, NULL, NULL );
      }
      else if (is_ctrl_key_pressed())
      {
      }
      else
      {
        set_object_visibility( object, !get_object_visibility( object ));
      }
      graphics_models_have_changed( three_d );
      rebuild_selected_list( three_d, display );
      update_all_menu_text( three_d );
    }
  }
  return( status );
}

static  DEF_EVENT_FUNCTION( right_mouse_press )
{
  VIO_Status status = VIO_OK;
  int        x, y;

  if( G_get_mouse_position( display->window, &x, &y ) )
  {
    display_struct      *three_d = get_three_d_window( display );
    object_struct       *object, *current;

    if( mouse_is_on_object_name( three_d, x, y, &object ) )
    {
      if ( get_current_object( three_d, &current ) && current != object )
      {
        set_current_object( three_d, object );
      }
      if ( current->object_type == MARKER )
      {
        set_cursor_to_marker( three_d, NULL, NULL );
      }
      rebuild_selected_list( three_d, display );
      update_all_menu_text( three_d );
    }
  }
  return( status );
}

