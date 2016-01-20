/**
 * \file rulers.c
 * \brief Drawing the rulers that can be superimposed on the slice view.
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

#include  <display.h>

/**
 * \brief Initialize the objects used by the slice ruler. 
 *
 * Adds a new model at the \c RULERS relative offset. Initially this
 * new model contains only a single lines object that is used to
 * represent all of the ticks.  This model will also contain a number
 * of text objects that represent the labels that appear next to the
 * ticks.
 *
 * \param model_ptr A pointer to the model_struct for the slice view.
 */
void initialize_slice_rulers(model_struct *model_ptr)
{
  object_struct     *object_ptr;
  model_struct      *sub_model_ptr;
  model_info_struct *model_info_ptr;
  lines_struct      *lines_ptr;

  object_ptr = create_object( MODEL );
  set_object_visibility( object_ptr, Ruler_initially_visible );
  add_object_to_model( model_ptr, object_ptr );
  sub_model_ptr = get_model_ptr( object_ptr );
  initialize_display_model( sub_model_ptr );
  model_info_ptr = get_model_info( sub_model_ptr );
  sub_model_ptr->n_objects = 0;
  model_info_ptr->view_type = PIXEL_VIEW;
  model_info_ptr->bitplanes = NORMAL_PLANES;

  object_ptr = create_object( LINES );
  lines_ptr = get_lines_ptr( object_ptr );
  initialize_lines( lines_ptr, Colour_bar_tick_colour );
  set_object_visibility( object_ptr, TRUE );
  add_object_to_model( sub_model_ptr, object_ptr );
}

/**
 * \brief Create a label for a tick mark on the ruler.
 *
 * Creates a text object to act as the label for either the X or Y
 * axis rulers in the slice view. Positions the label according to the
 * size of the text.
 *
 * \param position The position of the point in world coordinates.
 * \param tick_pt The position of the end of the tick mark.
 * \param is_x_axis True if the tick is on the X axis, otherwise it is
 * on the Y axis. Changes the position calculation.
 * \returns A pointer to the newly-created text object.
 */
static object_struct *
create_tick_label(int position, const VIO_Point *tick_pt, VIO_BOOL is_x_axis)
{
  object_struct *object_ptr;
  text_struct   *text_ptr;
  VIO_Point     pt;
  char          label[128];
  VIO_Real      cw;             /* Text width in pixels. */
  VIO_Real      ch;             /* Text height in pixels. */

  object_ptr = create_object( TEXT );
  if (object_ptr == NULL)
    return NULL;

  set_object_visibility( object_ptr, TRUE );
  snprintf( label, sizeof( label ) - 1, "%d", position );

  cw = G_get_text_length( label, Colour_bar_text_font, Colour_bar_text_size );
  if (is_x_axis)
  {
    fill_Point(pt, 
               Point_x( *tick_pt ) - cw / 2.0 + 0.5,
               Point_y( *tick_pt ) - Ruler_x_axis_y_offset, 0);
  }
  else
  {
    ch = G_get_text_height( Colour_bar_text_font, Colour_bar_text_size );
    fill_Point(pt, 
               Point_x( *tick_pt ) - cw - Ruler_big_tick_length,
               Point_y( *tick_pt ) - ch / 2.0 + 1, 0);
  }

  text_ptr = get_text_ptr( object_ptr );
  initialize_text( text_ptr, &pt, Colour_bar_text_colour, Colour_bar_text_font,
                   Colour_bar_text_size );
  text_ptr->string = create_string( label );
  return object_ptr;
}

/**
 * \brief Recreate the object used to display the rulers on the slice view.
 *
 * Rulers provide a detailed picture of the world coordinates of a slice.
 * Rulers cannot be drawn on the arbitrary view index in this implementation.
 *
 * \param slice_window A pointer to the display_struct of the slice window.
 * \param view_index The zero-based index of the view.
 */
void
rebuild_slice_rulers( display_struct *slice_window, int view_index )
{
  model_struct      *model_ptr;
  object_struct     *object_ptr;
  lines_struct      *lines_ptr;
  int               x_min_vp, x_max_vp, y_min_vp, y_max_vp;
  int               volume_index;
  VIO_Volume        volume;
  int               axis_index, x_index, y_index;
  VIO_Real          voxel[VIO_MAX_DIMENSIONS];
  VIO_Real          w_ll[VIO_N_DIMENSIONS];
  VIO_Real          w_ur[VIO_N_DIMENSIONS];
  int               x, y;
  int               dummy_index;
  VIO_BOOL          draw_ones;
  int               x_max, y_max;
  VIO_Real          ratio;

  /* Don't even bother to display this for the oblique plane yet.
   */
  if (view_index == get_arbitrary_view_index( slice_window ) )
    return;

  volume_index = get_current_volume_index( slice_window );
  if (volume_index < 0)
    return;

  if (!get_slice_visibility( slice_window, volume_index, view_index ))
  {
    return;
  }

  volume = get_nth_volume( slice_window, volume_index );
  model_ptr = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
  object_ptr = model_ptr->objects[2 * get_n_volumes( slice_window ) + RULERS];
  model_ptr = get_model_ptr( object_ptr );
  while (model_ptr->n_objects > 1)
  {
    /* Unfortunately there is no atomic call to remove and object from
     * the model and delete it. TODO: Add something like this!
     */
    delete_object( model_ptr->objects[1] );
    remove_ith_object_from_model( model_ptr, 1 );
  }
  lines_ptr = get_lines_ptr( model_ptr->objects[0] );

  delete_lines( lines_ptr );
  initialize_lines( lines_ptr, Colour_bar_tick_colour );
  
  if (!get_slice_visibility( slice_window, volume_index, view_index ) ||
      !get_object_visibility( object_ptr ))
  {
    return;
  }

  if (!slice_has_ortho_axes( slice_window, volume_index, view_index,
                             &x_index, &y_index, &axis_index ))
  {
    return;
  }

  /* Calculate the bounding box of the view in world coordinates.
   */
  get_slice_model_viewport( slice_window, SLICE_MODEL1 + view_index,
                            &x_min_vp, &x_max_vp, &y_min_vp, &y_max_vp );

  convert_pixel_to_voxel( slice_window, volume_index, x_min_vp, y_min_vp,
                          voxel, &dummy_index);
  convert_voxel_to_world( volume, voxel, &w_ll[0], &w_ll[1], &w_ll[2]);
  convert_pixel_to_voxel( slice_window, volume_index, x_max_vp, y_max_vp,
                          voxel, &dummy_index);
  convert_voxel_to_world( volume, voxel, &w_ur[0], &w_ur[1], &w_ur[2]);

  x = (int) VIO_ROUND( w_ll[x_index] );
  if (x < -500) 
    return;
  x_max = (int) VIO_ROUND( w_ur[x_index] );
  if (x_max > 500) 
    return;

  ratio = (VIO_Real) (x_max_vp - x_min_vp) / (x_max - x);
  draw_ones = (ratio > 3);
  if (ratio < 0.6)
    return;

  for ( ; x < x_max; x++)
  {
    VIO_Real world[VIO_N_DIMENSIONS];
    VIO_Real voxel[VIO_MAX_DIMENSIONS];
    VIO_Real p_x, p_y;
    VIO_Point pt1, pt2;

    world[x_index] = x;
    world[y_index] = w_ll[y_index];
    world[axis_index] = 0;

    convert_world_to_voxel( volume, world[0], world[1], world[2], voxel );
    convert_voxel_to_pixel( slice_window, volume_index, view_index,
                            voxel, &p_x, &p_y);
    if (p_x < Ruler_y_axis_x_offset) 
    {
      continue;
    }

    fill_Point(pt1, p_x, p_y + Ruler_x_axis_y_offset, 0);
    pt2 = pt1;

    if ((x % 10) == 0)
    {
      add_object_to_model( model_ptr, create_tick_label( x, &pt1, TRUE ));
      Point_y(pt2) -= Ruler_big_tick_length;
    }
    else if ((x % 5) == 0)
    {
      Point_y(pt2) -= Ruler_medium_tick_length;
    }
    else if (!draw_ones)
    {
      continue;
    }
    else
    {
      Point_y(pt2) -= Ruler_small_tick_length;
    }
    start_new_line( lines_ptr );
    add_point_to_line(lines_ptr, &pt1);
    add_point_to_line(lines_ptr, &pt2);
  }

  y = (int) VIO_ROUND( w_ll[y_index] );
  if (y < -500) 
    return;
  y_max = (int) VIO_ROUND( w_ur[y_index] );
  if (y_max > 500) 
    return;

  ratio = (VIO_Real) (y_max_vp - y_min_vp) / (VIO_Real) (y_max - y);
  draw_ones = (ratio > 3.0);
  if (ratio < 0.6)
    return;

  for ( ; y < y_max; y++)
  {
    VIO_Real world[VIO_N_DIMENSIONS];
    VIO_Real voxel[VIO_MAX_DIMENSIONS];
    VIO_Real p_x, p_y;
    VIO_Point pt1, pt2;

    world[x_index] = w_ll[x_index];
    world[y_index] = y;
    world[axis_index] = 0;

    convert_world_to_voxel( volume, world[0], world[1], world[2], voxel );
    convert_voxel_to_pixel( slice_window, volume_index, view_index,
                            voxel, &p_x, &p_y);

    if (p_y < Ruler_x_axis_y_offset) 
    {
      continue;
    }

    fill_Point(pt1, p_x + Ruler_y_axis_x_offset, p_y, 0);
    pt2 = pt1;
    if ((y % 10) == 0)
    {
      add_object_to_model( model_ptr, create_tick_label( y, &pt1, FALSE ));
      Point_x( pt2 ) -= Ruler_big_tick_length;
    }
    else if ((y % 5) == 0)
    {
      Point_x( pt2 ) -= Ruler_medium_tick_length;
    }
    else if (!draw_ones)
    {
      continue;
    }
    else
    {
      Point_x( pt2 ) -= Ruler_small_tick_length;
    }
    start_new_line( lines_ptr );
    add_point_to_line( lines_ptr, &pt1 );
    add_point_to_line( lines_ptr, &pt2 );
  }
}

/**
 * \brief Get the visibility state of the slice rulers. 
 *
 * The visibility state of the rulers model controls the visibility of 
 * the entire ruler.
 *
 * \param slice_window A pointer to the display_struct of the slice window.
 * \param view_index The zero-based index of the slice view.
 * \returns TRUE if the rulers are visible in this slice view.
 */
VIO_BOOL
get_slice_rulers_visibility( display_struct *slice_window, int view_index )
{
  model_struct      *model_ptr;
  object_struct     *object_ptr;

  model_ptr = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
  object_ptr = model_ptr->objects[2 * get_n_volumes( slice_window ) + RULERS];
  return( get_object_visibility( object_ptr ) );
}

/**
 * \brief Set the visibility state of the slice rulers. 
 *
 * The visibility state of the rulers model controls the visibility of 
 * the entire ruler.
 *
 * \param slice_window A pointer to the display_struct of the slice window.
 * \param view_index The zero-based index of the slice view.
 * \param state TRUE if the rulers should be displayed in this slice view.
 */
void
set_slice_rulers_visibility( display_struct *slice_window, int view_index,
                             VIO_BOOL state )
{
  model_struct      *model_ptr;
  object_struct     *object_ptr;

  model_ptr = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
  object_ptr = model_ptr->objects[2 * get_n_volumes( slice_window ) + RULERS];
  set_object_visibility( object_ptr, state );
}
