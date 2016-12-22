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
int Checkbox_height;
/** The width of the checkbox glyph in pixels. */
int Checkbox_width;

/**
 * Create the text objects associated with the marker window.
 * \param marker_window A pointer to the object list ("marker") window.
 */
static void
create_selected_text( display_struct *marker_window )
{
  model_struct         *model_ptr;
  marker_window_struct *mws_ptr;
  int                  i;
  object_struct        *object_ptr;
  text_struct          *text_ptr;
  lines_struct         *lines_ptr;
  pixels_struct        *pixels_ptr;

  model_ptr = get_graphics_model( marker_window, SELECTED_MODEL );
  mws_ptr = &marker_window->marker;
  Checkbox_width = checkbox.x_size / 2;
  Checkbox_height = checkbox.y_size;

  /* The first object in the model is the lines object that describes
   * the box around the selected item.
   */
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

  /* For the rest of the model we alternate between the text object for the
   * list item followed by the checkbox pixel object.
   */
  for_less( i, 0, mws_ptr->n_item_count )
  {
    object_ptr = create_object( TEXT );

    text_ptr = get_text_ptr( object_ptr );

    initialize_text( text_ptr, NULL, BLACK, (Font_types) Menu_window_font,
                     mws_ptr->font_size );

    add_object_to_model( model_ptr, object_ptr );

    object_ptr = create_object( PIXELS );

    pixels_ptr = get_pixels_ptr( object_ptr );

    initialize_pixels( pixels_ptr, 0, 0, Checkbox_width, Checkbox_height,
                       1.0, 1.0, RGB_PIXEL );

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
  model_struct  *model_ptr;
  object_struct *object_ptr;
  text_struct   *text_ptr;
  pixels_struct *pixels_ptr;
  marker_window_struct *mws_ptr = &marker_window->marker;

  model_ptr = get_graphics_model( marker_window, SELECTED_MODEL );

  object_ptr = model_ptr->objects[1 + index * 2];
  text_ptr = get_text_ptr( object_ptr );
  replace_string( &text_ptr->string, create_string( name ) );
  text_ptr->colour = colour;
  text_ptr->size = mws_ptr->font_size;
  fill_Point( text_ptr->origin,
              mws_ptr->selected_x_origin + Checkbox_width,
              mws_ptr->selected_y_origin - (Checkbox_height + 2) * index +
              ((Checkbox_height + 2) - mws_ptr->font_size) / 2,
              0.0 );
  set_object_visibility( object_ptr, TRUE );

  object_ptr = model_ptr->objects[2 + index * 2];
  pixels_ptr = get_pixels_ptr( object_ptr );
  pixels_ptr->x_position = mws_ptr->selected_x_origin;
  pixels_ptr->y_position = mws_ptr->selected_y_origin -
    (Checkbox_height + 2) * index - 1;
  set_object_visibility( object_ptr, TRUE );
}

/**
 * Get the bounding rectangle for a list item.
 * \param mws_ptr A pointer to the marker-window specific structure.
 * \param index The "list relative" index of the list item.
 * \param label The text that will be drawn in this list item.
 * \param x_min The returned minimum x coordinate.
 * \param x_max The returned maximum x coordinate.
 * \param y_min The returned minimum y coordinate.
 * \param y_max The returned maximum y coordinate.
 */
static void
get_box_limits( marker_window_struct *mws_ptr,
                int                  index,
                const VIO_STR        label,
                int                  *x_min,
                int                  *x_max,
                int                  *y_min,
                int                  *y_max )
{
  int width = G_get_text_length( label, (Font_types) Menu_window_font,
                                 mws_ptr->font_size );

  if( width <= 0 )
    width = 20;

  *x_min = VIO_ROUND( mws_ptr->selected_x_origin - mws_ptr->selected_x_offset );
  *x_max = VIO_ROUND(*x_min + width + Checkbox_width + mws_ptr->selected_x_offset + 4);

  *y_min = VIO_ROUND( mws_ptr->selected_y_origin -
                      (Checkbox_height + 2) * index -
                      mws_ptr->selected_y_offset);
  *y_max = VIO_ROUND( *y_min + (Checkbox_height + 2) +
                      mws_ptr->selected_y_offset);
}

/**
 * Sets the position of the rectangular lines object that surrounds the
 * selected object.
 *
 * \param mws_ptr A pointer to the specific state of the marker window.
 * \param selected_model A pointer to the model of the "selected list."
 * \param index The "list relative" index of the currently selected object.
 * \param label The label text that will be used for the selected object,
 * used to set the size of the selection rectangle.
 */
static void
set_current_box( marker_window_struct *mws_ptr,
                 model_struct         *selected_model,
                 int                  index,
                 const VIO_STR        label )
{
  int       x_start, x_end, y_start, y_end;
  VIO_Point *points;

  get_box_limits( mws_ptr, index, label, &x_start, &x_end, &y_start, &y_end );

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

/** The type of all object callback functions. */
typedef VIO_BOOL (*object_callback)( int global_index, int local_index,
                                     int depth, object_struct *object_ptr,
                                     void *data );

/**
 * Helper function for object iteration within models.
 *
 * \param model_ptr A pointer to the model-specific object structure to search.
 * \param depth The nesting depth of this call.
 * \param global_index A pointer to the initial global index value. Will be
 * updated by this function.
 * \param func The callback function.
 * \param data An opaque pointer to callback-specific data.
 * \returns TRUE if the iteration was stopped by the callback function
 * returning TRUE, otherwise FALSE.
 */
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
      if ( !iterate_helper( get_model_ptr( object_ptr ), depth + 1,
                            global_index, func, data ) )
      {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/**
 * Iterate over the objects in a model, calling the given callback
 * function for each object.
 * \param model_ptr A pointer to the model-specific object structure to search.
 * \param func The callback function.
 * \param data An opaque pointer to callback-specific data.
 * \returns The global index of the object within the model structure, or
 * -1 if the callback function never returns TRUE.
 */
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

/**
 * Callback to actually build the object list entries based on the
 * user's loaded objects.
 *
 * \param global_index The global index of the current object. This is the
 * overall position in the object list.
 * \param local_index The local index of the current object. This is the
 * specific position within the containing model.
 * \param depth The nesting depth of the current object.
 * \param object_ptr A pointer to the current object.
 * \param data Opaque pointer to callback-specific state data.
 * \returns TRUE if iteration should continue, FALSE if it should
 * terminate (e.g. because the item was found).
 */
static VIO_BOOL
rebuild_callback( int global_index, int local_index, int depth,
                  object_struct *object_ptr, void *data )
{
  struct rebuild_state *rebuild_info = (struct rebuild_state *) data;
  VIO_STR              name;
  char                 label[VIO_EXTREMELY_LARGE_STRING_SIZE];
  VIO_Colour           colour;
  int                  offset;
  int                  x, y;
  pixels_struct        *pixels_ptr;
  marker_window_struct *mws_ptr = &rebuild_info->marker_window->marker;
  VIO_Colour           bg_colour;
  int                  rel_index = global_index - mws_ptr->n_top_index;
  if ( rel_index < 0 )
    return TRUE;

  if ( rel_index >= mws_ptr->n_item_count )
    return FALSE;

  if( get_object_visibility( object_ptr ) )
  {
    if( !get_object_colour( object_ptr, &colour ) )
    {
      colour = Visible_colour;
    }
    offset = Checkbox_width*1;
  }
  else
  {
    colour = Invisible_colour;
    offset = Checkbox_width*0;
  }

  name = get_object_name( object_ptr );

  snprintf( label, sizeof( label ), "%*s %d %s", depth * 4, "",
            local_index, name );

  delete_string( name );

  set_text_entry( rebuild_info->marker_window, rel_index, label, colour );

  if( object_ptr == rebuild_info->selected_object )
  {
    set_current_box( mws_ptr, rebuild_info->selected_model, rel_index, label );
  }

  object_ptr = rebuild_info->selected_model->objects[2 + rel_index * 2];
  pixels_ptr = get_pixels_ptr( object_ptr );

  bg_colour = G_get_background_colour( rebuild_info->marker_window->window );

  for_less( x, 0, Checkbox_width )
  {
    for_less( y, 0, Checkbox_height )
    {
      VIO_Colour c = PIXEL_RGB_COLOUR( checkbox, offset + x, y );
      if ( c == MAGENTA )
      {
        c = bg_colour;  /* use background colour where transparent. */
      }
      PIXEL_RGB_COLOUR( *pixels_ptr, x, y ) = c;
    }
  }

  return TRUE;
}

/**
 * Reconstruct the list of "user" objects.
 *
 * \param display A pointer to the 3D window.
 * \param marker_window A pointer to the object list window.
 */
void
rebuild_selected_list( display_struct *display,
                       display_struct *marker_window )
{
  int                  i;
  model_struct         *selected_model;
  struct rebuild_state rebuild_info;
  int                  selected_index;
  marker_window_struct *mws_ptr = &marker_window->marker;

  assert( display->window_type == THREE_D_WINDOW );

  get_current_object( display, &rebuild_info.selected_object );

  selected_model = get_graphics_model( marker_window, SELECTED_MODEL );
  selected_index = get_current_object_index( display );

  /* We handle scrolling here. If the selected index is less than the
   * current top index, we move "up" the list so that the selected
   * index is the new top. If the selected index is greater than the
   * last displayed index, we move the list up so that the selected index
   * is the last displayed.
   */
  if ( selected_index < mws_ptr->n_top_index && mws_ptr->n_top_index > 0 )
  {
    mws_ptr->n_top_index = selected_index;
  }
  if ( selected_index >= ( mws_ptr->n_top_index + mws_ptr->n_item_count ))
  {
    mws_ptr->n_top_index = selected_index - mws_ptr->n_item_count + 1;
  }

  rebuild_info.selected_model = selected_model;
  rebuild_info.marker_window = marker_window;

  /* Create the graphical objects if necessary.
   */
  if( selected_model->n_objects == 0 )
  {
    create_selected_text( marker_window );
  }

  /* Make everything invisible by default.
   */
  for_less( i, 0, mws_ptr->n_item_count * 2 + 1 )
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

/**
 * Callback helper to implement search to determine if the mouse pointer is over
 * a specific object.
 *
 * \param global_index The global index of the current object. This is the
 * overall position in the object list.
 * \param local_index The local index of the current object. This is the
 * specific position within the containing model.
 * \param depth The nesting depth of the current object.
 * \param object_ptr A pointer to the current object.
 * \param data Opaque pointer to callback-specific state data.
 * \param f_cb TRUE if the test should be performed relative to the
 * object's checkbox, FALSE if it should be relative to the text.
 * \returns TRUE if iteration should continue, FALSE if it should
 * terminate (e.g. because the item was found).
 */
static VIO_BOOL
mouse_position_callback( int global_index, int local_index, int depth,
                         object_struct *object_ptr, void *data, VIO_BOOL f_cb )
{
  struct mouse_state   *mouse_info = (struct mouse_state *) data;
  int                  x_min, x_max, y_min, y_max;
  model_struct         *model_ptr;
  pixels_struct        *pixels_ptr;
  text_struct          *text_ptr;
  marker_window_struct *mws_ptr = &mouse_info->marker_window->marker;
  int                  rel_index = global_index - mws_ptr->n_top_index;
  if ( rel_index < 0 )
    return TRUE;

  if ( rel_index >= mws_ptr->n_item_count )
    return FALSE;

  model_ptr = get_graphics_model( mouse_info->marker_window, SELECTED_MODEL );
  if ( f_cb )
  {
    pixels_ptr = get_pixels_ptr( model_ptr->objects[2 + rel_index * 2] );

    x_min = pixels_ptr->x_position;
    y_min = pixels_ptr->y_position;
    x_max = pixels_ptr->x_position + pixels_ptr->x_size - 1;
    y_max = pixels_ptr->y_position + pixels_ptr->y_size - 1;
  }
  else
  {
    text_ptr = get_text_ptr( model_ptr->objects[1 + rel_index * 2] );

    get_box_limits( mws_ptr, rel_index, text_ptr->string,
                    &x_min, &x_max, &y_min, &y_max );
  }

  if( x_min <= mouse_info->x && mouse_info->x <= x_max &&
      y_min <= mouse_info->y && mouse_info->y <= y_max )
  {
    mouse_info->object_under_mouse = object_ptr;
    return FALSE;
  }
  return TRUE;
}

/**
 * Callback to implement search to determine if the mouse pointer is over
 * a specific user object's text entry.
 *
 * \param global_index The global index of the current object. This is the
 * overall position in the object list.
 * \param local_index The local index of the current object. This is the
 * specific position within the containing model.
 * \param depth The nesting depth of the current object.
 * \param object_ptr A pointer to the current object.
 * \param data Opaque pointer to callback-specific state data.
 * \returns TRUE if iteration should continue, FALSE if it should
 * terminate (e.g. because the item was found).
 */
static VIO_BOOL
mouse_name_callback( int global_index, int local_index, int depth,
                     object_struct *object_ptr, void *data )
{
  return mouse_position_callback( global_index, local_index, depth,
                                  object_ptr, data, FALSE );
}

/**
 * Determine whether the give window-relative pixel coordinates are
 * over a particular object's list text.
 * \param display A pointer to the 3D view window.
 * \param x The x coordinate of the mouse pointer.
 * \param y The y coordinate of the mouse pointer.
 * \param object_under_mouse A pointer which will be filled in if the
 * object is located.
 * \returns TRUE if the mouse is over a checkbox and object_under_mouse
 * is valid.
 */
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

/**
 * Callback to implement search to determine if the mouse pointer is over
 * a specific user object's checkbox.
 *
 * \param global_index The global index of the current object. This is the
 * overall position in the object list.
 * \param local_index The local index of the current object. This is the
 * specific position within the containing model.
 * \param depth The nesting depth of the current object.
 * \param object_ptr A pointer to the current object.
 * \param data Opaque pointer to callback-specific state data.
 * \returns TRUE if iteration should continue, FALSE if it should
 * terminate (e.g. because the item was found).
 */
static VIO_BOOL
mouse_checkbox_callback( int global_index, int local_index, int depth,
                         object_struct *object_ptr, void *data )
{
  return mouse_position_callback( global_index, local_index, depth,
                                  object_ptr, data, TRUE );
}


/**
 * Determine whether the give window-relative pixel coordinates are
 * over a particular object's checkbox glyph.
 * \param display A pointer to the 3D view window.
 * \param x The x coordinate of the mouse pointer.
 * \param y The y coordinate of the mouse pointer.
 * \param object_under_mouse A pointer which will be filled in if the
 * object is located.
 * \returns TRUE if the mouse is over a checkbox and object_under_mouse
 * is valid.
 */
VIO_BOOL
mouse_is_on_object_checkbox( display_struct *display, int x, int y,
                             object_struct **object_under_mouse )
{
  display_struct     *marker_window = get_display_by_type( MARKER_WINDOW );
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

/**
 * Callback to implement search for the global index associated with
 * a specific object structure.
 *
 * \param global_index The global index of the current object. This is the
 * overall position in the object list.
 * \param local_index The local index of the current object. This is the
 * specific position within the containing model.
 * \param depth The nesting depth of the current object.
 * \param object_ptr A pointer to the current object.
 * \param data Opaque pointer to callback-specific state data.
 * \returns TRUE if iteration should continue, FALSE if it should
 * terminate (e.g. because the item was found).
 */
static VIO_BOOL
find_object_callback( int global_index, int local_index, int depth,
                      object_struct *object_ptr, void *data )
{
  return ( object_ptr != (object_struct *) data );
}

/**
 * Given an object presumably known to have been loaded by the user,
 * find its global index within the object hierarchy.
 *
 * \param display A pointer to the 3D window.
 * \param object_ptr A pointer to the object we want to find.
 * \returns The global index of the object, or -1 if not found.
 */
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

/**
 * Callback to implement search for the object associated with
 * a specific global index.
 *
 * \param global_index The global index of the current object. This is the
 * overall position in the object list.
 * \param local_index The local index of the current object. This is the
 * specific position within the containing model.
 * \param depth The nesting depth of the current object.
 * \param object_ptr A pointer to the current object.
 * \param data Opaque pointer to callback-specific state data.
 * \returns TRUE if iteration should continue, FALSE if it should
 * terminate (e.g. because the item was found).
 */
static VIO_BOOL
find_index_callback( int global_index, int local_index, int depth,
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

/**
 * Given a global index, find the actual object associated with that
 * index.
 *
 * \param display A pointer to the 3D view window.
 * \param global_index The global index of the object.
 * \returns A pointer to the object, or NULL on failure.
 */
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

/**
 * Callback to implement search for the containing model of a specified
 * object.
 *
 * \param global_index The global index of the current object. This is the
 * overall position in the object list.
 * \param local_index The local index of the current object. This is the
 * specific position within the containing model.
 * \param depth The nesting depth of the current object.
 * \param object_ptr A pointer to the current object.
 * \param data Opaque pointer to callback-specific state data.
 * \returns TRUE if iteration should continue, FALSE if it should
 * terminate (e.g. because the item was found).
 */
static VIO_BOOL
find_model_callback( int global_index, int local_index, int depth,
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

/**
 * Search for the model that contains the given global object index.
 *
 * \param display A pointer to the 3D window.
 * \param global_index The index of the object whose model we need.
 * \returns A pointer to the model object, or NULL on failure.
 */
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
