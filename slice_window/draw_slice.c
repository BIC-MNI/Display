
#include  <def_graphics.h>

public  void  draw_slice( graphics, axis_index, x_min, x_max, y_min, y_max )
    graphics_struct   *graphics;
    int               axis_index;
    int               x_min, x_max;
    int               y_min, y_max;
{
    slice_window_struct  *slice;
    int                  x_offset, y_offset;
    Real                 x_scale, y_scale;
    int                  x_index, y_index;
    int                  x_size, y_size;
    int                  x_left, x_right, y_bottom, y_top;
    int                  x_voxel, y_voxel, x_voxel_end, y_voxel_end;
    void                 draw_pixels();
    void                 compute_pixel_location();

    slice = &graphics->slice;

    x_offset = slice->slice_views[axis_index].x_offset;
    y_offset = slice->slice_views[axis_index].y_offset;
    x_scale = slice->slice_views[axis_index].x_scale;
    y_scale = slice->slice_views[axis_index].y_scale;

    x_index = (axis_index + 1) % N_DIMENSIONS;
    y_index = (axis_index + 2) % N_DIMENSIONS;

    x_size = slice->volume->size[x_index];
    y_size = slice->volume->size[y_index];

    compute_pixel_location( 0, x_min, x_max, x_offset, x_scale,
                            &x_left, &x_voxel );
    compute_pixel_location( x_size-1, x_min, x_max, x_offset, x_scale,
                            &x_right, &x_voxel_end );
    compute_pixel_location( 0, y_min, y_max, y_offset, y_scale,
                            &y_bottom, &y_voxel );
    compute_pixel_location( y_size-1, y_min, y_max, y_offset, y_scale,
                            &y_top, &y_voxel_end );

    draw_pixels( graphics, axis_index,
                 slice->slice_views[axis_index].slice_index, slice->volume,
                 x_left, x_right, y_bottom, y_top, x_voxel, y_voxel,
                 x_scale, y_scale );
}

private  void  compute_pixel_location( x_voxel, x_min, x_max, x_offset,
                                          x_scale, pixel_position,
                                          voxel_position )
    int   x_voxel;
    int   x_min, x_max;
    int   x_offset;
    Real  x_scale;
    int   *pixel_position;
    int   *voxel_position;
{
    *pixel_position = x_min + x_offset + x_voxel;

    if( *pixel_position < x_min )
    {
        *pixel_position = x_min;
        *voxel_position = -x_offset;
    }
    else if( *pixel_position > x_max )
    {
        *pixel_position = x_max;
        *voxel_position = x_max - x_min - x_offset;
    }
}

#define  BLOCK_SIZE  256

private  void  draw_pixels( graphics, axis_index, slice_index, volume,
                            x_left, x_right, y_bottom, y_top, x_voxel, y_voxel,
                            x_scale, y_scale )
    graphics_struct  *graphics;
    int              axis_index;
    int              slice_index;
    volume_struct    *volume;
    int              x_left, x_right, y_bottom, y_top;
    int              x_voxel, y_voxel;
    Real             x_scale, y_scale;
{
    int             indices[N_DIMENSIONS];
    int             x_index, y_index, x, y;
    int             x_pixel, y_pixel, x_end_pixel, y_end_pixel, x_size;
    Pixel_colour    pixel_col;
    Pixel_colour    pixels[BLOCK_SIZE*BLOCK_SIZE];
    void            G_write_pixels();
    void            get_voxel_colour();

    x_index = (axis_index + 1) % N_DIMENSIONS;
    y_index = (axis_index + 2) % N_DIMENSIONS;

    indices[axis_index] = slice_index;
    indices[x_index] = x_voxel;
    indices[y_index] = y_voxel;

    for( x_pixel = x_left;  x_pixel <= x_right;  x_pixel += BLOCK_SIZE )
    {
        x_end_pixel = MIN( x_pixel + BLOCK_SIZE - 1, x_right );
        x_size = x_end_pixel - x_pixel + 1;
        for( y_pixel = y_bottom;  y_pixel <= y_top;  y_pixel += BLOCK_SIZE )
        {
            y_end_pixel = MIN( y_pixel + BLOCK_SIZE - 1, y_top );

            indices[x_index] = x_voxel + x_pixel - x_left;

            for_inclusive( x, x_pixel, x_end_pixel )
            {
                indices[y_index] = y_voxel + y_pixel - y_bottom;
                for_inclusive( y, y_pixel, y_end_pixel )
                {
                    get_voxel_colour( volume, indices[0], indices[1],
                                      indices[2], &pixel_col );

                    ACCESS_PIXEL( pixels, x - x_pixel, y - y_pixel,
                                  x_size ) = pixel_col;

                    ++indices[y_index];
                }
                ++indices[x_index];
            }

            G_write_pixels( &graphics->window,
                            x_left, y_bottom, x_right, y_top, pixels );
        }
    }
}

private  void  get_voxel_colour( volume, x, y, z, pixel_col )
    volume_struct   *volume;
    int             x, y, z;
    Pixel_colour    *pixel_col;
{
    Real   val, r, g, b;

    val = ACCESS_VOLUME_DATA( *volume, x, y, z );

    r = (val - volume->min_value) / (volume->max_value - volume->min_value);
    g = r;
    b = r;

    RGB_TO_PIXEL( r, g, b, *pixel_col );
}
