
#include  <def_graphics.h>
#include  <def_globals.h>

public  Status  initialize_slice_window( graphics )
    graphics_struct   *graphics;
{
    Status  status;
    Status  initialize_slice_models();
    Status  initialize_colour_coding();
    void    initialize_slice_window_events();

    graphics->slice.volume = (volume_struct *) 0;

    initialize_slice_window_events( graphics );

    status = initialize_slice_models( graphics );

    if( status == OK )
    {
        status = create_bitlist( 0, &graphics->slice.voxel_activity );
    }

    if( status == OK )
    {
        status = initialize_colour_coding( &graphics->slice.colour_coding );
    }

    return( status );
}

public  Status  set_slice_window_volume( graphics, volume )
    graphics_struct   *graphics;
    volume_struct     *volume;
{
    Status           status;
    int              c, x_index, y_index, n_voxels;
    Real             factor, min_thickness, max_thickness;
    void             get_2d_slice_axes();
    void             set_colour_coding_range();

    graphics->slice.volume = volume;

    for_less( c, 0, N_DIMENSIONS )
    {
        graphics->slice.slice_views[c].x_offset = 0;
        graphics->slice.slice_views[c].y_offset = 0;
        graphics->slice.slice_views[c].slice_index =
                                (int) (volume->size[c] / 2);
    }

    min_thickness = volume->slice_thickness[X_AXIS];
    max_thickness = volume->slice_thickness[X_AXIS];

    for_less( c, 1, N_DIMENSIONS )
    {
        if( min_thickness > volume->slice_thickness[c] )
        {
            min_thickness = volume->slice_thickness[c];
        }
        if( max_thickness < volume->slice_thickness[c] )
        {
            max_thickness = volume->slice_thickness[c];
        }
    }

    factor = 1.0 / min_thickness;

    for_less( c, 0, N_DIMENSIONS )
    {
        get_2d_slice_axes( c, &x_index, &y_index );

        graphics->slice.slice_views[c].x_scale = factor *
                                  volume->slice_thickness[x_index];

        graphics->slice.slice_views[c].y_scale = factor *
                                  volume->slice_thickness[y_index];
    }

    set_colour_coding_range( &graphics->slice.colour_coding,
                             volume->min_value, volume->max_value );

    n_voxels = volume->size[X_AXIS] * volume->size[Y_AXIS] *
               volume->size[Z_AXIS];

    status = delete_bitlist( &graphics->slice.voxel_activity );

    if( status == OK )
    {
        status = create_bitlist( n_voxels, &graphics->slice.voxel_activity );
    }

    return( status );
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

    status = delete_bitlist( &slice_window->voxel_activity );

    if( status == OK )
    {
        status = delete_colour_coding( &slice_window->colour_coding );
    }

    return( status );
}

public  Boolean  slice_window_has_volume( graphics )
    graphics_struct   *graphics;
{
    return( graphics->slice.volume != (volume_struct *) 0 );
}

public  Boolean  find_slice_view_mouse_is_in( graphics, x_pixel, y_pixel,
                                              axis_index )
    graphics_struct   *graphics;
    int               x_pixel, y_pixel;
    int               *axis_index;
{
    Boolean  found;
    int      c;
    int      x_min, x_max, y_min, y_max;
    void     get_slice_viewport();

    found = FALSE;

    if( slice_window_has_volume(graphics) )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            get_slice_viewport( graphics, c, &x_min, &x_max, &y_min, &y_max );

            if( x_pixel >= x_min && x_pixel <= x_max &&
                y_pixel >= y_min && y_pixel <= y_max )
            {
                *axis_index = c;
                found = TRUE;

                break;
            }
        }
    }

    return( found );
}

public  Boolean  convert_pixel_to_voxel( graphics, x_pixel, y_pixel, x, y, z,
                                         axis_index )
    graphics_struct   *graphics;
    int               x_pixel, y_pixel;
    int               *x, *y, *z;
    int               *axis_index;
{
    Boolean  found;
    Real     x_scale, y_scale;
    int      x_index, y_index;
    int      start_indices[N_DIMENSIONS];
    int      voxel_indices[N_DIMENSIONS];
    int      x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end;
    void     get_slice_view();
    void     get_2d_slice_axes();

    found = FALSE;

    if( find_slice_view_mouse_is_in( graphics, x_pixel, y_pixel, axis_index ) )
    {
        get_slice_view( graphics, *axis_index, &x_scale, &y_scale,
                        &x_pixel_start, &y_pixel_start,
                        &x_pixel_end, &y_pixel_end,
                        start_indices );

        if( x_pixel >= x_pixel_start && x_pixel <= x_pixel_end &&
            y_pixel >= y_pixel_start && y_pixel <= y_pixel_end )
        {
            get_2d_slice_axes( *axis_index, &x_index, &y_index );

            voxel_indices[*axis_index] = start_indices[*axis_index];

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

public  void  convert_voxel_to_pixel( graphics, axis_index, x_voxel, y_voxel,
                                      x_pixel, y_pixel )
    graphics_struct   *graphics;
    int               axis_index;
    int               x_voxel, y_voxel;
    int               *x_pixel, *y_pixel;
{
    int      x_index, y_index;
    int      x_min, x_max, y_min, y_max;
    Real     x_scale, y_scale;
    void     get_slice_viewport();
    void     get_2d_slice_axes();
    void     get_slice_scale();

    get_slice_viewport( graphics, axis_index, &x_min, &x_max, &y_min, &y_max );

    get_slice_scale( graphics, axis_index, &x_scale, &y_scale );

    get_2d_slice_axes( axis_index, &x_index, &y_index );

    *x_pixel = voxel_to_pixel( x_min,
                               graphics->slice.slice_views[axis_index].x_offset,
                               x_scale, x_voxel );

    *y_pixel = voxel_to_pixel( y_min,
                               graphics->slice.slice_views[axis_index].y_offset,
                               y_scale, y_voxel );
}

public  void  get_voxel_centre( graphics, x, y, z, centre )
    graphics_struct   *graphics;
    int               x, y, z;
    Point             *centre;
{
    fill_Point( *centre, 
          (Real) x * graphics->slice.volume->slice_thickness[X_AXIS],
          (Real) y * graphics->slice.volume->slice_thickness[Y_AXIS],
          (Real) z * graphics->slice.volume->slice_thickness[Z_AXIS] );
}

public  Boolean  convert_point_to_voxel( graphics, point, x, y, z )
    graphics_struct   *graphics;
    Point             *point;
    int               *x, *y, *z;
{
    volume_struct   *volume;
    Boolean         converted;

    converted = FALSE;

    if( get_slice_window_volume( graphics, &volume ) )
    {
        *x = ROUND( Point_x(*point) / volume->slice_thickness[X_AXIS] );
        *y = ROUND( Point_y(*point) / volume->slice_thickness[Y_AXIS] );
        *z = ROUND( Point_z(*point) / volume->slice_thickness[Z_AXIS] );

        if( *x == volume->size[X_AXIS] )  *x = volume->size[X_AXIS]-1;
        if( *y == volume->size[Y_AXIS] )  *y = volume->size[Y_AXIS]-1;
        if( *z == volume->size[Z_AXIS] )  *z = volume->size[Z_AXIS]-1;

        converted = (*x >= 0 && *x < volume->size[X_AXIS]-1 &&
                     *y >= 0 && *y < volume->size[Y_AXIS]-1 &&
                     *z >= 0 && *z < volume->size[Z_AXIS]-1);
    }

    return( converted );
}

private  void  get_slice_scale( graphics, axis_index, x_scale, y_scale )
    graphics_struct   *graphics;
    int               axis_index;
    Real              *x_scale;
    Real              *y_scale;
{
    int   x_index, y_index;
    void  get_2d_slice_axes();

    get_2d_slice_axes( axis_index, &x_index, &y_index );

    *x_scale = graphics->slice.slice_views[axis_index].x_scale;
    *y_scale = graphics->slice.slice_views[axis_index].y_scale;
}

public  void  get_slice_viewport( graphics, axis_index,
                                  x_min, x_max, y_min, y_max )
    graphics_struct   *graphics;
    int               axis_index;
    int               *x_min, *x_max, *y_min, *y_max;
{
    switch( axis_index )
    {
    case X_AXIS:
        *x_min = Slice_divider_left;
        *x_max = graphics->slice.x_split-1-Slice_divider_right;
        *y_min = graphics->slice.y_split+1+Slice_divider_bottom;
        *y_max = graphics->window.y_size-Slice_divider_top;
        break;

    case Y_AXIS:
        *x_min = graphics->slice.x_split+1+Slice_divider_left;
        *x_max = graphics->window.x_size-Slice_divider_right;
        *y_min = graphics->slice.y_split+1+Slice_divider_bottom;
        *y_max = graphics->window.y_size-Slice_divider_top;
        break;

    case Z_AXIS:
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

public  void  get_slice_view( graphics, axis_index, x_scale, y_scale,
                              x_pixel, y_pixel, x_pixel_end, y_pixel_end,
                              indices )
    graphics_struct  *graphics;
    int              axis_index;
    Real             *x_scale, *y_scale;
    int              *x_pixel, *y_pixel;
    int              *x_pixel_end, *y_pixel_end;
    int              indices[N_DIMENSIONS];
{
    int   x_axis_index, y_axis_index;
    int   x_offset, y_offset;
    int   x_size, y_size;
    int   x_min, x_max, y_min, y_max;
    void  get_slice_scale();
    void  get_slice_viewport();
    void  get_2d_slice_axes();

    indices[axis_index] = graphics->slice.slice_views[axis_index].slice_index;

    x_offset = graphics->slice.slice_views[axis_index].x_offset;
    y_offset = graphics->slice.slice_views[axis_index].y_offset;

    get_slice_scale( graphics, axis_index, x_scale, y_scale );

    get_2d_slice_axes( axis_index, &x_axis_index, &y_axis_index );

    x_size = graphics->slice.volume->size[x_axis_index];
    y_size = graphics->slice.volume->size[y_axis_index];

    get_slice_viewport( graphics, axis_index, &x_min, &x_max, &y_min, &y_max );

    *x_pixel = voxel_to_pixel( x_min, x_offset, *x_scale, 0 );
    indices[x_axis_index] = 0;

    *x_pixel_end = voxel_to_pixel( x_min, x_offset, *x_scale, x_size-1 );
    if( *x_pixel_end > x_max )
    {
        *x_pixel_end = x_max;
    }

    if( *x_pixel < x_min )
    {
        *x_pixel = x_min;
        indices[x_axis_index] = (int) (- (Real) x_offset / *x_scale );

        if( indices[x_axis_index] >= x_size )
        {
            *x_pixel = x_max + 1;
        }
    }

    *y_pixel = voxel_to_pixel( y_min, y_offset, *y_scale, 0 );
    indices[y_axis_index] = 0;

    *y_pixel_end = voxel_to_pixel( y_min, y_offset, *y_scale, y_size-1 );
    if( *y_pixel_end > y_max )
    {
        *y_pixel_end = y_max;
    }

    if( *y_pixel < y_min )
    {
        *y_pixel = y_min;
        indices[y_axis_index] = (int) (- (Real) y_offset / *y_scale );

        if( indices[y_axis_index] >= y_size )
        {
            *y_pixel = y_max + 1;
        }
    }
}

public   void     get_2d_slice_axes( axis_index, x_index, y_index )
    int   axis_index;
    int   *x_index;
    int   *y_index;
{
    switch( axis_index )
    {
    case X_AXIS:
        *x_index = Y_AXIS;
        *y_index = Z_AXIS;
        break;

    case Y_AXIS:
        *x_index = X_AXIS;
        *y_index = Z_AXIS;
        break;

    case Z_AXIS:
        *x_index = X_AXIS;
        *y_index = Y_AXIS;
        break;
    }
}

public  Boolean  get_current_voxel( graphics, x, y, z, axis_index )
    graphics_struct   *graphics;
    int               *x, *y, *z;
    int               *axis_index;
{
    void       get_mouse_in_pixels();
    int        x_mouse, y_mouse;
    Boolean    convert_pixel_to_voxel();

    get_mouse_in_pixels( graphics, &graphics->mouse_position,
                         &x_mouse, &y_mouse );

    return( convert_pixel_to_voxel( graphics, x_mouse, y_mouse, x, y, z,
                                    axis_index ) );
}

public  Boolean  get_voxel_activity( volume, voxel_activity, x, y, z )
    volume_struct   *volume;
    bitlist_struct  *voxel_activity;
    int             x, y, z;
{
    return( !get_bitlist_bit( voxel_activity,
                      ijk(x,y,z,volume->size[Y_AXIS],volume->size[Z_AXIS]) ) );
}

public  void  set_voxel_activity( volume, voxel_activity, x, y, z, value )
    volume_struct   *volume;
    bitlist_struct  *voxel_activity;
    int             x, y, z;
    Boolean         value;
{
    set_bitlist_bit( voxel_activity,
                     ijk(x,y,z,volume->size[Y_AXIS],volume->size[Z_AXIS]),
                     !value );
}

public  void  set_current_voxel( slice_window, x, y, z )
    graphics_struct   *slice_window;
    int               x, y, z;
{
    int               axis, indices[N_DIMENSIONS];
    Point             new_origin;
    void              get_voxel_centre();
    graphics_struct   *graphics;
    void              update_cursor();
    void              rebuild_slice_pixels();

    graphics = slice_window->associated[THREE_D_WINDOW];

    indices[X_AXIS] = x;
    indices[Y_AXIS] = y;
    indices[Z_AXIS] = z;

    get_voxel_centre( slice_window,
                      indices[X_AXIS], indices[Y_AXIS], indices[Z_AXIS],
                      &new_origin );

    if( !EQUAL_POINTS( new_origin, graphics->three_d.cursor.origin ) )
    {
        graphics->three_d.cursor.origin = new_origin;

        update_cursor( graphics );

        graphics->update_required = TRUE;
    }

    for_less( axis, 0, N_DIMENSIONS )
    {
        if( indices[axis] !=
               slice_window->slice.slice_views[axis].slice_index )
        {
            slice_window->slice.slice_views[axis].slice_index =
                                                         indices[axis];

            rebuild_slice_pixels( slice_window, axis );

            slice_window->update_required = TRUE;
        }
    }
}
