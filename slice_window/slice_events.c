
#include  <def_display.h>

static    DEF_EVENT_FUNCTION( window_size_changed );
static    DEF_EVENT_FUNCTION( handle_redraw );
static    DEF_EVENT_FUNCTION( handle_redraw_overlay );
static    DEF_EVENT_FUNCTION( update_probe );
static    DEF_EVENT_FUNCTION( update_translation );
static    DEF_EVENT_FUNCTION( terminate_translation );
static    DEF_EVENT_FUNCTION( handle_update_voxel );
static    DEF_EVENT_FUNCTION( end_picking_voxel );
static    DEF_EVENT_FUNCTION( left_mouse_down );
static    DEF_EVENT_FUNCTION( middle_mouse_down );
static    DEF_EVENT_FUNCTION( handle_update_low_limit );
static    DEF_EVENT_FUNCTION( handle_update_high_limit );
static    DEF_EVENT_FUNCTION( handle_update_both_limits );
static    DEF_EVENT_FUNCTION( end_picking_low_limit );
static    DEF_EVENT_FUNCTION( end_picking_high_limit );
static    DEF_EVENT_FUNCTION( end_picking_both_limits );
private  void  update_voxel_cursor(
    display_struct    *slice_window );
private  void  update_window_size(
    display_struct    *slice_window );
private  void  perform_translation(
    display_struct   *slice_window );
private  void  update_limit(
    display_struct   *slice_window,
    Boolean          low_limit_flag,
    Boolean          fixed_range_flag );
private  Boolean  get_mouse_colour_bar_value(
    display_struct   *slice_window,
    Real             *value );
private  Boolean  mouse_is_near_low_limit(
    display_struct   *slice_window );
private  Boolean  mouse_is_near_high_limit(
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

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   handle_update_voxel );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   end_picking_voxel );
    }
    else if( mouse_is_near_low_limit( display ) )
    {
        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   handle_update_low_limit );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   end_picking_low_limit );
    }
    else if( mouse_is_near_high_limit( display ) )
    {
        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   handle_update_high_limit );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   end_picking_high_limit );
    }

    record_mouse_pixel_position( display );
    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( middle_mouse_down )     /* ARGSUSED */
{
    int          view_index;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        push_action_table( &display->action_table, NO_EVENT );
        add_action_table_function( &display->action_table,
                                   NO_EVENT, update_translation );

        add_action_table_function( &display->action_table,
                                   MIDDLE_MOUSE_UP_EVENT,
                                   terminate_translation );

        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_translation );
    }
    else if( mouse_is_near_low_limit( display ) )
    {
        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   handle_update_both_limits );

        add_action_table_function( &display->action_table,
                                   MIDDLE_MOUSE_UP_EVENT,
                                   end_picking_both_limits );
    }

    record_mouse_pixel_position( display );
    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( end_picking_voxel )     /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT, end_picking_voxel );
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  handle_update_voxel );

    update_voxel_cursor( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_voxel )     /* ARGSUSED */
{
    int  x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) ||
        graphics_update_required( display ) )
        update_voxel_cursor( display );

    return( OK );
}

private  void  update_voxel_cursor(
    display_struct    *slice_window )
{
    int               c, indices[N_DIMENSIONS], axis_index;

    if( get_voxel_in_slice_window( slice_window, &indices[X],
                 &indices[Y], &indices[Z], &axis_index ) )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            if( slice_window->slice.slice_locked[c] )
                indices[c] = slice_window->slice.slice_index[c];
        }

        if( set_current_voxel( slice_window, indices[X], indices[Y],
                               indices[Z] ) )
        {
            set_update_required( slice_window, NORMAL_PLANES );
        }

        if( update_cursor_from_voxel( slice_window ) )
        {
            set_update_required( slice_window->associated[THREE_D_WINDOW],
                                 OVERLAY_PLANES );
        }

        if( update_current_marker( slice_window->associated[THREE_D_WINDOW],
                                   indices[X], indices[Y], indices[Z] ) )
        {
            rebuild_selected_list( slice_window->associated[THREE_D_WINDOW],
                                   slice_window->associated[MENU_WINDOW] );

            set_update_required( slice_window->associated[MENU_WINDOW],
                                 NORMAL_PLANES );
        }
    }
}

private  void  update_window_size(
    display_struct    *slice_window )
{
    int   x_size, y_size;

    G_get_window_size( slice_window->window, &x_size, &y_size );

    slice_window->slice.x_split = x_size * Slice_divider_x_position;
    slice_window->slice.y_split = y_size * Slice_divider_y_position;
}

private  DEF_EVENT_FUNCTION( update_probe )     /* ARGSUSED */
{
    int  x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) )
    {
        rebuild_probe( display );

        set_update_required( display, NORMAL_PLANES );
    }

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
    update_window_size( display );

    rebuild_slice_models( display );

    set_update_required( display, NORMAL_PLANES );
    set_update_required( display, OVERLAY_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_translation )    /* ARGSUSED */
{
    perform_translation( display );

    remove_action_table_function( &display->action_table,
                                  NO_EVENT, update_translation );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_translation );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_translation );
    pop_action_table( &display->action_table, NO_EVENT );

    return( OK );
}

private  DEF_EVENT_FUNCTION( update_translation )      /* ARGSUSED */
{
    perform_translation( display );

    return( OK );
}

private  void  perform_translation(
    display_struct   *slice_window )
{
    int        view_index, x1, y1, x2, y2, dx, dy;

    x1 = ROUND( Point_x( slice_window->prev_mouse_position ) );
    y1 = ROUND( Point_y( slice_window->prev_mouse_position ) );

    if( find_slice_view_mouse_is_in( slice_window, x1, y1, &view_index ) &&
        G_get_mouse_position( slice_window->window, &x2, &y2 ) )
    {
        dx = x2 - x1;
        dy = y2 - y1;

        slice_window->slice.slice_views[view_index].x_offset += dx;
        slice_window->slice.slice_views[view_index].y_offset += dy;

        set_slice_window_update( slice_window, view_index );

        record_mouse_pixel_position( slice_window );
    }

    set_update_required( slice_window, NORMAL_PLANES );
}

private  DEF_EVENT_FUNCTION( end_picking_low_limit )     /* ARGSUSED */
{
    update_limit( display, TRUE, FALSE );

    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_low_limit );
    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  end_picking_low_limit );

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

private  DEF_EVENT_FUNCTION( end_picking_high_limit )    /* ARGSUSED */
{
    update_limit( display, FALSE, FALSE );

    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_high_limit );
    remove_action_table_function( &display->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  end_picking_high_limit );

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

private  DEF_EVENT_FUNCTION( end_picking_both_limits )    /* ARGSUSED */
{
    update_limit( display, TRUE, TRUE );

    remove_action_table_function( &display->action_table,
                                  NO_EVENT,
                                  handle_update_both_limits );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  end_picking_both_limits );

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
    Boolean          low_limit_flag,
    Boolean          fixed_range_flag )
{
    Real                  range, min_value, max_value, value;
    Real                  volume_min, volume_max;
    Volume                volume;
    colour_coding_struct  *colour_coding;

    if( get_mouse_colour_bar_value( slice_window, &value ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_voxel_range( volume, &volume_min, &volume_max );

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

private  Boolean  get_mouse_colour_bar_value(
    display_struct   *slice_window,
    Real             *value )
{
    int                   x, y;
    Real                  ratio, min_value, max_value;
    Volume                volume;
    Boolean               found;

    found = FALSE;

    if( G_get_mouse_position( slice_window->window, &x, &y ) &&
        mouse_within_colour_bar( slice_window, (Real) x, (Real) y, &ratio ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_voxel_range( volume, &min_value, &max_value );
        *value = INTERPOLATE( ratio, min_value, max_value );

        *value = (Real) ROUND( *value );

        found = TRUE;
    }

    return( found );
}

private  Boolean  mouse_is_near_low_limit(
    display_struct   *slice_window )
{
    Real                  value;
    Boolean               near;
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

private  Boolean  mouse_is_near_high_limit(
    display_struct   *slice_window )
{
    Real                  value;
    Boolean               near;
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
