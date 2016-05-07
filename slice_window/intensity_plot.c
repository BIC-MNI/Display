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
#include <assert.h>

/**
 * Length of a intensity plot graph tick, in pixels.
 */
#define IP_TICK_LENGTH 10

/**
 * Object index of the plot (the actual graph curve) in the model.
 */
#define IP_PLOT_OBJ_IND 1

/**
 * Object index of the axes (lines and ticks) in the model.
 */
#define IP_AXIS_OBJ_IND 2

/**
 * Maximum number of data points that can be displayed. This is a
 * lame thing to constrain and it should probably be fixed.
 */
#define IP_MAX_DATA 10000

/**
 * The format to use for tick labels.
 */
const char tick_label_format[] = "%.4g";

/**
 * Text axis names.
 */
static char *axis_names[] = { "X", "Y", "Z", "T", "V" };

/**
 * Truncate a number to have exactly \c n_digits significant digits.
 *
 * \param value The value to truncate.
 * \param n_digits The number of signficant digits to preserve.
 * \returns The truncated value.
 */
VIO_Real trunc_to_n_digits(VIO_Real value, int n_digits)
{
  if (value != 0)
  {
    VIO_Real scale = pow(10.0, floor(log10(fabs(value))) + 1 - n_digits);
    value = scale * trunc(value / scale);
  }
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
  display->slice.intensity_plot_is_enabled = TRUE;
  display->slice.intensity_plot_is_scaled = TRUE;
}

/**
 * \brief Create a tick label for an intensity plot.
 *
 * TODO: combine with the function of the same name in slice_window/rulers.c
 * \param position The integer position value to be formatted and displayed.
 * \param tick_pt The pixel position of the tick (used to position the label).
 * \param is_horiz_axis TRUE if this label is for the horizontal axis,
 * otherwise it goes with the vertical axis.
 * \returns An pointer to a text object, or NULL if an error occurs.
 */
static object_struct *
create_tick_label(VIO_Real position, const VIO_Point *tick_pt,
                  VIO_BOOL is_horiz_axis)
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

  /* In some situations, the position value may be close to but not quite
   * zero. The sprintf() format we use will tend to do bad things in this
   * case, so it is useful to detect very small values of the position and
   * manually replace them with a real zero. This could be wrong if we are
   * looking at truly tiny structures (a few nanometres?)
   */
  if (fabs(position) < 1e-8)
    position = 0;

  snprintf( label, sizeof( label ) - 1, tick_label_format, position );

  cw = G_get_text_length( label, Colour_bar_text_font, Colour_bar_text_size );

  if ( is_horiz_axis )
  {
    fill_Point(pt,
               Point_x(*tick_pt) - cw / 2.0 + 0.5,
               Point_y(*tick_pt), 0);
  }
  else
  {
    ch = G_get_text_height( Colour_bar_text_font, Colour_bar_text_size );
    fill_Point(pt,
               Point_x( *tick_pt ) - cw,
               Point_y( *tick_pt ) - ch / 2.0 + 1, 0);
  }

  text_ptr = get_text_ptr( object_ptr );
  initialize_text( text_ptr, &pt, Colour_bar_text_colour, Colour_bar_text_font,
                   Colour_bar_text_size );
  text_ptr->string = create_string( label );
  return object_ptr;
}

/**
 * \brief Calculate the field of view of the slice pixels, in world units.
 *
 * To determine the domain of an intensity plot, we get the pixels object
 * for a given slice, and calculate its world coordinates with respect to
 * the desired horizontal axis (X, Y, or Z).
 *
 * \param display A pointer to the display_struct of the slice view window.
 * \param volume_index The index of the relevant loaded volume.
 * \param view_index The index of the current view.
 * \param horiz_axis_index The index of the relevant axis.
 * \param start The world coordinates of the start (left or lower) position.
 * \param end The world coordinates of the end (right or upper) position.
 * \returns TRUE if the calculation was performed successfully.
 */
VIO_BOOL
get_pixels_extent( display_struct *display, int volume_index, int view_index,
                   int horiz_axis_index,
                   VIO_Real start[], VIO_Real end[] )
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
  int i;

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

  /* Get the viewport in pixel coordinates for this view.
   * The viewport coordinates are relative to the entire window.
   */
  get_slice_viewport( display, view_index, &x_min, &x_max, &y_min, &y_max );

  x1 = pixels_ptr->x_position + x_min;
  y1 = pixels_ptr->y_position + y_min;

  if (horiz_axis_index == VIO_X || horiz_axis_index == VIO_Y)
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
  world[horiz_axis_index] = w[horiz_axis_index];

  for (i = 0; i < VIO_N_DIMENSIONS; i++)
    start[i] = world[i];

  convert_pixel_to_voxel( display, volume_index, x2, y2, v, &dummy);
  convert_voxel_to_world( volume, v, &w[0], &w[1], &w[2] );
  world[horiz_axis_index] = w[horiz_axis_index];

  for (i = 0; i < VIO_N_DIMENSIONS; i++)
    end[i] = world[i];

  return TRUE;
}

/**
 * \brief Calculate the end points of the plot.
 *
 * Gets the start and end points of the plot in world coordinates.
 * Also determines the colour to use for the plot line and the
 * volume axis index to use for the horizontal axis.
 *
 * \param display The display_struct of the slice view window.
 * \param volume_index The index of the currently-selected volume.
 * \param start The start point for the data.
 * \param end The ending point for the data.
 * \param colour The colour to use for the plot.
 * \returns The index of the volume axis to use, or -1 if there points
 * are not parallel to an axis.
 */
static int
get_plot_end_points( display_struct *display,
                     int volume_index,
                     VIO_Real start[],
                     VIO_Real end[],
                     VIO_Colour *colour)
{
  int        horiz_axis_index;
  VIO_Volume volume = get_nth_volume( display, volume_index );
  int        n_meas = display->slice.measure_number - 1;

  if ( n_meas < 0 )
    n_meas = N_MEASUREMENTS - 1;

  if ( display->slice.measure[n_meas].line != NULL )
  {
    start[VIO_X] = Point_x( display->slice.measure[n_meas].origin );
    start[VIO_Y] = Point_y( display->slice.measure[n_meas].origin );
    start[VIO_Z] = Point_z( display->slice.measure[n_meas].origin );
    end[VIO_X] = Point_x( display->slice.measure[n_meas].end );
    end[VIO_Y] = Point_y( display->slice.measure[n_meas].end );
    end[VIO_Z] = Point_z( display->slice.measure[n_meas].end );
    *colour = display->slice.measure[n_meas].colour;
    horiz_axis_index = -1;
  }
  else
  {
    int      axis_index = -1;
    int      view_index;

    *colour = WHITE;

    if (!get_slice_view_index_under_mouse( display, &view_index ))
      view_index = 0;

    if (display->slice.intensity_plot_axis >= 0)
    {
      horiz_axis_index = display->slice.intensity_plot_axis;
      /* If the user has selected a specific horizontal axis, we
       * need to also set the appropriate view index for determining
       * the right zoom and pan values.
       */
      switch (horiz_axis_index)
      {
      case VIO_X:
        view_index = 2;
        break;
      case VIO_Y:
        view_index = 0;
        break;
      case VIO_Z:
        view_index = 1;
        break;
      }
    }
    else
    {
      int dummy_index;

      if (get_volume_n_dimensions( volume ) > VIO_N_DIMENSIONS)
      {
        int label;
        VIO_Real r_voxel[VIO_MAX_DIMENSIONS];
        int i_voxel[VIO_MAX_DIMENSIONS];
        int i;

        horiz_axis_index = VIO_T;

        get_current_voxel( display, volume_index, r_voxel );

        convert_voxel_to_world( volume, r_voxel,
                                &start[VIO_X],
                                &start[VIO_Y],
                                &start[VIO_Z]);

        convert_real_to_int_voxel( get_volume_n_dimensions( volume ),
                                   r_voxel, i_voxel );
        label = get_voxel_label( display, volume_index,
                                 i_voxel[VIO_X], i_voxel[VIO_Y], i_voxel[VIO_Z]);
        if ( label != 0 )
          *colour = get_colour_of_label( display, volume_index, label );

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
        switch (axis_index)
        {
        case VIO_X:
          horiz_axis_index = VIO_Y; /* width */
          break;
        case VIO_Y:
          horiz_axis_index = VIO_Z; /* height */
          break;
        default:
          horiz_axis_index = VIO_X; /* width */
          break;
        }
      }
      else
      {
        horiz_axis_index = VIO_Y;
        axis_index = VIO_X;
      }
    }

    get_pixels_extent( display, volume_index, view_index, horiz_axis_index,
                       start, end );
  }
  return horiz_axis_index;
}

/**
 * Removes all intensity plot objects from the model, except for the
 * plot and axis lines objects. Removes all of the text labels.
 *
 * \param model_ptr The model containing the intensity plot objects.
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


/** Simple structure used to accumulate data in a volume when we are
 * plotting the timecourse of a label.
 */
struct accumulator_struct
{
  VIO_Volume volume; /**< The volume from which we are accumulating data. */
  int time;      /**< Time point associated with this accumulation. */
  double total;  /**< Total value of all points accumulated. */
  int n_points;  /**< Number of points accumulated. */
};

/**
 * Helper function called in sparse_array_apply().
 * \param n_dimensions Number of dimensions in the sparse array.
 * \param coord The coordinates of this location.
 * \param value The value stored at this coordinate.
 * \param data The address of an accumulator_struct.
 */
static void apply_accumulate( int n_dimensions, const int *coord, int value,
                              void *data )
{
  struct accumulator_struct *acc_ptr = (struct accumulator_struct *) data;

  acc_ptr->total += get_volume_real_value( acc_ptr->volume, coord[VIO_X], 
                                           coord[VIO_Y], coord[VIO_Z],
                                           acc_ptr->time, 0);
  acc_ptr->n_points++;
}

/**
 * \brief Get plot data from a time axis.
 *
 * \param volume The volume containing the data.
 * \param label_volume The label volume associated with the volume.
 * \param degrees_continuity The type of interpolation to perform.
 * \param n_samples The number of samples to take along the plot.
 * \param origin The spatial origin of the data collection.
 * \param data The buffer for the data samples (OUTPUT).
 * \param min_data The minimum value in the data buffer (OUTPUT).
 * \param max_data The maximum value in the data buffer (OUTPUT).
 */
static void
get_time_plot_data( VIO_Volume volume,
                    VIO_Volume label_volume,
                    int degrees_continuity,
                    VIO_Real origin[],
                    int n_samples,
                    VIO_Real data[],
                    VIO_Real *min_data,
                    VIO_Real *max_data)
{
  VIO_Real r_voxel[VIO_MAX_DIMENSIONS];
  int i_voxel[VIO_MAX_DIMENSIONS];
  int i;
  int label;
  int sizes[VIO_MAX_DIMENSIONS];
  sparse_array_t label_set;

  get_volume_sizes( volume, sizes );

  convert_world_to_voxel( volume,
                          origin[VIO_X],
                          origin[VIO_Y],
                          origin[VIO_Z],
                          r_voxel );

  convert_real_to_int_voxel( get_volume_n_dimensions( volume ),
                             r_voxel, i_voxel );

  label = get_3D_volume_label_data( label_volume,
                                    i_voxel[VIO_X],
                                    i_voxel[VIO_Y],
                                    i_voxel[VIO_Z] );

  sparse_array_initialize( &label_set, VIO_N_DIMENSIONS );

  if ( label != 0 )
  {
    int coord[VIO_N_DIMENSIONS];

    for (coord[VIO_X] = 0; coord[VIO_X] < sizes[VIO_X]; coord[VIO_X]++)
      for (coord[VIO_Y] = 0; coord[VIO_Y] < sizes[VIO_Y]; coord[VIO_Y]++)
        for (coord[VIO_Z] = 0; coord[VIO_Z] < sizes[VIO_Z]; coord[VIO_Z]++)
          if ( get_3D_volume_label_data( label_volume,
                                         coord[VIO_X],
                                         coord[VIO_Y],
                                         coord[VIO_Z] ) == label )
            sparse_array_insert( &label_set, coord, 0 );
  }

  r_voxel[VIO_V] = 0;
  *min_data = +DBL_MAX;
  *max_data = -DBL_MAX;
  for (i = 0; i < n_samples; i++)
  {
    double val;
    double x, y, z;

    r_voxel[VIO_T] = i;

    if ( label_set.n_entries > 0 )
    {
      struct accumulator_struct acc;

      acc.time = i;
      acc.volume = volume;
      acc.total = 0;
      acc.n_points = 0;

      sparse_array_apply( &label_set, &acc, apply_accumulate );

      if (acc.n_points > 0)
      {
        val = acc.total / acc.n_points;
      }
      else
      {
        val = 0.0;
      }
    }
    else
    {
      evaluate_volume( volume, r_voxel, NULL,
                       degrees_continuity, FALSE, 0.0,
                       &val, NULL, NULL );
    }
    if (val > *max_data)
      *max_data = val;
    if (val < *min_data)
      *min_data = val;
    data[i] = val;
  }

  sparse_array_free( &label_set );
}

/**
 * \brief Get plot data from a spatial axis.
 *
 * \param volume The volume containing the data.
 * \param distance The total length of the plot in world units.
 * \param n_samples The number of samples to take along the plot.
 * \param degrees_continuity The type of interpolation to perform.
 * \param start The starting point of the data collection.
 * \param end The ending point of the data collection.
 * \param data The buffer for the data samples (OUTPUT).
 * \param min_data The minimum value in the data buffer (OUTPUT).
 * \param max_data The maximum value in the data buffer (OUTPUT).
 */
static void
get_spatial_plot_data( VIO_Volume volume,
                       VIO_Real distance,
                       int n_samples,
                       int degrees_continuity,
                       const VIO_Real start[],
                       const VIO_Real end[],
                       VIO_Real data[],
                       VIO_Real *min_data,
                       VIO_Real *max_data)
{
  int        i;
  VIO_Vector step;
  VIO_Real   world[VIO_MAX_DIMENSIONS];
  VIO_Real   voxel[VIO_MAX_DIMENSIONS];

  fill_Vector(step,
              end[VIO_X] - start[VIO_X],
              end[VIO_Y] - start[VIO_Y],
              end[VIO_Z] - start[VIO_Z]);

  Vector_x(step) /= n_samples;
  Vector_y(step) /= n_samples;
  Vector_z(step) /= n_samples;

  world[VIO_X] = start[VIO_X];
  world[VIO_Y] = start[VIO_Y];
  world[VIO_Z] = start[VIO_Z];

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


/**
 * \brief Calculate the width of a tick label.
 *
 * \param value The value for which the tick label width should be calculated.
 * \returns The width of the label in pixels.
 */
static int
get_tick_label_width( VIO_Real value )
{
  char temp[128];
  snprintf( temp, sizeof(temp) - 1, tick_label_format, value );
  return G_get_text_length( temp, Colour_bar_text_font, Colour_bar_text_size );
}

/**
 * \brief Lay out the tick lines for the plot.
 *
 * \param model_ptr The model into which we should insert new objects.
 * \param lines_ptr The lines object for the tick marks.
 * \param min_value The minimum value along this axis.
 * \param max_value The maximum value along this axis.
 * \param n_pixels The number of pixels along this axis.
 * \param horz_offset The horizontal offset of the plot within the window.
 * \param vert_offset The vertical offset of the plot within the window.
 * \param is_horz_axis TRUE if this is a horizontal axis.
 */
static void
create_tick_lines( model_struct *model_ptr, lines_struct *lines_ptr,
                   VIO_Real min_value, VIO_Real max_value, VIO_Real n_pixels,
                   int horz_offset, int vert_offset,
                   VIO_BOOL is_horz_axis )
{
  VIO_Real range = max_value - min_value;
  VIO_Real min_tick;
  VIO_Real max_tick = max_value;
  VIO_Real delta_tick;
  VIO_Point pt1, pt2;
  VIO_Point ptLabel;
  VIO_Real cur_tick;
  object_struct *object_ptr;
  int n_digits;
  const int min_tick_width = 30;    /* minimum width of a tick. */
  const int min_n_ticks = 2;

  int desired_ticks = n_pixels / min_tick_width;
  if (desired_ticks < min_n_ticks)
    desired_ticks = min_n_ticks;

  delta_tick = trunc_to_n_digits( range / desired_ticks, 1 );
  if (delta_tick <= 1e-10)      /* Make sure it's positive */
    delta_tick = 1;

  /* Calculate a good value to use for the lowest tick mark.
   * This is done by truncating the minimum value to a fixed
   * number of significant digits. We don't want the truncated
   * value to be more than one delta less than the actual
   * minimum, so we keep increasing the number of significant
   * digits until we get something good.
   */
  for (n_digits = 1; n_digits < 5; n_digits++)
  {
    min_tick = trunc_to_n_digits( min_value, n_digits );
    if ( fabs(min_tick - min_value) < delta_tick )
      break;
  }

  for (cur_tick = min_tick; cur_tick <= max_tick; cur_tick += delta_tick)
  {
    VIO_Real tick_pos = (cur_tick - min_value) * n_pixels / range;

    /* Don't draw ticks on the wrong side of the other axis:
     */
    if (tick_pos < 0)
      continue;

    start_new_line( lines_ptr );
    if (is_horz_axis)
    {
      fill_Point( pt1, horz_offset + tick_pos, vert_offset, 0 );
      fill_Point( pt2, horz_offset + tick_pos, vert_offset - IP_TICK_LENGTH, 0 );
      fill_Point( ptLabel, Point_x( pt2 ), Point_y( pt2 ) - IP_TICK_LENGTH, 0 );
    }
    else
    {
      fill_Point(pt1, horz_offset, vert_offset + tick_pos, 0);
      fill_Point(pt2, horz_offset - IP_TICK_LENGTH, vert_offset + tick_pos, 0);
      ptLabel = pt2;
    }
    add_point_to_line( lines_ptr, &pt1 );
    add_point_to_line( lines_ptr, &pt2 );

    object_ptr = create_tick_label( cur_tick, &ptLabel, is_horz_axis );
    add_object_to_model( model_ptr, object_ptr );
  }
}

/**
 * Somewhat more general than distance_between_points(), this function
 * is intended to calculate the Euclidean distance between any two 
 * vectors of arbitrary dimensionality.
 *
 * \param v1 The first vector with \c n_dims components.
 * \param v2 The second vector with \c n_dims components.
 * \param n_dims The number of dimensions in the vectors.
 * \returns The distance between the two vectors.
 */
VIO_Real
euclidean_distance( const VIO_Real v1[], const VIO_Real v2[], int n_dims )
{
  int i;
  VIO_Real total = 0;
  for (i = 0; i < n_dims; i++)
  {
    VIO_Real diff = v1[i] - v2[i];
    total += diff * diff;
  }
  return sqrt(total);
}

/**
 * \brief Lay out the intensity plot.
 *
 * Based on the current settings, loaded volume(s), cursor position, and
 * measurement lines, this function will fill in all of the graphical
 * objects needed to visualize the intensity of the volume along the selected
 * axis.
 *
 * \param display The display_struct of the slice view window.
 */
void
rebuild_intensity_plot( display_struct *display )
{
  int volume_index = get_current_volume_index( display );
  int arb_view_index = get_arbitrary_view_index( display );
  model_struct *model_ptr = get_graphics_model( display, FULL_WINDOW_MODEL );
  VIO_Volume volume;
  VIO_Real *data;
  int horiz_axis_index;
  VIO_Real start[VIO_N_DIMENSIONS], end[VIO_N_DIMENSIONS];
  VIO_Colour plot_colour;
  VIO_Real min_data, max_data;
  VIO_Real plot_domain;
  int n_samples;
  VIO_Real min_value, max_value;
  object_struct *object_ptr;
  lines_struct *lines_ptr;
  int x_min, x_max;
  int y_min, y_max;
  VIO_Real x_start, x_end;

  assert( display->window_type == SLICE_WINDOW );

  clear_intensity_plot( model_ptr );

  /* We only display the intensity plot if it is explicitly enabled,
   * a volume is loaded, and the oblique (arbitrary) view is currently off.
   */
  if ( !display->slice.intensity_plot_is_enabled ||
       volume_index < 0 ||
       get_slice_visibility( display, volume_index, arb_view_index ))
  {
    set_object_visibility( model_ptr->objects[IP_PLOT_OBJ_IND], FALSE );
    set_object_visibility( model_ptr->objects[IP_AXIS_OBJ_IND], FALSE );
    return;
  }

  volume = get_nth_volume( display, volume_index );

  horiz_axis_index = get_plot_end_points( display, volume_index,
                                          start, end, &plot_colour );

  if ( horiz_axis_index == VIO_T )
  {
    int sizes[VIO_MAX_DIMENSIONS];
    get_volume_sizes( volume, sizes );

    plot_domain = n_samples = sizes[VIO_T];

    ALLOC( data, n_samples );

    get_time_plot_data( volume,
                        get_nth_label_volume( display, volume_index ),
                        display->slice.degrees_continuity,
                        start,
                        n_samples,
                        data, &min_data, &max_data );
  }
  else
  {
    VIO_Real min_step = get_volume_min_step(display, volume_index);

    plot_domain = euclidean_distance( start, end, VIO_N_DIMENSIONS );

    n_samples = VIO_ROUND(plot_domain / min_step);

    ALLOC( data, n_samples );

    get_spatial_plot_data( volume, plot_domain,
                           n_samples,
                           display->slice.degrees_continuity,
                           start, end,
                           data, &min_data, &max_data );
  }

  if ( display->slice.intensity_plot_is_scaled )
  {
    min_value = floor( min_data );
    max_value = ceil( max_data );
  }
  else
  {
    get_volume_real_range( volume, &min_value, &max_value );
  }

  object_ptr = model_ptr->objects[IP_PLOT_OBJ_IND];
  lines_ptr = get_lines_ptr( object_ptr );

  delete_lines( lines_ptr );

  initialize_lines( lines_ptr, plot_colour );

  get_slice_viewport( display, arb_view_index, &x_min, &x_max, &y_min, &y_max);

  const int cx_axis = IP_TICK_LENGTH + get_tick_label_width( max_value );
  const int cy_axis = 20;
  const int horz_offset = x_min + cx_axis;
  const int vert_offset = y_min + cy_axis;
  const VIO_Real plot_height = (y_max - y_min) - cy_axis;
  const VIO_Real plot_width = (x_max - x_min) - cx_axis - 8;
  const VIO_Real plot_range = max_value - min_value;
  VIO_Point pt;
  int i;

  /* Lay out the data plot itself.
   */
  for (i = 0; i < n_samples; i++)
  {
    fill_Point(pt,
               horz_offset + plot_width * (VIO_Real) i / n_samples,
               vert_offset + plot_height * (data[i] - min_value) / plot_range,
               0.0);
    add_point_to_line( lines_ptr, &pt );
  }

  set_object_visibility( object_ptr, TRUE );

  /*
   * Set up the axes and tick marks.
   */
  object_ptr = model_ptr->objects[IP_AXIS_OBJ_IND];
  lines_ptr = get_lines_ptr( object_ptr );

  delete_lines( lines_ptr );

  initialize_lines( lines_ptr, WHITE );

  set_object_visibility( object_ptr, TRUE );

  /* Set the positions and lengths of the axis lines.
   */
  start_new_line( lines_ptr );
  fill_Point( pt, horz_offset, vert_offset + plot_height, 0 );
  add_point_to_line( lines_ptr, &pt );
  Point_y( pt ) = vert_offset;
  add_point_to_line( lines_ptr, &pt );
  Point_x( pt ) = horz_offset + plot_width;
  add_point_to_line( lines_ptr, &pt );

  /* Add the horizontal axis tick marks.
   */
  if (horiz_axis_index < 0 || horiz_axis_index >= VIO_N_DIMENSIONS )
  {
    x_start = 0;
    x_end = plot_domain;
  }
  else
  {
    x_start = start[horiz_axis_index];
    x_end = end[horiz_axis_index];
  }

  create_tick_lines( model_ptr, lines_ptr, x_start, x_end, plot_width, horz_offset, vert_offset, TRUE );

  /* Add the Y-axis tick marks.
   */
  create_tick_lines( model_ptr, lines_ptr, min_value, max_value, plot_height, horz_offset, vert_offset, FALSE );


  /*
   * Finally add the X-axis label.
   */
  object_ptr = create_object( TEXT );
  fill_Point(pt, horz_offset + plot_width, y_min, 0);
  text_struct *text_ptr = get_text_ptr( object_ptr );
  initialize_text( text_ptr, &pt, Colour_bar_text_colour, Colour_bar_text_font,
                   Colour_bar_text_size + 2 );

  if ( horiz_axis_index < 0 )
  {
    text_ptr->string = create_string( "mm" );
  }
  else
  {
    text_ptr->string = create_string( axis_names[horiz_axis_index] );
  }
  add_object_to_model( model_ptr, object_ptr );

  set_slice_viewport_update( display, FULL_WINDOW_MODEL );
  FREE( data );
}

/**
 * \brief Menu command to enable or disable the intensity plot.
 *
 * \param display The display_struct of a top-level window.
 * \param menu_window The display_struct of the menu window.
 * \param menu_entry The menu entry associated with this command.
 * \returns VIO_OK if the command completed without error.
 */
DEF_MENU_FUNCTION(toggle_intensity_plot_enabled)
{
  display_struct *slice_window;

  if (get_slice_window( display, &slice_window ))
  {
    slice_window->slice.intensity_plot_is_enabled =
      !slice_window->slice.intensity_plot_is_enabled;
    set_slice_cursor_update( slice_window, -1 );
  }
  return VIO_OK;
}

/**
 * Handle menu updates for the toggle_intensity_plot_enabled menu command.
 */
DEF_MENU_UPDATE(toggle_intensity_plot_enabled)
{
  display_struct *slice_window;

  if (!get_slice_window( display, &slice_window ))
    return FALSE;

  set_menu_text_on_off( menu_window, menu_entry,
                        slice_window->slice.intensity_plot_is_enabled );
  return TRUE;
}

/**
 * \brief Menu command to select the scaling mode of the intensity plot.
 *
 * The intensity plot Y-axis can be scaled to the actual range of the
 * current data domain, or to the full range of the volume. This
 * selects which of those options is enabled.
 *
 * \param display The display_struct of a top-level window.
 * \param menu_window The display_struct of the menu window.
 * \param menu_entry The menu entry associated with this command.
 * \returns VIO_OK if the command completed without error.
 */
DEF_MENU_FUNCTION(toggle_intensity_plot_scaling)
{
  display_struct *slice_window;

  if (get_slice_window( display, &slice_window ))
  {
    slice_window->slice.intensity_plot_is_scaled =
      !slice_window->slice.intensity_plot_is_scaled;
    set_slice_cursor_update( slice_window, -1 );
  }
  return VIO_OK;
}

/**
 * Handle menu updates for the toggle_intensity_plot_scaling menu command.
 */
DEF_MENU_UPDATE(toggle_intensity_plot_scaling)
{
  display_struct *slice_window;

  if (!get_slice_window( display, &slice_window ))
    return FALSE;

  set_menu_text_on_off( menu_window, menu_entry,
                        slice_window->slice.intensity_plot_is_scaled);
  return slice_window->slice.intensity_plot_is_enabled;
}

/**
 * \brief Menu command to select the intensity plot axis.
 *
 * By default we are in "Auto" mode, where the intensity plot is selected
 * automatically. This command allows us to cycle through the valid axes of
 * the loaded volume, and force display of only that axis.
 *
 * \param display The display_struct of a top-level window.
 * \param menu_window The display_struct of the menu window.
 * \param menu_entry The menu entry associated with this command.
 * \returns VIO_OK if the command completed without error.
 */
DEF_MENU_FUNCTION(toggle_intensity_plot_axis)
{
  display_struct *slice_window;

  if (get_slice_window( display, &slice_window ))
  {
    int volume_index = get_current_volume_index( slice_window );
    VIO_Volume volume = get_nth_volume( slice_window, volume_index );
    int n_dimensions = get_volume_n_dimensions( volume );

    slice_window->slice.intensity_plot_axis++;
    if ( slice_window->slice.intensity_plot_axis >= n_dimensions )
      slice_window->slice.intensity_plot_axis = -1; /* To automatic */

    set_slice_cursor_update( slice_window, -1 );
  }
  return VIO_OK;
}

/**
 * Handle menu updates for the toggle_intensity_plot_axis menu command.
 */
DEF_MENU_UPDATE(toggle_intensity_plot_axis)
{
  display_struct *slice_window;
  char *axis_name;

  if (!get_slice_window( display, &slice_window ))
    return FALSE;

  if (slice_window->slice.intensity_plot_axis < 0)
    axis_name = "Auto";
  else
    axis_name = axis_names[slice_window->slice.intensity_plot_axis];
  set_menu_text_string( menu_window, menu_entry, axis_name );
  return slice_window->slice.intensity_plot_is_enabled;
}
