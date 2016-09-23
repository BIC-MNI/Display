/**
 * \file label_window.c
 * \brief Functions to create and implement the label list window.
 *
 * \copyright
              Copyright 1993-2016 David MacDonald and Robert D. Vincent,
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
#include "checkbox.h"

static int cb_height;
static int cb_width;

static DEF_EVENT_FUNCTION( left_mouse_press );
static DEF_EVENT_FUNCTION( right_mouse_press );
static DEF_EVENT_FUNCTION( middle_mouse_press );
static DEF_EVENT_FUNCTION( scroll_down );
static DEF_EVENT_FUNCTION( scroll_up );

void rebuild_label_window(display_struct *slice_window);

static  void  initialize_label_parameters(
    display_struct    *label_window )
{
    int                  x_size, y_size;
    label_window_struct *label;

    label = &label_window->label;
    G_get_window_size( label_window->window, &x_size, &y_size );

    cb_width = checkbox.x_size / 2;
    cb_height = checkbox.y_size;

    label->selected_x_offset = Selected_box_x_offset;
    label->selected_y_offset = Selected_box_y_offset;
    label->font_size = 15;
    label->selected_x_origin = Selected_x_origin;
    label->selected_y_origin = y_size - (label->font_size * 2.0);
    label->top_index = 0;
    label->n_visible = VIO_ROUND((VIO_Real) label->selected_y_origin / (cb_height * 2.0));
}

static DEF_EVENT_FUNCTION( handle_label_resize )
{
    display_struct *label_window = get_display_by_type( LABEL_WINDOW );
    display_struct *slice_window = get_display_by_type( SLICE_WINDOW );
    model_struct *model_ptr = get_graphics_model( label_window, SELECTED_MODEL );
    initialize_label_parameters( label_window );

    delete_object_list( model_ptr->n_objects, model_ptr->objects );
    model_ptr->n_objects = 0;

    rebuild_label_window( slice_window );
    return( VIO_OK );
}

/**
 * Initialize the label window and its data structures.
 * \param label_window The display_struct for the object (label) window.
 * \returns VIO_OK if all goes well.
 */
VIO_Status
initialize_label_window(display_struct *label_window)
{
    VIO_Status           status;
    label_window_struct *label;

    status = VIO_OK;
    label = &label_window->label;

    G_set_transparency_state( label_window->window, FALSE );

    initialize_resize_events( label_window );
    add_action_table_function( &label_window->action_table,
                               WINDOW_RESIZE_EVENT, handle_label_resize );

    label->default_x_size = 250;
    label->default_y_size = 500;

    initialize_label_parameters( label_window );

    add_action_table_function( &label_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT, left_mouse_press );

    add_action_table_function( &label_window->action_table,
                               RIGHT_MOUSE_DOWN_EVENT, right_mouse_press );

    add_action_table_function( &label_window->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT, middle_mouse_press );

    add_action_table_function( &label_window->action_table,
                               SCROLL_DOWN_EVENT, scroll_down );

    add_action_table_function( &label_window->action_table,
                               SCROLL_UP_EVENT, scroll_up );

    return( status );
}

static DEF_EVENT_FUNCTION( left_mouse_press )
{
  int        x, y;
  display_struct *slice_window = get_display_by_type( SLICE_WINDOW );
  int volume_index = get_current_volume_index( slice_window );
  display_struct *label_window = get_display_by_type( LABEL_WINDOW );
  label_window_struct *label = &label_window->label;

  if( G_get_mouse_position( display->window, &x, &y ) )
  {
    int i = VIO_ROUND((label->selected_y_origin - y) / (cb_height * 2.0) );
    int i_label = i + 1 + label->top_index;

    if ( x >= label->selected_x_origin &&
         x < label->selected_x_origin + cb_width )
    {
      VIO_BOOL fvis = is_label_visible( slice_window, volume_index, i_label );
      set_label_visible( slice_window, volume_index, i_label, !fvis );
      rebuild_label_window( slice_window );
      colour_coding_has_changed( slice_window, volume_index, UPDATE_LABELS );
    }
    else
    {
      slice_window->slice.current_paint_label = i_label;
      print( "Paint label set to: %d\n", i_label );
      rebuild_label_window( slice_window );
    }
  }
  return( VIO_OK );
}

static DEF_EVENT_FUNCTION( middle_mouse_press )
{
  VIO_Status status = VIO_OK;
  int        x, y;

  if( G_get_mouse_position( display->window, &x, &y ) )
  {
  }
  return( status );
}

static  DEF_EVENT_FUNCTION( right_mouse_press )
{
  VIO_Status status = VIO_OK;
  int        x, y;

  if( G_get_mouse_position( display->window, &x, &y ) )
  {
  }
  return( status );
}

static DEF_EVENT_FUNCTION( scroll_down )
{
    display_struct *label_window = get_display_by_type( LABEL_WINDOW );
    display_struct *slice_window = get_display_by_type( SLICE_WINDOW );
    int volume_index = get_current_volume_index( slice_window );
    int n_labels = get_num_labels( slice_window, volume_index );
    label_window_struct *label = &label_window->label;
    if ( label->top_index + label->n_visible + 1 < n_labels )
      label_window->label.top_index++;
    rebuild_label_window( slice_window );
    return VIO_OK;
}

static DEF_EVENT_FUNCTION( scroll_up )
{
    display_struct *label_window = get_display_by_type( LABEL_WINDOW );
    display_struct *slice_window = get_display_by_type( SLICE_WINDOW );
    if ( label_window->label.top_index > 0 )
      label_window->label.top_index--;
    rebuild_label_window( slice_window );
    return VIO_OK;
}

/**
 * Given a colour, return whether to use a black or white text with it
 * such that the contrast is maximized.
 */
static VIO_Colour get_text_colour(VIO_Colour bkgd_colour)
{
    int r = get_Colour_r(bkgd_colour);
    int g = get_Colour_g(bkgd_colour);
    int b = get_Colour_b(bkgd_colour);
    int yiq = ((r * 299) + (g * 587) + (b * 114)) / 1000;
    return (yiq >= 128) ? BLACK : WHITE;
}

void
create_label_objects( display_struct *slice_window, int n_visible,
                      model_struct *model_ptr )
{
  int i;
  object_struct *object_ptr;
  text_struct *text_ptr;
  pixels_struct *pixels_ptr;
  quadmesh_struct *quadmesh_ptr;
  VIO_Point origin;
  VIO_Surfprop  spr = { 1, 0, 0, 0, 1 };
  int volume_index = get_current_volume_index( slice_window );
  lines_struct *lines_ptr;

  object_ptr = create_object( LINES );
  lines_ptr = get_lines_ptr( object_ptr );
  initialize_lines_with_size( lines_ptr, WHITE, 4, TRUE );
  add_object_to_model( model_ptr, object_ptr );

  for_less( i, 0, n_visible )
  {
    VIO_Colour colour = get_colour_of_label( slice_window, volume_index, i );
    object_ptr = create_object( QUADMESH );

    quadmesh_ptr = get_quadmesh_ptr( object_ptr );

    initialize_quadmesh( quadmesh_ptr, colour, &spr, 2, 2 );
    FREE( quadmesh_ptr->normals );
    quadmesh_ptr->normals = NULL;

    add_object_to_model( model_ptr, object_ptr );
  }

  for_less( i, 0, n_visible )
  {
    object_ptr = create_object( TEXT );

    text_ptr = get_text_ptr( object_ptr );

    fill_Point( origin, 0.0, 0.0, 0.0 );

    initialize_text( text_ptr, &origin, BLACK,
                     (Font_types) Menu_window_font, 10 );

    add_object_to_model( model_ptr, object_ptr );
  }

  for_less( i, 0, n_visible )
  {
    object_ptr = create_object( PIXELS );

    pixels_ptr = get_pixels_ptr( object_ptr );

    initialize_pixels( pixels_ptr, 0, 0,
                       cb_width,
                       cb_height,
                       1.0,
                       1.0,
                       RGB_PIXEL );

    add_object_to_model( model_ptr, object_ptr );
  }
}


void
rebuild_label_window(display_struct *slice_window)
{
  int                 i;
  display_struct      *label_window = get_display_by_type( LABEL_WINDOW );
  label_window_struct *label = &label_window->label;
  object_struct       *object_ptr;
  text_struct         *text_ptr;
  lines_struct        *lines_ptr;
  quadmesh_struct     *quadmesh_ptr;
  pixels_struct       *pixels_ptr;
  int                 volume_index = get_current_volume_index( slice_window );
  int                 n_object = 0;
  model_struct        *model_ptr;
  int                 x_size, y_size;
  int                 i_label;
  VIO_Colour          colour;
  int                 show_selected;

  G_get_window_size( label_window->window, &x_size, &y_size );

  model_ptr = get_graphics_model( label_window, SELECTED_MODEL );
  if( model_ptr->n_objects == 0 )
  {
    create_label_objects( slice_window, label->n_visible, model_ptr );
  }

  object_ptr = model_ptr->objects[n_object++];
  lines_ptr = get_lines_ptr( object_ptr );

  show_selected = FALSE;
  for_less( i, 0, label->n_visible )
  {
    VIO_Real top, bottom, left, right;
    i_label = i + 1 + label->top_index;
    colour = get_colour_of_label( slice_window, volume_index, i_label );

    object_ptr = model_ptr->objects[n_object++];

    quadmesh_ptr = get_quadmesh_ptr( object_ptr );
    quadmesh_ptr->colours[0] = colour;

    left = label->selected_x_origin + cb_width + 2;
    right = left + 200;
    bottom = label->selected_y_origin - cb_height * 2.0 * i;
    top = bottom + cb_height;

    fill_Point( quadmesh_ptr->points[VIO_IJ(0, 0, 2)], left, bottom, 0.0 );
    fill_Point( quadmesh_ptr->points[VIO_IJ(0, 1, 2)], right, bottom, 0.0 );
    fill_Point( quadmesh_ptr->points[VIO_IJ(1, 1, 2)], right, top, 0.0 );
    fill_Point( quadmesh_ptr->points[VIO_IJ(1, 0, 2)], left, top, 0.0 );

    if ( i_label == slice_window->slice.current_paint_label )
    {
      fill_Point( lines_ptr->points[0], left - 1, bottom - 1, 0.0 );
      fill_Point( lines_ptr->points[1], right + 1, bottom - 1, 0.0 );
      fill_Point( lines_ptr->points[2], right + 1, top + 1, 0.0 );
      fill_Point( lines_ptr->points[3], left - 1, top + 1, 0.0 );
      show_selected = TRUE;
    }
  }

  set_object_visibility( model_ptr->objects[0], show_selected );

  for_less( i, 0, label->n_visible )
  {
    char tmp[256];
    i_label = i + 1 + label->top_index;
    colour = get_colour_of_label( slice_window, volume_index, i_label );


    object_ptr = model_ptr->objects[n_object++];

    text_ptr = get_text_ptr( object_ptr );

    fill_Point( text_ptr->origin,
                label->selected_x_origin + cb_width + 4,
                label->selected_y_origin - cb_height * 2.0 * i + ((cb_height - label->font_size) / 2.0 + 2),
                0.0 );

    text_ptr->colour = get_text_colour(colour);
    text_ptr->size = label->font_size;

    snprintf(tmp, sizeof(tmp) - 1, "%d", i_label );
    replace_string( &text_ptr->string, create_string( tmp ) );
  }

  for_less( i, 0, label->n_visible )
  {
    int x, y, offset;

    i_label = i + 1 + label->top_index;
    offset = cb_width * is_label_visible( slice_window, volume_index, i_label );
    object_ptr = model_ptr->objects[n_object++];

    pixels_ptr = get_pixels_ptr( object_ptr );

    pixels_ptr->x_position = label->selected_x_origin;
    pixels_ptr->y_position = label->selected_y_origin - cb_height * 2.0 * i;

    for_less( x, 0, cb_width )
    {
      for_less( y, 0, cb_height )
      {
        VIO_Colour c = PIXEL_RGB_COLOUR( checkbox, offset + x, y );
        if ( c == MAGENTA )
        {
          c = G_get_background_colour( label_window->window );
        }
        PIXEL_RGB_COLOUR( *pixels_ptr, x, y ) = c;
      }
    }
  }
  set_update_required( label_window, NORMAL_PLANES );
}
