
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_slice_window_events( graphics )
    graphics_struct   *graphics;
{
    DECL_EVENT_FUNCTION( window_size_changed );
    DECL_EVENT_FUNCTION( handle_redraw );
    DECL_EVENT_FUNCTION( start_translate );
    DECL_EVENT_FUNCTION( update_probe );
    void                 add_action_table_function();
    void                 update_window_size();
    void                 initialize_voxel_selection();

    update_window_size( graphics );

    add_action_table_function( &graphics->action_table, WINDOW_RESIZE_EVENT,
                               window_size_changed );
    add_action_table_function( &graphics->action_table, WINDOW_REDRAW_EVENT,
                               handle_redraw );
    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_translate );
    add_action_table_function( &graphics->action_table, NO_EVENT,
                               update_probe );

    fill_Point( graphics->prev_mouse_position, 0.0, 0.0, 0.0 );

    initialize_voxel_selection( graphics );
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
    void  update_window_size();
    void  rebuild_slice_models();
    void  set_update_required();

    update_window_size( graphics );

    rebuild_slice_models( graphics );

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( start_translate )
    /* ARGSUSED */
{
    void                  add_action_table_function();
    DECL_EVENT_FUNCTION(  handle_mouse_movement );
    DECL_EVENT_FUNCTION(  terminate_translation );

    add_action_table_function( &graphics->action_table,
                               MOUSE_MOVEMENT_EVENT,
                               handle_mouse_movement );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_translation );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               terminate_translation );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION( terminate_translation )
    /* ARGSUSED */
{
    void   remove_action_table_function();
    void   perform_translation();
    void   update_view();

    perform_translation( graphics );

    remove_action_table_function( &graphics->action_table,
                                  MOUSE_MOVEMENT_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  MIDDLE_MOUSE_UP_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  TERMINATE_EVENT );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_mouse_movement )      /* ARGSUSED */
{
    void   perform_translation();

    perform_translation( graphics );

    return( OK );
}

private  void  perform_translation( graphics )
    graphics_struct  *graphics;
{
    int        axis_index, x1, y1, x2, y2, dx, dy;
    Boolean    find_slice_view_mouse_is_in();
    void       get_mouse_in_pixels();
    void       set_slice_window_update();
    void       set_update_required();

    get_mouse_in_pixels( graphics, &graphics->prev_mouse_position, &x1, &y1 );

    if( find_slice_view_mouse_is_in( graphics, x1, y1, &axis_index ) )
    {
        get_mouse_in_pixels( graphics, &graphics->mouse_position, &x2, &y2 );

        dx = x2 - x1;
        dy = y2 - y1;

        graphics->slice.slice_views[axis_index].x_offset += dx;
        graphics->slice.slice_views[axis_index].y_offset += dy;

        set_slice_window_update( graphics, axis_index );
    }

    set_update_required( graphics, NORMAL_PLANES );

    graphics->prev_mouse_position = graphics->mouse_position;
}
