#include  <def_display.h>

private  DEF_EVENT_FUNCTION( right_mouse_down );
private  DEF_EVENT_FUNCTION( end_painting );
private  DEF_EVENT_FUNCTION( handle_update_painting );
private  void  update_paint_labels(
    display_struct  *display );
private  void  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2 );
private  void  paint_labels_at_point(
    display_struct   *slice_window,
    int              x,
    int              y );

public  void  initialize_voxel_labeling(
    display_struct    *slice_window )
{
    add_action_table_function( &slice_window->action_table,
                               RIGHT_MOUSE_DOWN_EVENT,
                               right_mouse_down );
}

private  DEF_EVENT_FUNCTION( right_mouse_down )    /* ARGSUSED */
{
    int           x_pixel, y_pixel;

    push_action_table( &display->action_table, NO_EVENT );

    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update_painting );

    add_action_table_function( &display->action_table,
                               RIGHT_MOUSE_UP_EVENT,
                               end_painting );

    
    G_get_mouse_position( display->window, &x_pixel, &y_pixel );
    paint_labels_at_point( display, x_pixel, y_pixel );
    record_mouse_pixel_position( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( end_painting )     /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  RIGHT_MOUSE_UP_EVENT, end_painting );
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  handle_update_painting );
    pop_action_table( &display->action_table, NO_EVENT );

    update_paint_labels( display );

    return( OK );
}

private  void  update_paint_labels(
    display_struct  *display )
{
    int  x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) )
    {
        sweep_paint_labels( display, x_prev, y_prev, x, y );
    }
}

private  DEF_EVENT_FUNCTION( handle_update_painting )     /* ARGSUSED */
{
    update_paint_labels( display );

    return( OK );
}

private  void  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2 )
{
    int         i, dx, dy, n, x, y;

    dx = ABS( x2 - x1 );
    dy = ABS( y2 - y1 );
    n = MAX( dx, dy );

    for_inclusive( i, 1, n )
    {
        x = ROUND( x1 + (Real) i / (Real) n * (x2 - x1) );
        y = ROUND( y1 + (Real) i / (Real) n * (y2 - y1) );

        paint_labels_at_point( slice_window, x, y );
    }
}

private  void  paint_labels_at_point(
    display_struct   *slice_window,
    int              x,
    int              y )
{
    Volume   volume;
    Real     brush_size, brush_size_squared;
    int      label, axis_index, a1, a2, value, int_brush_size;
    int      brush_size_squared, dx, dy;
    int      indices[N_DIMENSIONS], ind[N_DIMENSIONS];
    Boolean  update_required;

    if( get_slice_window_volume( slice_window, &volume ) &&
        convert_pixel_to_voxel( slice_window, x, y, &indices[X], &indices[Y],
                                &indices[Z], &axis_index ) )
    {
        update_required = FALSE;

        a1 = (axis_index + 1) % N_DIMENSIONS;
        a2 = (axis_index + 2) % N_DIMENSIONS;
        brush_size = slice_window->slice.brush_size;
        brush_size_squared = brush_size * brush_size;
        label = slice_window->slice.current_paint_label;
        ind[axis_index] = indices[axis_index];

        int_brush_size = CEILING( brush_size );

        for_inclusive( dx, -int_brush_size, int_brush_size )
        {
            ind[a1] = indices[a1] + dx;
            for_inclusive( dy, -int_brush_size, int_brush_size )
            {
                ind[a2] = indices[a2] + dy;
                if( (Real) (dy * dy + dx * dx) <= brush_size_squared &&
                    cube_is_within_volume( volume, ind ) )
                {
                    value = get_volume_auxiliary_data( volume, ind[X],
                                                       ind[Y], ind[Z] );
                    if( (value & LOWER_AUXILIARY_BITS) != label )
                    {
                        value = value & (~LOWER_AUXILIARY_BITS);
                        value = value | label;
                        set_volume_auxiliary_data( volume, ind[X],
                                                   ind[Y], ind[Z], value );
                        update_required = TRUE;
                    }
                }
            }
        }

        if( update_required )
        {
            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
    }
}
