
#include  <def_graphics.h>
#include  <def_globals.h>

#define  DIVIDER_INDEX      0
#define  X_SLICE_INDEX      1
#define  Y_SLICE_INDEX      2
#define  Z_SLICE_INDEX      3
#define  X_TEXT_INDEX       4
#define  Y_TEXT_INDEX       5
#define  Z_TEXT_INDEX       6
#define  X_CURSOR_INDEX     7
#define  Y_CURSOR_INDEX     8
#define  Z_CURSOR_INDEX     9
#define  VAL_CURSOR_INDEX  10

public  Status  initialize_slice_models( graphics )
    graphics_struct   *graphics;
{
    Status         status;
    Status         create_object();
    Status         create_lines_object();
    lines_struct   *lines;
    object_struct  *object;
    model_struct   *model;
    model_struct   *get_graphics_model();
    Status         add_object_to_model();
    void           rebuild_slice_models();

    model = get_graphics_model( graphics, SLICE_MODEL );

    status = create_lines_object( &object, &Slice_divider_colour,
                                  4, 2, 4 );

    lines = object->ptr.lines;
    lines->end_indices[0] = 2;
    lines->end_indices[1] = 4;
    lines->indices[0] = 0;
    lines->indices[1] = 1;
    lines->indices[2] = 2;
    lines->indices[3] = 3;

    status = add_object_to_model( model, object );

    status = create_object( &object, PIXELS );
    status = add_object_to_model( model, object );

    status = create_object( &object, PIXELS );
    status = add_object_to_model( model, object );

    status = create_object( &object, PIXELS );
    status = add_object_to_model( model, object );

    return( status );
}

public  void  rebuild_slice_models( graphics )
    graphics_struct   *graphics;
{
    void  rebuild_slice_divider();
    void  rebuild_slice_pixels();

    rebuild_slice_divider( graphics );

    rebuild_slice_pixels( graphics, X_AXIS );
    rebuild_slice_pixels( graphics, Y_AXIS );
    rebuild_slice_pixels( graphics, Z_AXIS );
}

public  void  rebuild_slice_divider( graphics )
    graphics_struct   *graphics;
{
    model_struct   *model;
    model_struct   *get_graphics_model();
    Point          *points;

    model = get_graphics_model(graphics,SLICE_MODEL);
    points = model->object_list[DIVIDER_INDEX]->ptr.lines->points;

    fill_Point( points[0], (Real) graphics->slice.x_split, 0.0, 0.0 );
    fill_Point( points[1], (Real) graphics->slice.x_split,
                           (Real) (graphics->window.y_size-1), 0.0 );
    fill_Point( points[2], 0.0, (Real) graphics->slice.y_split, 0.0 );
    fill_Point( points[3], (Real) (graphics->window.x_size-1),
                           (Real) graphics->slice.y_split, 0.0 );
}

public  void  rebuild_slice_pixels( graphics, axis_index )
    graphics_struct   *graphics;
    int               axis_index;
{
    model_struct   *model;
    model_struct   *get_graphics_model();
    pixels_struct  *pixels;
    int            voxel_indices[N_DIMENSIONS];
    int            x_pixel_start, x_pixel_end;
    int            y_pixel_start, y_pixel_end;
    void           get_slice_view();
    void           render_slice_to_pixels();

    model = get_graphics_model(graphics,SLICE_MODEL);

    pixels = model->object_list[X_SLICE_INDEX+axis_index]->ptr.pixels;

    get_slice_view( graphics, axis_index,
                    &x_pixel_start, &y_pixel_start,
                    &x_pixel_end, &y_pixel_end, voxel_indices );

    render_slice_to_pixels( pixels, axis_index, graphics->slice.volume,
                   voxel_indices,
                   x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end,
                   graphics->slice.slice_views[axis_index].x_scale,
                   graphics->slice.slice_views[axis_index].y_scale );
}

private  void  render_slice_to_pixels( pixels, axis_index, volume,
                              start_indices,
                              x_left, x_right, y_bottom, y_top,
                              x_scale, y_scale )
    pixels_struct    *pixels;
    int              axis_index;
    volume_struct    *volume;
    int              start_indices[N_DIMENSIONS];
    int              x_left, x_right, y_bottom, y_top;
    Real             x_scale, y_scale;
{
    Status          status;
    int             indices[N_DIMENSIONS];
    int             x_index, y_index, x, y;
    int             x_size, y_size;
    Pixel_colour    pixel_col;
    void            get_voxel_colour();
    void            get_2d_slice_axes();

    status = OK;

    if( pixels->x_min <= pixels->x_max && pixels->y_min <= pixels->y_max )
    {
        FREE1( status, pixels->pixels );
    }

    pixels->x_min = x_left;
    pixels->x_max = x_right;
    pixels->y_min = y_bottom;
    pixels->y_max = y_top;

    x_size = x_right - x_left + 1;
    y_size = y_top - y_bottom + 1;

    if( status == OK )
    {
        if( x_size > 0 && y_size > 0 )
        {
            CALLOC1( status, pixels->pixels, x_size * y_size, Pixel_colour );
        }
    }

    get_2d_slice_axes( axis_index, &x_index, &y_index );

    indices[axis_index] = start_indices[axis_index];

    for_inclusive( x, x_left, x_right )
    {
        indices[x_index] = start_indices[x_index] + x_scale * (x - x_left);

        for_inclusive( y, y_bottom, y_top )
        {
            indices[y_index] = start_indices[y_index] +
                               y_scale * (y - y_bottom);

            get_voxel_colour( volume, indices[0], indices[1],
                              indices[2], &pixel_col );

            ACCESS_PIXEL( pixels->pixels, x - x_left, y - y_bottom, x_size ) =
                 pixel_col;
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
