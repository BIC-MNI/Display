
#include  <def_graphics.h>
#include  <def_globals.h>

public  Status  initialize_slice_window( graphics )
    graphics_struct   *graphics;
{
    Status  status;
    Status  initialize_slice_models();
    Status  initialize_colour_coding();
    void    initialize_slice_window_events();
    int     c;

    graphics->slice.volume = (volume_struct *) 0;

    graphics->slice.temporary_indices_alloced = 0;

    initialize_slice_window_events( graphics );

    for_less( c, 0, N_DIMENSIONS )
    {
        graphics->slice.slice_views[c].update_flag = TRUE;
    }

    graphics->slice.slice_views[0].axis_map[0]  = Slice_view1_axis1;
    graphics->slice.slice_views[0].axis_flip[0] = Slice_view1_flip1;
    graphics->slice.slice_views[0].axis_map[1]  = Slice_view1_axis2;
    graphics->slice.slice_views[0].axis_flip[1] = Slice_view1_flip2;
    graphics->slice.slice_views[0].axis_map[2]  = Slice_view1_axis3;
    graphics->slice.slice_views[0].axis_flip[2] = Slice_view1_flip3;

    graphics->slice.slice_views[1].axis_map[0]  = Slice_view2_axis1;
    graphics->slice.slice_views[1].axis_flip[0] = Slice_view2_flip1;
    graphics->slice.slice_views[1].axis_map[1]  = Slice_view2_axis2;
    graphics->slice.slice_views[1].axis_flip[1] = Slice_view2_flip2;
    graphics->slice.slice_views[1].axis_map[2]  = Slice_view2_axis3;
    graphics->slice.slice_views[1].axis_flip[2] = Slice_view2_flip3;

    graphics->slice.slice_views[2].axis_map[0]  = Slice_view3_axis1;
    graphics->slice.slice_views[2].axis_flip[0] = Slice_view3_flip1;
    graphics->slice.slice_views[2].axis_map[1]  = Slice_view3_axis2;
    graphics->slice.slice_views[2].axis_flip[1] = Slice_view3_flip2;
    graphics->slice.slice_views[2].axis_map[2]  = Slice_view3_axis3;
    graphics->slice.slice_views[2].axis_flip[2] = Slice_view3_flip3;

    status = initialize_slice_models( graphics );

    if( status == OK )
    {
        status = initialize_colour_coding( &graphics->slice.colour_coding );
    }

    graphics->slice.fast_lookup_present = FALSE;

    return( status );
}

public  Status  set_slice_window_volume( graphics, volume )
    graphics_struct   *graphics;
    volume_struct     *volume;
{
    Status           status;
    Status           initialize_voxel_flags();
    Status           initialize_voxel_done_flags();
    Status           update_cursor_size();
    int              c, x_index, y_index, num_entries;
    int              size[N_DIMENSIONS];
    Real             factor, min_thickness, max_thickness;
    Real             thickness[N_DIMENSIONS];
    void             change_colour_coding_range();
    void             get_volume_size();
    void             get_volume_slice_thickness();

    graphics->slice.volume = volume;

    get_volume_size( volume, &size[X_AXIS], &size[Y_AXIS], &size[Z_AXIS] );
    get_volume_slice_thickness( volume, &thickness[X_AXIS], &thickness[Y_AXIS],
                                        &thickness[Z_AXIS] );

    for_less( c, 0, N_DIMENSIONS )
    {
        graphics->slice.slice_views[c].x_offset = 0;
        graphics->slice.slice_views[c].y_offset = 0;
        graphics->slice.slice_index[c] = (int) (size[c] / 2);
        graphics->slice.slice_locked[c] = FALSE;
    }

    graphics->associated[THREE_D_WINDOW]->three_d.cursor.box_size[X_AXIS] =
                          thickness[X_AXIS];
    graphics->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Y_AXIS] =
                          thickness[Y_AXIS];
    graphics->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Z_AXIS] =
                          thickness[Z_AXIS];

    status = update_cursor_size( graphics->associated[THREE_D_WINDOW] );

    min_thickness = thickness[X_AXIS];
    max_thickness = thickness[X_AXIS];

    for_less( c, 1, N_DIMENSIONS )
    {
        if( min_thickness > thickness[c] )
            min_thickness = thickness[c];
        if( max_thickness < thickness[c] )
            max_thickness = thickness[c];
    }

    factor = 1.0 / min_thickness;

    for_less( c, 0, N_DIMENSIONS )
    {
        x_index = graphics->slice.slice_views[c].axis_map[X_AXIS];
        y_index = graphics->slice.slice_views[c].axis_map[Y_AXIS];

        graphics->slice.slice_views[c].x_scale = factor * thickness[x_index];

        graphics->slice.slice_views[c].y_scale = factor * thickness[y_index];
    }

    num_entries = graphics->slice.volume->max_value -
                  graphics->slice.volume->min_value + 1;

    graphics->slice.fast_lookup_present =
                  (num_entries <= Max_fast_colour_lookup);

    if( status == OK && graphics->slice.fast_lookup_present )
        ALLOC1( status, graphics->slice.fast_lookup, num_entries,
                Pixel_colour );

    change_colour_coding_range( graphics,
                                (Real) volume->min_value,
                                (Real) volume->max_value );

    if( status == OK )
    {
        status = initialize_voxel_flags( &graphics->associated[THREE_D_WINDOW]
                                     ->three_d.surface_extraction.voxels_queued,
                                     get_n_voxels(graphics->slice.volume) );
    }

    if( status == OK )
    {
        status = initialize_voxel_done_flags(
                   &graphics->associated[THREE_D_WINDOW]
                    ->three_d.surface_extraction.voxel_done_flags,
                    get_n_voxels(graphics->slice.volume) );
    }

    return( status );
}

public  void  change_colour_coding_range( graphics, min_value, max_value )
    graphics_struct   *graphics;
    Real              min_value, max_value;
{
    int              val, min_val, max_val;
    void             set_colour_coding_range();
    Pixel_colour     get_colour_coding();

    set_colour_coding_range( &graphics->slice.colour_coding,
                             min_value, max_value );

    if( graphics->slice.fast_lookup_present )
    {
        min_val = graphics->slice.volume->min_value;
        max_val = graphics->slice.volume->max_value;
   
        for_inclusive( val, min_val, max_val )
        {
            graphics->slice.fast_lookup[val-min_val] =
                    get_colour_coding( &graphics->slice.colour_coding,
                                       (Real) val );
        }
    }
}

public  Boolean   get_slice_window_volume( graphics, volume )
    graphics_struct  *graphics;
    volume_struct    **volume;
{
    Boolean  volume_set;

    volume_set = FALSE;

    if( graphics->associated[SLICE_WINDOW] != (graphics_struct *) 0 )
    {
        *volume = graphics->associated[SLICE_WINDOW]->slice.volume;

        if( *volume != (volume_struct *) 0 )
        {
            volume_set = TRUE;
        }
    }

    return( volume_set );
}

public  Status  delete_slice_window( slice_window )
    slice_window_struct   *slice_window;
{
    Status   status;
    Status   delete_colour_coding();

    status = delete_colour_coding( &slice_window->colour_coding );

    if( status == OK && slice_window->temporary_indices_alloced > 0 )
    {
        FREE1( status, slice_window->temporary_indices );
    }

    if( status == OK && slice_window->fast_lookup_present )
        FREE1( status, slice_window->fast_lookup );

    return( status );
}

public  Boolean  slice_window_exists( graphics )
    graphics_struct   *graphics;
{
    return( graphics != (graphics_struct *) 0 &&
            graphics->associated[SLICE_WINDOW] != (graphics_struct *) 0 );
}

public  Boolean  find_slice_view_mouse_is_in( graphics, x_pixel, y_pixel,
                                              view_index )
    graphics_struct   *graphics;
    int               x_pixel, y_pixel;
    int               *view_index;
{
    Boolean  found;
    int      c;
    int      x_min, x_max, y_min, y_max;
    void     get_slice_viewport();

    found = FALSE;

    if( slice_window_exists(graphics) )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            get_slice_viewport( graphics, c, &x_min, &x_max, &y_min, &y_max );

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

public  Boolean  convert_pixel_to_voxel( graphics, x_pixel, y_pixel, x, y, z,
                                         view_index )
    graphics_struct   *graphics;
    int               x_pixel, y_pixel;
    int               *x, *y, *z;
    int               *view_index;
{
    Boolean  found;
    Real     x_scale, y_scale;
    int      x_index, y_index, axis_index;
    int      start_indices[N_DIMENSIONS];
    int      voxel_indices[N_DIMENSIONS];
    int      x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end;
    void     get_slice_view();

    found = FALSE;

    if( find_slice_view_mouse_is_in( graphics, x_pixel, y_pixel, view_index ) )
    {
        get_slice_view( graphics, *view_index, &x_scale, &y_scale,
                        &x_pixel_start, &y_pixel_start,
                        &x_pixel_end, &y_pixel_end,
                        start_indices );

        if( x_pixel >= x_pixel_start && x_pixel <= x_pixel_end &&
            y_pixel >= y_pixel_start && y_pixel <= y_pixel_end )
        {
            axis_index =
                graphics->slice.slice_views[*view_index].axis_map[Z_AXIS];
            x_index =
                graphics->slice.slice_views[*view_index].axis_map[X_AXIS];
            y_index =
                graphics->slice.slice_views[*view_index].axis_map[Y_AXIS];

            voxel_indices[axis_index] = start_indices[axis_index];

            voxel_indices[x_index] = start_indices[x_index] +
                                        (x_pixel - x_pixel_start) / x_scale;
            voxel_indices[y_index] = start_indices[y_index] +
                                        (y_pixel - y_pixel_start) / y_scale;

            *x = voxel_indices[X_AXIS];
            *y = voxel_indices[Y_AXIS];
            *z = voxel_indices[Z_AXIS];

            found = TRUE;
        }
    }

    return( found );
}

private  int  voxel_to_pixel( x_min, x_offset, x_scale, voxel )
    int   x_min, x_offset;
    Real  x_scale;
    int   voxel;
{
    return( x_min + x_offset + (Real) voxel * x_scale );
}

public  void  convert_voxel_to_pixel( graphics, view_index, x_voxel, y_voxel,
                                      x_pixel, y_pixel )
    graphics_struct   *graphics;
    int               view_index;
    int               x_voxel, y_voxel;
    int               *x_pixel, *y_pixel;
{
    int      x_index, y_index;
    int      x_min, x_max, y_min, y_max;
    int      size[N_DIMENSIONS];
    Real     x_scale, y_scale;
    void     get_slice_viewport();
    void     get_slice_scale();
    void     get_volume_size();

    get_slice_viewport( graphics, view_index, &x_min, &x_max, &y_min, &y_max );

    get_slice_scale( graphics, view_index, &x_scale, &y_scale );

    x_index = graphics->slice.slice_views[view_index].axis_map[X_AXIS];
    y_index = graphics->slice.slice_views[view_index].axis_map[Y_AXIS];

    get_volume_size( graphics->slice.volume,
                     &size[X_AXIS], &size[Y_AXIS], &size[Z_AXIS] );

    if( graphics->slice.slice_views[view_index].axis_flip[X_AXIS] )
        x_voxel = size[x_index] - 1 - x_voxel;

    if( graphics->slice.slice_views[view_index].axis_flip[Y_AXIS] )
        y_voxel = size[y_index] - 1 - y_voxel;

    *x_pixel = voxel_to_pixel( x_min,
                               graphics->slice.slice_views[view_index].x_offset,
                               x_scale, x_voxel );

    *y_pixel = voxel_to_pixel( y_min,
                               graphics->slice.slice_views[view_index].y_offset,
                               y_scale, y_voxel );
}

public  void  convert_voxel_to_point( graphics, x, y, z, centre )
    graphics_struct   *graphics;
    Real              x, y, z;
    Point             *centre;
{
    Real    dx, dy, dz;
    void    get_volume_slice_thickness();

    get_volume_slice_thickness( graphics->slice.volume, &dx, &dy, &dz );

    fill_Point( *centre, x * dx, y * dy, z * dz );
}

public  Boolean  convert_point_to_voxel( graphics, point, x, y, z )
    graphics_struct   *graphics;
    Point             *point;
    Real              *x, *y, *z;
{
    volume_struct   *volume;
    Boolean         converted;
    int             nx, ny, nz;
    Real            dx, dy, dz;
    void            get_volume_slice_thickness();

    converted = FALSE;

    if( get_slice_window_volume( graphics, &volume ) )
    {
        get_volume_size( volume, &nx, &ny, &nz );
        get_volume_slice_thickness( volume, &dx, &dy, &dz );

        *x = Point_x(*point) / dx;
        *y = Point_y(*point) / dy;
        *z = Point_z(*point) / dz;

        converted = (*x >= -0.5 && *x < (Real) nx + 0.5 &&
                     *y >= -0.5 && *y < (Real) ny + 0.5 &&
                     *z >= -0.5 && *z < (Real) nz + 0.5 );
    }

    return( converted );
}

private  void  get_slice_scale( graphics, view_index, x_scale, y_scale )
    graphics_struct   *graphics;
    int               view_index;
    Real              *x_scale;
    Real              *y_scale;
{
    *x_scale = graphics->slice.slice_views[view_index].x_scale;
    *y_scale = graphics->slice.slice_views[view_index].y_scale;
}

public  void  get_slice_viewport( graphics, view_index,
                                  x_min, x_max, y_min, y_max )
    graphics_struct   *graphics;
    int               view_index;
    int               *x_min, *x_max, *y_min, *y_max;
{
    switch( view_index )
    {
    case 0:
        *x_min = Slice_divider_left;
        *x_max = graphics->slice.x_split-1-Slice_divider_right;
        *y_min = graphics->slice.y_split+1+Slice_divider_bottom;
        *y_max = graphics->window.y_size-Slice_divider_top;
        break;

    case 1:
        *x_min = graphics->slice.x_split+1+Slice_divider_left;
        *x_max = graphics->window.x_size-Slice_divider_right;
        *y_min = graphics->slice.y_split+1+Slice_divider_bottom;
        *y_max = graphics->window.y_size-Slice_divider_top;
        break;

    case 2:
        *x_min = Slice_divider_left;
        *x_max = graphics->slice.x_split-1-Slice_divider_right;
        *y_min = Slice_divider_bottom;
        *y_max = graphics->slice.y_split-1-Slice_divider_top;
        break;

    default:
        *x_min = graphics->slice.x_split+1+Slice_divider_left;
        *x_max = graphics->window.x_size-Slice_divider_right;
        *y_min = Slice_divider_bottom;
        *y_max = graphics->slice.y_split-1-Slice_divider_top;
    }
}

public  void  get_slice_view( graphics, view_index, x_scale, y_scale,
                              x_pixel, y_pixel, x_pixel_end, y_pixel_end,
                              indices )
    graphics_struct  *graphics;
    int              view_index;
    Real             *x_scale, *y_scale;
    int              *x_pixel, *y_pixel;
    int              *x_pixel_end, *y_pixel_end;
    int              indices[N_DIMENSIONS];
{
    int   x_axis_index, y_axis_index, axis_index;
    int   x_offset, y_offset;
    int   x_size, y_size;
    int   x_min, x_max, y_min, y_max;
    int   size[N_DIMENSIONS];
    Real  start_offset;
    void  get_slice_scale();
    void  get_slice_viewport();
    void  get_volume_size();

    axis_index = graphics->slice.slice_views[view_index].axis_map[Z_AXIS];
    x_axis_index = graphics->slice.slice_views[view_index].axis_map[X_AXIS];
    y_axis_index = graphics->slice.slice_views[view_index].axis_map[Y_AXIS];

    indices[axis_index] = graphics->slice.slice_index[axis_index];

    x_offset = graphics->slice.slice_views[view_index].x_offset;
    y_offset = graphics->slice.slice_views[view_index].y_offset;

    get_slice_scale( graphics, view_index, x_scale, y_scale );

    get_volume_size( graphics->slice.volume,
                     &size[X_AXIS], &size[Y_AXIS], &size[Z_AXIS] );

    x_size = size[x_axis_index];
    y_size = size[y_axis_index];

    get_slice_viewport( graphics, view_index, &x_min, &x_max, &y_min, &y_max );

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

    if( graphics->slice.slice_views[view_index].axis_flip[X_AXIS] )
    {
        indices[x_axis_index] = x_size - 1 - indices[x_axis_index];
        *x_scale = -(*x_scale);
    }

    if( graphics->slice.slice_views[view_index].axis_flip[Y_AXIS] )
    {
        indices[y_axis_index] = y_size - 1 - indices[y_axis_index];
        *y_scale = -(*y_scale);
    }
}

public  Boolean  get_voxel_in_slice_window( graphics, x, y, z, view_index )
    graphics_struct   *graphics;
    int               *x, *y, *z;
    int               *view_index;
{
    graphics_struct   *slice_window;
    void              get_mouse_in_pixels();
    int               x_mouse, y_mouse;
    Boolean           found;
    Boolean           convert_pixel_to_voxel();

    slice_window = graphics->associated[SLICE_WINDOW];

    get_mouse_in_pixels( slice_window, &slice_window->mouse_position,
                         &x_mouse, &y_mouse );

    found = convert_pixel_to_voxel( slice_window, x_mouse, y_mouse, x, y, z,
                                    view_index );

    return( found );
}

public  Boolean  get_voxel_in_three_d_window( graphics, x, y, z )
    graphics_struct   *graphics;
    int               *x, *y, *z;
{
    Boolean          found;
    polygons_struct  *polygons;
    int              poly_index;
    Point            intersection_point;
    graphics_struct  *slice_window;
    Real             xr, yr, zr;

    found = FALSE;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        if( slice_window != (graphics_struct *) 0 )
        {
            if( convert_point_to_voxel( slice_window, &intersection_point,
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

public  Boolean  get_voxel_under_mouse( graphics, x, y, z, view_index )
    graphics_struct   *graphics;
    int               *x, *y, *z;
    int               *view_index;
{
    graphics_struct   *three_d, *slice_window;
    Boolean           found;
    Boolean           get_voxel_in_slice_window();
    Boolean           get_voxel_in_three_d_window();

    three_d = graphics->associated[THREE_D_WINDOW];
    slice_window = graphics->associated[SLICE_WINDOW];

    if( G_is_mouse_in_window( &slice_window->window ) )
    {
        found = get_voxel_in_slice_window( graphics, x, y, z, view_index );
    }
    else if( G_is_mouse_in_window( &three_d->window ) )
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

public  void  get_current_voxel( slice_window, x, y, z )
    graphics_struct   *slice_window;
    int               *x, *y, *z;
{
    *x = slice_window->slice.slice_index[X_AXIS];
    *y = slice_window->slice.slice_index[Y_AXIS];
    *z = slice_window->slice.slice_index[Z_AXIS];
}

public  Boolean  set_current_voxel( slice_window, x, y, z )
    graphics_struct   *slice_window;
    int               x, y, z;
{
    Boolean           changed;
    int               i, j, axis_index, indices[N_DIMENSIONS];
    void              set_slice_window_update();

    indices[X_AXIS] = x;
    indices[Y_AXIS] = y;
    indices[Z_AXIS] = z;

    changed = FALSE;

    for_less( i, 0, N_DIMENSIONS )
    {
        axis_index = slice_window->slice.slice_views[i].axis_map[Z_AXIS];

        if( indices[axis_index] != slice_window->slice.slice_index[axis_index] )
        {
            slice_window->slice.slice_index[axis_index] = indices[axis_index];

            for_less( j, i, N_DIMENSIONS )
            {
                if( slice_window->slice.slice_views[j].axis_map[Z_AXIS] ==
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

private  void  set_cursor_colour( slice_window )
    graphics_struct  *slice_window;
{
    int       x, y, z;
    Real      value;
    void      get_current_voxel();
    Boolean   get_isosurface_value();
    Boolean   voxel_contains_value();
    void      update_cursor_colour();
    void      G_ring_bell();

    if( get_isosurface_value( slice_window->associated[THREE_D_WINDOW], &value))
    {
        get_current_voxel( slice_window, &x, &y, &z );

        if( cube_is_within_volume( slice_window->slice.volume, x, y, z ) &&
            voxel_contains_value( slice_window->slice.volume, x, y, z, value ) )
        {
            update_cursor_colour( slice_window->associated[THREE_D_WINDOW],
                                  &Cursor_colour_on_surface );
            G_ring_bell( Cursor_beep_on_surface );
        }
        else
        {
            update_cursor_colour( slice_window->associated[THREE_D_WINDOW],
                                  &Cursor_colour_off_surface );
        }
    }
}

public  Boolean  update_cursor_from_voxel( slice_window )
    graphics_struct   *slice_window;
{
    int               x, y, z;
    Boolean           changed;
    Point             new_origin;
    void              convert_voxel_to_point();
    graphics_struct   *graphics;
    void              update_cursor();
    void              set_cursor_colour();
    void              get_current_voxel();

    graphics = slice_window->associated[THREE_D_WINDOW];

    get_current_voxel( slice_window, &x, &y, &z );

    convert_voxel_to_point( slice_window, (Real) x, (Real) y, (Real) z,
                            &new_origin );

    if( !EQUAL_POINTS( new_origin, graphics->three_d.cursor.origin ) )
    {
        graphics->three_d.cursor.origin = new_origin;

        set_cursor_colour( slice_window );

        update_cursor( graphics );

        changed = TRUE;
    }
    else
    {
        changed = FALSE;
    }

    return( changed );
}

public  Boolean  update_voxel_from_cursor( slice_window )
    graphics_struct   *slice_window;
{
    Real              x, y, z;
    Boolean           changed;
    graphics_struct   *graphics;
    void              set_cursor_colour();

    changed = FALSE;

    if( slice_window_exists(slice_window) )
    {
        graphics = slice_window->associated[THREE_D_WINDOW];

        if( convert_point_to_voxel( slice_window,
                                    &graphics->three_d.cursor.origin,
                                    &x, &y, &z ) )
        {
            changed = set_current_voxel( slice_window,
                                         ROUND( x ), ROUND( y ), ROUND( z ) );

            set_cursor_colour( slice_window );
        }
    }

    return( changed );
}

public  void  set_slice_window_update( graphics, view_index )
    graphics_struct  *graphics;
    int              view_index;
{
    graphics->slice.slice_views[view_index].update_flag = TRUE;
}

public  void  update_slice_window( graphics )
    graphics_struct  *graphics;
{
    int   c;
    void  rebuild_slice_pixels();

    for_less( c, 0, N_DIMENSIONS )
    {
        if( graphics->slice.slice_views[c].update_flag )
        {
            rebuild_slice_pixels( graphics, c );
            graphics->slice.slice_views[c].update_flag = FALSE;
        }
    }
}
