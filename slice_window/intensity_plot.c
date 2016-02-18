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
 if (x == 0) return 0;
  VIO_Real scale = pow(10.0, floor(log10(fabs(x))));
  VIO_Real value = scale * trunc(x / scale);
  return value;
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
  if (fabs(position) < 1e-8)
    position = 0;
  snprintf( label, sizeof( label ) - 1, "%.4g", position );

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

VIO_BOOL
get_pixels_extent( display_struct *display, int volume_index, int view_index,
                   int x_index,
                   VIO_Point *pt_start, VIO_Point *pt_end )
{
  object_struct *object_ptr;
  pixels_struct *pixels_ptr;
  VIO_Real v[VIO_MAX_DIMENSIONS];
  VIO_Real w[VIO_N_DIMENSIONS];
  VIO_Real world[VIO_N_DIMENSIONS];
  int dummy;
  int x_min, y_min, x_max, y_max;
  VIO_Volume volume;
  VIO_Real voxel[VIO_MAX_DIMENSIONS];
  int x1, x2, y1, y2;

  //printf("%s: %d %d %d\n", __func__, volume_index, view_index, x_index );

  if (display->window_type != SLICE_WINDOW)
    return FALSE;

  volume = get_nth_volume( display, volume_index );
  if (volume == NULL)
    return FALSE;

  object_ptr = get_slice_pixels_object( display, volume_index, view_index );
  if (object_ptr == NULL)
    return FALSE;

  pixels_ptr = get_pixels_ptr( object_ptr );
  if (pixels_ptr == NULL)
    return FALSE;

  get_current_voxel( display, volume_index, voxel );
  convert_voxel_to_world( volume, voxel, &world[0], &world[1], &world[2] );

  get_slice_viewport( display, view_index,
                      &x_min, &x_max, &y_min, &y_max);

#if 0
  printf("voxel %f %f %f\n", voxel[0], voxel[1], voxel[2]);
  printf("pixels %d %d %d %d\n",
         pixels_ptr->x_position, pixels_ptr->y_position,
         pixels_ptr->x_size, pixels_ptr->y_size);
#endif

  x1 = pixels_ptr->x_position + x_min;
  y1 = pixels_ptr->y_position + y_min;

  if (x_index == VIO_X || x_index == VIO_Y)
  {
    x2 = pixels_ptr->x_position + x_min + pixels_ptr->x_size - 1;
    y2 = y1;
  }
  else
  {
    x2 = x1;
    y2 = pixels_ptr->y_position + y_min + pixels_ptr->y_size - 1;
  }

  convert_pixel_to_voxel( display, volume_index, x1, y1, v, &dummy );
  convert_voxel_to_world( volume, v, &w[0], &w[1], &w[2] );
  world[x_index] = w[x_index];

  fill_Point(*pt_start, world[0], world[1], world[2] );

  //printf("Start %f %f %f\n", world[0], world[1], world[2] );

  convert_pixel_to_voxel( display, volume_index, x2, y2, v, &dummy);
  convert_voxel_to_world( volume, v, &w[0], &w[1], &w[2] );
  world[x_index] = w[x_index];

  fill_Point(*pt_end, world[0], world[1], world[2] );

  //printf("Finish %f %f %f\n", world[0], world[1], world[2] );

  return TRUE;
}

static int
get_plot_end_points( display_struct *display,
                     int volume_index,
                     VIO_Point *pt_start,
                     VIO_Point *pt_end,
                     VIO_Colour *colour)
{
  VIO_Real voxel[VIO_MAX_DIMENSIONS];
  int sizes[VIO_MAX_DIMENSIONS];
  int x_index;
  VIO_Volume volume = get_nth_volume( display, volume_index );
  int n_meas = display->slice.measure_number - 1;

  if (n_meas < 0)
    n_meas = N_MEASUREMENTS - 1;

  get_volume_sizes( volume, sizes );

  if ( display->slice.measure[n_meas].line != NULL )
  {
    *pt_start = display->slice.measure[n_meas].origin;
    *pt_end = display->slice.measure[n_meas].end;
    *colour = display->slice.measure[n_meas].colour;
    x_index = -1;
  }
  else
  {
    VIO_Real world_start[VIO_MAX_DIMENSIONS];
    int      axis_index = -1;
    int      view_index;

    *colour = WHITE;

    if (!get_slice_view_index_under_mouse( display, &view_index ))
      view_index = 0;

    get_current_voxel( display, volume_index, voxel );

    if (display->slice.intensity_plot_axis >= 0)
    {
      x_index = display->slice.intensity_plot_axis;
    }
    else
    {
      int dummy_index;

      if (get_volume_n_dimensions( volume ) > 3)
      {
        x_index = VIO_T;
        convert_voxel_to_world( volume, voxel, &world_start[VIO_X],
                                &world_start[VIO_Y],
                                &world_start[VIO_Z]);
        fill_Point(*pt_start, world_start[VIO_X],
                   world_start[VIO_Y], world_start[VIO_Z]);

        return VIO_T;
      }
      else if (get_axis_index_under_mouse( display, &dummy_index, &axis_index ))
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
        x_index = VIO_Y;
      }
    }

    get_pixels_extent( display, volume_index, view_index, x_index,
                       pt_start, pt_end );
  }
  return x_index;
}

/**
 * Removes all objects from the model, except for the plot and axis
 * lines objects.
 */
static void clear_intensity_plot( model_struct *model_ptr )
{
  /* Remove tick objects, if any.
   */
  while (model_ptr->n_objects > 3)
  {
    delete_object( model_ptr->objects[3] );
    remove_ith_object_from_model( model_ptr, 3 );
  }
}


static void
get_time_plot_data( VIO_Volume volume, 
                    int degrees_continuity,
                    const VIO_Point *pt_start,
                    int length,
                    VIO_Real data[], 
                    VIO_Real *min_data,
                    VIO_Real *max_data)
{
  VIO_Real voxel[VIO_MAX_DIMENSIONS];
  int i;
  double radius = 0;
  double delta = 1.0;
  int sizes[VIO_MAX_DIMENSIONS];

  get_volume_sizes( volume, sizes );

  convert_world_to_voxel( volume, 
                          Point_x(*pt_start),
                          Point_y(*pt_start),
                          Point_z(*pt_start), 
                          voxel );

  voxel[4] = 0;
  *min_data = +DBL_MAX;
  *max_data = -DBL_MAX;
  for (i = 0; i < length; i++) {
    double val;
    double x, y, z;

    voxel[VIO_T] = i;

    if (radius > 0.0) {
      double num = 0;
      double den = 0;
      VIO_Real t = i;
      for (x = voxel[VIO_X]-radius; x <= voxel[VIO_X]+radius; x += delta) {
        for (y = voxel[VIO_Y]-radius; y <= voxel[VIO_Y]+radius; y += delta) {
          for (z = voxel[VIO_Z]-radius; z <= voxel[VIO_Z]+radius; z += delta) {
            if (x >= 0 && x < sizes[VIO_X] &&
                y >= 0 && y < sizes[VIO_Y] &&
                z >= 0 && z < sizes[VIO_Z]) {
              num += get_volume_real_value( volume, x, y, z, t, 0);
              den += 1.0;
            }
          }
        }
      }
      if (den > 0) {
        val = num / den;
      }
      else {
        val = 0.0;
      }
    }
    else {
      evaluate_volume( volume, voxel, NULL,
                       degrees_continuity, FALSE, 0.0,
                       &val, NULL, NULL );
    }
    if (val > *max_data)
      *max_data = val;
    if (val < *min_data)
      *min_data = val;
    data[i] = val;
  }
}

static void
get_spatial_plot_data( VIO_Volume volume, 
                       VIO_Real distance, 
                       int n_samples,
                       int degrees_continuity,
                       const VIO_Point *pt_start,
                       const VIO_Point *pt_end,
                       VIO_Real data[], 
                       VIO_Real *min_data,
                       VIO_Real *max_data)
{
  int        i;
  VIO_Vector step;
  VIO_Real   world[VIO_MAX_DIMENSIONS];
  VIO_Real   voxel[VIO_MAX_DIMENSIONS];

  fill_Vector(step,
              Point_x(*pt_end) - Point_x(*pt_start),
              Point_y(*pt_end) - Point_y(*pt_start),
              Point_z(*pt_end) - Point_z(*pt_start));

  Vector_x(step) /= n_samples;
  Vector_y(step) /= n_samples;
  Vector_z(step) /= n_samples;
  
  world[VIO_X] = Point_x(*pt_start);
  world[VIO_Y] = Point_y(*pt_start);
  world[VIO_Z] = Point_z(*pt_start);

  *max_data = -DBL_MAX;
  *min_data = DBL_MAX;

  for (i = 0; i < n_samples; i++)
  {
    VIO_Real val;

    convert_world_to_voxel( volume, world[VIO_X], world[VIO_Y], world[VIO_Z],
                            voxel );

    evaluate_volume( volume, voxel, NULL,
                     degrees_continuity, FALSE, 0.0,
                     &val, NULL, NULL );

    if (val < *min_data)
      *min_data = val;
    if (val > *max_data)
      *max_data = val;
    data[i] = val;

    world[VIO_X] += Vector_x(step);
    world[VIO_Y] += Vector_y(step);
    world[VIO_Z] += Vector_z(step);
  }
}

#define TICK_LENGTH 10

static int
value_to_pixel_offset(VIO_Real value, VIO_Real min_value, VIO_Real range,
                      int n_pixels)
{
  return (value - min_value) / range * n_pixels;
}

void
rebuild_intensity_plot( display_struct *display )
{
  int volume_index = get_current_volume_index( display );
  if (volume_index < 0)
  {
    return;
  }

  int arb_view_index = get_arbitrary_view_index( display );

  model_struct *model_ptr = get_graphics_model( display, FULL_WINDOW_MODEL );

  clear_intensity_plot( model_ptr );

  if (get_slice_visibility( display, volume_index, arb_view_index ))
  {
    set_object_visibility( model_ptr->objects[1], FALSE );
    set_object_visibility( model_ptr->objects[2], FALSE );
    return;
  }

  VIO_Volume volume = get_nth_volume( display, volume_index );
  VIO_Real min_value, max_value;
  VIO_Real data[10000];
  VIO_BOOL use_volume_range = FALSE;

  get_volume_real_range( volume, &min_value, &max_value );

  char temp[128];
  snprintf( temp, sizeof(temp) - 1, "%g", max_value );

  int x_index;
  VIO_Point pt_start, pt_end;
  VIO_Colour plot_colour;
  VIO_Real min_data, max_data;
  VIO_Real distance;
  int n_samples;

  x_index = get_plot_end_points( display, volume_index, &pt_start, &pt_end,
                                 &plot_colour);
  if (x_index == VIO_T)
  {
    int sizes[VIO_MAX_DIMENSIONS];
    get_volume_sizes( volume, sizes );

    distance = sizes[VIO_T];

    n_samples = VIO_ROUND(distance);
    get_time_plot_data( volume,
                        display->slice.degrees_continuity,
                        &pt_start,
                        distance,
                        data, &min_data, &max_data);
  }
  else
  {
    VIO_Real min_step = get_volume_min_step(display, volume_index);

    distance = distance_between_points( &pt_start, &pt_end );

    n_samples = VIO_ROUND(distance / min_step);

    get_spatial_plot_data( volume, distance,
                           n_samples,
                           display->slice.degrees_continuity,
                           &pt_start, &pt_end,
                           data, &min_data, &max_data );
  }

  if (!use_volume_range)
  {
    min_value = floor(min_data);
    max_value = ceil(max_data);
  }

  object_struct *object_ptr = model_ptr->objects[1];
  lines_struct *lines_ptr = get_lines_ptr( object_ptr );

  delete_lines( lines_ptr );

  initialize_lines( lines_ptr, plot_colour );

  lines_ptr->n_points = 0;
  lines_ptr->n_items = 0;
  lines_ptr->line_thickness = 1.0;

  int x_min, x_max;
  int y_min, y_max;

  get_slice_viewport( display, arb_view_index,
                      &x_min, &x_max, &y_min, &y_max);

  const int cx_axis = TICK_LENGTH + G_get_text_length( temp,
                                                       Colour_bar_text_font,
                                                       Colour_bar_text_size );
  const int cy_axis = 20;
  VIO_Real y_range = (y_max - y_min) - cy_axis;
  VIO_Real x_range = (x_max - x_min) - cx_axis - 8;
  VIO_Point pt;

  VIO_Real d_range = max_value - min_value;
  int i;

  for (i = 0; i < n_samples; i++)
  {
    Point_x(pt) = x_min + cx_axis + x_range * (VIO_Real) i / n_samples;
    Point_y(pt) = y_min + cy_axis + y_range * (data[i] - min_value) / d_range;
    Point_z(pt) = 0.0;
    add_point_to_line( lines_ptr, &pt );
  }

  set_object_visibility( object_ptr, TRUE );

  /*
   * Set up the axes and tick marks.
   */
  object_ptr = model_ptr->objects[2];
  lines_ptr = get_lines_ptr( object_ptr );

  delete_lines( lines_ptr );

  initialize_lines( lines_ptr, WHITE );

  /* First arrange the axes.
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
  VIO_Real x_start, x_end;

  switch (x_index)
  {
  case VIO_X:
    x_start = Point_x(pt_start);
    x_end = Point_x(pt_end);
    break;
  case VIO_Y:
    x_start = Point_y(pt_start);
    x_end = Point_y(pt_end);
    break;
  case VIO_Z:
    x_start = Point_z(pt_start);
    x_end = Point_z(pt_end);
    break;
  default:
    x_start = 0;
    x_end = distance;
    break;
  }

  VIO_Real min_tick = get_nice_value( x_start );
  VIO_Real max_tick = x_end;
  VIO_Real cur_tick;

  if (d_tick <= 0)
    d_tick = 1;

  for (cur_tick = min_tick; cur_tick <= max_tick; cur_tick += d_tick)
  {
    start_new_line( lines_ptr );
    Point_x(pt) = x_min + cx_axis + value_to_pixel_offset(cur_tick,
                                                          x_start,
                                                          distance,
                                                          x_range);
    Point_y(pt) = y_min + cy_axis;
    Point_z(pt) = 0;
    add_point_to_line( lines_ptr, &pt );
    Point_y(pt) -= TICK_LENGTH;
    add_point_to_line( lines_ptr, &pt );

    object_ptr = create_tick_label( cur_tick, &pt, TRUE, TICK_LENGTH );
    add_object_to_model( model_ptr, object_ptr );
  }


  /* Add the Y-axis tick marks.
   */
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
                                    &pt, FALSE, TICK_LENGTH );
    add_object_to_model( model_ptr, object_ptr );

    Point_x(pt) -= TICK_LENGTH;
    add_point_to_line( lines_ptr, &pt);
  }

  /* 
   * Finally add the X-axis label.
   */
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

  set_slice_viewport_update( display, FULL_WINDOW_MODEL );
}

DEF_MENU_FUNCTION(toggle_intensity_plot_scaling)
{
  return VIO_OK;
}

DEF_MENU_UPDATE(toggle_intensity_plot_scaling)
{
  return VIO_OK;
}

DEF_MENU_FUNCTION(toggle_intensity_plot_axis)
{
  return VIO_OK;
}

DEF_MENU_UPDATE(toggle_intensity_plot_axis)
{
  return VIO_OK;
}


