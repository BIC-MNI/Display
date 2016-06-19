/**
 * \file slice_events.c
 * \brief Handle window events for the slice window.
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

static    DEF_EVENT_FUNCTION( window_size_changed );
static    DEF_EVENT_FUNCTION( handle_redraw );
static    DEF_EVENT_FUNCTION( update_probe );

static    DEF_EVENT_FUNCTION( update_translation );
static    DEF_EVENT_FUNCTION( terminate_translation );

static    DEF_EVENT_FUNCTION( handle_update_slice_dividers );
static    DEF_EVENT_FUNCTION( terminate_setting_slice_dividers );

static    DEF_EVENT_FUNCTION( update_picking_slice );
static    DEF_EVENT_FUNCTION( terminate_picking_slice );

static    DEF_EVENT_FUNCTION( update_slice_zooming );
static    DEF_EVENT_FUNCTION( terminate_slice_zooming );

static    DEF_EVENT_FUNCTION( handle_update_voxel );
static    DEF_EVENT_FUNCTION( terminate_picking_voxel );

static    DEF_EVENT_FUNCTION( left_mouse_down );
static    DEF_EVENT_FUNCTION( middle_mouse_down );
static    DEF_EVENT_FUNCTION( scroll_down );
static    DEF_EVENT_FUNCTION( scroll_up );
static    DEF_EVENT_FUNCTION( handle_update_low_limit );
static    DEF_EVENT_FUNCTION( handle_update_high_limit );
static    DEF_EVENT_FUNCTION( handle_update_both_limits );
static    DEF_EVENT_FUNCTION( terminate_picking_low_limit );
static    DEF_EVENT_FUNCTION( terminate_picking_high_limit );
static    DEF_EVENT_FUNCTION( terminate_picking_both_limits );

static  void  update_limit(
    display_struct   *slice_window,
    VIO_BOOL          low_limit_flag,
    VIO_BOOL          fixed_range_flag );
static  VIO_BOOL  get_mouse_colour_bar_value(
    display_struct   *slice_window,
    VIO_Real         *value,
    VIO_BOOL         nearest);
static  VIO_BOOL   mouse_is_near_slice_dividers(
    display_struct   *slice_window );
static  VIO_BOOL  mouse_is_near_low_limit(
    display_struct   *slice_window );
static  VIO_BOOL  mouse_is_near_high_limit(
    display_struct   *slice_window );

static void initialize_measurement( display_struct *slice_window );

/**
 * \brief Install handlers for events in the slice window.
 *
 * The slice window intercepts standard events like resize and redraw.
 * It also handles left, middle, and right mouse down events, which are
 * used to request various UI tasks.
 * The scroll wheel events are used to set the zoom level of the slice.
 *
 * \param slice_window The display_struct of the slice window.
 */
void  initialize_slice_window_events(
    display_struct    *slice_window )
{
    add_action_table_function( &slice_window->action_table, WINDOW_RESIZE_EVENT,
                               window_size_changed );
    add_action_table_function( &slice_window->action_table, WINDOW_REDRAW_EVENT,
                               handle_redraw );
    add_action_table_function( &slice_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               left_mouse_down );
    add_action_table_function( &slice_window->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               middle_mouse_down );
    add_action_table_function( &slice_window->action_table,
                               SCROLL_DOWN_EVENT,
                               scroll_down );
    add_action_table_function( &slice_window->action_table,
                               SCROLL_UP_EVENT,
                               scroll_up );
    add_action_table_function( &slice_window->action_table, NO_EVENT,
                               update_probe );

    fill_Point( slice_window->prev_mouse_position, 0.0, 0.0, 0.0 );

    initialize_measurement( slice_window );
}

/**
 * Prompt the user to select the colour coding type for the volume colour
 * coding.
 *
 * \param slice_window The display_struct of the slice window.
 */
static void
prompt_volume_coding_type( display_struct *slice_window )
{
    Colour_coding_types cc_type;
    int volume_index = get_current_volume_index(slice_window);

    if (volume_index < 0)
    {
        return;
    }
    if( get_user_coding_type( "Select a new colour coding type", &cc_type ) == VIO_OK )
    {
        set_colour_coding_type( &slice_window->slice.
                                volumes[volume_index].colour_coding,
                                cc_type );

        colour_coding_has_changed( slice_window, volume_index,
                                   UPDATE_SLICE );
    }
}

/**
 * Returns true if the mouse is over the colour bar. As an additional
 * service, it calculates the y positions of the upper and lower limits, in
 * pixels.
 * \param slice_window A pointer to the display_struct of the slice window.
 * \param y_lo A pointer to a location to receive the pixel position of the
 * lower limit.
 * \param y_hi A pointer to a location to receive the pixel position of the
 * upper limit.
 * \returns TRUE if the mouse cursor was in the colour bar and no other error
 * occurs.
 */
static VIO_BOOL  get_colour_bar_positions( display_struct *slice_window,
                                           VIO_Real *y_lo, VIO_Real *y_hi)
{
    int               x, y;
    VIO_Real          ratio;
    VIO_Real          lo_limit, hi_limit;
    VIO_Real          lo_range, hi_range;
    int               volume_index;
    VIO_Volume        volume;
    int               x_min, x_max;
    int               y_min, y_max;

    if ( !G_get_mouse_position( slice_window->window, &x, &y ) )
        return FALSE;

    if ( !mouse_within_colour_bar( slice_window, x, y, &ratio ))
        return FALSE;

    volume_index = get_current_volume_index( slice_window );
    if (volume_index < 0)
        return FALSE;

    if (!get_slice_window_volume( slice_window, &volume ) )
        return FALSE;

    get_volume_real_range( volume, &lo_range, &hi_range );
    get_colour_coding_min_max( &slice_window->slice.
                               volumes[volume_index].colour_coding,
                               &lo_limit, &hi_limit);
    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );

    int bottom = y_min + slice_window->slice.colour_bar.bottom_offset;
    int top = y_max - slice_window->slice.colour_bar.top_offset;

    *y_lo = bottom + ((top - bottom) *
                              (lo_limit - lo_range) / (hi_range - lo_range));
    *y_hi = bottom + ((top - bottom) *
                              (hi_limit - lo_range) / (hi_range - lo_range));
    return TRUE;
}


/**
 * \brief Initialize the measurement line structures.
 *
 * \param slice_window A pointer to the display_struct of the slice window.
 */
static void
initialize_measurement( display_struct *slice_window )
{
    int i;
    for (i = 0; i < N_MEASUREMENTS; i++)
    {
        slice_window->slice.measure[i].line = NULL;
        slice_window->slice.measure[i].text = NULL;
        if (i == 0)
          slice_window->slice.measure[i].colour = Measure_colour;
        else if (i == 1)
          slice_window->slice.measure[i].colour = MAGENTA;
        else
          slice_window->slice.measure[i].colour = CYAN;
    }
    slice_window->slice.measure_view = -1;
    slice_window->slice.measure_number = 0;
}

/**
 * \brief Remove the measurement lines from the slice view.
 *
 * Called when an event occurs such that the measurement lines need to
 * be deleted from the slice view - e.g. when the window is resized or
 * when a non-measurement mouse click occurs.
 *
 * \param slice_window A pointer to the display_struct for the slice window.
 */
static void
remove_measurement( display_struct *slice_window )
{
    model_struct *model_ptr;
    int view_index = slice_window->slice.measure_view;
    int i;

    if (view_index < 0)
        return;

    model_ptr = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    for (i = 0; i < N_MEASUREMENTS; i++)
    {
      if (slice_window->slice.measure[i].line != NULL)
      {
        remove_object_from_model( model_ptr,
                                  slice_window->slice.measure[i].line );
        remove_object_from_model( model_ptr,
                                  slice_window->slice.measure[i].text );
        delete_object( slice_window->slice.measure[i].line );
        delete_object( slice_window->slice.measure[i].text );

        slice_window->slice.measure[i].line = NULL;
        slice_window->slice.measure[i].text = NULL;
      }
    }
    slice_window->slice.measure_view = -1; /* Unassociated. */
    slice_window->slice.measure_number = 0;
}

/**
 * \brief Update the position of a measurement line.
 *
 * Called in response to mouse movement during a "ctrl-left click"
 * measurement event.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static
DEF_EVENT_FUNCTION( update_measurement )
{
    int       volume_index, axis_index;
    VIO_Real  voxel[VIO_MAX_DIMENSIONS];
    VIO_Real  wx, wy, wz;
    VIO_Real  px, py;

    if( get_voxel_in_slice_window( display, voxel, &volume_index,
                                   &axis_index ) )
    {
      VIO_Volume volume = get_nth_volume( display, volume_index );
      char buffer[128];
      text_struct *text_ptr;
      lines_struct *line_ptr;
      VIO_Real distance;
      VIO_Real x0, x1;
      VIO_Real y0, y1;
      VIO_Real y_text_off;
      struct measurement_line *meas_ptr = 
        &display->slice.measure[display->slice.measure_number];

      text_ptr = get_text_ptr( meas_ptr->text );
      line_ptr = get_lines_ptr( meas_ptr->line );
      convert_voxel_to_world( volume, voxel, &wx, &wy, &wz );
      fill_Point( meas_ptr->end, wx, wy, wz );

      delete_string( text_ptr->string );
      distance = distance_between_points( &meas_ptr->origin, &meas_ptr->end );
      sprintf( buffer, "%g", distance );
      text_ptr->string = create_string( buffer );

      /* Figure out the best text position.
       */
      x0 = Point_x( line_ptr->points[0] );
      x1 = Point_x( line_ptr->points[1] );
      y0 = Point_y( line_ptr->points[0] );
      y1 = Point_y( line_ptr->points[1] );

      y_text_off = ((x0 - x1) * (y0 - y1) < 0) ? 4.0 : -12.0;

      fill_Point( text_ptr->origin,
                  (x0 + x1) / 2.0 + 4.0,
                  (y0 + y1) / 2.0 + y_text_off,
                  0.0 );

      convert_voxel_to_pixel( display, volume_index,
                              display->slice.measure_view,
                              voxel, &px, &py );

      fill_Point(line_ptr->points[1], px, py, 0.0);

      G_set_update_flag( display->window ); /* Immediate update */
    }
    return (VIO_OK);
}

/**
 * \brief Stop dragging a measurement line.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static
DEF_EVENT_FUNCTION( terminate_measurement )
{
    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_measurement );

    /* Use the next slot next time. */
    if (++display->slice.measure_number >= N_MEASUREMENTS)
        display->slice.measure_number = 0;

    /*
     * Trigger update of the intensity profile graph.
     */
    set_slice_cursor_update( display, get_arbitrary_view_index( display ) );

    G_set_update_flag( display->window ); /* immediate update */
    return( VIO_OK );
}

/**
 * \brief Start dragging a measurement line.
 *
 * These are one of N_MEASUREMENTS lines that can be used to show a
 * specific segment in any direction along the slice view. Each is
 * displayed with a length in world units (generally mm).
 *
 * \param slice_window A pointer to the display_struct for the slice window.
 * \param view_index The keyboard code, if any.
 */
static void
start_measurement(display_struct *slice_window, int view_index)
{
    int          volume_index, axis_index;
    VIO_Real     voxel[VIO_MAX_DIMENSIONS];
    VIO_Real     wx, wy, wz;
    lines_struct *line_ptr;
    text_struct  *text_ptr;
    model_struct *model_ptr;
    VIO_Real     px, py;
    struct measurement_line *meas_ptr;

    if (slice_window->slice.measure_view != -1 &&
        slice_window->slice.measure_view != view_index)
    {
        remove_measurement( slice_window );
    }

    meas_ptr = &slice_window->slice.measure[slice_window->slice.measure_number];

    if( !get_voxel_in_slice_window( slice_window, voxel, &volume_index,
                                    &axis_index ) )
    {
        return;
    }

    VIO_Volume volume = get_nth_volume( slice_window, volume_index );
    VIO_Colour col = meas_ptr->colour;
    convert_voxel_to_world( volume, voxel, &wx, &wy, &wz );
    fill_Point( meas_ptr->origin, wx, wy, wz );

    if (meas_ptr->line == NULL)
    {
        meas_ptr->line = create_object( LINES );
        line_ptr = get_lines_ptr( meas_ptr->line );
        set_object_visibility( meas_ptr->line, TRUE );

        initialize_lines(line_ptr, col );
        line_ptr->n_points = 2;
        line_ptr->n_items = 1;
        ALLOC( line_ptr->points, line_ptr->n_points );
        ALLOC( line_ptr->end_indices, line_ptr->n_items );
        ALLOC( line_ptr->indices, line_ptr->n_points );
        line_ptr->end_indices[0] = 2;
        line_ptr->indices[0] = 0;
        line_ptr->indices[1] = 1;

        meas_ptr->text = create_object( TEXT );
        text_ptr = get_text_ptr( meas_ptr->text );
        set_object_visibility( meas_ptr->text, TRUE );

        initialize_text( text_ptr, NULL, col,
                         Measure_text_font,
                         Measure_text_size );

        model_ptr = get_graphics_model( slice_window,
                                        SLICE_MODEL1 + view_index );
        add_object_to_model( model_ptr, meas_ptr->line );
        add_object_to_model( model_ptr, meas_ptr->text );
    }
    else
    {
        line_ptr = get_lines_ptr( meas_ptr->line );
    }

    convert_voxel_to_pixel( slice_window, volume_index, view_index,
                            voxel, &px, &py );

    fill_Point( line_ptr->points[0], px, py, 0.0 );
    fill_Point( line_ptr->points[1], px, py, 0.0 );

    slice_window->slice.measure_view = view_index;
    G_set_update_flag( slice_window->window ); /* immediate update */
}

/* ARGSUSED */

/**
 * \brief Event handler for left mouse button down events.
 *
 * The left mouse button is used to change the current cursor position, if
 * the mouse pointer is over a slice view. If the mouse pointer is near the
 * middle of the slice dividers, this starts adjusting the relative sizes of
 * the four panels. If the mouse pointer is over the colour bar, the upper
 * or lower coding limits will be adjusted.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( left_mouse_down )
{
    int          view_index;
    VIO_Real     lo_y, hi_y;

    if( get_n_volumes( display ) == 0 )
        return( VIO_OK );

    if (!is_ctrl_key_pressed())
      remove_measurement( display );

    if( mouse_is_near_slice_dividers( display ) )
    {
        push_action_table( &display->action_table, NO_EVENT );
        push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT);

        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   handle_update_slice_dividers );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   terminate_setting_slice_dividers );
        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_setting_slice_dividers );
    }
    else if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        push_action_table( &display->action_table, NO_EVENT );
        push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT);

        if( is_shift_key_pressed() )
        {
            add_action_table_function( &display->action_table,
                                       NO_EVENT, update_translation );

            add_action_table_function( &display->action_table,
                                       LEFT_MOUSE_UP_EVENT,
                                       terminate_translation );
            add_action_table_function( &display->action_table,
                                       TERMINATE_INTERACTION_EVENT,
                                       terminate_translation );
        }
        else if (is_ctrl_key_pressed() )
        {
            add_action_table_function( &display->action_table,
                                       NO_EVENT, update_measurement );

            add_action_table_function( &display->action_table,
                                       LEFT_MOUSE_UP_EVENT,
                                       terminate_measurement );
            add_action_table_function( &display->action_table,
                                       TERMINATE_INTERACTION_EVENT,
                                       terminate_measurement );

            start_measurement(display, view_index);
        }
        else
        {
            add_action_table_function( &display->action_table,
                                       NO_EVENT,
                                       handle_update_voxel );

            add_action_table_function( &display->action_table,
                                       LEFT_MOUSE_UP_EVENT,
                                       terminate_picking_voxel );
            add_action_table_function( &display->action_table,
                                       TERMINATE_INTERACTION_EVENT,
                                       terminate_picking_voxel );

            set_voxel_cursor_from_mouse_position( display );
        }
    }
    else if (is_shift_key_pressed() &&
             get_colour_bar_positions( display, &lo_y, &hi_y))
    {
        int x, y;

        G_get_mouse_position( display->window, &x, &y);
        if (y <= lo_y)
            set_under_colour( display, NULL, NULL );
        else if (y >= hi_y)
            set_over_colour( display, NULL, NULL );
        else
            prompt_volume_coding_type( display );
        return VIO_ERROR;
    }
    else if( mouse_is_near_low_limit( display ) )
    {
        push_action_table( &display->action_table, NO_EVENT );
        push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT);

        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   handle_update_low_limit );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   terminate_picking_low_limit );
        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_picking_low_limit );
    }
    else if( mouse_is_near_high_limit( display ) )
    {
        push_action_table( &display->action_table, NO_EVENT );
        push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT);

        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   handle_update_high_limit );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   terminate_picking_high_limit );
        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_picking_high_limit );
    }


    record_mouse_pixel_position( display );

    return( VIO_OK );
}

/** Amount by which the volume opacity is changed during a ctrl+scroll
 * event
 */
#define OPACITY_DELTA 0.08

/**
 * \brief Event handler for scroll down events.
 *
 * Changes the zoom level of the slice image under the mouse pointer, or
 * the opacity of the volume if the control key is depressed.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( scroll_down )
{
    int          view_index;

    if( get_n_volumes( display ) == 0 )
        return( VIO_OK );

    remove_measurement( display );

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        if (is_ctrl_key_pressed() )
        {
          int volume_index = get_current_volume_index( display );
          VIO_Real opacity = display->slice.volumes[volume_index].opacity;
          opacity -= OPACITY_DELTA;
          if (opacity < 0.0)
          {
              opacity = 0.0;
          }
          set_volume_opacity(display, volume_index, opacity);
        }
        else
        {
            scale_slice_view( display, view_index, 1.0/(1.1) );
        }
    }
    return( VIO_OK );
}

/**
 * \brief Event handler for scroll up events.
 *
 * Changes the zoom level of the slice image under the mouse pointer, or
 * the opacity of the volume if the control key is pressed.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( scroll_up )
{
    int          view_index;

    if( get_n_volumes( display ) == 0 )
        return( VIO_OK );

    remove_measurement( display );

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        if (is_ctrl_key_pressed() )
        {
          int volume_index = get_current_volume_index( display );
          VIO_Real opacity = display->slice.volumes[volume_index].opacity;
          opacity += OPACITY_DELTA;
          if (opacity > 1.0)
          {
              opacity = 1.0;
          }
          set_volume_opacity(display, volume_index, opacity);
        }
        else
        {
            scale_slice_view( display, view_index, 1.1 );
        }
    }
    return( VIO_OK );
}

/* ARGSUSED */

/**
 * \brief Event handler for middle mouse button down events.
 *
 * The middle mouse button is used to change the current slice, if
 * the mouse pointer is over a slice view and the shift key is not
 * pressed. If the shift key is pressed, then the zoom level will
 * be adjusted.
 *
 * If the mouse cursor is over the colour bar, both the upper and
 * lower colour limits will be updated simultaneously.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( middle_mouse_down )
{
    int          view_index;
    VIO_Real         value;

    if( get_n_volumes( display ) == 0 )
        return( VIO_OK );

    remove_measurement( display );

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        push_action_table( &display->action_table, NO_EVENT );
        push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT);

        if( is_shift_key_pressed() )
        {
            add_action_table_function( &display->action_table,
                                       NO_EVENT, update_slice_zooming );

            add_action_table_function( &display->action_table,
                                       MIDDLE_MOUSE_UP_EVENT,
                                       terminate_slice_zooming );
            add_action_table_function( &display->action_table,
                                       TERMINATE_INTERACTION_EVENT,
                                       terminate_slice_zooming );
        }
        else
        {
            add_action_table_function( &display->action_table,
                                       NO_EVENT, update_picking_slice );

            add_action_table_function( &display->action_table,
                                       MIDDLE_MOUSE_UP_EVENT,
                                       terminate_picking_slice );
            add_action_table_function( &display->action_table,
                                       TERMINATE_INTERACTION_EVENT,
                                       terminate_picking_slice );
        }
    }
    else if( get_mouse_colour_bar_value( display, &value, FALSE ) )
    {
        push_action_table( &display->action_table, NO_EVENT );
        push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT);

        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   handle_update_both_limits );

        add_action_table_function( &display->action_table,
                                   MIDDLE_MOUSE_UP_EVENT,
                                   terminate_picking_both_limits );
        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_picking_both_limits );
    }

    record_mouse_pixel_position( display );

    return( VIO_OK );
}

/**
 * Sets the current voxel cursor position for the given volume.
 *
 * \param slice_window The display_struct of the slice window.
 * \param volume_index The zero-based index of the desired volume.
 * \param voxel The new voxel coordinates for the cursor.
 */
static  void  set_slice_voxel_position(
    display_struct *slice_window,
    int            volume_index,
    const VIO_Real voxel[] )
{
    display_struct    *display;
    int               c, sizes[VIO_MAX_DIMENSIONS];
    VIO_Real          clipped_voxel[VIO_MAX_DIMENSIONS];
    VIO_Volume        volume = get_nth_volume( slice_window, volume_index );
    int               n_dimensions;

    get_volume_sizes( volume, sizes );

    n_dimensions = get_volume_n_dimensions( volume );

    for_less( c, 0, n_dimensions )
    {
        if( voxel[c] < -0.5 )
            clipped_voxel[c] = -0.5;
        else if( voxel[c] > (VIO_Real) sizes[c] - 0.5 )
            clipped_voxel[c] = (VIO_Real) sizes[c] - 0.5;
        else
            clipped_voxel[c] = voxel[c];
    }

    if( set_current_voxel( slice_window, volume_index, clipped_voxel ) )
    {
        display = get_three_d_window( slice_window );

        if( update_cursor_from_voxel( slice_window ) )
            set_update_required( display, get_cursor_bitplanes() );

        if( update_current_marker( display, volume_index, clipped_voxel ) )
        {
            set_update_required( get_display_by_type( MENU_WINDOW ),
                                 NORMAL_PLANES );

            rebuild_selected_list( display,
                                   get_display_by_type( MARKER_WINDOW ) );
            set_update_required( get_display_by_type( MARKER_WINDOW ),
                                 NORMAL_PLANES );
        }
    }
}

/* ----------------------------------------------------------------------- */

/**
 * Sets the current volume's voxel cursor position to coincide with the
 * position of the mouse pointer.
 *
 * \param slice_window The display_struct of the slice window.
 */
  void  set_voxel_cursor_from_mouse_position(
    display_struct    *slice_window )
{
    int    volume_index, axis_index;
    VIO_Real   voxel[VIO_MAX_DIMENSIONS];

    if( get_voxel_in_slice_window( slice_window, voxel, &volume_index,
                                   &axis_index ) )
    {
        set_slice_voxel_position( slice_window, volume_index, voxel );
    }
}

/**
 * Change the current cursor position, according to mouse
 * movements. Generally invoked in response to a left button press
 * over one of the slice views.
 *
 * \param slice_window The display_struct of the slice window.
 */
static  void  update_voxel_cursor(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
        set_voxel_cursor_from_mouse_position( slice_window );
}

/* ARGSUSED */

/**
 * \brief Event handler for left mouse button up events during cursor movement.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( terminate_picking_voxel )
{
    update_voxel_cursor( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_picking_voxel );

    return( VIO_OK );
}

/**
 * \brief Event handler for mouse movement events during cursor movement.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( handle_update_voxel )
{
    update_voxel_cursor( display );

    return( VIO_OK );
}

/* ----------------------------------------------------------------------- */

/**
 * Change the slice viewed, according to mouse movements. This is used
 * to implement the slice selection using the middle button.
 *
 * \param slice_window The display_struct of the slice window.
 */
static  void  update_voxel_slice(
    display_struct    *slice_window )
{
    int        view_index, dy, x, y, x_prev, y_prev;
    int        c, volume_index;
    VIO_Real       voxel[VIO_MAX_DIMENSIONS];
    VIO_Real       perp_axis[VIO_N_DIMENSIONS];

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) &&
        find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
    {
        dy = y - y_prev;

        if( dy != 0 )
        {
            volume_index = get_current_volume_index( slice_window );

            get_current_voxel( slice_window, volume_index, voxel );

            get_slice_perp_axis( slice_window, volume_index, view_index,
                                 perp_axis );

            for_less( c, 0, VIO_N_DIMENSIONS )
                voxel[c] += (VIO_Real) dy * Move_slice_speed * perp_axis[c];

            if( voxel_is_within_volume( get_nth_volume(slice_window,
                                           volume_index), voxel ) )
                set_slice_voxel_position( slice_window, volume_index, voxel );
        }
    }
}

/* ARGSUSED */

/**
 * \brief Event handler for left mouse button up events during mouse-button slice selection.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( terminate_picking_slice )
{
    update_voxel_slice( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_picking_slice );

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * \brief Event handler for mouse movement events during mouse-button slice selection.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( update_picking_slice )
{
    update_voxel_slice( display );

    return( VIO_OK );
}

/* ----------------------------------------------------------------------- */

/**
 * Set the zoom level of the slice. This is used to implement the zooming
 * of the slice with Shift+Middle Button. It is _not_ used for the scroll
 * wheel!
 *
 * \param slice_window The display_struct of the slice window.
 */
static  void  update_voxel_zoom(
    display_struct    *slice_window )
{
    int        view_index, x, y, x_prev, y_prev, dy;
    VIO_Real       scale_factor;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) &&
        find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
    {
        dy = y - y_prev;

        scale_factor = pow( 2.0, (VIO_Real) dy / Pixels_per_double_size );

        scale_slice_view( slice_window, view_index, scale_factor );
    }
}

/* ARGSUSED */

/**
 * \brief Event handler for left mouse button up events during mouse-button zooming.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( terminate_slice_zooming )
{
    update_voxel_zoom( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_slice_zooming );

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * \brief Event handler for mouse movement events during mouse-button zooming.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( update_slice_zooming )
{
    update_voxel_zoom( display );

    return( VIO_OK );
}

/* ------------------------------------------------------ */

/**
 * Translate the slice view according to the mouse movement. This is used
 * to implement the "Shift+Left button" dragging of the slice views.
 *
 * \param slice_window The display_struct of the slice window.
 */
static  void  perform_translation(
    display_struct   *slice_window )
{
    int        view_index, x, y, x_prev, y_prev, dx, dy;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) &&
        find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
    {
        dx = x - x_prev;
        dy = y - y_prev;

        translate_slice_view( slice_window, view_index, (VIO_Real) dx, (VIO_Real) dy );
        set_slice_window_update( slice_window, -1, view_index, UPDATE_BOTH );

        record_mouse_pixel_position( slice_window );
    }
}

/* ARGSUSED */

/**
 * \brief Event handler for left mouse button up events during mouse-button slice translation.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( terminate_translation )
{
    perform_translation( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_translation );

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * \brief Event handler for mouse movement events during mouse-button slice translation.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( update_translation )
{
    perform_translation( display );

    return( VIO_OK );
}

/* ----------------------------------------------------------------------- */

/* ARGSUSED */

/**
 * \brief Trigger update of the voxel position and data readout.
 *
 * This function is called in response to "NO_EVENT" events, which
 * generally correspond to timer or mouse movement events. It is
 * used to update the voxel information readout located in the lower
 * left of the slice view window.
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( update_probe )
{
    int  x, y, x_prev, y_prev;
    int  version;
    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) )
        set_probe_update( display );

#if !defined(__APPLE__)
    /* Check the FreeGLUT version and make sure it is later than 2.4.0 
     * before we try setting the cursor.
     */
#ifndef GLUT_VERSION
#define GLUT_VERSION 0x1FC
#endif
    version = glutGet(GLUT_VERSION);
    if (version <= 20400)
        return VIO_OK;
#endif /* !defined(__APPLE__) */

    /** TODO: Figure out how to make this generic. DMcD never
     * implemented cursor setting in his graphics library.
     */
    if (mouse_is_near_low_limit(display)) {
      glutSetCursor(GLUT_CURSOR_INFO);
    }
    else if (mouse_is_near_high_limit(display)) {
      glutSetCursor(GLUT_CURSOR_INFO);
    }
    else if (mouse_is_near_slice_dividers(display)) {
      glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    }
    else {
      glutSetCursor(GLUT_CURSOR_INHERIT);
    }

    return( VIO_OK );
}

/* ARGSUSED */
/**
 * \brief Handle window redraw events.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( handle_redraw )
{
    set_slice_viewport_update( display, FULL_WINDOW_MODEL );

    return( VIO_OK );
}

/**
 * \brief Handle window geometry changed events.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( window_size_changed )
{
    int   view;

    remove_measurement( display );

    for_less( view, 0, N_SLICE_VIEWS )
        resize_slice_view( display, view );

    update_all_slice_models( display );
    set_slice_window_all_update( display, -1, UPDATE_BOTH );
    resize_histogram( display );

    return( VIO_OK );
}

/**
 * \brief Stop changing the lower colour coding limit.
 *
 * Called in response to a mouse up event.
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( terminate_picking_low_limit )
{
    update_limit( display, TRUE, FALSE );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_picking_low_limit );

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * \brief Event handler that updates the lower colour coding limit.
 *
 * This function is called in response to a left mouse down or
 * subsequent mouse drag event. It updates the lower colour coding
 * limit according to the current mouse position.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( handle_update_low_limit )
{
    int   x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) ||
        graphics_update_required( display ) )
    {
        update_limit( display, TRUE, FALSE );
    }

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * \brief Stop changing the upper colour coding limit.
 *
 * Called in response to a mouse up event.
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( terminate_picking_high_limit )
{
    update_limit( display, FALSE, FALSE );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_picking_high_limit );

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * \brief Event handler that updates the upper colour coding limit.
 *
 * This function is called in response to a left mouse down or
 * subsequent mouse drag event. It updates the upper colour coding
 * limit according to the current mouse position.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( handle_update_high_limit )
{
    int   x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) ||
        graphics_update_required( display ) )
    {
        update_limit( display, FALSE, FALSE );
    }

    return( VIO_OK );
}

/* ARGSUSED */

/**
 * \brief Stop changing both colour coding limits.
 *
 * Called in response to a mouse up event.
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( terminate_picking_both_limits )
{
    update_limit( display, TRUE, TRUE );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_picking_both_limits );

    return( VIO_OK );
}

/**
 * \brief Event handler that updates both colour coding limits simultaneously.
 *
 * This function is called in response to a middle mouse down or
 * subsequent mouse drag event. It updates both colour coding
 * limits according to the current mouse position.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( handle_update_both_limits )
{
    int   x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) ||
        graphics_update_required( display ) )
    {
        update_limit( display, TRUE, TRUE );
    }

    return( VIO_OK );
}

/**
 * \brief Update the upper or lower colour coding limit.
 *
 * This function is called in response to a mouse down or mouse drag
 * event. It updates either the upper, lower, or both colour coding
 * limits according to the y position of the mouse.
 *
 * \param slice_window The display_struct of the slice window.
 * \param low_limit_flag If TRUE, update the lower limit. If FALSE, update
 * the upper limit.
 * \param fixed_range_flag If TRUE, update both limits by maintaining a fixed
 * distance between the limits.
 */
static  void  update_limit(
    display_struct   *slice_window,
    VIO_BOOL          low_limit_flag,
    VIO_BOOL          fixed_range_flag )
{
    VIO_Real                  range, min_value, max_value, value;
    VIO_Real                  volume_min, volume_max;
    VIO_Volume                volume;
    colour_coding_struct  *colour_coding;

    if( get_mouse_colour_bar_value( slice_window, &value, TRUE ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_real_range( volume, &volume_min, &volume_max );

        colour_coding = &slice_window->slice.volumes
                       [get_current_volume_index(slice_window)].colour_coding;
        min_value = colour_coding->min_value;
        max_value = colour_coding->max_value;

        range = max_value - min_value;

        if( low_limit_flag )
        {
            if( fixed_range_flag )
            {
                min_value = value;
                max_value = value + range;
            }
            else
            {
#ifdef CANNOT_MOVE_THROUGH_OTHER_LIMIT
                if( value > max_value )
                    min_value = max_value;
                else
#endif
                    min_value = value;
            }
        }
        else
        {
            if( fixed_range_flag )
            {
                min_value = value - range;
                max_value = value;
            }
            else
            {
#ifdef CANNOT_MOVE_THROUGH_OTHER_LIMIT
                if( value < min_value )
                    max_value = min_value;
                else
#endif
                    max_value = value;
            }
        }

#ifdef CANNOT_MOVE_OUTSIDE_VOLUME_RANGE
        if( fixed_range_flag )
        {
            if( min_value < volume_min )
            {
                min_value = volume_min;
                max_value = volume_min + range;
            }

            if( max_value > volume_max )
            {
                min_value = volume_max - range;
                max_value = volume_max;
            }
        }
#endif

        change_colour_coding_range( slice_window,
                get_current_volume_index(slice_window), min_value, max_value );
    }

    record_mouse_pixel_position( slice_window );
}

/**
 * \brief Get the colour bar value associated with the current mouse position.
 *
 * If the \p nearest flag is clear, the cursor must be within the colour
 * bar for this function to return TRUE, indicating success.
 *
 * If the \p nearest flag is set, the value nearest to the current mouse
 * position is returned in the \p value parameter. This means that the
 * mouse can be anywhere on the screen. This is used to "capture" the mouse
 * after the button is pressed.
 *
 * \param slice_window The display_struct of the slice window.
 * \param value A pointer to the location that will receive the colour bar
 * value corresponding to the mouse cursor location.
 * \param nearest A flag indicating that this function should return TRUE
 * even if the mouse is outside the colour bar (or even the window).
 * \returns TRUE if the colour bar value is valid.
 */
static  VIO_BOOL  get_mouse_colour_bar_value(
    display_struct  *slice_window,
    VIO_Real        *value,
    VIO_BOOL        nearest)
{
    int             x, y;
    VIO_Real        ratio, min_value, max_value;
    VIO_Volume      volume;

    if( G_get_mouse_position( slice_window->window, &x, &y ) &&
        /* Order matters here, we only examine "nearest" if this function
         * fails. That way we get the update ratio in any case.
         */
        (mouse_within_colour_bar( slice_window, x, y, &ratio ) || nearest) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        if (ratio < 0.0)
            ratio = 0.0;
        else if (ratio > 1.0)
            ratio = 1.0;
        get_volume_real_range( volume, &min_value, &max_value );
        *value = VIO_INTERPOLATE( ratio, min_value, max_value );
        return TRUE;
    }
    return FALSE;
}

/**
 * Returns TRUE if the mouse cursor in the colour bar, and is nearer to the
 * lower limit of the colour coding range. Used to decide which of the values
 * will be updated on a mouse click in the colour bar.
 *
 * \param slice_window A pointer to the display_struct of the slice window.
 * \returns TRUE if the mouse cursor is closer to the lower limit.
 */
static  VIO_BOOL  mouse_is_near_low_limit(
    display_struct   *slice_window )
{
    VIO_Real              value, min_value, max_value;
    VIO_BOOL              is_near = FALSE;
    colour_coding_struct  *colour_coding;

    if( get_mouse_colour_bar_value( slice_window, &value, FALSE ) )
    {
        colour_coding = &slice_window->slice.volumes
                       [get_current_volume_index(slice_window)].colour_coding;

        get_colour_coding_min_max( colour_coding, &min_value, &max_value );

        if( (min_value <= max_value &&
             value < (colour_coding->min_value+colour_coding->max_value)/2.0) ||
            (min_value > max_value &&
             value > (colour_coding->min_value+colour_coding->max_value)/2.0) )
            is_near = TRUE;
    }

    return( is_near );
}

/**
 * Returns TRUE if the mouse cursor in the colour bar, and is nearer to the
 * upper limit of the colour coding range. Used to decide which of the values
 * will be updated on a mouse click in the colour bar.
 *
 * \param slice_window A pointer to the display_struct of the slice window.
 * \returns TRUE if the mouse cursor is closer to the upper limit.
 */
static  VIO_BOOL  mouse_is_near_high_limit(
    display_struct   *slice_window )
{
    VIO_Real              value, min_value, max_value;
    VIO_BOOL              is_near = FALSE;
    colour_coding_struct  *colour_coding;

    if( get_mouse_colour_bar_value( slice_window, &value, FALSE ) )
    {
        colour_coding = &slice_window->slice.volumes
                       [get_current_volume_index(slice_window)].colour_coding;

        get_colour_coding_min_max( colour_coding, &min_value, &max_value );

        if( (min_value <= max_value &&
             value > (colour_coding->min_value+colour_coding->max_value)/2.0) ||
            (min_value > max_value &&
             value < (colour_coding->min_value+colour_coding->max_value)/2.0) )
            is_near = TRUE;
    }

    return( is_near );
}

/** parameter for mouse_is_near_slice_dividers() */
#define  NEAR_ENOUGH  10

/**
 * \brief Check whether the mouse pointer is close to the centre of the
 * slice dividers.
 *
 * Used to detect when a mouse down event should initiate dragging of the
 * slice dividers.
 *
 * \param slice_window A pointer to the display_struct of the slice window.
 * \returns TRUE if the mouse pointer is close
 */
static  VIO_BOOL  mouse_is_near_slice_dividers(
    display_struct   *slice_window )
{
    int       x, y, x_div, y_div, dx, dy;
    VIO_BOOL  is_near = FALSE;

    if( G_get_mouse_position( slice_window->window, &x, &y ) )
    {
        get_slice_divider_intersection( slice_window, &x_div, &y_div );

        dx = x - x_div;
        dy = y - y_div;
        is_near = VIO_ABS(dx) < NEAR_ENOUGH && VIO_ABS(dy) < NEAR_ENOUGH;
    }

    return( is_near );
}
/* ----------------------------------------------------------------------- */

/**
 * Set the center of the slice divider to be the current location of
 * the mouse. This is used to adjust the relative sizes of the four slice
 * panels.
 *
 * \param slice_window The display_struct of the slice window.
 */
static  void  update_slice_dividers(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
    {
        set_slice_divider_intersection( slice_window, x, y );
    }
}

/* ARGSUSED */

/**
 * \brief Event handler for left mouse button up events during movement of the slice dividers.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( terminate_setting_slice_dividers )
{
    update_slice_dividers( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_setting_slice_dividers );

    return( VIO_OK );
}

/**
 * \brief Event handler for mouse movement events during movement of the slice dividers.
 *
 * \param display A pointer to the display_struct for the window.
 * \param event_type The event type.
 * \param key_pressed The keyboard code, if any.
 * \returns VIO_OK if processing of this event should continue, or
 * VIO_ERROR if processing should stop.
 */
static  DEF_EVENT_FUNCTION( handle_update_slice_dividers )
{
    update_slice_dividers( display );

    return( VIO_OK );
}
