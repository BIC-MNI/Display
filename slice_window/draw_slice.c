
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_stdio.h>
#include  <def_alloc.h>

#define  DIVIDER_INDEX      0
#define  SLICE1_INDEX       1
#define  SLICE2_INDEX       2
#define  SLICE3_INDEX       3
#define  CURSOR1_INDEX      4
#define  CURSOR2_INDEX      5
#define  CURSOR3_INDEX      6
#define  TEXT1_INDEX        7
#define  TEXT2_INDEX        8
#define  TEXT3_INDEX        9
#define  X_PROBE_INDEX     10
#define  Y_PROBE_INDEX     11
#define  Z_PROBE_INDEX     12
#define  VAL_PROBE_INDEX   13

public  Status  initialize_slice_models( graphics )
    graphics_struct   *graphics;
{
    Status         status;
    int            i;
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

    for_inclusive( i, CURSOR1_INDEX, CURSOR3_INDEX )
    {
        status = create_lines_object( &object, &Slice_cursor_colour,
                                      8, 4, 8 );

        lines = object->ptr.lines;
        lines->end_indices[0] = 2;
        lines->end_indices[1] = 4;
        lines->end_indices[2] = 6;
        lines->end_indices[3] = 8;
        lines->indices[0] = 0;
        lines->indices[1] = 1;
        lines->indices[2] = 2;
        lines->indices[3] = 3;
        lines->indices[4] = 4;
        lines->indices[5] = 5;
        lines->indices[6] = 6;
        lines->indices[7] = 7;

        if( status == OK )
        {
            status = add_object_to_model( model, object );
        }
    }

    for_inclusive( i, X_PROBE_INDEX, VAL_PROBE_INDEX )
    {
        status = create_object( &object, TEXT );
        if( status == OK )
        {
            object->ptr.text->colour = Slice_text_colour;
            status = add_object_to_model( model, object );
        }
    }

    return( status );
}

public  void  rebuild_slice_models( graphics )
    graphics_struct   *graphics;
{
    void  rebuild_slice_divider();
    void  rebuild_probe();
    void  set_slice_window_update();

    rebuild_slice_divider( graphics );

    rebuild_probe( graphics );

    set_slice_window_update( graphics, X_AXIS );
    set_slice_window_update( graphics, Y_AXIS );
    set_slice_window_update( graphics, Z_AXIS );
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

public  void  rebuild_probe( graphics )
    graphics_struct   *graphics;
{
    model_struct   *model;
    model_struct   *get_graphics_model();
    Boolean        active;
    int            i, x_voxel, y_voxel, z_voxel, view_index;
    text_struct    *text;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;
    void           get_slice_viewport();
    Boolean        get_voxel_in_slice_window();

    active = get_voxel_in_slice_window( graphics, &x_voxel, &y_voxel, &z_voxel,
                                        &view_index );

    model = get_graphics_model(graphics,SLICE_MODEL);

    get_slice_viewport( graphics, -1, &x_min, &x_max, &y_min, &y_max );

    for_inclusive( i, X_PROBE_INDEX, VAL_PROBE_INDEX )
    {
        x_pos = x_min + Probe_x_pos + (i - X_PROBE_INDEX) * Probe_x_delta;
        y_pos = y_max - Probe_y_pos - (i - X_PROBE_INDEX) * Probe_y_delta;

        text = model->object_list[i]->ptr.text;

        if( active )
        {
            switch( i )
            {
            case X_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_x_format, x_voxel );
                break;
            case Y_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_y_format, y_voxel );
                break;
            case Z_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_z_format, z_voxel );
                break;
            case VAL_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_val_format,
                        (Real) ACCESS_VOLUME_DATA( *graphics->slice.volume,
                                                   x_voxel, y_voxel, z_voxel) );
                break;
            }
        }
        else
        {
            text->text[0] = (char) 0;
        }

        fill_Point( text->origin, x_pos, y_pos, 0.0 );
    }
}

public  void  rebuild_slice_pixels( graphics, view_index )
    graphics_struct   *graphics;
    int               view_index;
{
    Status         status;
    model_struct   *model;
    model_struct   *get_graphics_model();
    pixels_struct  *pixels;
    int            axis_index;
    int            voxel_indices[N_DIMENSIONS];
    int            x_pixel_start, x_pixel_end;
    int            y_size, y_pixel_start, y_pixel_end;
    void           get_slice_view();
    void           render_slice_to_pixels();
    text_struct    *text;
    char           *format;
    Real           x_scale, y_scale;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;
    void           get_slice_viewport();
    void           rebuild_cursor();

    model = get_graphics_model(graphics,SLICE_MODEL);

    axis_index = graphics->slice.slice_views[view_index].axis_index;

    pixels = model->object_list[SLICE1_INDEX+view_index]->ptr.pixels;

    get_slice_view( graphics, view_index, &x_scale, &y_scale,
                    &x_pixel_start, &y_pixel_start,
                    &x_pixel_end, &y_pixel_end, voxel_indices );

    y_size = y_pixel_end - y_pixel_start + 1;

    if( y_size > graphics->slice.temporary_indices_alloced )
    {
        CHECK_ALLOC1( status, graphics->slice.temporary_indices,
                      graphics->slice.temporary_indices_alloced,
                      y_size, int, DEFAULT_CHUNK_SIZE );

        if( status == OK )
        {
            graphics->slice.temporary_indices_alloced = y_size;
        }
        else
        {
            graphics->slice.temporary_indices_alloced = 0;
        }
    }

    render_slice_to_pixels( graphics->slice.temporary_indices,
                   pixels, axis_index, graphics->slice.volume,
                   &graphics->slice.colour_coding,
                   voxel_indices,
                   x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end,
                   x_scale, y_scale );

    text = model->object_list[TEXT1_INDEX+view_index]->ptr.text;

    switch( axis_index )
    {
    case X_AXIS:  format = Slice_index_x_format;  break;
    case Y_AXIS:  format = Slice_index_y_format;  break;
    case Z_AXIS:  format = Slice_index_z_format;  break;
    }

    (void) sprintf( text->text, format,
                    graphics->slice.slice_index[axis_index] );

    get_slice_viewport( graphics, view_index, &x_min, &x_max, &y_min, &y_max );

    x_pos = x_min + (int) Point_x(Slice_index_offset);
    y_pos = y_min + (int) Point_y(Slice_index_offset);

    fill_Point( text->origin, x_pos, y_pos, 0.0 );

    rebuild_cursor( graphics, X_AXIS );
    rebuild_cursor( graphics, Y_AXIS );
    rebuild_cursor( graphics, Z_AXIS );
}

public  void  rebuild_cursor( graphics, view_index )
    graphics_struct   *graphics;
    int               view_index;
{
    model_struct   *model;
    model_struct   *get_graphics_model();
    int            x_index, y_index, x_start, y_start, x_end, y_end;
    int            x_centre, y_centre, dx, dy;
    int            axis_index;
    lines_struct   *lines;
    int            x, y;
    void           get_2d_slice_axes();
    void           convert_voxel_to_pixel();
    int            x_min, x_max, y_min, y_max;
    void           get_slice_viewport();

    model = get_graphics_model(graphics,SLICE_MODEL);

    axis_index = graphics->slice.slice_views[view_index].axis_index;

    lines = model->object_list[CURSOR1_INDEX+view_index]->ptr.lines;

    get_2d_slice_axes( axis_index, &x_index, &y_index );

    x = graphics->slice.slice_index[x_index];
    y = graphics->slice.slice_index[y_index];

    convert_voxel_to_pixel( graphics, view_index, x, y, &x_start, &y_start );
    convert_voxel_to_pixel( graphics, view_index, x+1, y+1, &x_end, &y_end );

    --x_end;
    --y_end;

    x_centre = (x_start + x_end) / 2;
    y_centre = (y_start + y_end) / 2;

    get_slice_viewport( graphics, view_index, &x_min, &x_max, &y_min, &y_max );

    if( x_centre < x_min )
    {
        dx = x_min - x_centre;
        x_centre = x_min;
        x_start += dx;
        x_end += dx;
    }
    else if( x_centre > x_max )
    {
        dx = x_max - x_centre;
        x_centre = x_max;
        x_start += dx;
        x_end += dx;
    }

    if( y_centre < y_min )
    {
        dy = y_min - y_centre;
        y_centre = y_min;
        y_start += dy;
        y_end += dy;
    }
    else if( y_centre > y_max )
    {
        dy = y_max - y_centre;
        y_centre = y_max;
        y_start += dy;
        y_end += dy;
    }

    fill_Point( lines->points[0], x_centre, y_end + Cursor_start_pixel, 0.0 );
    fill_Point( lines->points[1], x_centre, y_end + Cursor_end_pixel, 0.0 );

    fill_Point( lines->points[2], x_centre, y_start - Cursor_start_pixel, 0.0 );
    fill_Point( lines->points[3], x_centre, y_start - Cursor_end_pixel, 0.0 );

    fill_Point( lines->points[4], x_start - Cursor_start_pixel, y_centre, 0.0 );
    fill_Point( lines->points[5], x_start - Cursor_end_pixel, y_centre, 0.0 );

    fill_Point( lines->points[6], x_end + Cursor_start_pixel, y_centre, 0.0 );
    fill_Point( lines->points[7], x_end + Cursor_end_pixel, y_centre, 0.0 );
}

private  void  render_slice_to_pixels( temporary_indices, pixels, axis_index,
                                       volume, colour_coding, start_indices,
                                       x_left, x_right, y_bottom, y_top,
                                       x_scale, y_scale )
    int                   temporary_indices[];
    pixels_struct         *pixels;
    int                   axis_index;
    volume_struct         *volume;
    colour_coding_struct  *colour_coding;
    int                   start_indices[N_DIMENSIONS];
    int                   x_left, x_right, y_bottom, y_top;
    Real                  x_scale, y_scale;
{
    Status          status;
    int             new_size, old_size, indices[N_DIMENSIONS];
    int             x_index, y_index, x, y, prev_y;
    int             x_size, y_size;
    Pixel_colour    pixel_col;
    Real            dx, dy;
    Pixel_colour    get_voxel_colour();
    void            get_2d_slice_axes();

    status = OK;

    if( pixels->x_min <= pixels->x_max && pixels->y_min <= pixels->y_max )
    {
        old_size = (pixels->x_max - pixels->x_min + 1) *
                   (pixels->y_max - pixels->y_min + 1);
    }
    else
    {
        old_size = 0;
    }

    pixels->x_min = x_left;
    pixels->x_max = x_right;
    pixels->y_min = y_bottom;
    pixels->y_max = y_top;

    x_size = x_right - x_left + 1;
    y_size = y_top - y_bottom + 1;

    if( x_size <= 0 || y_size <= 0 )
    {
        new_size = 0;
    }
    else
    {
        new_size = x_size * y_size;
    }

    dx = 1.0 / x_scale;
    dy = 1.0 / y_scale;

    if( status == OK )
    {
        if( new_size > 0 )
        {
            CHECK_ALLOC1( status, pixels->pixels, old_size, new_size,
                          Pixel_colour, DEFAULT_CHUNK_SIZE );
        }
        else if( old_size > 0 )
        {
            FREE1( status, pixels->pixels );
        }
    }

    get_2d_slice_axes( axis_index, &x_index, &y_index );

    indices[axis_index] = start_indices[axis_index];

    for_less( y, 0, y_size )
    {
        temporary_indices[y] = start_indices[y_index] + y * dy;
    }

    for_less( x, 0, x_size )
    {
        indices[x_index] = start_indices[x_index] + x * dx;

        prev_y = -1;

        for_less( y, 0, y_size )
        {
            indices[y_index] = temporary_indices[y];

            if( indices[y_index] != prev_y )
            {
                pixel_col = get_voxel_colour( volume, colour_coding,
                                              indices[0], indices[1],
                                              indices[2] );

                prev_y = indices[y_index];
            }

            ACCESS_PIXEL( pixels->pixels, x, y, x_size ) = pixel_col;
        }
    }
}

private  Pixel_colour  get_voxel_colour( volume, colour_coding, x, y, z )
    volume_struct         *volume;
    colour_coding_struct  *colour_coding;
    int                   x, y, z;
{
    Pixel_colour    pixel_col;
    Pixel_colour    get_colour_coding();
    int             val;
    Boolean         activity_flag, inactivity_flag;

    if( Display_activities &&
        (!(activity_flag=get_voxel_activity_flag( volume, x, y, z )) ||
         (inactivity_flag=get_voxel_inactivity_flag( volume, x, y, z )) ) )
    {
        if( activity_flag )
        {
            COLOUR_TO_PIXEL( Inactive_voxel_colour, pixel_col );
        }
        else if( inactivity_flag )
        {
            COLOUR_TO_PIXEL( Inactive_and_not_active_voxel_colour, pixel_col );
        }
        else
        {
            COLOUR_TO_PIXEL( Not_active_voxel_colour, pixel_col );
        }
    }
    else
    {
        val = ACCESS_VOLUME_DATA( *volume, x, y, z );

        pixel_col = get_colour_coding( colour_coding, (Real) val );
    }

    return( pixel_col );
}
