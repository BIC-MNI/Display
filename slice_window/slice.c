
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_slice_window( graphics )
    graphics_struct   *graphics;
{
    DECL_EVENT_FUNCTION( window_size_changed );
    void                 add_action_table_function();
    void                 update_window_size();
    int                  c;

    graphics->slice.volume = (volume_struct *) 0;

    update_window_size( graphics );

    add_action_table_function( &graphics->action_table, WINDOW_RESIZE_EVENT,
                               window_size_changed );

    for_less( c, 0, N_DIMENSIONS )
    {
        graphics->slice.slice_views[c].slice_index = 0;
        graphics->slice.slice_views[c].x_offset = 0;
        graphics->slice.slice_views[c].y_offset = 0;
        graphics->slice.slice_views[c].x_scale = 1.0;
        graphics->slice.slice_views[c].y_scale = 1.0;
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

private  DEF_EVENT_FUNCTION( window_size_changed )    /* ARGSUSED */
{
    update_window_size( graphics );

    return( OK );
}

public  void  update_slice_window( graphics )
    graphics_struct  *graphics;
{
    int   x_min, x_max, y_min, y_max;
    void  draw_2d_line();
    void  draw_slice();

    draw_2d_line( graphics, PIXEL_VIEW, &Slice_split_colour,
                  (Real) graphics->slice.x_split, 0.0,
                  (Real) graphics->slice.x_split,
                  (Real) (graphics->window.y_size-1) );

    draw_2d_line( graphics, PIXEL_VIEW, &Slice_split_colour,
                  0.0, (Real) graphics->slice.y_split,
                  (Real) (graphics->window.x_size-1),
                  (Real) graphics->slice.y_split );

    x_min = 0;
    x_max = graphics->slice.x_split-2;
    y_min = graphics->slice.y_split+2;
    y_max = graphics->window.y_size-1;

    draw_slice( graphics, X_AXIS, x_min, x_max, y_min, y_max );

    x_min = graphics->slice.x_split+2;
    x_max = graphics->window.x_size-1;
    y_min = graphics->slice.y_split+2;
    y_max = graphics->window.y_size-1;

    draw_slice( graphics, Y_AXIS, x_min, x_max, y_min, y_max );

    x_min = 0;
    x_max = graphics->slice.x_split-2;
    y_min = 0;
    y_max = graphics->slice.y_split-2;

    draw_slice( graphics, Z_AXIS, x_min, x_max, y_min, y_max );
}

public  Status  delete_slice_window_info( slice_window )
    slice_window_struct   *slice_window;
{
    return( OK );
}
