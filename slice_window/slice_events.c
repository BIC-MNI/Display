/* ----------------------------------------------------------------------------
@COPYRIGHT  :
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
---------------------------------------------------------------------------- */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif

#include  <display.h>

static    DEF_EVENT_FUNCTION( window_size_changed );
static    DEF_EVENT_FUNCTION( handle_redraw );
static    DEF_EVENT_FUNCTION( handle_redraw_overlay );
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
    VIO_Real             *value );
static  VIO_BOOL   mouse_is_near_slice_dividers(
    display_struct   *slice_window );
static  VIO_BOOL  mouse_is_near_low_limit(
    display_struct   *slice_window );
static  VIO_BOOL  mouse_is_near_high_limit(
    display_struct   *slice_window );
static  VIO_BOOL  get_nearest_mouse_colour_bar_value(
    display_struct   *slice_window,
    VIO_Real             *value );

  void  initialize_slice_window_events(
    display_struct    *slice_window )
{
    add_action_table_function( &slice_window->action_table, WINDOW_RESIZE_EVENT,
                               window_size_changed );
    add_action_table_function( &slice_window->action_table, WINDOW_REDRAW_EVENT,
                               handle_redraw );
    add_action_table_function( &slice_window->action_table,
                               REDRAW_OVERLAY_EVENT,
                               handle_redraw_overlay );
    add_action_table_function( &slice_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               left_mouse_down );
    add_action_table_function( &slice_window->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               middle_mouse_down );
    add_action_table_function( &slice_window->action_table, NO_EVENT,
                               update_probe );

    fill_Point( slice_window->prev_mouse_position, 0.0, 0.0, 0.0 );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( left_mouse_down )
{
    int          view_index;

    if( get_n_volumes( display ) == 0 )
        return( VIO_OK );

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

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( middle_mouse_down )
{
    int          view_index;
    VIO_Real         value;

    if( get_n_volumes( display ) == 0 )
        return( VIO_OK );

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
    else if( get_mouse_colour_bar_value( display->associated[SLICE_WINDOW],
                                         &value ) )

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

static  void  set_slice_voxel_position(
    display_struct    *slice_window,
    int               volume_index,
    VIO_Real              voxel[] )
{
    display_struct    *display;
    int               c, sizes[VIO_MAX_DIMENSIONS];
    VIO_Real              clipped_voxel[VIO_MAX_DIMENSIONS];

    get_volume_sizes( get_nth_volume(slice_window,volume_index), sizes );

    for_less( c, 0, VIO_N_DIMENSIONS )
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
            /*rebuild_selected_list( display, slice_window->associated[MENU_WINDOW] );*/
            set_update_required( slice_window->associated[MENU_WINDOW], NORMAL_PLANES );

            rebuild_selected_list( display, slice_window->associated[MARKER_WINDOW] );
            set_update_required( slice_window->associated[MARKER_WINDOW], NORMAL_PLANES );
        }
    }
}

/* ----------------------------------------------------------------------- */

  void  set_voxel_cursor_from_mouse_position(
    display_struct    *slice_window )
{
    int    volume_index, axis_index;
    VIO_Real   voxel[VIO_N_DIMENSIONS];

    if( get_voxel_in_slice_window( slice_window, voxel, &volume_index,
                                   &axis_index ) )
    {
        set_slice_voxel_position( slice_window, volume_index, voxel );
    }
}

static  void  update_voxel_cursor(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
        set_voxel_cursor_from_mouse_position( slice_window );
}

/* ARGSUSED */

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

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_update_voxel )
{
    update_voxel_cursor( display );

    return( VIO_OK );
}

/* ----------------------------------------------------------------------- */

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

static  DEF_EVENT_FUNCTION( update_picking_slice )
{
    update_voxel_slice( display );

    return( VIO_OK );
}

/* ----------------------------------------------------------------------- */

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

static  DEF_EVENT_FUNCTION( update_slice_zooming )
{
    update_voxel_zoom( display );

    return( VIO_OK );
}

/* ------------------------------------------------------ */

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

static  DEF_EVENT_FUNCTION( update_translation )
{
    perform_translation( display );

    return( VIO_OK );
}

/* ----------------------------------------------------------------------- */

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( update_probe )
{
    int  x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) )
        set_probe_update( display );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_redraw )
{
    set_slice_viewport_update( display, FULL_WINDOW_MODEL );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_redraw_overlay )
{
    int  i;

    for_less( i, 0, N_MODELS )
    {
        if( get_model_bitplanes( get_graphics_model(display,i) ) ==
                                                            OVERLAY_PLANES )
            set_slice_viewport_update( display, i );
    }

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( window_size_changed )
{
    int   view;

    for_less( view, 0, N_SLICE_VIEWS )
        resize_slice_view( display, view );

    update_all_slice_models( display );
    set_slice_window_all_update( display, -1, UPDATE_BOTH );
    resize_histogram( display );

    return( VIO_OK );
}

/* ------------------------------------------------------------------ */

/* ARGSUSED */

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

/* ARGSUSED */

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

static  void  update_limit(
    display_struct   *slice_window,
    VIO_BOOL          low_limit_flag,
    VIO_BOOL          fixed_range_flag )
{
    VIO_Real                  range, min_value, max_value, value;
    VIO_Real                  volume_min, volume_max;
    VIO_Volume                volume;
    colour_coding_struct  *colour_coding;

    if( get_nearest_mouse_colour_bar_value( slice_window, &value ) &&
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

static  VIO_BOOL  get_mouse_colour_bar_value(
    display_struct   *slice_window,
    VIO_Real             *value )
{
    int                   x, y;
    VIO_Real                  ratio, min_value, max_value;
    VIO_Volume                volume;
    VIO_BOOL               found;

    found = FALSE;

    if( G_get_mouse_position( slice_window->window, &x, &y ) &&
        mouse_within_colour_bar( slice_window, (VIO_Real) x, (VIO_Real) y, &ratio ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_real_range( volume, &min_value, &max_value );
        *value = VIO_INTERPOLATE( ratio, min_value, max_value );
        found = TRUE;
    }

    return( found );
}

static  VIO_BOOL  get_nearest_mouse_colour_bar_value(
    display_struct   *slice_window,
    VIO_Real             *value )
{
    int                   x, y;
    VIO_Real                  ratio, min_value, max_value;
    VIO_Volume                volume;
    VIO_BOOL               found;

    found = FALSE;

    if( G_get_mouse_position( slice_window->window, &x, &y ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        (void) mouse_within_colour_bar( slice_window, (VIO_Real) x, (VIO_Real) y,
                                        &ratio );

        if( ratio < 0.0 )
            ratio = 0.0;
        else if( ratio > 1.0 )
            ratio = 1.0;

        get_volume_real_range( volume, &min_value, &max_value );
        *value = VIO_INTERPOLATE( ratio, min_value, max_value );

        found = TRUE;
    }
    else
        found = FALSE;

    return( found );
}

static  VIO_BOOL  mouse_is_near_low_limit(
    display_struct   *slice_window )
{
    VIO_Real                  value, min_value, max_value;
    VIO_BOOL               near;
    colour_coding_struct  *colour_coding;

    near = FALSE;

    if( get_mouse_colour_bar_value( slice_window, &value ) )
    {
        colour_coding = &slice_window->slice.volumes
                       [get_current_volume_index(slice_window)].colour_coding;

        get_colour_coding_min_max( colour_coding, &min_value, &max_value );

        if( min_value <= max_value &&
            value < (colour_coding->min_value+colour_coding->max_value)/2.0 ||
            min_value > max_value &&
            value > (colour_coding->min_value+colour_coding->max_value)/2.0 )
            near = TRUE;
    }

    return( near );
}

static  VIO_BOOL  mouse_is_near_high_limit(
    display_struct   *slice_window )
{
    VIO_Real                  value, min_value, max_value;
    VIO_BOOL               near;
    colour_coding_struct  *colour_coding;

    near = FALSE;

    if( get_mouse_colour_bar_value( slice_window, &value ) )
    {
        colour_coding = &slice_window->slice.volumes
                       [get_current_volume_index(slice_window)].colour_coding;

        get_colour_coding_min_max( colour_coding, &min_value, &max_value );

        if( min_value <= max_value &&
            value > (colour_coding->min_value+colour_coding->max_value)/2.0 ||
            min_value > max_value &&
            value < (colour_coding->min_value+colour_coding->max_value)/2.0 )
            near = TRUE;
    }

    return( near );
}

#define  NEAR_ENOUGH  10

static  VIO_BOOL  mouse_is_near_slice_dividers(
    display_struct   *slice_window )
{
    int       x, y, x_div, y_div, dx, dy;
    VIO_BOOL   near;

    near = FALSE;

    if( G_get_mouse_position( slice_window->window, &x, &y ) )
    {
        get_slice_divider_intersection( slice_window, &x_div, &y_div );

        dx = x - x_div;
        dy = y - y_div;
        near = VIO_ABS(dx) < NEAR_ENOUGH && VIO_ABS(dy) < NEAR_ENOUGH;
    }

    return( near );
}
/* ----------------------------------------------------------------------- */

static  void  update_slice_dividers(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
    {
        set_slice_divider_position( slice_window, x, y );
    }
}

/* ARGSUSED */

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

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_update_slice_dividers )
{
    update_slice_dividers( display );

    return( VIO_OK );
}
