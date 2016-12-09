/**
 * \file selected.c
 * \brief Build and maintain the object list associated with the marker
 * or object window.
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

#include <display.h>
#include <assert.h>
#include "checkbox.h"

/** The height of the checkbox glyph in pixels. */
static int cb_height;
/** The width of the checkbox glyph in pixels. */
static int cb_width;

/**
 * Create the text objects associated with the marker window.
 */
static  void
create_selected_text( display_struct *marker_window )
{
    model_struct   *model_ptr = get_graphics_model( marker_window,
                                                    SELECTED_MODEL );
    marker_window_struct *marker = &marker_window->marker;
    int            i;
    object_struct  *object_ptr;
    VIO_Point      origin;
    text_struct    *text_ptr;
    lines_struct   *lines_ptr;

    object_ptr = create_object( LINES );

    lines_ptr = get_lines_ptr( object_ptr );

    initialize_lines( lines_ptr, Selected_colour );

    ALLOC( lines_ptr->points, 4 );
    ALLOC( lines_ptr->end_indices, 1 );
    ALLOC( lines_ptr->indices, 4 + 1 );

    lines_ptr->n_points = 4;
    lines_ptr->n_items = 1;

    lines_ptr->indices[0] = 0;
    lines_ptr->indices[1] = 1;
    lines_ptr->indices[2] = 2;
    lines_ptr->indices[3] = 3;
    lines_ptr->indices[4] = 0;
    lines_ptr->end_indices[0] = 4 + 1;

    add_object_to_model( model_ptr, object_ptr );

    for_less( i, 0, N_selected_displayed )
    {
        object_ptr = create_object( TEXT );

        text_ptr = get_text_ptr( object_ptr );

        fill_Point( origin,
                    marker->selected_x_origin + cb_width,
                    marker->selected_y_origin - (cb_height+2) * i + ((cb_height+2) - marker->font_size) / 2,
                    0.0 );

        initialize_text( text_ptr, &origin, BLACK,
                         (Font_types) Menu_window_font, marker->font_size );

        add_object_to_model( model_ptr, object_ptr );
    }

    cb_width = checkbox.x_size / 2;
    cb_height = checkbox.y_size;

    for_less( i, 0, N_selected_displayed )
    {
        object_ptr = create_object( PIXELS );

        pixels_struct *pixels_ptr = get_pixels_ptr( object_ptr );

        initialize_pixels( pixels_ptr, 
                           marker->selected_x_origin,
                           marker->selected_y_origin - cb_height * 2.0 * i,
                           cb_width,
                           cb_height,
                           1.0,
                           1.0,
                           RGB_PIXEL );

        add_object_to_model( model_ptr, object_ptr );
    }
}

/**
 * Set the contents of a list item's text in the marker window.
 *
 * \param marker_window A pointer to the marker window.
 * \param index The index of the object in the selected list.
 * \param name The new name to display for the object.
 * \param colour The new colour for this object's list entry.
 */
static void
set_text_entry( display_struct *marker_window,
                int            index,
                VIO_STR        name,
                VIO_Colour     colour )
{
  model_struct  *model_ptr = get_graphics_model( marker_window, SELECTED_MODEL );
  object_struct *object_ptr = model_ptr->objects[index + 1];
  text_struct   *text_ptr = get_text_ptr( object_ptr );

  replace_string( &text_ptr->string, create_string( name ) );
  text_ptr->colour = colour;
  text_ptr->size = marker_window->marker.font_size;
  fill_Point( text_ptr->origin,
              marker_window->marker.selected_x_origin + cb_width,
              marker_window->marker.selected_y_origin - (cb_height + 2) * index + ((cb_height + 2) - marker_window->marker.font_size) / 2,
              0.0 );
  set_object_visibility( object_ptr, TRUE );

  object_ptr = model_ptr->objects[index + N_selected_displayed + 1];

  pixels_struct *pixels_ptr = get_pixels_ptr( object_ptr );

  pixels_ptr->x_position = marker_window->marker.selected_x_origin;
  pixels_ptr->y_position = marker_window->marker.selected_y_origin - (cb_height+2) * index - 1;
  
  set_object_visibility( object_ptr, TRUE );
}

/**
 * Get the bounding rectangle for a list item.
 */
static void
get_box_limits( marker_window_struct *marker,
                int                  index,
                const VIO_STR        label,
                int                  *x_min,
                int                  *x_max,
                int                  *y_min,
                int                  *y_max )
{
    int            width;

    width = (int) G_get_text_length( label, (Font_types) Menu_window_font,
                                     marker->font_size );

    if( width <= 0 )
        width = 20;

    *x_min = VIO_ROUND( marker->selected_x_origin - marker->selected_x_offset );
    *x_max = VIO_ROUND(*x_min + width + cb_width + marker->selected_x_offset + 4);

    *y_min = VIO_ROUND( marker->selected_y_origin -
                        (cb_height+2) * index - 
                        marker->selected_y_offset);
    *y_max = VIO_ROUND( *y_min + (cb_height+2) + marker->selected_y_offset);
}

/** 
 * Sets the position of the rectangular lines object that surrounds the 
 * selected object.
 *
 * \param marker A pointer to the specific state of the marker window.
 * \param selected_model A pointer to the model of the "selected list."
 * \param index The index of the currently selected object.
 * \param label The label text that will be used for the selected object,
 * used to set the size of the select
 */
static  void
set_current_box( marker_window_struct *marker,
                 model_struct         *selected_model,
                 int                  index,
                 const VIO_STR        label )
{
    int            x_start, x_end, y_start, y_end;
    VIO_Point      *points;

    get_box_limits( marker, index, label, &x_start, &x_end, &y_start, &y_end );

    points = get_lines_ptr( selected_model->objects[0] )->points;

    /* Adjust the highlight box such that it is inset slightly within the
     * overall height of the list item.
     */
    y_start -= 1;
    y_end -= 2;

    /* Now set the four corners of the highlight box.
     */
    fill_Point( points[0], x_start, y_start, 0.0 );
    fill_Point( points[1], x_end, y_start, 0.0 );
    fill_Point( points[2], x_end, y_end, 0.0 );
    fill_Point( points[3], x_start, y_end, 0.0 );

    set_object_visibility( selected_model->objects[0], TRUE );
}

typedef VIO_BOOL (*object_callback)( int global_index, int local_index,
                                     int depth, object_struct *object_ptr,
                                     void *data );

static VIO_BOOL
iterate_helper( model_struct *model_ptr, int depth, int *global_index,
                object_callback func, void *data )
{
  int local_index;

  for_less( local_index, 0, model_ptr->n_objects )
  {
    object_struct *object_ptr = model_ptr->objects[local_index];
    if (func != NULL)
    {
      if (!(*func)( *global_index, local_index, depth, object_ptr, data ))
      {
        return FALSE;
      }
    }
    (*global_index)++;
    if ( object_ptr->object_type == MODEL )
    {
      if (!iterate_helper( get_model_ptr( object_ptr ), depth + 1, 
                           global_index, func, data ) )
      {
        return FALSE;
      }
    }
  }
  return TRUE;
}

static int 
iterate_objects( model_struct *model_ptr, object_callback func,
                 void *data )
{
  int n = 0;
  if (!iterate_helper( model_ptr, 0, &n, func, data ))
  {
    return n;
  }
  return -1;
}

/**
 * Structure used to keep track of the list rebuilding process.
 */
struct rebuild_state
{
  /** Input pointer to the marker window */
  display_struct *marker_window;
  /** Input pointer to the current selected object. */
  object_struct  *selected_object;
  /** Input pointer to the current selected model. */
  model_struct *selected_model;
};

static VIO_BOOL
rebuild_callback( int global_index, int local_index, int depth,
                  object_struct *object_ptr, void *data )
{
  struct rebuild_state *rebuild_info = (struct rebuild_state *) data;
  VIO_STR    name;
  char       label[VIO_EXTREMELY_LARGE_STRING_SIZE];
  VIO_Colour colour;
  int        offset;
  int        x, y;
  pixels_struct *pixels_ptr;

  if ( global_index >= N_selected_displayed )
  {
    return FALSE;
  }

  name = get_object_name( object_ptr );

  if( get_object_visibility( object_ptr ) )
  {
    if( !get_object_colour( object_ptr, &colour ) )
    {
      colour = Visible_colour;
    }
    offset = cb_width*1;
  }
  else
  {
    colour = Invisible_colour;
    offset = cb_width*0;
  }

  snprintf( label, sizeof( label ), "%*s %d %s", depth * 4, "",
            local_index, name );

  set_text_entry( rebuild_info->marker_window, global_index, label, colour );

  if( object_ptr == rebuild_info->selected_object )
  {
    set_current_box( &rebuild_info->marker_window->marker,
                     rebuild_info->selected_model, global_index, label );
  }

  object_ptr = rebuild_info->selected_model->objects[global_index + 1 + N_selected_displayed];
  pixels_ptr = get_pixels_ptr( object_ptr );

  for_less( x, 0, cb_width )
  {
    for_less( y, 0, cb_height )
    {
      VIO_Colour c = PIXEL_RGB_COLOUR( checkbox, offset + x, y );
      if ( c == MAGENTA )
      {
        c = G_get_background_colour( rebuild_info->marker_window->window );
      }
      PIXEL_RGB_COLOUR( *pixels_ptr, x, y ) = c;
    }
  }

  delete_string( name );

  return TRUE;
}

void
rebuild_selected_list( display_struct *display,
                       display_struct *marker_window )
{
    int            i;
    model_struct   *selected_model;
    struct rebuild_state rebuild_info;

    assert( display->window_type == THREE_D_WINDOW );

    get_current_object( display, &rebuild_info.selected_object );

    selected_model = get_graphics_model( marker_window, SELECTED_MODEL );

    rebuild_info.selected_model = selected_model;
    rebuild_info.marker_window = marker_window;

    if( selected_model->n_objects == 0 )
    {
        create_selected_text( marker_window );
    }

    for_less( i, 0, N_selected_displayed * 2 + 1 )
    {
        set_object_visibility( selected_model->objects[i], FALSE );
    }

    iterate_objects( get_graphics_model( display, THREED_MODEL ),
                     rebuild_callback,
                     &rebuild_info );

    set_update_required( marker_window, NORMAL_PLANES );
}

/**
 * Structure used when searching for a particular object relative to
 * the mouse position.
 */
struct mouse_state
{
  /** A pointer to the marker window. */
  display_struct *marker_window;
  /** The x coordinate of the mouse. */
  int x;
  /** The y coordinate of the mouse. */
  int y;
  /** A pointer to the returned object. */
  object_struct *object_under_mouse;
};

static VIO_BOOL
mouse_name_callback( int global_index, int local_index, int depth,
                     object_struct *object_ptr, void *data )
{
  struct mouse_state *mouse_info = (struct mouse_state *) data;
  int x_min, x_max, y_min, y_max;
  model_struct *model_ptr = get_graphics_model( mouse_info->marker_window,
                                                SELECTED_MODEL );
  text_struct *text_ptr = get_text_ptr( model_ptr->objects[global_index + 1] );

  VIO_STR label = text_ptr->string;

  get_box_limits( &mouse_info->marker_window->marker, global_index, label,
                  &x_min, &x_max, &y_min, &y_max );

  if( x_min <= mouse_info->x && mouse_info->x <= x_max && 
      y_min <= mouse_info->y && mouse_info->y <= y_max )
  {
    mouse_info->object_under_mouse = object_ptr;
    return FALSE;
  }
  return TRUE;
}

VIO_BOOL
mouse_is_on_object_name( display_struct *display,
                         int            x,
                         int            y,
                         object_struct  **object_under_mouse )
{
    display_struct *marker_window = get_display_by_type( MARKER_WINDOW );
    struct mouse_state mouse_info;

    assert( display->window_type == THREE_D_WINDOW );

    mouse_info.marker_window = marker_window;
    mouse_info.x = x;
    mouse_info.y = y;
    mouse_info.object_under_mouse = NULL;

    iterate_objects( get_graphics_model( display, THREED_MODEL ),
                     mouse_name_callback,
                     &mouse_info );
                                    
    *object_under_mouse = mouse_info.object_under_mouse;
    return( *object_under_mouse != NULL );
}

static VIO_BOOL
mouse_checkbox_callback( int global_index, int local_index, int depth,
                         object_struct *object_ptr, void *data )
{
  struct mouse_state *mouse_info = (struct mouse_state *) data;
  int x_min, x_max, y_min, y_max;
  model_struct *model_ptr = get_graphics_model( mouse_info->marker_window,
                                                SELECTED_MODEL );
  pixels_struct *pixels_ptr = get_pixels_ptr( model_ptr->objects[global_index + N_selected_displayed + 1] );

  x_min = pixels_ptr->x_position;
  y_min = pixels_ptr->y_position;
  x_max = pixels_ptr->x_position + pixels_ptr->x_size - 1;
  y_max = pixels_ptr->y_position + pixels_ptr->y_size - 1;

  if( x_min <= mouse_info->x && mouse_info->x <= x_max && 
      y_min <= mouse_info->y && mouse_info->y <= y_max )
  {
    mouse_info->object_under_mouse = object_ptr;
    return FALSE;
  }
  return TRUE;
}

VIO_BOOL
mouse_is_on_object_checkbox( display_struct *display, int x, int y,
                             object_struct **object_under_mouse )
{
    display_struct *marker_window = get_display_by_type( MARKER_WINDOW );
    struct mouse_state mouse_info;

    assert( display->window_type == THREE_D_WINDOW );

    mouse_info.marker_window = marker_window;
    mouse_info.x = x;
    mouse_info.y = y;
    mouse_info.object_under_mouse = NULL;

    iterate_objects( get_graphics_model( display, THREED_MODEL ),
                     mouse_checkbox_callback,
                     &mouse_info );
                                    
    *object_under_mouse = mouse_info.object_under_mouse;
    return( *object_under_mouse != NULL );
}

static VIO_BOOL
find_object_callback(int global_index, int local_index, int depth, 
                     object_struct *object_ptr, void *data )
{
  if ( object_ptr == (object_struct *) data )
  {
    return FALSE;
  }
  return TRUE;
}

int
find_object_in_hierarchy( display_struct *display, object_struct *object_ptr )
{
  assert( display->window_type == THREE_D_WINDOW );

  return iterate_objects( get_graphics_model( display, THREED_MODEL ),
                          find_object_callback,
                          object_ptr );
}

/** 
 * Structure that keeps track of the "state" of a search for a specific
 * object index.
 */
struct find_state
{
  /** The index we are looking for. */
  int find_index;

  /** A pointer to the returned object. */
  object_struct *object_ptr;
};

static VIO_BOOL
find_index_callback(int global_index, int local_index, int depth, 
                    object_struct *object_ptr, void *data )
{
  struct find_state *find_info = (struct find_state *) data;

  if ( find_info->find_index == global_index )
  {
    find_info->object_ptr = object_ptr;
    return FALSE;
  }
  return TRUE;
}

object_struct *
find_index_in_hierarchy( display_struct *display, int global_index )
{
  struct find_state find_info;

  assert( display->window_type == THREE_D_WINDOW );

  find_info.find_index = global_index;
  find_info.object_ptr = NULL;

  iterate_objects( get_graphics_model( display, THREED_MODEL ),
                   find_index_callback,
                   &find_info );
                                    
  return find_info.object_ptr;
}

static VIO_BOOL
find_model_callback(int global_index, int local_index, int depth, 
                    object_struct *object_ptr, void *data )
{
  struct find_state *find_info = (struct find_state *) data;

  if (object_ptr->object_type == MODEL )
  {
    model_struct *model_ptr = get_model_ptr( object_ptr );
    if ( find_info->find_index > global_index &&
         find_info->find_index <= global_index + model_ptr->n_objects )
    {
      find_info->object_ptr = object_ptr;
      return FALSE;
    }
  }
  return TRUE;
}

object_struct *
find_containing_model( display_struct *display, int global_index )
{
  struct find_state find_info;

  find_info.find_index = global_index;
  find_info.object_ptr = NULL;

  iterate_objects( get_graphics_model( display, THREED_MODEL ),
                   find_model_callback,
                   &find_info );
  
  if ( find_info.object_ptr != NULL )
  {
    return find_info.object_ptr;
  }
  return display->models[THREED_MODEL];
}
