
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_slice_window( graphics )
    graphics_struct   *graphics;
{
    DECL_EVENT_FUNCTION( window_size_changed );
    void                 add_action_table_function();
    void                 update_window_size();

    graphics->slice.volume = (volume_struct *) 0;

    update_window_size( graphics );

    add_action_table_function( &graphics->action_table, WINDOW_RESIZE_EVENT,
                               window_size_changed );
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
    void  draw_2d_line();

    draw_2d_line( graphics, PIXEL_VIEW, &Slice_split_colour,
                  (Real) graphics->slice.x_split, 0.0,
                  (Real) graphics->slice.x_split,
                  (Real) (graphics->window.y_size-1) );

    draw_2d_line( graphics, PIXEL_VIEW, &Slice_split_colour,
                  0.0, (Real) graphics->slice.y_split,
                  (Real) (graphics->window.x_size-1),
                  (Real) graphics->slice.y_split );
}

public  Status  delete_slice_window_info( slice_window )
    slice_window_struct   *slice_window;
{
    return( OK );
}
