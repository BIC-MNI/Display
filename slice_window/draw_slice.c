
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_files.h>
#include  <def_arrays.h>

#define  DIVIDER_INDEX                  0
#define  SLICE1_INDEX                   1
#define  SLICE2_INDEX                   2
#define  SLICE3_INDEX                   3
#define  CURSOR1_INDEX                  4
#define  CURSOR2_INDEX                  5
#define  CURSOR3_INDEX                  6
#define  TEXT1_INDEX                    7
#define  TEXT2_INDEX                    8
#define  TEXT3_INDEX                    9
#define  X_TRANSFORMED_PROBE_INDEX     10
#define  Y_TRANSFORMED_PROBE_INDEX     11
#define  Z_TRANSFORMED_PROBE_INDEX     12
#define  X_TALAIRACH_PROBE_INDEX       13
#define  Y_TALAIRACH_PROBE_INDEX       14
#define  Z_TALAIRACH_PROBE_INDEX       15
#define  VAL_PROBE_INDEX               16

static    void           render_slice_to_pixels();

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

    for_inclusive( i, TEXT1_INDEX, TEXT3_INDEX )
    {
        status = create_object( &object, TEXT );
        if( status == OK )
        {
            object->ptr.text->font = (Font_types) Slice_text_font;
            object->ptr.text->size = Slice_text_font_size;
            object->ptr.text->colour = Slice_text_colour;
            status = add_object_to_model( model, object );
        }
    }

    for_inclusive( i, X_TRANSFORMED_PROBE_INDEX, VAL_PROBE_INDEX )
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
    void  rebuild_colour_bar();

    rebuild_slice_divider( graphics );
    rebuild_probe( graphics );
    rebuild_colour_bar( graphics );

    set_slice_window_update( graphics, 0 );
    set_slice_window_update( graphics, 1 );
    set_slice_window_update( graphics, 2 );
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
    volume_struct  *volume;
    int            i, x_voxel, y_voxel, z_voxel, view_index;
    Real           x_tal_voxel, y_tal_voxel, z_tal_voxel;
    Real           x_talairach, y_talairach, z_talairach;
    text_struct    *text;
    int            nx, ny, nz;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;
    void           get_slice_viewport();
    Boolean        get_voxel_in_slice_window();
    void           convert_voxel_to_talairach();
    void           convert_talairach_to_mm();
    void           get_volume_size();

    active = get_voxel_in_slice_window( graphics, &x_voxel, &y_voxel, &z_voxel,
                                        &view_index );

    model = get_graphics_model(graphics,SLICE_MODEL);

    get_slice_viewport( graphics, -1, &x_min, &x_max, &y_min, &y_max );

    if( get_slice_window_volume( graphics,  &volume ) )
        get_volume_size( volume, &nx, &ny, &nz );

    convert_voxel_to_talairach( (Real) x_voxel, (Real) y_voxel, (Real) z_voxel,
                                nx, ny, nz,
                                &x_tal_voxel, &y_tal_voxel, &z_tal_voxel );

    convert_talairach_to_mm( x_tal_voxel, y_tal_voxel, z_tal_voxel,
                             &x_talairach, &y_talairach, &z_talairach );

    for_inclusive( i, X_TRANSFORMED_PROBE_INDEX, VAL_PROBE_INDEX )
    {
        x_pos = x_min + Probe_x_pos + (i - X_TRANSFORMED_PROBE_INDEX)
                                       * Probe_x_delta;
        y_pos = y_max - Probe_y_pos - (i - X_TRANSFORMED_PROBE_INDEX)
                                       * Probe_y_delta;

        text = model->object_list[i]->ptr.text;

        if( active )
        {
            switch( i )
            {
            case X_TRANSFORMED_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_x_transformed_format,
                                x_voxel+1 );
                break;
            case Y_TRANSFORMED_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_y_transformed_format,
                                y_voxel+1 );
                break;
            case Z_TRANSFORMED_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_z_transformed_format,
                                z_voxel+1 );
                break;

            case X_TALAIRACH_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_x_talairach_format,
                                x_talairach );
                break;
            case Y_TALAIRACH_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_y_talairach_format,
                                y_talairach );
                break;
            case Z_TALAIRACH_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_z_talairach_format,
                                z_talairach );
                break;
            case VAL_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_val_format,
                        (Real) GET_VOLUME_DATA( *graphics->slice.volume,
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
    int            axis_index, x_index, y_index;
    int            voxel_indices[N_DIMENSIONS];
    int            x_pixel_start, x_pixel_end;
    int            x_size, y_pixel_start, y_pixel_end;
    void           get_slice_view();
    text_struct    *text;
    char           *format;
    Real           x_scale, y_scale;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;
    void           get_slice_viewport();
    void           rebuild_cursor();
    Boolean        print_cursor;
    Real           real_pos[N_DIMENSIONS];

    model = get_graphics_model(graphics,SLICE_MODEL);

    axis_index = graphics->slice.slice_views[view_index].axis_map[Z_AXIS];
    x_index = graphics->slice.slice_views[view_index].axis_map[X_AXIS];
    y_index = graphics->slice.slice_views[view_index].axis_map[Y_AXIS];

    pixels = model->object_list[SLICE1_INDEX+view_index]->ptr.pixels;

    get_slice_view( graphics, view_index, &x_scale, &y_scale,
                    &x_pixel_start, &y_pixel_start,
                    &x_pixel_end, &y_pixel_end, voxel_indices );

    x_size = x_pixel_end - x_pixel_start + 1;

    if( x_size > graphics->slice.temporary_indices_alloced )
    {
        SET_ARRAY_SIZE( status, graphics->slice.temporary_indices,
                        graphics->slice.temporary_indices_alloced,
                        x_size, DEFAULT_CHUNK_SIZE );

        if( status == OK )
        {
            graphics->slice.temporary_indices_alloced = x_size;
        }
        else
        {
            graphics->slice.temporary_indices_alloced = 0;
        }
    }

    render_slice_to_pixels( graphics->slice.temporary_indices,
                   pixels, axis_index, x_index, y_index, graphics->slice.volume,
                   graphics->slice.fast_lookup_present,
                   graphics->slice.fast_lookup,
                   &graphics->slice.colour_coding,
                   voxel_indices,
                   x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end,
                   x_scale, y_scale );

    print_cursor = get_voxel_corresponding_to_point( graphics,
            &graphics->associated[THREE_D_WINDOW]->three_d.cursor.origin,
            &real_pos[X_AXIS], &real_pos[Y_AXIS], &real_pos[Z_AXIS] );

    if( print_cursor &&
        real_pos[axis_index] == (Real) ((int) real_pos[axis_index]) )
    {
            print_cursor = FALSE;
    }

    text = model->object_list[TEXT1_INDEX+view_index]->ptr.text;

    if( print_cursor )
    {
        switch( axis_index )
        {
        case X_AXIS:  format = Slice_index_xc_format;  break;
        case Y_AXIS:  format = Slice_index_yc_format;  break;
        case Z_AXIS:  format = Slice_index_zc_format;  break;
        }

        (void) sprintf( text->text, format,
                        graphics->slice.slice_index[axis_index] + 1,
                        real_pos[axis_index] + 1.0 );

    }
    else
    {
        if( graphics->slice.slice_locked[axis_index] )
        {
            switch( axis_index )
            {
            case X_AXIS:  format = Slice_index_x_locked_format;  break;
            case Y_AXIS:  format = Slice_index_y_locked_format;  break;
            case Z_AXIS:  format = Slice_index_z_locked_format;  break;
            }
        }
        else
        {
            switch( axis_index )
            {
            case X_AXIS:  format = Slice_index_x_format;  break;
            case Y_AXIS:  format = Slice_index_y_format;  break;
            case Z_AXIS:  format = Slice_index_z_format;  break;
            }
        }

        (void) sprintf( text->text, format,
                        graphics->slice.slice_index[axis_index] + 1 );

    }

    get_slice_viewport( graphics, view_index, &x_min, &x_max, &y_min, &y_max );

    x_pos = x_min + (int) Point_x(Slice_index_offset);
    y_pos = y_min + (int) Point_y(Slice_index_offset);

    fill_Point( text->origin, x_pos, y_pos, 0.0 );

    rebuild_cursor( graphics, 0 );
    rebuild_cursor( graphics, 1 );
    rebuild_cursor( graphics, 2 );
}

public  void  rebuild_cursor( graphics, view_index )
    graphics_struct   *graphics;
    int               view_index;
{
    model_struct   *model;
    model_struct   *get_graphics_model();
    int            x_index, y_index, x_start, y_start, x_end, y_end;
    int            x_centre, y_centre, dx, dy;
    Real           start_pixel[N_DIMENSIONS], end_pixel[N_DIMENSIONS];
    lines_struct   *lines;
    int            x, y;
    void           convert_voxel_to_pixel();
    int            x_min, x_max, y_min, y_max;
    void           get_slice_viewport();

    model = get_graphics_model(graphics,SLICE_MODEL);

    x_index = graphics->slice.slice_views[view_index].axis_map[X_AXIS];
    y_index = graphics->slice.slice_views[view_index].axis_map[Y_AXIS];

    lines = model->object_list[CURSOR1_INDEX+view_index]->ptr.lines;

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

    start_pixel[X_AXIS] = Cursor_start_pixel_x;
    start_pixel[Y_AXIS] = Cursor_start_pixel_y;
    start_pixel[Z_AXIS] = Cursor_start_pixel_z;

    end_pixel[X_AXIS] = Cursor_end_pixel_x;
    end_pixel[Y_AXIS] = Cursor_end_pixel_y;
    end_pixel[Z_AXIS] = Cursor_end_pixel_z;

    fill_Point( lines->points[0], x_centre, y_end + start_pixel[y_index], 0.0 );
    fill_Point( lines->points[1], x_centre, y_end + end_pixel[y_index], 0.0 );

    fill_Point( lines->points[2], x_centre, y_start-start_pixel[y_index], 0.0 );
    fill_Point( lines->points[3], x_centre, y_start-end_pixel[y_index], 0.0 );

    fill_Point( lines->points[4], x_start-start_pixel[x_index], y_centre, 0.0 );
    fill_Point( lines->points[5], x_start-end_pixel[x_index], y_centre, 0.0 );

    fill_Point( lines->points[6], x_end + start_pixel[x_index], y_centre, 0.0 );
    fill_Point( lines->points[7], x_end + end_pixel[x_index], y_centre, 0.0 );
}

private  void  render_slice_to_pixels( temporary_indices, pixels, axis_index,
                                       x_index, y_index, volume,
                                       fast_lookup_present, fast_lookup,
                                       colour_coding, start_indices,
                                       x_left, x_right, y_bottom, y_top,
                                       x_scale, y_scale )
    int                   temporary_indices[];
    pixels_struct         *pixels;
    int                   axis_index, x_index, y_index;
    volume_struct         *volume;
    Boolean               fast_lookup_present;
    Pixel_colour          *fast_lookup[];
    colour_coding_struct  *colour_coding;
    int                   start_indices[N_DIMENSIONS];
    int                   x_left, x_right, y_bottom, y_top;
    Real                  x_scale, y_scale;
{
    Status          status;
    int             new_size, old_size, indices[N_DIMENSIONS];
    int             x, y, prev_x;
    int             x_size, y_size;
    int             val, min_value;
    int             lookup_index;
    Colour          col;
    Pixel_colour    pixel_col, *pixel_ptr;
    Real            dx, dy;
    Pixel_colour    get_voxel_colour();
    void            get_colour_coding();
    Boolean         activity_flag, label_flag;

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
        SET_ARRAY_SIZE( status, pixels->pixels, old_size, new_size,
                        DEFAULT_CHUNK_SIZE );
    }

    indices[axis_index] = start_indices[axis_index];

    for_less( x, 0, x_size )
    {
        temporary_indices[x] = start_indices[x_index] + x * dx;
    }

    min_value = volume->min_value;

    for_less( y, 0, y_size )
    {
        pixel_ptr = &pixels->pixels[y * x_size];

        indices[y_index] = start_indices[y_index] + y * dy;

        prev_x = -1;

        for_less( x, 0, x_size )
        {
            indices[x_index] = temporary_indices[x];

            if( indices[x_index] != prev_x )
            {
                if( Display_activities )
                {
                    activity_flag = get_voxel_activity_flag( volume,
                                      indices[0], indices[1], indices[2] );
                    label_flag = get_voxel_label_flag( volume,
                                      indices[0], indices[1], indices[2] );

                    if( activity_flag )
                    {
                        if( label_flag )
                            lookup_index = 2;
                        else
                            lookup_index = 0;
                    }
                    else
                    {
                        if( label_flag )
                            lookup_index = 3;
                        else
                            lookup_index = 1;
                    }
                }
                else
                {
                    lookup_index = 0;
                }

                val = GET_VOLUME_DATA( *volume,
                                       indices[0], indices[1], indices[2]);

                if( fast_lookup_present )
                    pixel_col = fast_lookup[lookup_index][val-min_value];
                else
                {
                    get_colour_coding( colour_coding, (Real) val, &col );
                    COLOUR_TO_PIXEL( col, pixel_col );
                }

                prev_x = indices[x_index];
            }

            *pixel_ptr = pixel_col;
            ++pixel_ptr;
        }
    }
}
