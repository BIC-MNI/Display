
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

private  void  set_voxel_cursor(
    display_struct    *slice_window );
private  void  update_window_size(
    display_struct    *slice_window );
private  void  update_limit(
    display_struct   *slice_window,
    BOOLEAN          low_limit_flag,
    BOOLEAN          fixed_range_flag );
private  BOOLEAN  get_mouse_colour_bar_value(
    display_struct   *slice_window,
    Real             *value );
private  BOOLEAN   mouse_is_near_slice_dividers(
    display_struct   *slice_window );
private  BOOLEAN  mouse_is_near_low_limit(
    display_struct   *slice_window );
private  BOOLEAN  mouse_is_near_high_limit(
    display_struct   *slice_window );

public  void  initialize_slice_window_events(
    display_struct    *slice_window )
{
    update_window_size( slice_window );

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

private  DEF_EVENT_FUNCTION( left_mouse_down )    /* ARGSUSED */
{
    int          view_index;

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

        if( is_shift_key_pressed( display ) )
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

            set_voxel_cursor( display );
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
    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( middle_mouse_down )     /* ARGSUSED */
{
    int          view_index;
    Real         value;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        push_action_table( &display->action_table, NO_EVENT );
        push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT);

        if( is_shift_key_pressed( display ) )
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
    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

private  void  set_slice_voxel_position(
    display_struct    *slice_window,
    Real              voxel[] )
{
    int    c, sizes[MAX_DIMENSIONS];
    Real   clipped_voxel[MAX_DIMENSIONS];

    get_volume_sizes( get_volume(slice_window), sizes );

    for_less( c, 0, N_DIMENSIONS )
    {
        if( voxel[c] < -0.5 )
            clipped_voxel[c] = -0.5;
        else if( voxel[c] > (Real) sizes[c] - 0.5 )
            clipped_voxel[c] = (Real) sizes[c] - 0.5;
        else
            clipped_voxel[c] = voxel[c];
    }

    if( set_current_voxel( slice_window, clipped_voxel ) )
    {
        set_update_required( slice_window, NORMAL_PLANES );

        if( update_cursor_from_voxel( slice_window ) )
        {
            set_update_required( slice_window->associated[THREE_D_WINDOW],
                                 get_cursor_bitplanes() );
        }

        if( update_current_marker( slice_window->associated[THREE_D_WINDOW],
                                   clipped_voxel ) )
        {
            rebuild_selected_list( slice_window->associated[THREE_D_WINDOW],
                                   slice_window->associated[MENU_WINDOW] );

            set_update_required( slice_window->associated[MENU_WINDOW],
                                 NORMAL_PLANES );
        }
    }
}

/* ----------------------------------------------------------------------- */

private  void  set_voxel_cursor(
    display_struct    *slice_window )
{
    int    axis_index;
    Real   voxel[N_DIMENSIONS];

    if( get_voxel_in_slice_window( slice_window, voxel, &axis_index ) )
    {
        set_slice_voxel_position( slice_window, voxel );
    }
}

private  void  update_voxel_cursor(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
        set_voxel_cursor( slice_window );
}

private  DEF_EVENT_FUNCTION( terminate_picking_voxel )     /* ARGSUSED */
{
    update_voxel_cursor( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_picking_voxel );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_voxel )     /* ARGSUSED */
{
    update_voxel_cursor( display );

    return( OK );
}

/* ----------------------------------------------------------------------- */

private  void  update_voxel_slice(
    display_struct    *slice_window )
{
    int        view_index, dy, x, y, x_prev, y_prev;
    int        c;
    Real       voxel[MAX_DIMENSIONS];
    Real       perp_axis[N_DIMENSIONS];

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) &&
        find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
    {
        dy = y - y_prev;

        if( dy != 0 )
        {
            get_current_voxel( slice_window, voxel );

            get_slice_perp_axis( slice_window, view_index, perp_axis );

            for_less( c, 0, N_DIMENSIONS )
                voxel[c] += (Real) dy * Move_slice_speed * perp_axis[c];

            if( voxel_is_within_volume( get_volume(slice_window), voxel ) )
                set_slice_voxel_position( slice_window, voxel );
        }
    }
}

private  DEF_EVENT_FUNCTION( terminate_picking_slice )     /* ARGSUSED */
{
    update_voxel_slice( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_picking_slice );

    return( OK );
}

private  DEF_EVENT_FUNCTION( update_picking_slice )     /* ARGSUSED */
{
    update_voxel_slice( display );

    return( OK );
}

/* ----------------------------------------------------------------------- */

private  void  update_voxel_zoom(
    display_struct    *slice_window )
{
    int        view_index, x, y, x_prev, y_prev, dy;
    Real       scale_factor;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) &&
        find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
    {
        dy = y - y_prev;

        scale_factor = pow( 2.0, dy / Pixels_per_double_size );

        scale_slice_view( slice_window, view_index, scale_factor );

        set_slice_window_update( slice_window, view_index );
    }
}

private  DEF_EVENT_FUNCTION( terminate_slice_zooming )     /* ARGSUSED */
{
    update_voxel_zoom( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_slice_zooming );

    return( OK );
}

private  DEF_EVENT_FUNCTION( update_slice_zooming )     /* ARGSUSED */
{
    update_voxel_zoom( display );

    return( OK );
}

/* ------------------------------------------------------ */

private  void  perform_translation(
    display_struct   *slice_window )
{
    int        view_index, x, y, x_prev, y_prev, dx, dy;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) &&
        find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
    {
        dx = x - x_prev;
        dy = y - y_prev;

        translate_slice_view( slice_window, view_index, dx, dy );

        set_slice_window_update( slice_window, view_index );

        record_mouse_pixel_position( slice_window );
    }

    set_update_required( slice_window, NORMAL_PLANES );
}

private  DEF_EVENT_FUNCTION( terminate_translation )    /* ARGSUSED */
{
    perform_translation( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_translation );

    return( OK );
}

private  DEF_EVENT_FUNCTION( update_translation )      /* ARGSUSED */
{
    perform_translation( display );

    return( OK );
}

/* ----------------------------------------------------------------------- */

private  void  update_window_size(
    display_struct    *slice_window )
{
#ifdef OLD   /* ------------------- */
    int   x_size, y_size;

    G_get_window_size( slice_window->window, &x_size, &y_size );

    slice_window->slice.x_split = x_size * Slice_divider_x_position;
    slice_window->slice.y_split = y_size * Slice_divider_y_position;
#endif  /* ------------------- */
}

private  DEF_EVENT_FUNCTION( update_probe )     /* ARGSUSED */
{
    int  x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) )
        rebuild_probe( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_redraw )     /* ARGSUSED */
{
    set_update_required( display, NORMAL_PLANES );
    set_update_required( display, OVERLAY_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_redraw_overlay )     /* ARGSUSED */
{
    set_update_required( display, OVERLAY_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( window_size_changed )    /* ARGSUSED */
{
    int   view;

    update_window_size( display );

    for_less( view, 0, N_SLICE_VIEWS )
        resize_slice_view( display, view );

    rebuild_slice_models( display );

    resize_histogram( display );

    set_update_required( display, NORMAL_PLANES );
    set_update_required( display, OVERLAY_PLANES );

    return( OK );
}

/* ------------------------------------------------------------------ */

private  DEF_EVENT_FUNCTION( terminate_picking_low_limit )     /* ARGSUSED */
{
    update_limit( display, TRUE, FALSE );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_picking_low_limit );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_low_limit )      /* ARGSUSED */
{
    int   x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) ||
        graphics_update_required( display ) )
    {
        update_limit( display, TRUE, FALSE );
    }

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_picking_high_limit )    /* ARGSUSED */
{
    update_limit( display, FALSE, FALSE );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_picking_high_limit );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_high_limit )      /* ARGSUSED */
{
    int   x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) ||
        graphics_update_required( display ) )
    {
        update_limit( display, FALSE, FALSE );
    }

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_picking_both_limits )    /* ARGSUSED */
{
    update_limit( display, TRUE, TRUE );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_picking_both_limits );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_both_limits )      /* ARGSUSED */
{
    int   x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) ||
        graphics_update_required( display ) )
    {
        update_limit( display, TRUE, TRUE );
    }

    return( OK );
}

private  void  update_limit(
    display_struct   *slice_window,
    BOOLEAN          low_limit_flag,
    BOOLEAN          fixed_range_flag )
{
    Real                  range, min_value, max_value, value;
    Real                  volume_min, volume_max;
    Volume                volume;
    colour_coding_struct  *colour_coding;

    if( get_mouse_colour_bar_value( slice_window, &value ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_real_range( volume, &volume_min, &volume_max );

        colour_coding = &slice_window->slice.colour_coding;
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
                if( value > max_value ) 
                    min_value = max_value;
                else
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
                if( value < min_value ) 
                    max_value = min_value;
                else
                    max_value = value;
            }
        }

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

        change_colour_coding_range( slice_window, min_value, max_value );
    }

    record_mouse_pixel_position( slice_window );
}

private  BOOLEAN  get_mouse_colour_bar_value(
    display_struct   *slice_window,
    Real             *value )
{
    int                   x, y;
    Real                  ratio, min_value, max_value;
    Volume                volume;
    BOOLEAN               found;

    found = FALSE;

    if( G_get_mouse_position( slice_window->window, &x, &y ) &&
        mouse_within_colour_bar( slice_window, (Real) x, (Real) y, &ratio ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_real_range( volume, &min_value, &max_value );
        *value = INTERPOLATE( ratio, min_value, max_value );
        found = TRUE;
    }

    return( found );
}

private  BOOLEAN  mouse_is_near_low_limit(
    display_struct   *slice_window )
{
    Real                  value;
    BOOLEAN               near;
    colour_coding_struct  *colour_coding;

    near = FALSE;

    if( get_mouse_colour_bar_value( slice_window, &value ) )
    {
        colour_coding = &slice_window->slice.colour_coding;
        if( value < (colour_coding->min_value+colour_coding->max_value)/2.0 )
            near = TRUE;
    }

    return( near );
}

private  BOOLEAN  mouse_is_near_high_limit(
    display_struct   *slice_window )
{
    Real                  value;
    BOOLEAN               near;
    colour_coding_struct  *colour_coding;

    near = FALSE;

    if( get_mouse_colour_bar_value( slice_window, &value ) )
    {
        colour_coding = &slice_window->slice.colour_coding;
        if( value > (colour_coding->min_value+colour_coding->max_value)/2.0 )
            near = TRUE;
    }

    return( near );
}

#define  NEAR_ENOUGH  7

private  BOOLEAN  mouse_is_near_slice_dividers(
    display_struct   *slice_window )
{
    int       x, y, x_div, y_div, dx, dy;
    BOOLEAN   near;

    near = FALSE;

    if( G_get_mouse_position( slice_window->window, &x, &y ) )
    {
        get_slice_divider_intersection( slice_window, &x_div, &y_div );

        dx = x - x_div;
        dy = y - y_div;
        near = ABS(dx) < NEAR_ENOUGH && ABS(dy) < NEAR_ENOUGH;
    }

    return( near );
}
/* ----------------------------------------------------------------------- */

private  void  update_slice_dividers(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
    {
        set_slice_divider_position( slice_window, x, y );
    }
}

private  DEF_EVENT_FUNCTION( terminate_setting_slice_dividers )     /* ARGSUSED */
{
    update_slice_dividers( display );

    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  terminate_setting_slice_dividers );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_slice_dividers )     /* ARGSUSED */
{
    update_slice_dividers( display );

    return( OK );
}

