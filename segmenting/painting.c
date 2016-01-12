/**
 * \file painting.c
 * \brief Functions to implement labeling of the loaded volumes(s).
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

static  DEF_EVENT_FUNCTION( right_mouse_down );
static  DEF_EVENT_FUNCTION( end_painting );
static  DEF_EVENT_FUNCTION( handle_update_painting );

static  int  update_paint_labels(
    display_struct  *slice_window );

static void erase_brush(display_struct *slice_window);

static  void   update_brush(
    display_struct    *slice_window,
    int               x,
    int               y,
    VIO_BOOL           erase_brush );

static  int  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2,
    int               label );

static  void  paint_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    VIO_Real         start_voxel[],
    VIO_Real         end_voxel[],
    int              label );


/**
 * Set the current painting mode flag. This switches between free and
 * line drawing modes.
 */
void
set_painting_mode(display_struct *display, VIO_BOOL freestyle)
{
  display_struct *slice_window;

  if (get_slice_window(display, &slice_window))
  {
    int curr_view_index = slice_window->slice.painting_view_index;


    /* If we've switched back to free painting mode from line painting
     * mode, we have to remove the brush outline from the current view.
     */
    if (freestyle && !Toggle_freestyle_painting && curr_view_index >= 0)
    {
      model_struct *model = get_graphics_model( slice_window,
                                                SLICE_MODEL1 + curr_view_index);
      remove_object_from_model( model, slice_window->slice.brush_outline );

      erase_brush(slice_window);
    }
  }
  Toggle_freestyle_painting = freestyle;
}

/**
 * Get the current painting mode flag.
 */
VIO_BOOL
get_painting_mode(display_struct *display)
{
  return Toggle_freestyle_painting;
}

/**
 * Get the integer label to apply to newly painting voxels.
 * \param display Any top-level display_struct
 * \returns The current paint label.
 */
int  get_current_paint_label(
    display_struct    *display )
{
    display_struct    *slice_window;

    if( get_slice_window( display, &slice_window ) )
        return( slice_window->slice.current_paint_label );
    else
        return( 0 );
}

/**
 * Get the integer label to apply to newly erased voxels.
 * \param display Any top-level display_struct
 * \returns The current erase label.
 */
int
get_current_erase_label(display_struct *display)
{
    display_struct *slice_window;

    if (get_slice_window( display, &slice_window ) )
        return ( slice_window->slice.current_erase_label );
    else
        return ( 0 );
}

/**
 * Initialize data structures and event handling for voxel labeling
 * and painting. This function sets the "right mouse down" event
 * in the slice window as the event that initiates actual painting.
 * It also initializes some important state variables.
 * \param slice_window The display_struct of the slice window.
 */
void  initialize_voxel_labeling(
    display_struct    *slice_window )
{
    add_action_table_function( &slice_window->action_table,
                               RIGHT_MOUSE_DOWN_EVENT,
                               right_mouse_down );
    slice_window->slice.brush_outline = create_object( LINES );
    initialize_lines( get_lines_ptr(slice_window->slice.brush_outline),
                      Brush_outline_colour );

    slice_window->slice.segmenting.n_starts_alloced = 0;
    slice_window->slice.segmenting.mouse_scale_factor =
                                               Initial_mouse_scale_factor;
    slice_window->slice.painting_view_index = -1;
    slice_window->slice.painting_volume_index = -1;
    slice_window->slice.segmenting.fast_updating_allowed =
                           Default_fast_painting_flag;
    slice_window->slice.segmenting.cursor_follows_paintbrush =
                           Default_cursor_follows_paintbrush_flag;

    slice_window->slice.segmenting.brush_index = 0;
    slice_window->slice.segmenting.brush[0].radius[VIO_X] =
      Default_x_brush_radius;
    slice_window->slice.segmenting.brush[0].radius[VIO_Y] =
      Default_y_brush_radius;
    slice_window->slice.segmenting.brush[0].radius[VIO_Z] =
      Default_z_brush_radius;

    slice_window->slice.segmenting.brush[1].radius[VIO_X] =
      Secondary_x_brush_radius;
    slice_window->slice.segmenting.brush[1].radius[VIO_Y] =
      Secondary_y_brush_radius;
    slice_window->slice.segmenting.brush[1].radius[VIO_Z] =
      Secondary_z_brush_radius;
}

/**
 * Delete any data associated with voxel labeling operations. This
 * only consists of freeing the fields used to optimize the so-called
 * fast painting mode.
 *
 * \param slice The slice_window_struct for the slice window.
 */
void  delete_voxel_labeling(
    slice_window_struct    *slice )
{
    if( slice->segmenting.n_starts_alloced > 0 )
    {
        FREE( slice->segmenting.y_starts );
    }

    delete_object(slice->brush_outline);
}

/**
 * Scales the mouse position according to the current setting of
 * the mouse_scale_factor. Scaling is calculated as
 * x_scaled = x_origin + mouse_scale_factor * (x - x_origin).
 * This means that distances from the original "mouse down" position
 * are scaled by the factor. It is not at all clear to me that this
 * functionality is useful.
 */
static  int  scale_x_mouse(
    display_struct  *slice_window,
    int             x )
{
    VIO_Real   x_real;
    if( slice_window->slice.segmenting.mouse_scale_factor > 0.0 &&
        slice_window->slice.segmenting.mouse_scale_factor != 1.0 )
    {
        x_real = (VIO_Real) slice_window->slice.segmenting.x_mouse_start +
                 slice_window->slice.segmenting.mouse_scale_factor *
                 (VIO_Real) (x - slice_window->slice.segmenting.x_mouse_start);
        x = VIO_ROUND( x_real );
    }

    return( x );
}

/**
 * Scales the mouse position according to the current setting of
 * the mouse_scale_factor. Scaling is calculated as
 * y_scaled = y_origin + mouse_scale_factor * (y - y_origin).
 * This means that distances from the original "mouse down" position
 * are scaled by the factor. It is not at all clear to me that this
 * functionality is useful.
 */
static  int  scale_y_mouse(
    display_struct  *slice_window,
    int             y )
{
    VIO_Real   y_real;
    if( slice_window->slice.segmenting.mouse_scale_factor > 0.0 &&
        slice_window->slice.segmenting.mouse_scale_factor != 1.0 )
    {
        y_real = (VIO_Real) slice_window->slice.segmenting.y_mouse_start +
                 slice_window->slice.segmenting.mouse_scale_factor *
                 (VIO_Real) (y - slice_window->slice.segmenting.y_mouse_start);
        y = VIO_ROUND( y_real );
    }

    return( y );
}

/**
 * Draw straight lines only. This implements a mode wherein repeated right
 * clicks draw straight lines between the clicked coordinates. The process
 * terminates when the mouse moves to another slice view.
 */
static void
draw_straight_lines(display_struct *slice_window)
{
  int x_pixel, y_pixel, label, view_index, volume_index;

  G_get_mouse_position( slice_window->window, &x_pixel, &y_pixel );
  if (!find_slice_view_mouse_is_in( slice_window, x_pixel, y_pixel,
                                    &view_index ))
  {
      view_index = -1;
  }

  volume_index = get_current_volume_index( slice_window );

  if (view_index == slice_window->slice.painting_view_index)
  {
    if( is_shift_key_pressed( ) )
      label = get_current_erase_label( slice_window );
    else
      label = get_current_paint_label( slice_window );

    /*
     * If the control key is pressed, we don't actually draw the line.
     */
    if (!is_ctrl_key_pressed())
    {
      /*
       * Draw a straight line between current and previous position.
       */
      sweep_paint_labels( slice_window,
                          slice_window->slice.segmenting.x_mouse_start,
                          slice_window->slice.segmenting.y_mouse_start,
                          x_pixel, y_pixel,
                          label );

      set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS );

      update_all_menu_text( slice_window );
    }
  }
  else if (Draw_brush_outline)
  {
    if (slice_window->slice.painting_view_index >= 0)
    {
      /* If the view index has changed, we need to remove the
       * brush from the current view.
       */
      remove_object_from_model(get_graphics_model( slice_window,
                                                   SLICE_MODEL1 + slice_window->slice.painting_view_index ),
                               slice_window->slice.brush_outline );
    }

    /* Erase the brush from this view.
     */
    erase_brush(slice_window);

    if (view_index >= 0)
    {
      /* And add it to the new view. Its position will be updated below.
       */
      add_object_to_model(get_graphics_model(slice_window,
                                             SLICE_MODEL1 + view_index),
                          slice_window->slice.brush_outline );
    }
  }

  slice_window->slice.painting_volume_index = volume_index;
  slice_window->slice.painting_view_index = view_index;
  slice_window->slice.segmenting.x_mouse_start = x_pixel;
  slice_window->slice.segmenting.y_mouse_start = y_pixel;

  if( Draw_brush_outline)
  {
    update_brush( slice_window, x_pixel, y_pixel, TRUE );
  }
}


/**
 * Handle a right button press on the mouse. This initiates a painting
 * stroke.
 */
static  DEF_EVENT_FUNCTION( right_mouse_down )
{
    display_struct  *slice_window;
    int             volume_index;
    int             view_index;
    int             x_pixel, y_pixel;
    int             label;

    if( !get_slice_window( display, &slice_window ))
        return( VIO_OK );

    volume_index = get_current_volume_index( slice_window );

    /*
     * START OF MANDLIZATION:
     * If not in Toggle_freestyle_painting mode draw a straight line.
     */

    if ( !Toggle_freestyle_painting )
    {
      draw_straight_lines(slice_window);
      return( VIO_OK );
    }

    /*
     * EOF MANDLIZATION
     */

    push_action_table( &slice_window->action_table, NO_EVENT );

    add_action_table_function( &slice_window->action_table,
                               NO_EVENT,
                               handle_update_painting );

    add_action_table_function( &slice_window->action_table,
                               RIGHT_MOUSE_UP_EVENT,
                               end_painting );

    (void) G_get_mouse_position( slice_window->window, &x_pixel, &y_pixel );

    slice_window->slice.segmenting.x_mouse_start = x_pixel;
    slice_window->slice.segmenting.y_mouse_start = y_pixel;

    undo_start(slice_window, volume_index);

    if( is_shift_key_pressed() )
        label = get_current_erase_label( slice_window );
    else
        label = get_current_paint_label( slice_window );

    if (find_slice_view_mouse_is_in(slice_window, x_pixel, y_pixel,
                                    &view_index ))
    {
        slice_window->slice.painting_volume_index = volume_index;
        slice_window->slice.painting_view_index = view_index;

        sweep_paint_labels( slice_window, x_pixel, y_pixel, x_pixel, y_pixel,
                            label );

        if( Draw_brush_outline)
        {
            int model_index = SLICE_MODEL1 + view_index;
            add_object_to_model(get_graphics_model(slice_window, model_index),
                                slice_window->slice.brush_outline);
            update_brush( slice_window, x_pixel, y_pixel, FALSE );
        }
    }
    else
    {
        slice_window->slice.painting_volume_index = -1;
        slice_window->slice.painting_view_index = -1;
    }

    record_mouse_pixel_position( slice_window );

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * End a painting stroke when in "free" painting mode.
 */
static  DEF_EVENT_FUNCTION( end_painting )
{
    int   volume_index;

    remove_action_table_function( &display->action_table,
                                  RIGHT_MOUSE_UP_EVENT, end_painting );
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  handle_update_painting );
    pop_action_table( &display->action_table, NO_EVENT );

    volume_index = update_paint_labels( display );

    undo_finish(display, volume_index);

    if( Draw_brush_outline &&
        display->slice.painting_view_index >= 0 )
    {
        remove_object_from_model( get_graphics_model( display,
                       SLICE_MODEL1 + display->slice.painting_view_index ),
                                  display->slice.brush_outline );

        display->slice.painting_view_index = -1;
    }

    set_slice_window_all_update( display, volume_index, UPDATE_LABELS );

    update_all_menu_text( display );

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * Update a painting stroke when in "free" painting mode.
 */
static  DEF_EVENT_FUNCTION( handle_update_painting )
{
    (void) update_paint_labels( display );

    return( VIO_OK );
}

/**
 * Gets the voxel coordinate, volume index, and view index
 * corresponding to the the given x and y pixel coordinates, generally
 * derived from the mouse position.
 *
 * If the Snap_brush_to_centres global is set, the returned
 * coordinates are rounded to a whole voxel position.
 *
 * \param slice_window The display_struct corresponding to the slice window.
 * \param x_pixel The X coordinate of the mouse, in pixels.
 * \param y_pixel The Y coordinate of the mouse, in pixels.
 * \param voxel The voxel coordinates corresponding to this pixel position.
 * \param volume_index The volume index corresponding to this pixel position.
 * \param view_index The view index corresponding to this pixel position.
 */
static  VIO_BOOL  get_brush_voxel_centre(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel,
    VIO_Real          voxel[],
    int               *volume_index,
    int               *view_index )
{
    VIO_BOOL  inside;

    inside = get_volume_corresponding_to_pixel( slice_window, x_pixel, y_pixel,
                                        volume_index, view_index, voxel );

    if( inside && Snap_brush_to_centres )
    {
        voxel[VIO_X] = (VIO_Real) VIO_ROUND( voxel[VIO_X] );
        voxel[VIO_Y] = (VIO_Real) VIO_ROUND( voxel[VIO_Y] );
        voxel[VIO_Z] = (VIO_Real) VIO_ROUND( voxel[VIO_Z] );
    }

    return( inside );
}

/**
 * Apply paint labels between the previous (x1, y1) and current (x2, y2)
 * mouse position.
 *
 * \param slice_window The display_struct of the slice view window.
 * \param x1 The previous X coordinate of the mouse, in pixels.
 * \param y1 The previous Y coordinate of the mouse, in pixels.
 * \param x2 The current X coordinate of the mouse, in pixels.
 * \param y2 The current Y coordinate of the mouse, in pixels.
 * \param label The label value to apply to the affected voxels.
 */
static  int  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2,
    int               label )
{
    int      view_index, view_index2;
    int      volume_index, volume_index2;
    VIO_Real start_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real end_voxel[VIO_MAX_DIMENSIONS];

    if( get_brush_voxel_centre( slice_window, x1, y1, start_voxel,
                                &volume_index, &view_index ) &&
        get_brush_voxel_centre( slice_window, x2, y2, end_voxel,
                                &volume_index2, &view_index2 ) &&
        volume_index == slice_window->slice.painting_volume_index &&
        volume_index2 == slice_window->slice.painting_volume_index &&
        view_index == slice_window->slice.painting_view_index &&
        view_index2 == slice_window->slice.painting_view_index)
    {
        if( slice_window->slice.segmenting.cursor_follows_paintbrush )
            set_voxel_cursor_from_mouse_position( slice_window );
        paint_labels( slice_window, volume_index, view_index,
                      start_voxel, end_voxel, label );
    }
    else
        volume_index = get_current_volume_index( slice_window );

    return( volume_index );
}

/**
 * Implement mouse movements during painting.
 * \param slice_window A pointer to the slice window's display_struct.
 * \returns The current volume index.
 */
static  int
update_paint_labels(display_struct  *slice_window )
{
    int  x, y, x_prev, y_prev, label, volume_index;

    if( pixel_mouse_moved(slice_window,&x,&y,&x_prev,&y_prev) )
    {
        if( is_shift_key_pressed() )
            label = get_current_erase_label( slice_window );
        else
            label = get_current_paint_label( slice_window );

        volume_index = sweep_paint_labels( slice_window,
                          scale_x_mouse(slice_window,x_prev),
                          scale_y_mouse(slice_window,y_prev),
                          scale_x_mouse(slice_window,x),
                          scale_y_mouse(slice_window,y),
                          label );

        if( Draw_brush_outline )
        {
            update_brush( slice_window,
                          scale_x_mouse(slice_window,x),
                          scale_y_mouse(slice_window,y), TRUE );
        }
    }
    else
        volume_index = get_current_volume_index( slice_window );

    return( volume_index );
}

/**
 * Get the brush dimensions associated with the current volume and view.
 * We check that the view has orthogonal axes and refuse to paint if not.
 *
 * \param slice_window The display_struct of the slice view window.
 * \param volume_index The index of the active volume.
 * \param view_index The index of the slice view under the mouse.
 * \param a1 The axis that corresponds to the horizontal direction in this
 * view.
 * \param a2 The axis that corresponds to the vertical direction in this
 * view.
 * \param axis The axis that corresponds to the perpendicular (slice)
 * direction in this view.
 * \param radius The radius of the current brush in each of the 3 directions.
 */
static  VIO_BOOL  get_brush(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    int              *a1,
    int              *a2,
    int              *axis,
    VIO_Real         radius[] )
{
    int      c;
    VIO_BOOL okay;
    VIO_Real separations[VIO_MAX_DIMENSIONS];

    okay = FALSE;

    if( slice_has_ortho_axes( slice_window, volume_index, view_index,
                              a1, a2, axis ) )
    {
        get_volume_separations( get_nth_volume(slice_window, volume_index),
                                separations );

        radius[*a1] = slice_window->slice.x_brush_radius /
                      VIO_FABS( separations[*a1] );

        radius[*a2] = slice_window->slice.y_brush_radius /
                      VIO_FABS( separations[*a2] );
        radius[*axis] = slice_window->slice.z_brush_radius /
                      VIO_FABS( separations[*axis] );

        for_less( c, 0, VIO_N_DIMENSIONS )
        {
            if( radius[c] != 0.0 && radius[c] < 0.5 )
                radius[c] = 0.5;
        }

        okay = TRUE;
    }

    return( okay );
}

/**
 * Tests whether the current voxel coordinate is inside the current
 * painting brush coordinates.
 *
 * \param origin The current position of the brush in voxel coordinates.
 * \param delta A vector representing the distance the brush moved in
 * voxel coordinates.
 * \param radius The three-dimensional brush radius.
 * \param voxel The voxel coordinate position to test.
 */
static  VIO_BOOL  inside_swept_brush(
    const VIO_Real   origin[],
    const VIO_Vector *delta,
    const VIO_Real   radius[],
    const int        voxel[] )
{
    int      c;
    VIO_Real     d, mag, t, t_min, t_max;
    VIO_Point    voxel_offset, voxel_origin;
    VIO_Vector   scaled_delta;
    VIO_BOOL  inside;

    if( radius[VIO_X] == 0.0 && radius[VIO_Y] == 0.0 && radius[VIO_Z] == 0.0 )
    {
        if( delta == NULL )
        {
            fill_Vector( scaled_delta, 0.0, 0.0, 0.0 );
        }
        else
            scaled_delta = *delta;

        fill_Point( voxel_origin, origin[VIO_X], origin[VIO_Y], origin[VIO_Z] );
        inside = clip_line_to_box( &voxel_origin, &scaled_delta,
                              (VIO_Real) voxel[VIO_X] - 0.5, (VIO_Real) voxel[VIO_X] + 0.5,
                              (VIO_Real) voxel[VIO_Y] - 0.5, (VIO_Real) voxel[VIO_Y] + 0.5,
                              (VIO_Real) voxel[VIO_Z] - 0.5, (VIO_Real) voxel[VIO_Z] + 0.5,
                              &t_min, &t_max ) &&
                 t_min <= 1.0 && t_max >= 0.0;
    }
    else
    {
        for_less( c, 0, VIO_N_DIMENSIONS )
        {
            if( radius[c] == 0.0 )
                Vector_coord(voxel_offset,c) = 0.0f;
            else
                Vector_coord(voxel_offset,c) = (VIO_Point_coord_type)
                               (((VIO_Real) voxel[c] - origin[c]) / radius[c]);
        }

        if( delta != NULL )
        {
            for_less( c, 0, VIO_N_DIMENSIONS )
            {
                if( radius[c] == 0.0 )
                    Vector_coord(scaled_delta,c) = Vector_coord(*delta,c);
                else
                    Vector_coord(scaled_delta,c) = (VIO_Point_coord_type)
                                          (RVector_coord(*delta,c) / radius[c]);
            }

            d = DOT_VECTORS( scaled_delta, scaled_delta );
        }
        else
            d = 0.0;

        if( d != 0.0 )
        {
            t = DOT_VECTORS( voxel_offset, scaled_delta ) / d;

            if( t < 0.0 )
                t = 0.0;
            else if( t > 1.0 )
                t = 1.0;

            for_less( c, 0, VIO_N_DIMENSIONS )
            {
                Vector_coord( voxel_offset, c ) -= (VIO_Point_coord_type) t *
                                                 Vector_coord(scaled_delta,c);
            }
        }

        mag = DOT_VECTORS( voxel_offset, voxel_offset );

        inside = (mag <= 1.0);
    }

    return( inside );
}

/**
 * Implements the "fast" painting algorithm. Fast painting just means
 * that painting takes place only in the current slice, and NOT in
 * adjacent slices. Writes label colour directly into the pixel object
 * for this slice, which is pretty weird.
 */
static  void  fast_paint_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    int              a1,
    int              a2,
    int              axis,
    const VIO_Real   start_voxel[],
    const int        min_voxel[],
    const int        max_voxel[],
    const VIO_Vector *delta,
    const VIO_Real   radius[],
    int              label )
{
    VIO_Volume     volume;
    int            value, sizes[VIO_MAX_DIMENSIONS], tmp;
    VIO_Real       min_threshold, max_threshold, volume_value;
    int            ind[VIO_N_DIMENSIONS], new_n_starts, *y_starts, y_inc, x_inc;
    int            x_min_pixel, y_min_pixel, x_max_pixel, y_max_pixel;
    pixels_struct  *pixels;
    VIO_Real       x_offset, x_scale, y_offset, y_scale;
    VIO_Real       x_trans, y_trans;
    VIO_Real       real_x_start, real_x_end, real_y_start;
    int            i, j, x_start, x_end, y_start, y_end;
    VIO_Colour     colour;
    VIO_BOOL       update_required;

    volume = get_nth_volume( slice_window, volume_index );
    min_threshold = slice_window->slice.segmenting.min_threshold;
    max_threshold = slice_window->slice.segmenting.max_threshold;
    update_required = FALSE;

    get_volume_sizes( volume, sizes );

    pixels = get_pixels_ptr( get_label_slice_pixels_object(
                               slice_window, volume_index, view_index ) );

    colour = get_colour_of_label( slice_window, volume_index, label );

    get_voxel_to_pixel_transform( slice_window, volume_index, view_index,
                                  &a1, &a2,
                                  &x_scale, &x_trans, &y_scale, &y_trans );

    if( x_scale >= 0.0 )
    {
        x_offset = 0.5;
        x_inc = 1;
    }
    else
    {
        x_offset = -0.5;
        x_inc = -1;
    }

    if( y_scale >= 0.0 )
    {
        y_offset = 0.5;
        y_inc = 1;
    }
    else
    {
        y_offset = -0.5;
        y_inc = -1;
    }

    /* Adjust the x_trans and y_trans values by the current position
     * of the pixels object corresponding to this slice.
     */
    x_trans -= (VIO_Real) pixels->x_position;
    y_trans -= (VIO_Real) pixels->y_position;

    ind[axis] = min_voxel[axis];

    new_n_starts = max_voxel[a2] - min_voxel[a2] + 3;
    if( new_n_starts > slice_window->slice.segmenting.n_starts_alloced )
    {
        SET_ARRAY_SIZE( slice_window->slice.segmenting.y_starts,
                        slice_window->slice.segmenting.n_starts_alloced,
                        new_n_starts, DEFAULT_CHUNK_SIZE );
        slice_window->slice.segmenting.n_starts_alloced = new_n_starts;
    }

    /* Pre-calculate all of the y_start values. This is probably done
     * to avoid having to recompute this every time through the
     * inner loop. It is of questionable value.
     */
    y_starts = slice_window->slice.segmenting.y_starts;

    for_inclusive( ind[a2], min_voxel[a2]-1, max_voxel[a2]+1 )
    {
        real_y_start = y_scale * ((VIO_Real) ind[a2] - y_offset) + y_trans;

        y_start = VIO_CEILING( real_y_start );

        if( y_start < 0 )
            y_start = 0;
        else if( y_start > pixels->y_size )
            y_start = pixels->y_size;

        y_starts[ind[a2] - min_voxel[a2]+1] = y_start;
    }

    real_x_start = x_scale * ((VIO_Real) min_voxel[a1] - x_offset) + x_trans;
    x_min_pixel = VIO_CEILING( real_x_start );
    real_x_start = x_scale * ((VIO_Real) max_voxel[a1] - x_offset) + x_trans;
    x_max_pixel = VIO_CEILING( real_x_start );
    if( x_min_pixel > x_max_pixel )
    {
        tmp = x_min_pixel;
        x_min_pixel = x_max_pixel;
        x_max_pixel = tmp;
    }

    real_y_start = y_scale * ((VIO_Real) min_voxel[a2] - y_offset) + y_trans;
    y_min_pixel = VIO_CEILING( real_y_start );
    real_y_start = y_scale * ((VIO_Real) max_voxel[a2] - y_offset) + y_trans;
    y_max_pixel = VIO_CEILING( real_y_start );
    if( y_min_pixel > y_max_pixel )
    {
        tmp = y_min_pixel;
        y_min_pixel = y_max_pixel;
        y_max_pixel = tmp;
    }

    for_inclusive( ind[a1], min_voxel[a1], max_voxel[a1] )
    {
        real_x_start = x_scale * ((VIO_Real) ind[a1] - x_offset) + x_trans;
        real_x_end = x_scale * ((VIO_Real) (ind[a1]+x_inc) - x_offset) + x_trans;

        x_start = VIO_CEILING( real_x_start );
        x_end = VIO_CEILING( real_x_end );

        if( x_start < 0 )
            x_start = 0;
        if( x_end > pixels->x_size )
            x_end = pixels->x_size;

        for_inclusive( ind[a2], min_voxel[a2], max_voxel[a2] )
        {
            y_start = y_starts[ind[a2] - min_voxel[a2] + 1];
            y_end = y_starts[ind[a2] - min_voxel[a2] + 1 + y_inc];

            if( inside_swept_brush( start_voxel, delta, radius, ind ) )
            {
                value = get_voxel_label( slice_window, volume_index,
                                         ind[VIO_X], ind[VIO_Y], ind[VIO_Z] );

                if( value == label )
                    continue;

                if( min_threshold < max_threshold )
                {
                    volume_value = get_volume_real_value( volume,
                                                          ind[VIO_X], ind[VIO_Y], ind[VIO_Z], 0, 0 );

                    if( volume_value < min_threshold ||
                        volume_value > max_threshold )
                        continue;
                }

                set_voxel_label( slice_window, volume_index,
                                 ind[VIO_X], ind[VIO_Y], ind[VIO_Z], label );

                undo_save( slice_window, volume_index, ind, value);

                /* Update the pixel object directly.
                 */
                for_less( i, x_start, x_end )
                {
                    for_less( j, y_start, y_end )
                    {
                        PIXEL_RGB_COLOUR( *pixels, i, j ) = colour;
                    }
                }

                update_required = TRUE;
            }
        }
    }

    if( update_required )
    {
        set_slice_composite_update( slice_window, view_index,
                                    pixels->x_position + x_min_pixel,
                                    pixels->x_position + x_max_pixel,
                                    pixels->y_position + y_min_pixel,
                                    pixels->y_position + y_max_pixel );
    }
}

/**
 * Paint labels in the selected volume and view, from the start position
 * up to the ending position.
 * \param slice_window The display_struct of the slice view window.
 * \param volume_index The number of the selected volume.
 * \param view_index The number of the selected view.
 * \param start_voxel The start voxel coordinate.
 * \param end_voxel The final voxel coordinate.
 * \param label The label value to apply.
 */
static  void  paint_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    VIO_Real             start_voxel[],
    VIO_Real             end_voxel[],
    int              label )
{
    VIO_Volume     volume, label_volume;
    int            a1, a2, axis, c, sizes[VIO_MAX_DIMENSIONS];
    int            min_voxel[VIO_N_DIMENSIONS], max_voxel[VIO_N_DIMENSIONS];
    VIO_Real       min_limit, max_limit;
    VIO_Real       radius[VIO_N_DIMENSIONS];
    VIO_Vector     delta;
    VIO_BOOL       update_required;

    if( get_brush( slice_window, volume_index, view_index,
                   &a1, &a2, &axis, radius ) )
    {
        volume = get_nth_volume( slice_window, volume_index );
        label_volume = get_nth_label_volume( slice_window, volume_index );
        update_required = FALSE;
        get_volume_sizes( volume, sizes );

        /* Delta will just hold the difference between end voxel and
         * the start voxel.
         */
        for_less( c, 0, VIO_N_DIMENSIONS )
        {
            Vector_coord(delta,c) = (VIO_Point_coord_type)
                                       (end_voxel[c] - start_voxel[c]);
        }

        for_less( c, 0, VIO_N_DIMENSIONS )
        {
            min_limit = MIN( start_voxel[c], end_voxel[c] ) - radius[c];
            max_limit = MAX( start_voxel[c], end_voxel[c] ) + radius[c];

            if( min_limit == max_limit )
            {
                min_voxel[c] = VIO_ROUND( min_limit );
                max_voxel[c] = VIO_ROUND( min_limit );
            }
            else
            {
                min_voxel[c] = VIO_FLOOR( min_limit + 0.5 );
                max_voxel[c] = VIO_CEILING( max_limit + 0.5 );
            }

            if( min_voxel[c] < 0 )
                min_voxel[c] = 0;
            if( max_voxel[c] >= sizes[c] )
                max_voxel[c] = sizes[c] - 1;
        }

        if( slice_window->slice.segmenting.fast_updating_allowed &&
            radius[axis] == 0.0 &&
            label_volume != NULL &&
            is_label_volume_initialized( label_volume ) &&
            !slice_window->slice.volumes[volume_index].
                             views[view_index].update_labels_flag )
        {
            fast_paint_labels( slice_window, volume_index, view_index,
                               a1, a2, axis,
                               start_voxel, min_voxel, max_voxel,
                               &delta, radius, label );
        }
        else
        {
            VIO_Real min_threshold = slice_window->slice.segmenting.min_threshold;
            VIO_Real max_threshold = slice_window->slice.segmenting.max_threshold;
            VIO_Real volume_value;
            int      value;
            int      ind[VIO_N_DIMENSIONS];

            for_inclusive( ind[a1], min_voxel[a1], max_voxel[a1] )
            {
                for_inclusive( ind[a2], min_voxel[a2], max_voxel[a2] )
                {
                    for_inclusive( ind[axis], min_voxel[axis], max_voxel[axis] )
                    {
                        if( inside_swept_brush( start_voxel, &delta,
                                                radius, ind ) )
                        {
                            value = get_voxel_label( slice_window, volume_index,
                                                     ind[VIO_X], ind[VIO_Y], ind[VIO_Z] );

                            if( value == label )
                                continue;

                            if( min_threshold < max_threshold )
                            {
                                volume_value = get_volume_real_value( volume,
                                              ind[VIO_X], ind[VIO_Y], ind[VIO_Z], 0, 0 );

                                if( volume_value < min_threshold ||
                                    volume_value > max_threshold )
                                    continue;
                            }

                            set_voxel_label( slice_window, volume_index,
                                             ind[VIO_X], ind[VIO_Y], ind[VIO_Z], label );

                            undo_save( slice_window, volume_index, ind, value);

                            update_required = TRUE;
                        }
                    }
                }
            }
        }

        if( update_required )
        {
            set_slice_window_all_update( slice_window, volume_index,
                                         UPDATE_LABELS );
        }
    }
}

typedef  enum  { POSITIVE_X, POSITIVE_Y, NEGATIVE_X, NEGATIVE_Y,
                 N_DIRECTIONS } Directions;

static  int  dx[N_DIRECTIONS] = { 1,  0, -1,  0 };
static  int  dy[N_DIRECTIONS] = { 0,  1,  0, -1 };

static   void    add_point_to_contour(
    int              x_centre_pixel,
    int              y_centre_pixel,
    int              a1,
    int              a2,
    VIO_Real             x_scale,
    VIO_Real             x_trans,
    VIO_Real             y_scale,
    VIO_Real             y_trans,
    int              voxel[],
    Directions       dir,
    lines_struct     *lines )
{
    int     x_pixel, y_pixel, next_dir;
    VIO_Real    real_x_pixel, real_y_pixel;
    VIO_Point   point;

    next_dir = (dir + 1) % (int) N_DIRECTIONS;

    real_x_pixel = x_scale * ((VIO_Real) voxel[a1] +
                              ((VIO_Real) dx[dir] + (VIO_Real) dx[next_dir]) / 2.0) +
                   x_trans;
    real_y_pixel = y_scale * ((VIO_Real) voxel[a2] +
                              ((VIO_Real) dy[dir] + (VIO_Real) dy[next_dir]) / 2.0) +
                   y_trans;

    x_pixel = VIO_ROUND( real_x_pixel );
    y_pixel = VIO_ROUND( real_y_pixel );

    if( x_pixel < x_centre_pixel )
        x_pixel -= Brush_outline_offset;
    else if( x_pixel > x_centre_pixel )
        x_pixel += Brush_outline_offset;

    if( y_pixel < y_centre_pixel )
        y_pixel -= Brush_outline_offset;
    else if( y_pixel > y_centre_pixel )
        y_pixel += Brush_outline_offset;

    fill_Point( point, (VIO_Real) x_pixel, (VIO_Real) y_pixel, 0.0 );

    add_point_to_line( lines, &point );
}

static  VIO_BOOL  neighbour_is_inside(
    const VIO_Real   centre[],
    const VIO_Real   radius[],
    int        a1,
    int        a2,
    int        voxel[],
    Directions dir )
{
    VIO_BOOL   inside;

    voxel[a1] += dx[dir];
    voxel[a2] += dy[dir];

    inside = inside_swept_brush( centre, (VIO_Vector *) NULL, radius, voxel );

    voxel[a1] -= dx[dir];
    voxel[a2] -= dy[dir];

    return( inside );
}

static  void  get_brush_contour(
    display_struct    *slice_window,
    int               x_centre_pixel,
    int               y_centre_pixel,
    int               volume_index,
    int               view_index,
    int               a1,
    int               a2,
    const VIO_Real    centre[VIO_N_DIMENSIONS],
    const VIO_Real    radius[VIO_N_DIMENSIONS],
    const int         start_voxel[VIO_N_DIMENSIONS],
    Directions        start_dir,
    lines_struct      *lines )
{
    int          current_voxel[VIO_N_DIMENSIONS];
    Directions   dir;
    VIO_Real     x_scale, x_trans, y_scale, y_trans;

    get_voxel_to_pixel_transform( slice_window, volume_index, view_index,
                                  &a1, &a2,
                                  &x_scale, &x_trans, &y_scale, &y_trans );

    current_voxel[VIO_X] = start_voxel[VIO_X];
    current_voxel[VIO_Y] = start_voxel[VIO_Y];
    current_voxel[VIO_Z] = start_voxel[VIO_Z];
    dir = start_dir;

    do
    {
        add_point_to_contour( x_centre_pixel, y_centre_pixel,
                              a1, a2, x_scale, x_trans, y_scale, y_trans,
                              current_voxel, dir, lines );

        dir = (Directions) (((int) dir + 1) % (int) N_DIRECTIONS);

        while( neighbour_is_inside( centre, radius, a1, a2,
                                    current_voxel, dir ) )
        {
            current_voxel[a1] += dx[dir];
            current_voxel[a2] += dy[dir];
            dir = (Directions) (((int) dir - 1 + (int) N_DIRECTIONS) %
                                (int) N_DIRECTIONS);
        }
    }
    while( current_voxel[VIO_X] != start_voxel[VIO_X] ||
           current_voxel[VIO_Y] != start_voxel[VIO_Y] ||
           current_voxel[VIO_Z] != start_voxel[VIO_Z] ||
           dir != start_dir );

    ADD_ELEMENT_TO_ARRAY( lines->indices, lines->end_indices[lines->n_items-1],
                          0, DEFAULT_CHUNK_SIZE );
}

/**
 * Given a lines_struct object, find the maximum and minimum pixel
 * extents of the lines. This is used to optimize incremental painting
 * and repainting of the brush outline.
 * \param lines The lines_struct to examine.
 * \param x_min The minimum x coordinate in pixel units.
 * \param x_max The maximum x coordinate in pixel units.
 * \param y_min The minimum y coordinate in pixel units.
 * \param y_max The maximum y coordinate in pixel units.
 */
static  VIO_BOOL   get_lines_limits(
    const lines_struct  *lines,
    int           *x_min,
    int           *x_max,
    int           *y_min,
    int           *y_max )
{
    if (lines->n_points > 0)
    {
        int i;
        int line_pixels = VIO_ROUND(lines->line_thickness);

        *x_min = *x_max = (int) Point_x(lines->points[0]);
        *y_min = *y_max = (int) Point_y(lines->points[0]);

        for_less( i, 1, lines->n_points )
        {
            int x = (int) Point_x(lines->points[i]);
            int y = (int) Point_y(lines->points[i]);
            if( x < *x_min )
                *x_min = x;
            if( x > *x_max )
                *x_max = x;
            if( y < *y_min )
                *y_min = y;
            if( y > *y_max )
                *y_max = y;
        }

        /* Extend the area by the line width in each direction.
         */
        (*x_min) -= line_pixels;
        (*x_max) += line_pixels;
        (*y_min) -= line_pixels;
        (*y_max) += line_pixels;
    }
    else
    {
        *x_min = *x_max = 0;
        *y_min = *y_max = 0;
    }
    return( lines->n_points > 0 );
}

static void
erase_brush(display_struct *slice_window)
{
  lines_struct *lines = get_lines_ptr( slice_window->slice.brush_outline );
  int x_min, x_max;
  int y_min, y_max;

  if (get_lines_limits( lines, &x_min, &x_max, &y_min, &y_max ) &&
      slice_window->slice.painting_view_index >= 0 )
    {
      set_slice_composite_update( slice_window,
                                  slice_window->slice.painting_view_index,
                                  x_min, x_max, y_min, y_max );
    }
}

/**
 * Update the position of the brush outline.
 */
static  void   update_brush(
    display_struct    *slice_window,
    int               x,
    int               y,
    VIO_BOOL          erase_flag )
{
    VIO_Real      centre[VIO_MAX_DIMENSIONS];
    int           volume_index, view_index;
    int           axis, a1, a2, start_voxel[VIO_N_DIMENSIONS];
    int           x_min, x_max, y_min, y_max;
    VIO_Real      radius[VIO_N_DIMENSIONS];
    lines_struct  *lines;

    if( erase_flag)
    {
      erase_brush(slice_window);
    }

    lines = get_lines_ptr( slice_window->slice.brush_outline );
    delete_lines( lines );
    initialize_lines( lines, Brush_outline_colour );

    if( get_brush_voxel_centre( slice_window, x, y, centre,
                                &volume_index, &view_index ) &&
        view_index == slice_window->slice.painting_view_index &&
        get_brush( slice_window, volume_index, view_index, &a1, &a2, &axis, radius ) )
    {
        start_voxel[a1] = VIO_ROUND( centre[a1] );
        start_voxel[a2] = VIO_ROUND( centre[a2] );
        start_voxel[axis] = VIO_ROUND( centre[axis] );

        while( inside_swept_brush( centre, (VIO_Vector *) NULL, radius,
                                   start_voxel ) )
            ++start_voxel[a1];

        if( start_voxel[a1] > VIO_ROUND( centre[a1] ) )
            --start_voxel[a1];

        get_brush_contour( slice_window, x, y, volume_index, view_index, a1, a2,
                           centre, radius, start_voxel, POSITIVE_X, lines );

        if( get_lines_limits( lines, &x_min, &x_max, &y_min, &y_max ) )
        {
            set_slice_composite_update( slice_window, view_index,
                                        x_min, x_max, y_min, y_max );
        }
    }
}

  void  flip_labels_around_zero(
    display_struct  *slice_window )
{
    int             label_x, label_x_opp;
    int             int_voxel[VIO_MAX_DIMENSIONS], sizes[VIO_MAX_DIMENSIONS];
    int             int_voxel_opp[VIO_MAX_DIMENSIONS];
    VIO_Real            voxel[VIO_MAX_DIMENSIONS], flip_voxel;
    VIO_Volume          label_volume;

    label_volume = get_label_volume( slice_window );

    convert_world_to_voxel( label_volume, 0.0, 0.0, 0.0, voxel );

    flip_voxel = voxel[VIO_X];

    get_volume_sizes( label_volume, sizes );

    for_less( int_voxel[VIO_X], 0, sizes[VIO_X] )
    {
        int_voxel_opp[VIO_X] = VIO_ROUND( flip_voxel +
                                  (flip_voxel - (VIO_Real) int_voxel[VIO_X]) );
        if( int_voxel_opp[VIO_X] <= int_voxel[VIO_X] ||
            int_voxel_opp[VIO_X] < 0 || int_voxel_opp[VIO_X] >= sizes[VIO_X] )
            continue;

        for_less( int_voxel[VIO_Y], 0, sizes[VIO_Y] )
        {
            int_voxel_opp[VIO_Y] = int_voxel[VIO_Y];
            for_less( int_voxel[VIO_Z], 0, sizes[VIO_Z] )
            {
                int_voxel_opp[VIO_Z] = int_voxel[VIO_Z];

                label_x = get_voxel_label( slice_window,
                                 get_current_volume_index(slice_window),
                                 int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z] );
                label_x_opp = get_voxel_label( slice_window,
                        get_current_volume_index(slice_window),
                        int_voxel_opp[VIO_X], int_voxel_opp[VIO_Y], int_voxel_opp[VIO_Z] );

                set_voxel_label( slice_window,
                                 get_current_volume_index(slice_window),
                                 int_voxel_opp[VIO_X], int_voxel_opp[VIO_Y],
                                 int_voxel_opp[VIO_Z], label_x );
                set_voxel_label( slice_window,
                                 get_current_volume_index(slice_window),
                                 int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z],
                                 label_x_opp );
            }
        }
    }
}

  void  translate_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              delta[] )
{
    int               c, label;
    int               src_voxel[VIO_MAX_DIMENSIONS], dest_voxel[VIO_MAX_DIMENSIONS];
    int               sizes[VIO_MAX_DIMENSIONS];
    int               first[VIO_MAX_DIMENSIONS], last[VIO_MAX_DIMENSIONS];
    int               increment[VIO_MAX_DIMENSIONS];
    VIO_progress_struct   progress;
    VIO_Volume            label_volume;

    label_volume = get_nth_label_volume( slice_window, volume_index );

    get_volume_sizes( label_volume, sizes );

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        if( delta[c] > 0 )
        {
            first[c] = sizes[c]-1;
            last[c] = -1;
            increment[c] = -1;
        }
        else
        {
            first[c] = 0;
            last[c] = sizes[c];
            increment[c] = 1;
        }
    }

    initialize_progress_report( &progress, FALSE, sizes[VIO_X] * sizes[VIO_Y],
                                "Translating Labels" );

    for( dest_voxel[VIO_X] = first[VIO_X];  dest_voxel[VIO_X] != last[VIO_X];
         dest_voxel[VIO_X] += increment[VIO_X] )
    {
        src_voxel[VIO_X] = dest_voxel[VIO_X] - delta[VIO_X];

        for( dest_voxel[VIO_Y] = first[VIO_Y];  dest_voxel[VIO_Y] != last[VIO_Y];
             dest_voxel[VIO_Y] += increment[VIO_Y] )
        {
            src_voxel[VIO_Y] = dest_voxel[VIO_Y] - delta[VIO_Y];

            for( dest_voxel[VIO_Z] = first[VIO_Z];  dest_voxel[VIO_Z] != last[VIO_Z];
                 dest_voxel[VIO_Z] += increment[VIO_Z] )
            {
                src_voxel[VIO_Z] = dest_voxel[VIO_Z] - delta[VIO_Z];

                if( int_voxel_is_within_volume( label_volume, src_voxel ) )
                {
                    label = get_voxel_label( slice_window, volume_index,
                                 src_voxel[VIO_X], src_voxel[VIO_Y], src_voxel[VIO_Z] );
                }
                else
                    label = 0;

                set_voxel_label( slice_window, volume_index,
                                 dest_voxel[VIO_X], dest_voxel[VIO_Y], dest_voxel[VIO_Z],
                                 label );
            }

            update_progress_report( &progress, dest_voxel[VIO_X] * sizes[VIO_Y] +
                                               dest_voxel[VIO_Y] + 1 );
        }
    }

    terminate_progress_report( &progress );
}

void
set_voxel_label_with_undo(display_struct *slice_window, int volume_index,
                          int voxel[], int label)
{
    if (slice_window->slice.toggle_undo_feature)
    {
        int value = get_voxel_label(slice_window, volume_index,
                                    voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z]);
        if (value != label)
        {
            undo_save(slice_window, volume_index, voxel, value);
        }
    }
    set_voxel_label( slice_window, volume_index,
                     voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z], label );
}

void  copy_labels_slice_to_slice(
    display_struct   *slice_window,
    int              volume_index,
    int              axis,
    int              src_voxel,
    int              dest_voxel,
    VIO_Real         min_threshold,
    VIO_Real         max_threshold )
{
    int              x, y, a1, a2, value;
    int              sizes[VIO_N_DIMENSIONS], src_indices[VIO_N_DIMENSIONS];
    int              dest_indices[VIO_N_DIMENSIONS];
    VIO_Real         volume_value;
    VIO_Volume       volume, label_volume;

    volume = get_nth_volume( slice_window, volume_index );
    label_volume = get_nth_label_volume( slice_window, volume_index );

    get_volume_sizes( label_volume, sizes );
    a1 = (axis + 1) % VIO_N_DIMENSIONS;
    a2 = (axis + 2) % VIO_N_DIMENSIONS;

    src_indices[axis] = src_voxel;
    dest_indices[axis] = dest_voxel;

    for_less( x, 0, sizes[a1] )
    {
        src_indices[a1] = x;
        dest_indices[a1] = x;
        for_less( y, 0, sizes[a2] )
        {
            src_indices[a2] = y;
            dest_indices[a2] = y;

            value = get_voxel_label( slice_window, volume_index,
                                     src_indices[VIO_X], src_indices[VIO_Y],
                                     src_indices[VIO_Z] );

            if( min_threshold < max_threshold )
            {
                volume_value = get_volume_real_value( volume,
                            dest_indices[VIO_X], dest_indices[VIO_Y], dest_indices[VIO_Z],
                            0, 0 );
                if( volume_value < min_threshold ||
                    volume_value > max_threshold )
                    value = 0;
            }

            set_voxel_label_with_undo(slice_window, volume_index,
                                      dest_indices, value);
        }
    }
}
