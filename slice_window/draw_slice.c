
#include  <def_graphics.h>

public  void  draw_slice( graphics, axis_index, x_pixel_start, y_pixel_start,
                          x_pixel_end, y_pixel_end, voxel_indices )
    graphics_struct   *graphics;
    int               axis_index;
    int               x_pixel_start, y_pixel_start;
    int               x_pixel_end, y_pixel_end;
    int               voxel_indices[N_DIMENSIONS];
{
    slice_window_struct  *slice;
    void                 draw_pixels();

    slice = &graphics->slice;

    draw_pixels( graphics, axis_index, slice->volume, voxel_indices,
                 x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end,
                 graphics->slice.slice_views[axis_index].x_scale,
                 graphics->slice.slice_views[axis_index].y_scale );
}

#define  BLOCK_SIZE  256

private  void  draw_pixels( graphics, axis_index, volume, start_indices,
                            x_left, x_right, y_bottom, y_top, x_scale, y_scale )
    graphics_struct  *graphics;
    int              axis_index;
    volume_struct    *volume;
    int              start_indices[N_DIMENSIONS];
    int              x_left, x_right, y_bottom, y_top;
    Real             x_scale, y_scale;
{
    int             indices[N_DIMENSIONS];
    int             x_index, y_index, x, y;
    int             x_pixel, y_pixel, x_end_pixel, y_end_pixel, x_size;
    Pixel_colour    pixel_col;
    Pixel_colour    pixels[BLOCK_SIZE*BLOCK_SIZE];
    void            G_write_pixels();
    void            get_voxel_colour();
    void            get_2d_slice_axes();

    get_2d_slice_axes( axis_index, &x_index, &y_index );

    indices[axis_index] = start_indices[axis_index];

    for( x_pixel = x_left;  x_pixel <= x_right;  x_pixel += BLOCK_SIZE )
    {
        x_end_pixel = MIN( x_pixel + BLOCK_SIZE - 1, x_right );
        x_size = x_end_pixel - x_pixel + 1;
        for( y_pixel = y_bottom;  y_pixel <= y_top;  y_pixel += BLOCK_SIZE )
        {
            y_end_pixel = MIN( y_pixel + BLOCK_SIZE - 1, y_top );

            for_inclusive( x, x_pixel, x_end_pixel )
            {
                indices[x_index] = start_indices[x_index] +
                                   x_scale * (x - x_left);
                for_inclusive( y, y_pixel, y_end_pixel )
                {
                    indices[y_index] = start_indices[y_index] +
                                       y_scale * (y - y_bottom);
                    get_voxel_colour( volume, indices[0], indices[1],
                                      indices[2], &pixel_col );

                    ACCESS_PIXEL( pixels, x - x_pixel, y - y_pixel,
                                  x_size ) = pixel_col;
                }
            }

            G_write_pixels( &graphics->window,
                            x_pixel, y_pixel, x_end_pixel, y_end_pixel,
                            pixels );
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
