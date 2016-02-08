/**
 * \file intensity_plot.c
 *
 * \brief Intensity plots along axes or lines.
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
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <display.h>
#include <float.h>

/**
 * Attempt to get a good spacing value for tick marks.
 * TODO: still needs work
 * \param x the tentative tick separation.
 * \returns The recommended tick separation.
 */
VIO_Real get_nice_value(VIO_Real x)
{
  double l10 = log10(x);
  double t1 = pow(10.0, floor(1-l10));
  double t2 = round(x * t1);
  return round(t2 / t1);
}

/**
 * Initialize the structures used by the intensity plot.
 *
 * \param display A pointer to the display_struct for the slice window.
 */
void
initialize_intensity_plot(display_struct *display)
{
  model_struct *model_ptr = get_graphics_model( display, FULL_WINDOW_MODEL );
  object_struct *object_ptr;
  lines_struct *lines_ptr;

  /* Create graph */
  object_ptr = create_object( LINES );
  lines_ptr = get_lines_ptr( object_ptr );
  initialize_lines( lines_ptr, LIGHT_GRAY );
  set_object_visibility( object_ptr, TRUE );
  add_object_to_model( model_ptr, object_ptr );

  /* Create ticks */
  object_ptr = create_object( LINES );
  lines_ptr = get_lines_ptr( object_ptr );
  initialize_lines( lines_ptr, WHITE );
  set_object_visibility( object_ptr, TRUE );
  add_object_to_model( model_ptr, object_ptr );

  display->slice.intensity_plot_axis = Initial_intensity_plot_axis;
}

/**
 * \brief Create a tick label for an intensity plot.
 *
 * TODO: combine with the function of the same name in slice_window/rulers.c
 * \param position The integer position value to be formatted and displayed.
 * \param tick_pt The pixel position of the tick (used to position the label).
 * \param is_x_axis TRUE if this is for the horizontal axis, otherwise it
 * goes with the vertical axis.
 * \param tick_length The length of a tick line (used to position the label).
 * \returns An pointer to a text object.
 */
static object_struct *
create_tick_label(VIO_Real position, const VIO_Point *tick_pt, 
                  VIO_BOOL is_x_axis, int tick_length)
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
  snprintf( label, sizeof( label ) - 1, "%g", position );

  cw = G_get_text_length( label, Colour_bar_text_font, Colour_bar_text_size );

  if (is_x_axis)
  {
    fill_Point(pt,
               Point_x(*tick_pt) - cw / 2.0 + 0.5,
               Point_y(*tick_pt) - tick_length, 0);
  }
  else
  {
    ch = G_get_text_height( Colour_bar_text_font, Colour_bar_text_size );
    fill_Point(pt,
               Point_x( *tick_pt ) - cw - tick_length,
               Point_y( *tick_pt ) - ch / 2.0 + 1, 0);
  }

  text_ptr = get_text_ptr( object_ptr );
  initialize_text( text_ptr, &pt, Colour_bar_text_colour, Colour_bar_text_font,
                   Colour_bar_text_size );
  text_ptr->string = create_string( label );
  return object_ptr;
}

void
rebuild_intensity_plot(display_struct *display)
{
  display_struct *slice_window;

  if (!get_slice_window( display, &slice_window ))
  {
    return;
  }

  /* Remove text objects, if any.
   */
  model_struct *model_ptr = get_graphics_model( display, FULL_WINDOW_MODEL );
  while (model_ptr->n_objects > 3)
  {
    delete_object( model_ptr->objects[3] );
    remove_ith_object_from_model( model_ptr, 3 );
  }

  int volume_index = get_current_volume_index( slice_window );
  int arb_view_index = get_arbitrary_view_index( display );

  if (get_slice_visibility( display, volume_index, arb_view_index ))
  {
    set_object_visibility( model_ptr->objects[1], FALSE );
    set_object_visibility( model_ptr->objects[2], FALSE );
    return;
  }

  VIO_Volume volume = get_nth_volume( slice_window, volume_index );
  VIO_Real min_value, max_value;
  VIO_Real data[10000];
  VIO_BOOL use_volume_range = FALSE;

  get_volume_real_range( volume, &min_value, &max_value );

  char temp[128];
  snprintf( temp, sizeof(temp) - 1, "%g", max_value );

  VIO_Real max_data = -DBL_MAX;
  VIO_Real min_data = DBL_MAX;
  VIO_Real world_start[VIO_MAX_DIMENSIONS];
  VIO_Real world_end[VIO_MAX_DIMENSIONS];
  VIO_Vector step;
  int i;
  int sizes[VIO_MAX_DIMENSIONS];
  int x_index;

  get_volume_sizes( volume, sizes );

  VIO_Point pt_start, pt_end;
  VIO_Real voxel[VIO_MAX_DIMENSIONS];
  int n_meas = slice_window->slice.measure_number - 1;
  if (n_meas < 0)
    n_meas = N_MEASUREMENTS - 1;

  if (slice_window->slice.measure_line[n_meas] != NULL)
  {
    pt_start = slice_window->slice.measure_origin[n_meas];
    pt_end = slice_window->slice.measure_end[n_meas];
    x_index = -1;
  }
  else
  {
    VIO_Real voxel_start[VIO_MAX_DIMENSIONS];
    VIO_Real voxel_end[VIO_MAX_DIMENSIONS];
    int      axis_index = -1;
    int      view_index;

    get_slice_view_index_under_mouse( display, &view_index );

    if (display->slice.intensity_plot_axis >= 0)
    {
      x_index = display->slice.intensity_plot_axis;
    }
    else
    {
      if (get_volume_n_dimensions( volume ) > 3)
      {
        x_index = VIO_T;
      }
      else if (get_axis_index_under_mouse( display, &volume_index, &axis_index ))
      {
        /* The zooming behavior would be somewhat strange if we always
         * used the perpendicular axis for the plot axis, because the
         * perpendicular axis doesn't zoom from the user's
         * perspective. To make zooming somewhat intuitive, I assign
         * axes to the intensity plot according to the following
         * reasoning:
         *
         * 1. We should always either the row or column axis of a slice
         *    as the axis for the intensity plot, so that zooming and
         *    panning can be used to adjust the horizontal axis of the
         *    intensity plot.
         *   
         * 2. Since X is the column axis for two plots, and Z is the row
         *    axis for for two, there is no way to have a simple rule like
         *    "always use the row axis" - we'd always miss an axis.
         *
         * 3. So we use the column (width) axis for the view with perpendicular
         *    X and Z, which gives us Y and X. We use the row (height) axis for
         *    the view with perpendicular Y, since we covered it's width 
         *    axis elsewhere.
         */
        switch (axis_index) {
        case VIO_X:
          x_index = VIO_Y;      /* width */
          break;
        case VIO_Y:
          x_index = VIO_Z;      /* height */
          break;
        default:
          x_index = VIO_X;      /* width */
          break;
        }
      }
      else
      {
        x_index = VIO_X;
      }
    }

    get_current_voxel( slice_window, volume_index, voxel );

    /* Copy non-spatial dimensions unchanged. */
    for (i = VIO_N_DIMENSIONS; i < VIO_MAX_DIMENSIONS; i++)
    {
      voxel_start[i] = voxel[i];
      voxel_end[i] = voxel[i];
      world_start[i] = voxel[i];
      world_end[i] = voxel[i];
    }

    if (axis_index < 0)
    {
      voxel_start[x_index] = world_start[x_index] = 0;
      voxel_end[x_index] = world_end[x_index] = sizes[x_index] - 1;
    }
    else
    {
      VIO_Real x, y;
      int x_min, x_max;
      int y_min, y_max;

      get_slice_viewport( slice_window, view_index,
                          &x_min, &x_max, &y_min, &y_max);

      convert_voxel_to_pixel( display, volume_index, view_index, voxel,
                              &x, &y); 
      x += x_min;
      y += y_min;

      if (axis_index == VIO_Y)
      {
        x_min = x_max = x;
      }
      else
      {
        y_min = y_max = y;
      }

      convert_pixel_to_voxel( display, volume_index, x_min, y_min,
                              voxel_start, &view_index);

      convert_pixel_to_voxel( display, volume_index, x_max, y_max,
                              voxel_end, &view_index );
    }

    convert_voxel_to_world( volume, voxel_start, 
                            &world_start[VIO_X],
                            &world_start[VIO_Y],
                            &world_start[VIO_Z]);

    convert_voxel_to_world( volume, voxel_end, 
                            &world_end[VIO_X],
                            &world_end[VIO_Y],
                            &world_end[VIO_Z]);
    fill_Point(pt_start, world_start[VIO_X], world_start[VIO_Y], world_start[VIO_Z]);

    fill_Point(pt_end, world_end[VIO_X], world_end[VIO_Y], world_end[VIO_Z]);
  }
  VIO_Real distance = distance_between_points( &pt_start, &pt_end );

  fill_Vector(step,
              Point_x(pt_end) - Point_x(pt_start),
              Point_y(pt_end) - Point_y(pt_start),
              Point_z(pt_end) - Point_z(pt_start));

  world_start[0] = Point_x(pt_start);
  world_start[1] = Point_y(pt_start);
  world_start[2] = Point_z(pt_start);

  /* Normalize the vector to length one */
  Vector_x(step) /= distance;
  Vector_y(step) /= distance;
  Vector_z(step) /= distance;

  for (i = 0; i < distance; i++)
  {
    VIO_Real val;

    convert_world_to_voxel( volume,
                            world_start[0],
                            world_start[1],
                            world_start[2],
                            voxel );

    evaluate_volume( volume, voxel, NULL, 
                     slice_window->slice.degrees_continuity, FALSE, 0.0,
                     &val, NULL, NULL );

    if (val < min_data)
      min_data = val;
    if (val > max_data)
      max_data = val;
    data[i] = val;

    world_start[VIO_X] += Vector_x(step);
    world_start[VIO_Y] += Vector_y(step);
    world_start[VIO_Z] += Vector_z(step);
  }

  if (!use_volume_range)
  {
    min_value = floor(min_data);
    max_value = ceil(max_data);
  }

  object_struct *object_ptr = model_ptr->objects[1];
  lines_struct *lines_ptr = get_lines_ptr( object_ptr );

  delete_lines( lines_ptr );

  initialize_lines( lines_ptr, WHITE );

  lines_ptr->n_points = 0;
  lines_ptr->n_items = 0;
  lines_ptr->line_thickness = 1.0;

  int x_min, x_max;
  int y_min, y_max;

  get_slice_viewport( slice_window, arb_view_index,
                      &x_min, &x_max, &y_min, &y_max);

  const int cx_axis = 10 + G_get_text_length( temp, Colour_bar_text_font,
                                              Colour_bar_text_size );
  const int cy_axis = 20;
  VIO_Real y_range = (y_max - y_min) - cy_axis;
  VIO_Real x_range = (x_max - x_min) - cx_axis - 8;
  VIO_Point pt;

  VIO_Real d_range = max_value - min_value;

  for (i = 0; i < distance; i++)
  {
    Point_x(pt) = x_min + cx_axis + x_range * (VIO_Real) i / distance;
    Point_y(pt) = y_min + cy_axis + y_range * (data[i] - min_value) / d_range;
    Point_z(pt) = 0.0;
    add_point_to_line( lines_ptr, &pt );
  }

  set_object_visibility( object_ptr, TRUE );

  object_ptr = model_ptr->objects[2];
  lines_ptr = get_lines_ptr( object_ptr );

  delete_lines( lines_ptr );

  initialize_lines( lines_ptr, WHITE );

  /* Set up the axes.
   */
  start_new_line( lines_ptr );
  Point_x(pt) = x_min + cx_axis;
  Point_y(pt) = y_min + cy_axis + y_range;
  Point_z(pt) = 0;
  add_point_to_line( lines_ptr, &pt);
  Point_y(pt) = y_min + cy_axis;
  add_point_to_line( lines_ptr, &pt );
  Point_x(pt) = x_min + cx_axis + x_range;
  add_point_to_line( lines_ptr, &pt );

  /* Add the X-axis tick marks.
   */
  VIO_Real d_tick = get_nice_value( distance / 10.0 );
  int n_ticks = (int) ceil( distance / d_tick );

  for (i = 0; i < n_ticks; i++)
  {
    start_new_line( lines_ptr );
    Point_x(pt) = x_min + cx_axis + (i * d_tick) * x_range / distance;
    Point_y(pt) = y_min + cy_axis;
    Point_z(pt) = 0;
    add_point_to_line( lines_ptr, &pt );
    Point_y(pt) -= 10;
    add_point_to_line( lines_ptr, &pt );

    object_ptr = create_tick_label( i * d_tick, &pt, TRUE, 10 );
    add_object_to_model( model_ptr, object_ptr );
  }

  object_ptr = create_object( TEXT );
  Point_x(pt) = x_min + cx_axis + x_range;
  Point_y(pt) = y_min;
  Point_z(pt) = 0;
  text_struct *text_ptr = get_text_ptr( object_ptr );
  initialize_text( text_ptr, &pt, Colour_bar_text_colour, Colour_bar_text_font,
                   Colour_bar_text_size+2 );

  if (x_index < 0)
  {
    text_ptr->string = create_string( "mm" );
  }
  else
  {
    const char *axis_names[] = {"X", "Y", "Z", "T", "V"};
    text_ptr->string = create_string( axis_names[x_index] );
  }
  add_object_to_model( model_ptr, object_ptr );

  /* Add the Y-axis tick marks */
  d_tick = get_nice_value( d_range / 10.0 );
  n_ticks = (int) ceil( d_range / d_tick );

  for (i = 0; i < n_ticks; i++)
  {
    start_new_line( lines_ptr );
    Point_x(pt) = x_min + cx_axis;
    Point_y(pt) = y_min + cy_axis + (i * d_tick) * y_range / d_range;
    Point_z(pt) = 0;
    add_point_to_line( lines_ptr, &pt );

    object_ptr = create_tick_label( min_value + (i * d_tick),
                                    &pt, FALSE, 10 );
    add_object_to_model( model_ptr, object_ptr );

    Point_x(pt) -= 10;
    add_point_to_line( lines_ptr, &pt);
  }

  set_slice_viewport_update( slice_window, FULL_WINDOW_MODEL );
}

DEF_MENU_FUNCTION(toggle_intensity_plot_scaling)
{
}

DEF_MENU_UPDATE(toggle_intensity_plot_scaling)
{
}

DEF_MENU_FUNCTION(toggle_intensity_plot_axis)
{
}

DEF_MENU_UPDATE(toggle_intensity_plot_axis)
{
}


