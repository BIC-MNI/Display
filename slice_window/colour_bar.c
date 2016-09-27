/**
 * \file colour_bar.c
 * \brief Draw and maintain the "colour bar" widget.
 *
 * The colour bar is the widget that generally appears on the left side
 * of the slice window. It displays the mapping between voxel values and
 * colours, and allows the user to select the lower and upper
 * bound of the colour map.
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

#include  <display.h>

static  VIO_Real  get_y_pos(
    VIO_Real    value,
    VIO_Real    min_value,
    VIO_Real    max_value,
    VIO_Real    bottom,
    VIO_Real    top );

typedef enum
{
    HISTOGRAM,
    BAR,
    TICKS,
    FIRST_TEXT
} Colour_bar_objects;

/**
 * Create the graphical object that will be used to display the colour bar
 * object itself. This used to be a quadmesh, but we now use a polygons
 * object. The appearance in the UI should be identical.
 * \returns The newly-created object.
 */
static object_struct *
create_colour_bar_object(void)
{
    object_struct   *object_ptr = create_object( POLYGONS );
    VIO_Surfprop    spr = { 1, 0, 0, 0, 1 }; /* surface properties */
    polygons_struct *polygons_ptr = get_polygons_ptr( object_ptr );
    int             n_vertices = 2 * Colour_bar_resolution;
    int             i;

    initialize_polygons( polygons_ptr, WHITE, &spr );

    polygons_ptr->colour_flag = PER_VERTEX_COLOURS;
    REALLOC( polygons_ptr->colours, n_vertices );
    polygons_ptr->n_points = n_vertices;
    polygons_ptr->n_items = (Colour_bar_resolution - 1) * 2;
    REALLOC( polygons_ptr->points, n_vertices );
    REALLOC( polygons_ptr->indices, polygons_ptr->n_items * 3);
    REALLOC( polygons_ptr->end_indices, polygons_ptr->n_items );

    for (i = 0; i < polygons_ptr->n_items; i += 2)
    {
      int n = i * 3;

      /* Each 'row' of the colour bar consists of two triangles, which
       * are combined to make a thin rectangle.
       */
      polygons_ptr->indices[n + 0] = i + 0;
      polygons_ptr->indices[n + 1] = i + 1;
      polygons_ptr->indices[n + 2] = i + 2;
      polygons_ptr->end_indices[i + 0] = n + 3;

      polygons_ptr->indices[n + 3] = i + 1;
      polygons_ptr->indices[n + 4] = i + 2;
      polygons_ptr->indices[n + 5] = i + 3;
      polygons_ptr->end_indices[i + 1] = n + 6;
    }
    return object_ptr;
}

/**
 * Update the position and colours of the colour bar object.
 * \param object_ptr The object to update.
 * \param left The left (minimum x) coordinate of the colour bar.
 * \param right The right (maximum x) coordinate of the colour bar.
 * \param bottom The bottom (minimum y) coordinate of the colour bar.
 * \param top The top (maximum y) coordinate of the colour bar.
 * \param min_value The minimum value of the overall data range. 
 * \param max_value The maximum value of the overall data range.
 * \param ccs_ptr A pointer to the colour coding structure.
 */
static void
update_colour_bar_object( object_struct        *object_ptr, 
                          VIO_Real             left,
                          VIO_Real             right,
                          VIO_Real             bottom, 
                          VIO_Real             top,
                          VIO_Real             min_value,
                          VIO_Real             max_value,
                          colour_coding_struct *ccs_ptr)
{
  int i;
  polygons_struct *polygons = get_polygons_ptr( object_ptr );
  VIO_Real ratio, value, y;
  VIO_Colour colour;
  VIO_Real range = (VIO_Real) (Colour_bar_resolution - 1);

  for_less( i, 0, Colour_bar_resolution )
  {
    ratio = (VIO_Real) i / (VIO_Real) range;

    /* set the points */

    y = VIO_INTERPOLATE( ratio, bottom, top );

    fill_Point( polygons->points[i * 2 + 0], left, y, 0.0 );

    fill_Point( polygons->points[i * 2 + 1], right, y, 0.0 );

    /* set the colours */

    value = VIO_INTERPOLATE( ratio, min_value, max_value );

    colour = get_colour_code( ccs_ptr, value );

    polygons->colours[i * 2 + 0] = polygons->colours[i * 2 + 1] = colour;
  }
}

/**
 * Initialize data structures used to display the colour bar widget
 * in the slice window.
 *
 * \param slice_window The display_struct of the slice window.
 */

void
initialize_colour_bar( display_struct *slice_window )
{
    object_struct     *object;
    lines_struct      *lines;
    model_struct      *model;
    model_info_struct *model_info;
    colour_bar_struct *colour_bar;

    colour_bar = &slice_window->slice.colour_bar;

    colour_bar->top_offset = Colour_bar_top_offset;
    colour_bar->bottom_offset = Colour_bar_bottom_offset;
    colour_bar->left_offset = Colour_bar_left_offset;
    colour_bar->bar_width = Colour_bar_width;
    colour_bar->tick_width = Colour_bar_tick_width;
    colour_bar->desired_n_intervals = Colour_bar_desired_intervals;

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );
    model_info = get_model_info( model );

    model_info->render.shaded_mode = FILLED;
    model_info->render.shading_type = GOURAUD_SHADING;
    model_info->render.master_light_switch = FALSE;
    model_info->render.backface_flag = FALSE;

    /* Create and add the colour bar object itself. */
    add_object_to_model( model, create_colour_bar_object() );

    object = create_object( LINES );

    lines = get_lines_ptr( object );
    initialize_lines( lines, WHITE );
    delete_lines( lines );
    lines->colour_flag = PER_ITEM_COLOURS;
    lines->line_thickness = 1.0f;
    lines->n_points = 0;
    lines->n_items = 0;

    add_object_to_model( model, object );
}

typedef  struct
{
    int      priority;
    VIO_Real value;
} number_entry;

void
rebuild_ticks_and_text(colour_bar_struct *colour_bar,
                       model_struct *model,
                       int lines_offset,
                       int text_offset,
                       VIO_Real min_range, VIO_Real max_range,
                       VIO_Real min_limit, VIO_Real max_limit,
                       VIO_Real bottom, VIO_Real top,
                       VIO_BOOL histogram_present
                       )
{
  lines_struct *lines;

  const int x_tick_start = colour_bar->left_offset + colour_bar->bar_width;
  const int x_tick_end = x_tick_start + colour_bar->tick_width;
  VIO_Real range;
  VIO_Real delta;
  int n_numbers;
  struct number_entry
  {
    enum Label_priority {
      PRI_NORMAL=0,               /* Normal ticks */
      PRI_RANGE=1,                /* Absolute range */
      PRI_LIMIT=2                 /* Coding limits */
    } priority;
    VIO_Real value;
  };
  struct number_entry *numbers;
  struct number_entry entry;
  char buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
  VIO_Real mult_value;
  VIO_Point point;
  object_struct *object;
  text_struct *text;
  VIO_Real last_y;
  VIO_Real next_y;
  int i;
  int x, y;
  VIO_Colour colour;
  VIO_Real value;

  lines = get_lines_ptr( model->objects[lines_offset] );

  delete_lines( lines );

  lines->n_points = 0;
  lines->n_items = 0;

  while( model->n_objects > text_offset )
  {
    int object_index = model->n_objects - 1;
    delete_object( model->objects[object_index] );
    remove_ith_object_from_model( model, object_index );
  }

  range = max_range - min_range;

  delta = get_good_round_value( range /
                                (VIO_Real) colour_bar->desired_n_intervals );

  n_numbers = 0;
  numbers = NULL;

  mult_value = round_to_nearest_multiple( min_range, delta );
  while( mult_value <= min_range && delta > 0.0 )
    mult_value = round_to_nearest_multiple( mult_value + delta, delta );

  value = min_range;
  while( value <= max_range )
  {
    entry.value = value;

    if( value == min_limit || value == max_limit )
      entry.priority = PRI_LIMIT;
    else if( (value == min_range || value == max_range) )
      entry.priority = PRI_RANGE;
    else
      entry.priority = PRI_NORMAL;

    ADD_ELEMENT_TO_ARRAY( numbers, n_numbers, entry, DEFAULT_CHUNK_SIZE );

    if( value < min_limit && mult_value >= min_limit )
      value = min_limit;
    else if( value < max_limit && mult_value >= max_limit )
      value = max_limit;
    else if( value < max_range && mult_value >= max_range )
      value = max_range;
    else
    {
      value = mult_value;
      mult_value = round_to_nearest_multiple( mult_value + delta, delta );
    }

    if( delta <= 0.0 )
      break;
  }

  last_y = 0.0;

  for_less( i, 0, n_numbers )
  {
    y = get_y_pos( numbers[i].value, min_range, max_range, bottom, top );

    if( i < n_numbers-1 )
      next_y = get_y_pos( numbers[i+1].value, min_range, max_range,
                          bottom, top );

    if( (i == 0 || y - last_y > Colour_bar_closest_text) &&
        (i == n_numbers-1 ||
         next_y - y > Colour_bar_closest_text ||
         numbers[i].priority > numbers[i+1].priority ) )
    {
      if( numbers[i].priority == PRI_LIMIT &&
          numbers[i].value == min_limit)
        colour = Colour_bar_min_limit_colour;
      else if( numbers[i].priority == PRI_LIMIT &&
               numbers[i].value == max_limit )
        colour = Colour_bar_max_limit_colour;
      else if( numbers[i].priority == PRI_RANGE )
        colour = Colour_bar_range_colour;
      else
        colour = Colour_bar_text_colour;

      SET_ARRAY_SIZE( lines->colours, lines->n_items,
                      lines->n_items+1, DEFAULT_CHUNK_SIZE );

      lines->colours[lines->n_items] = colour;

      start_new_line( lines );

      if( numbers[i].priority == 2 )
        x = colour_bar->left_offset;
      else
        x = x_tick_start;

      fill_Point( point, x, y, 0.0 );

      add_point_to_line( lines, &point );

      if( Histogram_extra_width > 0.0 && histogram_present &&
          (numbers[i].value == min_limit ||
           numbers[i].value == max_limit) )
      {
        fill_Point( point, x_tick_end + Histogram_extra_width, y, 0.0 );
      }
      else
      {
        fill_Point( point, x_tick_end, y, 0.0 );
      }
      add_point_to_line( lines, &point );

      object = create_object( TEXT );
      text = get_text_ptr( object );
      text->font = (Font_types) Colour_bar_text_font;
      text->size = Colour_bar_text_size;
      text->colour = colour;
      fill_Point( text->origin, x_tick_end,
                  y - G_get_text_height( text->font, text->size ) / 2.0,
                  0.0 );
      sprintf( buffer, Colour_bar_number_format, numbers[i].value );

      text->string = create_string( buffer );

      add_object_to_model( model, object );

      last_y = y;
    }
  }

  if( n_numbers > 0 )
    FREE( numbers );
}

/**
 * Construct a new colour bar object for the slice window.
 *
 * \param slice_window The display_struct of the slice window.
 */
void
rebuild_colour_bar( display_struct *slice_window )
{
    int                 volume_index;
    int                 x_min, x_max, y_min, y_max;
    VIO_Real            min_value, max_value;
    VIO_Real            start_threshold, end_threshold;
    colour_bar_struct   *colour_bar;
    object_struct       *object;
    VIO_Volume          volume;
    model_struct        *model;
    VIO_Real            top;
    VIO_Real            bottom;

    object = slice_window->models[COLOUR_BAR_MODEL];

    if( !get_slice_window_volume( slice_window, &volume ) ||
        is_an_rgb_volume(volume) )
    {
        set_object_visibility( object, FALSE );
        return;
    }
    else
        set_object_visibility( object, TRUE );

    colour_bar = &slice_window->slice.colour_bar;

    get_volume_real_range( volume, &min_value, &max_value );

    volume_index = get_current_volume_index( slice_window );

    get_colour_coding_min_max( &slice_window->slice.
                               volumes[volume_index].colour_coding,
                               &start_threshold,
                               &end_threshold );

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );

    /* rebuild the points */

    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );

    bottom = colour_bar->bottom_offset;
    top = (VIO_Real) y_max - (VIO_Real) y_min - colour_bar->top_offset;

    update_colour_bar_object( model->objects[BAR],
                              colour_bar->left_offset,
                              colour_bar->left_offset + colour_bar->bar_width,
                              bottom,
                              top,
                              min_value,
                              max_value,
                              &slice_window->slice.volumes[volume_index].colour_coding);

    /* now rebuild the tick marks and numbers */
    rebuild_ticks_and_text( colour_bar, model,
                            TICKS, FIRST_TEXT,
                            min_value, max_value,
                            start_threshold, end_threshold,
                            bottom, top,
                            slice_window->slice.unscaled_histogram_lines.n_points > 0);

    set_slice_viewport_update( slice_window, COLOUR_BAR_MODEL );
}

/**
 * Calculates the y pixel position on the colour map corresponding to a
 * given voxel value.
 *
 * \param value The voxel value for which we will calculate the y position.
 * \param min_value The minimum voxel value.
 * \param max_value The maximum voxel value.
 * \param bottom The y-coordinate of the bottom of the widget.
 * \param top The y-coordinate of the top of the widget.
 */
static VIO_Real
get_y_pos(
    VIO_Real    value,
    VIO_Real    min_value,
    VIO_Real    max_value,
    VIO_Real    bottom,
    VIO_Real    top )
{
    if( min_value != max_value )
        return( VIO_INTERPOLATE( (value - min_value) / (max_value - min_value ),
                             bottom, top ) );
    else
        return( 0.0 );
}

/**
 * Get the y coordinate (in pixels) of the row corresponding to the
 * colour map value.
 * \param slice_window The display_struct of the slice window.
 * \param value The value which we want to check.
 */
int
get_colour_bar_y_pos(display_struct *slice_window, VIO_Real value )
{
    VIO_Volume          volume;
    int                 x_min, y_min, x_max, y_max;
    VIO_Real            top, bottom, min_value, max_value;
    colour_bar_struct   *colour_bar;

    if( !get_slice_window_volume( slice_window, &volume ) ||
        is_an_rgb_volume( volume ) )
        return( -1 );

    colour_bar = &slice_window->slice.colour_bar;

    get_volume_real_range( volume, &min_value, &max_value );
    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );

    bottom = (VIO_Real) colour_bar->bottom_offset;
    top    = (VIO_Real) y_max - (VIO_Real) y_min - colour_bar->top_offset;

    return( VIO_ROUND(get_y_pos( value, min_value, max_value, bottom, top )) );
}

/**
 * Check whether the pixel address is currently inside the colour bar.
 * \param slice_window
 * \param x The x pixel coordinate (of the mouse, e.g.)
 * \param y The y pixel coordinate (of the mouse, e.g.)
 * \param ratio Returns the ratio of the y parameter to the
 * height of the entire widget.
 * \returns True if the pixel coordinates are inside the colour bar.
 */
VIO_BOOL
mouse_within_colour_bar(display_struct *slice_window,
                        VIO_Real       x,
                        VIO_Real       y,
                        VIO_Real       *ratio )
{
    int               x_min, x_max, y_min, y_max;
    VIO_Real          top, bottom;
    VIO_BOOL          within;
    colour_bar_struct *colour_bar;
    VIO_Volume        volume;

    if( !get_slice_window_volume( slice_window, &volume ) ||
        is_an_rgb_volume( volume ) )
        return( FALSE );

    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );

    x -= (VIO_Real) x_min;
    y -= (VIO_Real) y_min;

    colour_bar = &slice_window->slice.colour_bar;

    bottom = (VIO_Real) colour_bar->bottom_offset;
    top = (VIO_Real) y_max - (VIO_Real) y_min - colour_bar->top_offset;

    within = y >= bottom && y <= top && colour_bar->left_offset <= x &&
             (x <= colour_bar->left_offset +
                   colour_bar->bar_width + colour_bar->tick_width);

    if( bottom == top )
        *ratio = 0.0;
    else
        *ratio = (y - bottom) / (top - bottom);

    return( within );
}

/**
 * Get the dimensions of the space available for the histogram.
 *
 * \param slice_window The display_struct of the slice window.
 * \param x1 Returns the minimum x pixel coordinate for the histogram.
 * \param x2 Returns the maximum x pixel coordinate for the histogram.
 */
void  get_histogram_space(
    display_struct      *slice_window,
    int                 *x1,
    int                 *x2 )
{
    int                 x_min, x_max, y_min, y_max;
    colour_bar_struct   *colour_bar;

    colour_bar = &slice_window->slice.colour_bar;
    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );

    *x1 = x_min + VIO_ROUND( colour_bar->left_offset + colour_bar->bar_width +
                             colour_bar->tick_width );
    *x2 = x_max;
}
