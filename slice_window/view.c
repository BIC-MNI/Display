
#include  <def_display.h>

private  void  get_slice_scale(
    display_struct    *display,
    int               view_index,
    Real              *x_scale,
    Real              *y_scale );

public  void  initialize_slice_window_view(
    display_struct    *slice_window )
{
    Volume           volume;
    int              c, x_index, y_index, z_index;
    int              size[N_DIMENSIONS];
    Real             factor, min_thickness, max_thickness;
    Real             thickness[N_DIMENSIONS];

    slice_window->slice.slice_views[0].axis_map[0]  = Slice_view1_axis1;
    slice_window->slice.slice_views[0].axis_flip[0] = Slice_view1_flip1;
    slice_window->slice.slice_views[0].axis_map[1]  = Slice_view1_axis2;
    slice_window->slice.slice_views[0].axis_flip[1] = Slice_view1_flip2;
    slice_window->slice.slice_views[0].axis_map[2]  = Slice_view1_axis3;
    slice_window->slice.slice_views[0].axis_flip[2] = Slice_view1_flip3;

    slice_window->slice.slice_views[1].axis_map[0]  = Slice_view2_axis1;
    slice_window->slice.slice_views[1].axis_flip[0] = Slice_view2_flip1;
    slice_window->slice.slice_views[1].axis_map[1]  = Slice_view2_axis2;
    slice_window->slice.slice_views[1].axis_flip[1] = Slice_view2_flip2;
    slice_window->slice.slice_views[1].axis_map[2]  = Slice_view2_axis3;
    slice_window->slice.slice_views[1].axis_flip[2] = Slice_view2_flip3;

    slice_window->slice.slice_views[2].axis_map[0]  = Slice_view3_axis1;
    slice_window->slice.slice_views[2].axis_flip[0] = Slice_view3_flip1;
    slice_window->slice.slice_views[2].axis_map[1]  = Slice_view3_axis2;
    slice_window->slice.slice_views[2].axis_flip[1] = Slice_view3_flip2;
    slice_window->slice.slice_views[2].axis_map[2]  = Slice_view3_axis3;
    slice_window->slice.slice_views[2].axis_flip[2] = Slice_view3_flip3;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_sizes( volume, size );
        get_volume_separations( volume, thickness );

        for_less( c, 0, N_DIMENSIONS )
        {
            slice_window->slice.slice_views[c].x_offset = 0;
            slice_window->slice.slice_views[c].y_offset = 0;
            slice_window->slice.slice_index[c] = (int) (size[c] / 2);
            slice_window->slice.slice_locked[c] = FALSE;
        }

        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[X] =
                          ABS( thickness[X] );
        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Y] =
                          ABS( thickness[Y] );
        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Z] =
                          ABS( thickness[Z] );

        update_cursor_size( slice_window->associated[THREE_D_WINDOW] );

        min_thickness = ABS( thickness[X] );
        max_thickness = ABS( thickness[X] );

        for_less( c, 1, N_DIMENSIONS )
        {
            if( min_thickness > ABS( thickness[c] ) )
                min_thickness = ABS( thickness[c] );
            if( max_thickness < ABS( thickness[c] ) )
                max_thickness = ABS( thickness[c] );
        }

        factor = 1.0 / min_thickness;
        factor = 1.0;

        for_less( c, 0, N_DIMENSIONS )
        {
            x_index = slice_window->slice.slice_views[c].axis_map[X];
            y_index = slice_window->slice.slice_views[c].axis_map[Y];
            z_index = slice_window->slice.slice_views[c].axis_map[Z];

            if( thickness[x_index] < 0.0 )
                slice_window->slice.slice_views[c].axis_flip[X] =
                   !slice_window->slice.slice_views[c].axis_flip[X];

            if( thickness[y_index] < 0.0 )
                slice_window->slice.slice_views[c].axis_flip[Y] =
                   !slice_window->slice.slice_views[c].axis_flip[Y];

            if( thickness[z_index] < 0.0 )
                slice_window->slice.slice_views[c].axis_flip[Z] =
                   !slice_window->slice.slice_views[c].axis_flip[Z];

            slice_window->slice.slice_views[c].x_scale =
                                    factor*ABS(thickness[x_index]);

            slice_window->slice.slice_views[c].y_scale =
                                    factor*ABS(thickness[y_index]);
        }
    }
}

public  Boolean  find_slice_view_mouse_is_in(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *view_index )
{
    Boolean          found;
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

public  Boolean  convert_pixel_to_voxel(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *x,
    int               *y,
    int               *z,
    int               *view_index )
{
    Boolean  found;
    Real     x_scale, y_scale;
    int      x_index, y_index, axis_index;
    int      start_indices[N_DIMENSIONS];
    int      voxel_indices[N_DIMENSIONS];
    int      x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end;

    found = FALSE;

    if( find_slice_view_mouse_is_in( display, x_pixel, y_pixel, view_index ) )
    {
        get_slice_view( display, *view_index, &x_scale, &y_scale,
                        &x_pixel_start, &y_pixel_start,
                        &x_pixel_end, &y_pixel_end,
                        start_indices );

        if( x_pixel >= x_pixel_start && x_pixel <= x_pixel_end &&
            y_pixel >= y_pixel_start && y_pixel <= y_pixel_end )
        {
            axis_index =
                display->slice.slice_views[*view_index].axis_map[Z];
            x_index =
                display->slice.slice_views[*view_index].axis_map[X];
            y_index =
                display->slice.slice_views[*view_index].axis_map[Y];

            voxel_indices[axis_index] = start_indices[axis_index];

            voxel_indices[x_index] = start_indices[x_index] +
                            (int) ((Real) (x_pixel - x_pixel_start) / x_scale );
            voxel_indices[y_index] = start_indices[y_index] +
                            (int) ((Real) (y_pixel - y_pixel_start) / y_scale );

            *x = voxel_indices[X];
            *y = voxel_indices[Y];
            *z = voxel_indices[Z];

            found = TRUE;
        }
    }

    return( found );
}

private  int  voxel_to_pixel(
    int   x_min,
    int   x_offset,
    Real  x_scale,
    int   voxel )
{
    return( x_min + x_offset + (Real) voxel * x_scale );
}

public  void  convert_voxel_to_pixel(
    display_struct    *display,
    int               view_index,
    int               x_voxel,
    int               y_voxel,
    int               *x_pixel,
    int               *y_pixel )
{
    int      x_index, y_index;
    int      x_min, x_max, y_min, y_max;
    int      size[N_DIMENSIONS];
    Real     x_scale, y_scale;

    get_slice_viewport( display, view_index, &x_min, &x_max, &y_min, &y_max );

    get_slice_scale( display, view_index, &x_scale, &y_scale );

    x_index = display->slice.slice_views[view_index].axis_map[X];
    y_index = display->slice.slice_views[view_index].axis_map[Y];

    get_volume_sizes( get_volume(display), size );

    if( display->slice.slice_views[view_index].axis_flip[X] )
        x_voxel = size[x_index] - 1 - x_voxel;

    if( display->slice.slice_views[view_index].axis_flip[Y] )
        y_voxel = size[y_index] - 1 - y_voxel;

    *x_pixel = voxel_to_pixel( x_min,
                               display->slice.slice_views[view_index].x_offset,
                               x_scale, x_voxel );

    *y_pixel = voxel_to_pixel( y_min,
                               display->slice.slice_views[view_index].y_offset,
                               y_scale, y_voxel );
}

public  Boolean  get_voxel_corresponding_to_point(
    display_struct    *display,
    Point             *point,
    Real              *x,
    Real              *y,
    Real              *z )
{
    Volume          volume;
    Real            pos[N_DIMENSIONS];
    Boolean         converted;

    converted = FALSE;

    if( get_slice_window_volume( display, &volume ) )
    {
        convert_world_to_voxel( volume,
                            Point_x(*point), Point_y(*point), Point_z(*point),
                            x, y, z );

        pos[X] = *x;
        pos[Y] = *y;
        pos[Z] = *z;
        converted = voxel_is_within_volume( volume, pos );
    }

    return( converted );
}

private  void  get_slice_scale(
    display_struct    *display,
    int               view_index,
    Real              *x_scale,
    Real              *y_scale )
{
    *x_scale = display->slice.slice_views[view_index].x_scale;
    *y_scale = display->slice.slice_views[view_index].y_scale;
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

public  void  get_slice_view(
    display_struct   *display,
    int              view_index,
    Real             *x_scale,
    Real             *y_scale,
    int              *x_pixel,
    int              *y_pixel,
    int              *x_pixel_end,
    int              *y_pixel_end,
    int              indices[N_DIMENSIONS] )
{
    int   x_axis_index, y_axis_index, axis_index;
    int   x_offset, y_offset;
    int   x_size, y_size;
    int   x_min, x_max, y_min, y_max;
    int   size[N_DIMENSIONS];
    Real  start_offset;

    axis_index = display->slice.slice_views[view_index].axis_map[Z];
    x_axis_index = display->slice.slice_views[view_index].axis_map[X];
    y_axis_index = display->slice.slice_views[view_index].axis_map[Y];

    indices[axis_index] = display->slice.slice_index[axis_index];

    x_offset = display->slice.slice_views[view_index].x_offset;
    y_offset = display->slice.slice_views[view_index].y_offset;

    get_slice_scale( display, view_index, x_scale, y_scale );

    get_volume_sizes( get_volume(display), size );

    x_size = size[x_axis_index];
    y_size = size[y_axis_index];

    get_slice_viewport( display, view_index, &x_min, &x_max, &y_min, &y_max );

    *x_pixel = voxel_to_pixel( x_min, x_offset, *x_scale, 0 );
    indices[x_axis_index] = 0;

    *x_pixel_end = voxel_to_pixel( x_min, x_offset, *x_scale, x_size ) - 1;
    if( *x_pixel_end > x_max )
    {
        *x_pixel_end = x_max;
    }

    if( *x_pixel < x_min )
    {
        start_offset = -(Real) x_offset / *x_scale;
        indices[x_axis_index] = (int) start_offset;

        if( start_offset != (Real) indices[x_axis_index] )
        {
            ++indices[x_axis_index];
            *x_pixel = voxel_to_pixel( x_min, x_offset, *x_scale,
                                       indices[x_axis_index] );
        }
        else
        {
            *x_pixel = x_min;
        }

        if( indices[x_axis_index] >= x_size )
        {
            *x_pixel = x_max + 1;
        }
    }

    *y_pixel = voxel_to_pixel( y_min, y_offset, *y_scale, 0 );
    indices[y_axis_index] = 0;

    *y_pixel_end = voxel_to_pixel( y_min, y_offset, *y_scale, y_size ) - 1;
    if( *y_pixel_end > y_max )
    {
        *y_pixel_end = y_max;
    }

    if( *y_pixel < y_min )
    {
        start_offset = -(Real) y_offset / *y_scale;
        indices[y_axis_index] = (int) start_offset;

        if( start_offset != (Real) indices[y_axis_index] )
        {
            ++indices[y_axis_index];
            *y_pixel = voxel_to_pixel( y_min, y_offset, *y_scale,
                                       indices[y_axis_index] );
        }
        else
        {
            *y_pixel = y_min;
        }

        if( indices[y_axis_index] >= y_size )
        {
            *y_pixel = y_max + 1;
        }
    }

    if( display->slice.slice_views[view_index].axis_flip[X] )
    {
        indices[x_axis_index] = x_size - 1 - indices[x_axis_index];
        *x_scale = -(*x_scale);
    }

    if( display->slice.slice_views[view_index].axis_flip[Y] )
    {
        indices[y_axis_index] = y_size - 1 - indices[y_axis_index];
        *y_scale = -(*y_scale);
    }
}

public  Boolean  get_voxel_in_slice_window(
    display_struct    *display,
    int               *x,
    int               *y,
    int               *z,
    int               *view_index )
{
    display_struct    *slice_window;
    int               x_mouse, y_mouse;
    Boolean           found;

    slice_window = display->associated[SLICE_WINDOW];

    (void) G_get_mouse_position( slice_window->window, &x_mouse, &y_mouse );

    found = convert_pixel_to_voxel( slice_window, x_mouse, y_mouse, x, y, z,
                                    view_index );

    return( found );
}

public  Boolean  get_voxel_in_three_d_window(
    display_struct    *display,
    int               *x,
    int               *y,
    int               *z )
{
    Boolean          found;
    object_struct    *object;
    int              object_index;
    Point            intersection_point;
    display_struct   *slice_window;
    Real             xr, yr, zr;

    found = FALSE;

    if( get_mouse_scene_intersection( display, &object, &object_index,
                                      &intersection_point ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        if( slice_window != (display_struct  *) 0 )
        {
            if( get_voxel_corresponding_to_point( slice_window,
                                                  &intersection_point,
                                                  &xr, &yr, &zr ) )
            {
                *x = ROUND( xr );
                *y = ROUND( yr );
                *z = ROUND( zr );
                found = TRUE;
            }
        }
    }

    return( found );
}

public  Boolean  get_voxel_under_mouse(
    display_struct    *display,
    int               *x,
    int               *y,
    int               *z,
    int               *view_index )
{
    display_struct    *three_d, *slice_window;
    Boolean           found;

    three_d = display->associated[THREE_D_WINDOW];

    if( !get_slice_window(three_d,&slice_window) )
        found = FALSE;
    else if( G_is_mouse_in_window( slice_window->window ) )
    {
        found = get_voxel_in_slice_window( display, x, y, z, view_index );
    }
    else if( G_is_mouse_in_window( three_d->window ) )
    {
        found = get_voxel_in_three_d_window( three_d, x, y, z );
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
    int               *x,
    int               *y,
    int               *z )
{
    *x = slice_window->slice.slice_index[X];
    *y = slice_window->slice.slice_index[Y];
    *z = slice_window->slice.slice_index[Z];
}

public  Boolean  set_current_voxel(
    display_struct    *slice_window,
    int               x,
    int               y,
    int               z )
{
    Boolean           changed;
    int               i, j, axis_index, indices[N_DIMENSIONS];

    indices[X] = x;
    indices[Y] = y;
    indices[Z] = z;

    changed = FALSE;

    for_less( i, 0, N_DIMENSIONS )
    {
        axis_index = slice_window->slice.slice_views[i].axis_map[Z];

        if( indices[axis_index] != slice_window->slice.slice_index[axis_index] )
        {
            slice_window->slice.slice_index[axis_index] = indices[axis_index];

            for_less( j, i, N_DIMENSIONS )
            {
                if( slice_window->slice.slice_views[j].axis_map[Z] ==
                    axis_index)
                {
                    set_slice_window_update( slice_window, j );
                }
            }

            changed = TRUE;
        }
    }

    return( changed );
}
