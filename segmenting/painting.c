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

    
    (void) G_get_mouse_position( display->window, &x_pixel, &y_pixel );
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
    Real     x_brush_radius, y_brush_radius, z_brush_radius;
    Real     x_brush_radius_squared, y_brush_radius_squared;
    Real     x_term, y_term, z_term;
    Real     z_brush_radius_squared;
    int      label, axis, a1, a2, value;
    int      int_x_brush_radius, int_y_brush_radius, int_z_brush_radius;
    int      dx, dy, dz;
    Real     indices[N_DIMENSIONS];
    int      ind[N_DIMENSIONS];
    Boolean  update_required;

    if( get_slice_window_volume( slice_window, &volume ) &&
        convert_pixel_to_voxel( slice_window, x, y, &indices[X], &indices[Y],
                                &indices[Z], &axis ) )
    {
        update_required = FALSE;

        a1 = (axis + 1) % N_DIMENSIONS;
        a2 = (axis + 2) % N_DIMENSIONS;
        x_brush_radius = slice_window->slice.x_brush_radius;
        y_brush_radius = slice_window->slice.y_brush_radius;
        z_brush_radius = slice_window->slice.z_brush_radius;

        if( x_brush_radius <= 0.0 )
            x_brush_radius = 0.0000001;
        if( y_brush_radius <= 0.0 )
            y_brush_radius = 0.0000001;
        if( z_brush_radius <= 0.0 )
            z_brush_radius = 0.0000001;

        x_brush_radius_squared = x_brush_radius * x_brush_radius;
        y_brush_radius_squared = y_brush_radius * y_brush_radius;
        z_brush_radius_squared = z_brush_radius * z_brush_radius;

        int_x_brush_radius = CEILING( x_brush_radius );
        int_y_brush_radius = CEILING( y_brush_radius );
        int_z_brush_radius = CEILING( z_brush_radius );
        label = slice_window->slice.current_paint_label;

        for_inclusive( dx, -int_x_brush_radius, int_x_brush_radius )
        {
            ind[a1] = ROUND(indices[a1]) + dx;
            x_term = dx * dx / x_brush_radius_squared;
            for_inclusive( dy, -int_y_brush_radius, int_y_brush_radius )
            {
                ind[a2] = ROUND(indices[a2]) + dy;
                y_term = dy * dy / y_brush_radius_squared;
                for_inclusive( dz, -int_z_brush_radius, int_z_brush_radius )
                {
                    ind[axis] = ROUND(indices[axis]) + dz;
                    z_term = dz * dz / z_brush_radius_squared;

                    if( x_term + y_term + z_term <= 1.0 &&
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
        }

        if( update_required )
        {
            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
    }
}

public  void  copy_labels_slice_to_slice(
    Volume           volume,
    int              axis,
    int              src_voxel,
    int              dest_voxel )
{
    int   x, y, a1, a2, value;
    int   sizes[N_DIMENSIONS], src_indices[N_DIMENSIONS];
    int   dest_indices[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );
    a1 = (axis + 1) % N_DIMENSIONS;
    a2 = (axis + 2) % N_DIMENSIONS;

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

            value = get_volume_auxiliary_data( volume,
                                               src_indices[X],
                                               src_indices[Y],
                                               src_indices[Z] );

            set_volume_auxiliary_data( volume, dest_indices[X],
                                               dest_indices[Y],
                                               dest_indices[Z], value );
        }
    }
}
