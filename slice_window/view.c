
#include  <display.h>

public  void  initialize_slice_window_view(
    display_struct    *slice_window )
{
    Volume           volume;
    int              axis, view;
    int              size[N_DIMENSIONS];
    Real             thickness[N_DIMENSIONS];

    slice_window->slice.slice_views[0].axis_map[0]  = Slice_view1_axis1;
    slice_window->slice.slice_views[0].axis_map[1]  = Slice_view1_axis2;
    slice_window->slice.slice_views[0].axis_map[2]  = Slice_view1_axis3;

    slice_window->slice.slice_views[1].axis_map[0]  = Slice_view2_axis1;
    slice_window->slice.slice_views[1].axis_map[1]  = Slice_view2_axis2;
    slice_window->slice.slice_views[1].axis_map[2]  = Slice_view2_axis3;

    slice_window->slice.slice_views[2].axis_map[0]  = Slice_view3_axis1;
    slice_window->slice.slice_views[2].axis_map[1]  = Slice_view3_axis2;
    slice_window->slice.slice_views[2].axis_map[2]  = Slice_view3_axis3;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_sizes( volume, size );
        get_volume_separations( volume, thickness );

        for_less( view, 0, N_DIMENSIONS )
        {
            axis = slice_window->slice.slice_views[view].axis_map[Z];
            slice_window->slice.slice_index[view] = size[axis] / 2.0;
            slice_window->slice.slice_locked[view] = FALSE;
        }

        for_less( view, 0, N_DIMENSIONS )
            reset_slice_view( slice_window, view );

        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[X] =
                          ABS( thickness[X] );
        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Y] =
                          ABS( thickness[Y] );
        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Z] =
                          ABS( thickness[Z] );

        update_cursor_size( slice_window->associated[THREE_D_WINDOW] );
    }
}

private  record_slice_display_limits(
    display_struct    *slice_window,
    int               view )
{
    int            x_min, x_max, y_min, y_max;
    int            x_index, y_index;
    Real           bottom_left_voxel[MAX_DIMENSIONS];
    Real           top_right_voxel[MAX_DIMENSIONS];
    Volume         volume;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        x_index = slice_window->slice.slice_views[view].axis_map[X];
        y_index = slice_window->slice.slice_views[view].axis_map[Y];
        get_slice_viewport( slice_window, view,
                            &x_min, &x_max, &y_min, &y_max );

        (void) convert_slice_pixel_to_voxel( volume,
                    0, 0,
                    slice_window->slice.slice_index, x_index, y_index,
                    slice_window->slice.slice_views[view].x_trans,
                    slice_window->slice.slice_views[view].y_trans,
                    slice_window->slice.slice_views[view].x_scaling,
                    slice_window->slice.slice_views[view].y_scaling,
                    bottom_left_voxel );

        (void) convert_slice_pixel_to_voxel( volume,
                    x_max - x_min, y_max - y_min,
                    slice_window->slice.slice_index, x_index, y_index,
                    slice_window->slice.slice_views[view].x_trans,
                    slice_window->slice.slice_views[view].y_trans,
                    slice_window->slice.slice_views[view].x_scaling,
                    slice_window->slice.slice_views[view].y_scaling,
                    top_right_voxel );

        slice_window->slice.slice_views[view].lower_view_limits[0] =
                                bottom_left_voxel[x_index];
        slice_window->slice.slice_views[view].upper_view_limits[0] =
                                top_right_voxel[x_index];
        slice_window->slice.slice_views[view].lower_view_limits[1] =
                                bottom_left_voxel[y_index];
        slice_window->slice.slice_views[view].upper_view_limits[1] =
                                top_right_voxel[y_index];
    }
}

public  void  reset_slice_view(
    display_struct    *slice_window,
    int               view )
{
    Volume volume;
    int    x_index, y_index;
    int    x_min, x_max, y_min, y_max;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        x_index = slice_window->slice.slice_views[view].axis_map[X];
        y_index = slice_window->slice.slice_views[view].axis_map[Y];

        get_slice_viewport( slice_window, view,
                            &x_min, &x_max, &y_min, &y_max );

        fit_volume_slice_to_viewport( volume, x_index, y_index,
                              FALSE, FALSE,
                              x_max - x_min + 1, y_max - y_min + 1,
                              Slice_fit_oversize,
                              &slice_window->slice.slice_views[view].x_scaling,
                              &slice_window->slice.slice_views[view].y_scaling,
                              &slice_window->slice.slice_views[view].x_trans,
                              &slice_window->slice.slice_views[view].y_trans );

        record_slice_display_limits( slice_window, view );
    }
}

public  void  fit_slice_to_view(
    display_struct    *slice_window,
    int               view )
{
    int            x_min, x_max, y_min, y_max;
    int            x_index, y_index;
    Real           x_scale, y_scale, x_trans, y_trans;
    Volume         volume;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        x_index = slice_window->slice.slice_views[view].axis_map[X];
        y_index = slice_window->slice.slice_views[view].axis_map[Y];
        get_slice_viewport( slice_window, view,
                            &x_min, &x_max, &y_min, &y_max );

        fit_slice_voxel_range_to_viewport( volume, x_index, y_index,
            slice_window->slice.slice_views[view].lower_view_limits[0],
            slice_window->slice.slice_views[view].upper_view_limits[0],
            slice_window->slice.slice_views[view].lower_view_limits[1],
            slice_window->slice.slice_views[view].upper_view_limits[1],
            x_max - x_min + 1, y_max - y_min + 1,
            &x_scale, &y_scale, &x_trans, &y_trans );

        slice_window->slice.slice_views[view].x_scaling = x_scale;
        slice_window->slice.slice_views[view].y_scaling = y_scale;
        slice_window->slice.slice_views[view].x_trans = x_trans;
        slice_window->slice.slice_views[view].y_trans = y_trans;
    }
}

public  void  scale_slice_view(
    display_struct    *slice_window,
    int               view,
    Real              scale_factor )
{
    Volume volume;
    int    x_min, x_max, y_min, y_max;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        get_slice_viewport( slice_window, view,
                            &x_min, &x_max, &y_min, &y_max );

        scale_slice_about_viewport_centre( scale_factor,
             x_max - x_min + 1, y_max - y_min + 1,
             &slice_window->slice.slice_views[view].x_scaling,
             &slice_window->slice.slice_views[view].y_scaling,
             &slice_window->slice.slice_views[view].x_trans,
             &slice_window->slice.slice_views[view].y_trans );

        record_slice_display_limits( slice_window, view );
    }
}

public  void  translate_slice_view(
    display_struct    *slice_window,
    int               view,
    Real              dx,
    Real              dy )
{
    Volume volume;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        slice_window->slice.slice_views[view].x_trans += dx;
        slice_window->slice.slice_views[view].y_trans += dy;

        record_slice_display_limits( slice_window, view );
    }
}

public  BOOLEAN  find_slice_view_mouse_is_in(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *view_index )
{
    BOOLEAN          found;
    int              c;
    int              x_min, x_max, y_min, y_max;
    display_struct   *slice_window;

    found = FALSE;

    if( get_slice_window(display,&slice_window) )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            get_slice_viewport( slice_window, c,
                                &x_min, &x_max, &y_min, &y_max );

            if( x_pixel >= x_min && x_pixel <= x_max &&
                y_pixel >= y_min && y_pixel <= y_max )
            {
                *view_index = c;
                found = TRUE;

                break;
            }
        }
    }

    return( found );
}

public  BOOLEAN  convert_pixel_to_voxel(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    Real              voxel[],
    int               *view_index )
{
    BOOLEAN           found;
    Volume            volume;
    display_struct    *slice_window;
    int               x_min, x_max, y_min, y_max;

    found = FALSE;

    if( find_slice_view_mouse_is_in( display, x_pixel, y_pixel, view_index ) &&
        get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        get_slice_viewport( slice_window, *view_index,
                            &x_min, &x_max, &y_min, &y_max );
        x_pixel -= x_min;
        y_pixel -= y_min;

        found = convert_slice_pixel_to_voxel( volume, x_pixel, y_pixel,
                    slice_window->slice.slice_index,
                    slice_window->slice.slice_views[*view_index].axis_map[X],
                    slice_window->slice.slice_views[*view_index].axis_map[Y],
                    slice_window->slice.slice_views[*view_index].x_trans,
                    slice_window->slice.slice_views[*view_index].y_trans,
                    slice_window->slice.slice_views[*view_index].x_scaling,
                    slice_window->slice.slice_views[*view_index].y_scaling,
                    voxel );
    }

    return( found );
}

public  void  convert_voxel_to_pixel(
    display_struct    *display,
    int               view_index,
    Real              x_voxel,
    Real              y_voxel,
    int               *x_pixel,
    int               *y_pixel )
{
    Volume            volume;
    display_struct    *slice_window;
    int               x_index, y_index;
    int               x_min, x_max, y_min, y_max;
    Real              voxel_position[N_DIMENSIONS];
    Real              x_real_pixel, y_real_pixel;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( display, &volume ) )
    {
        x_index = slice_window->slice.slice_views[view_index].axis_map[X];
        y_index = slice_window->slice.slice_views[view_index].axis_map[Y];
        voxel_position[x_index] = x_voxel;
        voxel_position[y_index] = y_voxel;
        voxel_position[slice_window->slice.slice_views[view_index].axis_map[Z]]=
                                       slice_window->slice.slice_index[Z];

        convert_voxel_to_slice_pixel( volume, voxel_position, x_index, y_index,
                        slice_window->slice.slice_views[view_index].x_trans,
                        slice_window->slice.slice_views[view_index].y_trans,
                        slice_window->slice.slice_views[view_index].x_scaling,
                        slice_window->slice.slice_views[view_index].y_scaling,
                        &x_real_pixel, &y_real_pixel );

        get_slice_viewport( display, view_index,
                            &x_min, &x_max, &y_min, &y_max );

        *x_pixel = ROUND( x_real_pixel ) + x_min;
        *y_pixel = ROUND( y_real_pixel ) + y_min;
    }
    else
    {
        HANDLE_INTERNAL_ERROR( "convert_voxel_to_pixel" );
    }
}

public  BOOLEAN  get_voxel_corresponding_to_point(
    display_struct    *display,
    Point             *point,
    Real              voxel[] )
{
    Volume          volume;
    BOOLEAN         converted;

    converted = FALSE;

    if( get_slice_window_volume( display, &volume ) )
    {
        convert_world_to_voxel( volume,
                            Point_x(*point), Point_y(*point), Point_z(*point),
                            voxel );

        converted = voxel_is_within_volume( volume, voxel );
    }

    return( converted );
}

public  void  get_slice_viewport(
    display_struct    *display,
    int               view_index,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max )
{
    int  x_size, y_size;

    G_get_window_size( display->window, &x_size, &y_size );

    switch( view_index )
    {
    case 0:
        *x_min = Slice_divider_left;
        *x_max = display->slice.x_split-1-Slice_divider_right;
        *y_min = display->slice.y_split+1+Slice_divider_bottom;
        *y_max = y_size-Slice_divider_top;
        break;

    case 1:
        *x_min = display->slice.x_split+1+Slice_divider_left;
        *x_max = x_size-Slice_divider_right;
        *y_min = display->slice.y_split+1+Slice_divider_bottom;
        *y_max = y_size-Slice_divider_top;
        break;

    case 2:
        *x_min = Slice_divider_left;
        *x_max = display->slice.x_split-1-Slice_divider_right;
        *y_min = Slice_divider_bottom;
        *y_max = display->slice.y_split-1-Slice_divider_top;
        break;

    default:
        *x_min = display->slice.x_split+1+Slice_divider_left;
        *x_max = x_size-Slice_divider_right;
        *y_min = Slice_divider_bottom;
        *y_max = display->slice.y_split-1-Slice_divider_top;
    }
}

public  BOOLEAN  get_voxel_in_slice_window(
    display_struct    *display,
    Real              voxel[],
    int               *view_index )
{
    display_struct    *slice_window;
    int               x_mouse, y_mouse;
    BOOLEAN           found;

    slice_window = display->associated[SLICE_WINDOW];

    (void) G_get_mouse_position( slice_window->window, &x_mouse, &y_mouse );

    found = convert_pixel_to_voxel( slice_window, x_mouse, y_mouse, voxel,
                                    view_index );

    return( found );
}

public  BOOLEAN  get_voxel_in_three_d_window(
    display_struct    *display,
    Real              voxel[] )
{
    BOOLEAN          found;
    object_struct    *object;
    int              object_index;
    Point            intersection_point;
    display_struct   *slice_window;

    found = FALSE;

    if( get_mouse_scene_intersection( display, &object, &object_index,
                                      &intersection_point ) &&
        get_slice_window( display, &slice_window ) )
    {
        found = get_voxel_corresponding_to_point( slice_window,
                                                  &intersection_point,
                                                  voxel );
    }

    return( found );
}

public  BOOLEAN  get_voxel_under_mouse(
    display_struct    *display,
    Real              voxel[],
    int               *view_index )
{
    display_struct    *three_d, *slice_window;
    BOOLEAN           found;

    three_d = display->associated[THREE_D_WINDOW];

    if( !get_slice_window(three_d,&slice_window) )
        found = FALSE;
    else if( G_is_mouse_in_window( slice_window->window ) )
    {
        found = get_voxel_in_slice_window( display, voxel, view_index );
    }
    else if( G_is_mouse_in_window( three_d->window ) )
    {
        found = get_voxel_in_three_d_window( three_d, voxel );
        *view_index = 2;
    }
    else
    {
        found = FALSE;
    }

    return( found );
}

public  void  get_current_voxel(
    display_struct    *slice_window,
    Real              voxel[] )
{
    voxel[X] = slice_window->slice.slice_index[X];
    voxel[Y] = slice_window->slice.slice_index[Y];
    voxel[Z] = slice_window->slice.slice_index[Z];
}

public  BOOLEAN  set_current_voxel(
    display_struct    *slice_window,
    Real              voxel[] )
{
    BOOLEAN           changed;
    int               i, j, sizes[MAX_DIMENSIONS];

    changed = FALSE;

    get_volume_sizes( get_volume(slice_window), sizes );

    for_less( i, 0, N_DIMENSIONS )
    {
        if( voxel[i] != slice_window->slice.slice_index[i] )
        {
            slice_window->slice.slice_index[i] = voxel[i];

            for_less( j, 0, N_DIMENSIONS )
            {
                if( slice_window->slice.slice_views[j].axis_map[Z] == i )
                {
                    set_slice_window_update( slice_window, j );
                }
            }

            changed = TRUE;
        }
    }

    return( changed );
}
