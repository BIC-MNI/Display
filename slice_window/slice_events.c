
#include  <def_graphics.h>
#include  <def_globals.h>

static    DECL_EVENT_FUNCTION( window_size_changed );
static    DECL_EVENT_FUNCTION( handle_redraw );
static    DECL_EVENT_FUNCTION( update_probe );
static    DECL_EVENT_FUNCTION( update_translation );
static    DECL_EVENT_FUNCTION( terminate_translation );
static    DECL_EVENT_FUNCTION( handle_update_voxel );
static    DECL_EVENT_FUNCTION( end_picking_voxel );
static    DECL_EVENT_FUNCTION( left_mouse_down );
static    DECL_EVENT_FUNCTION( middle_mouse_down );
static    DECL_EVENT_FUNCTION( handle_update_low_limit );
static    DECL_EVENT_FUNCTION( handle_update_high_limit );
static    DECL_EVENT_FUNCTION( handle_update_both_limits );
static    DECL_EVENT_FUNCTION( end_picking_low_limit );
static    DECL_EVENT_FUNCTION( end_picking_high_limit );
static    DECL_EVENT_FUNCTION( end_picking_both_limits );
static    void                 update_window_size();
static    void                 perform_translation();

public  void  initialize_slice_window_events( graphics )
    graphics_struct   *graphics;
{
    void                 add_action_table_function();
    void                 initialize_voxel_selection();

    update_window_size( graphics );

    add_action_table_function( &graphics->action_table, WINDOW_RESIZE_EVENT,
                               window_size_changed );
    add_action_table_function( &graphics->action_table, WINDOW_REDRAW_EVENT,
                               handle_redraw );
    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               left_mouse_down );
    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               middle_mouse_down );
    add_action_table_function( &graphics->action_table, NO_EVENT,
                               update_probe );

    fill_Point( graphics->prev_mouse_position, 0.0, 0.0, 0.0 );
}

private  DEF_EVENT_FUNCTION( left_mouse_down )
    /* ARGSUSED */
{
    int          view_index;
    void         add_action_table_function();
    void         set_update_required();

    graphics->prev_mouse_position = graphics->mouse_position;

    if( get_slice_view_index_under_mouse( graphics, &view_index ) )
    {
        add_action_table_function( &graphics->action_table,
                                   NO_EVENT,
                                   handle_update_voxel );

        add_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   end_picking_voxel );
    }
    else if( mouse_is_near_low_limit( graphics ) )
    {
        add_action_table_function( &graphics->action_table,
                                   NO_EVENT,
                                   handle_update_low_limit );

        add_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   end_picking_low_limit );
    }
    else if( mouse_is_near_high_limit( graphics ) )
    {
        add_action_table_function( &graphics->action_table,
                                   NO_EVENT,
                                   handle_update_high_limit );

        add_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   end_picking_high_limit );
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( middle_mouse_down )
    /* ARGSUSED */
{
    int          view_index;
    void         add_action_table_function();
    void         perform_translation();
    void         set_update_required();

    graphics->prev_mouse_position = graphics->mouse_position;

    if( get_slice_view_index_under_mouse( graphics, &view_index ) )
    {
        add_action_table_function( &graphics->action_table,
                                   MOUSE_MOVEMENT_EVENT, update_translation );

        add_action_table_function( &graphics->action_table,
                                   MIDDLE_MOUSE_UP_EVENT,
                                   terminate_translation );

        add_action_table_function( &graphics->action_table,
                                   TERMINATE_EVENT,
                                   terminate_translation );
    }
    else if( mouse_is_near_low_limit( graphics ) )
    {
        add_action_table_function( &graphics->action_table,
                                   NO_EVENT,
                                   handle_update_both_limits );

        add_action_table_function( &graphics->action_table,
                                   MIDDLE_MOUSE_UP_EVENT,
                                   end_picking_both_limits );
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( end_picking_voxel )
    /* ARGSUSED */
{
    void    remove_action_table_function();
    void    update_voxel_cursor();

    remove_action_table_function( &graphics->action_table, LEFT_MOUSE_UP_EVENT,
                                  end_picking_voxel );
    remove_action_table_function( &graphics->action_table, NO_EVENT,
                                  handle_update_voxel );

    update_voxel_cursor( graphics );
}

private  DEF_EVENT_FUNCTION( handle_update_voxel )
    /* ARGSUSED */
{
    Boolean  mouse_moved();

    if( mouse_moved(graphics) || graphics_update_required( graphics ) )
    {
        update_voxel_cursor( graphics );
    }

    return( OK );
}

private  void  update_voxel_cursor( slice_window )
    graphics_struct   *slice_window;
{
    int               c, indices[N_DIMENSIONS], axis_index;
    Boolean           get_voxel_in_slice_window();
    Boolean           set_current_voxel();
    Boolean           update_cursor_from_voxel();
    void              set_update_required();

    if( get_voxel_in_slice_window( slice_window, &indices[X_AXIS],
                 &indices[Y_AXIS], &indices[Z_AXIS], &axis_index ) )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            if( slice_window->slice.slice_locked[c] )
                indices[c] = slice_window->slice.slice_index[c];
        }

        if( set_current_voxel( slice_window, indices[X_AXIS], indices[Y_AXIS],
                               indices[Z_AXIS] ) )
        {
            set_update_required( slice_window, NORMAL_PLANES );
        }

        if( update_cursor_from_voxel( slice_window ) )
        {
            set_update_required( slice_window->associated[THREE_D_WINDOW],
                                 OVERLAY_PLANES );
        }
    }
}

private  void  update_window_size( graphics )
    graphics_struct   *graphics;
{
    int   x_size, y_size;

    x_size = graphics->window.x_size;
    y_size = graphics->window.y_size;

    graphics->slice.x_split = x_size / 2;
    graphics->slice.y_split = y_size / 2;
}

private  DEF_EVENT_FUNCTION( update_probe )
    /* ARGSUSED */
{
    void     rebuild_probe();
    Boolean  mouse_moved();
    void     set_update_required();

    if( mouse_moved( graphics ) )
    {
        rebuild_probe( graphics );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_redraw )
    /* ARGSUSED */
{
    void     set_update_required();

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( window_size_changed )    /* ARGSUSED */
{
    void  rebuild_slice_models();
    void  set_update_required();

    update_window_size( graphics );

    rebuild_slice_models( graphics );

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_translation )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   update_view();

    perform_translation( graphics );

    remove_action_table_function( &graphics->action_table,
                                  MOUSE_MOVEMENT_EVENT, update_translation );
    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_translation );
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT, terminate_translation );

    return( OK );
}

private  DEF_EVENT_FUNCTION( update_translation )      /* ARGSUSED */
{
    void     perform_translation();

    perform_translation( graphics );

    return( OK );
}

private  void  perform_translation( graphics )
    graphics_struct  *graphics;
{
    int        view_index, x1, y1, x2, y2, dx, dy;
    Boolean    find_slice_view_mouse_is_in();
    void       get_mouse_in_pixels();
    void       set_slice_window_update();
    void       set_update_required();

    get_mouse_in_pixels( graphics, &graphics->prev_mouse_position, &x1, &y1 );

    if( find_slice_view_mouse_is_in( graphics, x1, y1, &view_index ) )
    {
        get_mouse_in_pixels( graphics, &graphics->mouse_position, &x2, &y2 );

        dx = x2 - x1;
        dy = y2 - y1;

        graphics->slice.slice_views[view_index].x_offset += dx;
        graphics->slice.slice_views[view_index].y_offset += dy;

        set_slice_window_update( graphics, view_index );
    }

    set_update_required( graphics, NORMAL_PLANES );

    graphics->prev_mouse_position = graphics->mouse_position;
}

private  DEF_EVENT_FUNCTION( end_picking_low_limit )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   update_limit();

    update_limit( graphics, TRUE, FALSE );

    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT, handle_update_low_limit );
    remove_action_table_function( &graphics->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  end_picking_low_limit );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_low_limit )      /* ARGSUSED */
{
    Boolean  mouse_moved();
    void     update_limit();

    if( mouse_moved(graphics) || graphics_update_required( graphics ) )
    {
        update_limit( graphics, TRUE, FALSE );
    }

    return( OK );
}

private  DEF_EVENT_FUNCTION( end_picking_high_limit )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   update_limit();

    update_limit( graphics, FALSE, FALSE );

    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT, handle_update_high_limit );
    remove_action_table_function( &graphics->action_table,
                                  LEFT_MOUSE_UP_EVENT,
                                  end_picking_high_limit );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_high_limit )      /* ARGSUSED */
{
    Boolean  mouse_moved();
    void     update_limit();

    if( mouse_moved(graphics) || graphics_update_required( graphics ) )
    {
        update_limit( graphics, FALSE, FALSE );
    }

    return( OK );
}

private  DEF_EVENT_FUNCTION( end_picking_both_limits )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   update_limit();

    update_limit( graphics, TRUE, TRUE );

    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT,
                                  handle_update_both_limits );
    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  end_picking_both_limits );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_both_limits )      /* ARGSUSED */
{
    Boolean  mouse_moved();
    void     update_limit();

    if( mouse_moved(graphics) || graphics_update_required( graphics ) )
    {
        update_limit( graphics, TRUE, TRUE );
    }

    return( OK );
}

private  void  update_limit( graphics, low_limit_flag, fixed_range_flag )
    graphics_struct  *graphics;
    Boolean          low_limit_flag;
    Boolean          fixed_range_flag;
{
    Real                  range, min_value, max_value, value;
    Real                  volume_min, volume_max;
    volume_struct         *volume;
    colour_coding_struct  *colour_coding;
    Boolean               get_mouse_colour_bar_value();
    void                  change_colour_coding_range();

    if( get_mouse_colour_bar_value( graphics, &value ) &&
        get_slice_window_volume( graphics, &volume ) )
    {
        volume_min = volume->min_value;
        volume_max = volume->max_value;

        colour_coding = &graphics->slice.colour_coding;
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

        change_colour_coding_range( graphics, min_value, max_value );
    }

    graphics->prev_mouse_position = graphics->mouse_position;
}

private  Boolean  get_mouse_colour_bar_value( graphics, value )
    graphics_struct  *graphics;
    Real             *value;
{
    int                   x, y;
    Real                  ratio;
    volume_struct         *volume;
    Boolean               found;
    Boolean               mouse_within_colour_bar();
    void                  get_mouse_in_pixels();

    found = FALSE;

    get_mouse_in_pixels( graphics, &graphics->mouse_position, &x, &y );

    if( mouse_within_colour_bar( graphics, (Real) x, (Real) y, &ratio ) &&
        get_slice_window_volume( graphics, &volume ) )
    {
        *value = INTERPOLATE( ratio, volume->min_value, volume->max_value );

        *value = (Real) ROUND( *value );

        found = TRUE;
    }

    return( found );
}

private  Boolean  mouse_is_near_low_limit( graphics )
    graphics_struct  *graphics;
{
    Real                  value;
    Boolean               near;
    colour_coding_struct  *colour_coding;

    near = FALSE;

    if( get_mouse_colour_bar_value( graphics, &value ) )
    {
        colour_coding = &graphics->slice.colour_coding;
        if( value < (colour_coding->min_value+colour_coding->max_value)/2.0 )
            near = TRUE;
    }

    return( near );
}

private  Boolean  mouse_is_near_high_limit( graphics )
    graphics_struct  *graphics;
{
    Real                  value;
    Boolean               near;
    colour_coding_struct  *colour_coding;

    near = FALSE;

    if( get_mouse_colour_bar_value( graphics, &value ) )
    {
        colour_coding = &graphics->slice.colour_coding;
        if( value > (colour_coding->min_value+colour_coding->max_value)/2.0 )
            near = TRUE;
    }

    return( near );
}
