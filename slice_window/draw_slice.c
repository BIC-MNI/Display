
#include  <def_display.h>

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
#define  VOXEL_PROBE_INDEX             16
#define  VAL_PROBE_INDEX               17
#define  MAX_MODEL_INDEX               18

private  void  render_slice_to_pixels(
    display_struct        *slice_window,
    pixels_struct         *pixels,
    int                   x_index,
    int                   y_index,
    int                   axis_index,
    int                   start_indices[N_DIMENSIONS],
    int                   x_left,
    int                   x_right,
    int                   y_bottom,
    int                   y_top,
    Real                  x_scale,
    Real                  y_scale );

public  void  initialize_slice_models(
    display_struct    *slice_window )
{
    int            i;
    lines_struct   *lines;
    object_struct  *object;
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL );

    object = create_object( LINES );
    lines = get_lines_ptr( object );

    initialize_lines( lines, Slice_divider_colour );

    ALLOC( lines->points, 4 );
    ALLOC( lines->end_indices, 2 );
    ALLOC( lines->indices, 4 );

    lines->n_points = 4;
    lines->n_items = 2;

    lines->end_indices[0] = 2;
    lines->end_indices[1] = 4;
    lines->indices[0] = 0;
    lines->indices[1] = 1;
    lines->indices[2] = 2;
    lines->indices[3] = 3;

    add_object_to_model( model, object );

    object = create_object( PIXELS );
    initialize_pixels( get_pixels_ptr(object), 0, 0, 0, 0, 1.0, 1.0,
                       RGB_PIXEL );
    add_object_to_model( model, object );

    object = create_object( PIXELS );
    initialize_pixels( get_pixels_ptr(object), 0, 0, 0, 0, 1.0, 1.0,
                       RGB_PIXEL );
    add_object_to_model( model, object );

    object = create_object( PIXELS );
    initialize_pixels( get_pixels_ptr(object), 0, 0, 0, 0, 1.0, 1.0,
                       RGB_PIXEL );
    add_object_to_model( model, object );

    for_inclusive( i, CURSOR1_INDEX, CURSOR3_INDEX )
    {
        object = create_object( LINES );
        lines = get_lines_ptr( object );
        initialize_lines( lines, Slice_cursor_colour );

        ALLOC( lines->points, 8 );
        ALLOC( lines->end_indices, 4 );
        ALLOC( lines->indices, 8 );

        lines->n_points = 8;
        lines->n_items = 4;

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

        add_object_to_model( model, object );
    }

    for_inclusive( i, TEXT1_INDEX, TEXT3_INDEX )
    {
        object = create_object( TEXT );

        get_text_ptr(object)->font = (Font_types) Slice_text_font;
        get_text_ptr(object)->size = Slice_text_font_size;
        get_text_ptr(object)->colour = Slice_text_colour;
        add_object_to_model( model, object );
    }

    for_inclusive( i, X_TRANSFORMED_PROBE_INDEX, MAX_MODEL_INDEX )
    {
        object = create_object( TEXT );

        get_text_ptr(object)->colour = Slice_text_colour;
        add_object_to_model( model, object );
    }
}

public  void  rebuild_slice_models(
    display_struct    *slice_window )
{
    rebuild_slice_divider( slice_window );
    rebuild_probe( slice_window );
    rebuild_colour_bar( slice_window );

    set_slice_window_update( slice_window, 0 );
    set_slice_window_update( slice_window, 1 );
    set_slice_window_update( slice_window, 2 );
}

public  void  rebuild_slice_divider(
    display_struct    *slice_window )
{
    model_struct   *model;
    Point          *points;
    int            x_size, y_size;

    model = get_graphics_model(slice_window,SLICE_MODEL);
    points = get_lines_ptr(model->objects[DIVIDER_INDEX])->points;
    G_get_window_size( slice_window->window, &x_size, &y_size );

    fill_Point( points[0], (Real) slice_window->slice.x_split, 0.0, 0.0 );
    fill_Point( points[1], (Real) slice_window->slice.x_split,
                           (Real) (y_size-1), 0.0 );
    fill_Point( points[2], 0.0, (Real) slice_window->slice.y_split, 0.0 );
    fill_Point( points[3], (Real) (x_size-1),
                           (Real) slice_window->slice.y_split, 0.0 );
}

public  void  rebuild_probe(
    display_struct    *slice_window )
{
    model_struct   *model;
    Boolean        active;
    Volume         volume;
    int            i, x_voxel, y_voxel, z_voxel, view_index;
    Real           x_tal_voxel, y_tal_voxel, z_tal_voxel;
    Real           x_talairach, y_talairach, z_talairach;
    text_struct    *text;
    int            sizes[N_DIMENSIONS];
    Real           value, voxel_value;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;

    active = get_voxel_in_slice_window( slice_window,
                                        &x_voxel, &y_voxel, &z_voxel,
                                        &view_index );

    model = get_graphics_model(slice_window,SLICE_MODEL);

    get_slice_viewport( slice_window, -1, &x_min, &x_max, &y_min, &y_max );

    if( get_slice_window_volume( slice_window, &volume ) )
        get_volume_sizes( volume, sizes );

    convert_voxel_to_talairach( (Real) x_voxel, (Real) y_voxel, (Real) z_voxel,
                                sizes[X], sizes[Y], sizes[Z],
                                &x_tal_voxel, &y_tal_voxel, &z_tal_voxel );

    convert_talairach_to_mm( x_tal_voxel, y_tal_voxel, z_tal_voxel,
                             &x_talairach, &y_talairach, &z_talairach );

    if( active )
    {
        GET_VOXEL_3D( voxel_value, volume, x_voxel, y_voxel, z_voxel );
        value = CONVERT_VOXEL_TO_VALUE( get_volume(slice_window), voxel_value );
    }

    for_less( i, X_TRANSFORMED_PROBE_INDEX, MAX_MODEL_INDEX )
    {
        x_pos = x_min + Probe_x_pos + (i - X_TRANSFORMED_PROBE_INDEX)
                                       * Probe_x_delta;
        y_pos = y_max - Probe_y_pos - (i - X_TRANSFORMED_PROBE_INDEX)
                                       * Probe_y_delta;

        text = get_text_ptr( model->objects[i] );

        if( active )
        {
            switch( i )
            {
            case X_TRANSFORMED_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_x_transformed_format,
                                x_voxel+1 );
                break;
            case Y_TRANSFORMED_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_y_transformed_format,
                                y_voxel+1 );
                break;
            case Z_TRANSFORMED_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_z_transformed_format,
                                z_voxel+1 );
                break;

            case X_TALAIRACH_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_x_talairach_format,
                                x_talairach );
                break;
            case Y_TALAIRACH_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_y_talairach_format,
                                y_talairach );
                break;
            case Z_TALAIRACH_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_z_talairach_format,
                                z_talairach );
                break;
            case VOXEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_val_format,
                                voxel_value );
                break;
            case VAL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_val_format, value );
                break;
            }
        }
        else
        {
            text->string[0] = (char) 0;
        }

        fill_Point( text->origin, x_pos, y_pos, 0.0 );
    }
}

public  void  rebuild_slice_pixels(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    pixels_struct  *pixels;
    int            axis_index, x_index, y_index;
    int            voxel_indices[N_DIMENSIONS];
    int            x_pixel_start, x_pixel_end;
    int            x_size, y_pixel_start, y_pixel_end;
    text_struct    *text;
    char           *format;
    Real           x_scale, y_scale;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;
    Boolean        print_cursor;
    Real           real_pos[N_DIMENSIONS];

    model = get_graphics_model(slice_window,SLICE_MODEL);

    axis_index = slice_window->slice.slice_views[view_index].axis_map[Z];
    x_index = slice_window->slice.slice_views[view_index].axis_map[X];
    y_index = slice_window->slice.slice_views[view_index].axis_map[Y];

    pixels = get_pixels_ptr( model->objects[SLICE1_INDEX+view_index] );

    get_slice_view( slice_window, view_index, &x_scale, &y_scale,
                    &x_pixel_start, &y_pixel_start,
                    &x_pixel_end, &y_pixel_end, voxel_indices );

    x_size = x_pixel_end - x_pixel_start + 1;

    if( x_size > slice_window->slice.temporary_indices_alloced )
    {
        SET_ARRAY_SIZE( slice_window->slice.temporary_indices,
                        slice_window->slice.temporary_indices_alloced,
                        x_size, DEFAULT_CHUNK_SIZE );

        slice_window->slice.temporary_indices_alloced = x_size;
    }

    render_slice_to_pixels( slice_window, pixels, x_index, y_index,
                            axis_index,
                            voxel_indices, x_pixel_start, x_pixel_end,
                            y_pixel_start, y_pixel_end, x_scale, y_scale );

    print_cursor = get_voxel_corresponding_to_point( slice_window,
            &slice_window->associated[THREE_D_WINDOW]->three_d.cursor.origin,
            &real_pos[X], &real_pos[Y], &real_pos[Z] );

    if( print_cursor &&
        real_pos[axis_index] == (Real) ((int) real_pos[axis_index]) )
    {
        print_cursor = FALSE;
    }

    text = get_text_ptr( model->objects[TEXT1_INDEX+view_index] );

    if( print_cursor )
    {
        switch( axis_index )
        {
        case X:  format = Slice_index_xc_format;  break;
        case Y:  format = Slice_index_yc_format;  break;
        case Z:  format = Slice_index_zc_format;  break;
        }

        (void) sprintf( text->string, format,
                        slice_window->slice.slice_index[axis_index] + 1,
                        real_pos[axis_index] + 1.0 );

    }
    else
    {
        if( slice_window->slice.slice_locked[axis_index] )
        {
            switch( axis_index )
            {
            case X:  format = Slice_index_x_locked_format;  break;
            case Y:  format = Slice_index_y_locked_format;  break;
            case Z:  format = Slice_index_z_locked_format;  break;
            }
        }
        else
        {
            switch( axis_index )
            {
            case X:  format = Slice_index_x_format;  break;
            case Y:  format = Slice_index_y_format;  break;
            case Z:  format = Slice_index_z_format;  break;
            }
        }

        (void) sprintf( text->string, format,
                        slice_window->slice.slice_index[axis_index] + 1 );

    }

    get_slice_viewport( slice_window,
                        view_index, &x_min, &x_max, &y_min, &y_max );

    x_pos = x_min + (int) Point_x(Slice_index_offset);
    y_pos = y_min + (int) Point_y(Slice_index_offset);

    fill_Point( text->origin, x_pos, y_pos, 0.0 );

    rebuild_cursor( slice_window, 0 );
    rebuild_cursor( slice_window, 1 );
    rebuild_cursor( slice_window, 2 );
}

public  void  rebuild_cursor(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    int            x_index, y_index, x_start, y_start, x_end, y_end;
    int            x_centre, y_centre, dx, dy;
    Real           start_pixel[N_DIMENSIONS], end_pixel[N_DIMENSIONS];
    lines_struct   *lines;
    int            x, y;
    int            x_min, x_max, y_min, y_max;

    model = get_graphics_model(slice_window,SLICE_MODEL);

    x_index = slice_window->slice.slice_views[view_index].axis_map[X];
    y_index = slice_window->slice.slice_views[view_index].axis_map[Y];

    lines = get_lines_ptr( model->objects[CURSOR1_INDEX+view_index] );

    x = slice_window->slice.slice_index[x_index];
    y = slice_window->slice.slice_index[y_index];

    convert_voxel_to_pixel( slice_window, view_index, x, y, &x_start, &y_start );
    convert_voxel_to_pixel( slice_window, view_index, x+1, y+1, &x_end, &y_end );

    --x_end;
    --y_end;

    x_centre = (x_start + x_end) / 2;
    y_centre = (y_start + y_end) / 2;

    get_slice_viewport( slice_window, view_index, &x_min, &x_max, &y_min, &y_max );

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

    start_pixel[X] = Cursor_start_pixel_x;
    start_pixel[Y] = Cursor_start_pixel_y;
    start_pixel[Z] = Cursor_start_pixel_z;

    end_pixel[X] = Cursor_end_pixel_x;
    end_pixel[Y] = Cursor_end_pixel_y;
    end_pixel[Z] = Cursor_end_pixel_z;

    fill_Point( lines->points[0], x_centre, y_end + start_pixel[y_index], 0.0 );
    fill_Point( lines->points[1], x_centre, y_end + end_pixel[y_index], 0.0 );

    fill_Point( lines->points[2], x_centre, y_start-start_pixel[y_index], 0.0 );
    fill_Point( lines->points[3], x_centre, y_start-end_pixel[y_index], 0.0 );

    fill_Point( lines->points[4], x_start-start_pixel[x_index], y_centre, 0.0 );
    fill_Point( lines->points[5], x_start-end_pixel[x_index], y_centre, 0.0 );

    fill_Point( lines->points[6], x_end + start_pixel[x_index], y_centre, 0.0 );
    fill_Point( lines->points[7], x_end + end_pixel[x_index], y_centre, 0.0 );
}

private  void  render_slice_to_pixels(
    display_struct        *slice_window,
    pixels_struct         *pixels,
    int                   x_index,
    int                   y_index,
    int                   axis_index,
    int                   start_indices[N_DIMENSIONS],
    int                   x_left,
    int                   x_right,
    int                   y_bottom,
    int                   y_top,
    Real                  x_scale,
    Real                  y_scale )
{
    Volume                volume;
    int                   *temporary_indices, sizes[N_DIMENSIONS];
    int                   start_volume_index, volume_index;
    Boolean               fast_lookup_present, display_activity;
    Colour                **fast_lookup;
    int                   old_size;
    int                   x, y, prev_x_offset, x_offset;
    int                   prev_y_offset, y_offset;
    int                   x_size, y_size;
    int                   val, min_value;
    Real                  min_val, max_val;
    int                   label, new_label;
    int                   voxel_indices[3];
    unsigned char         *label_ptr;
    unsigned char         *byte_data;
    unsigned short        *short_data;
    Colour                col;
    Colour                *pixel_ptr;
    Real                  dx, dy;
    Colour                *lookup;
    void                  *void_ptr;

    volume = get_volume( slice_window );
    get_volume_sizes( volume, sizes );
    get_volume_voxel_range( volume, &min_val, &max_val );
    min_value = (int) min_val;
    temporary_indices = slice_window->slice.temporary_indices;
    fast_lookup_present = slice_window->slice.fast_lookup_present;
    fast_lookup = slice_window->slice.fast_lookup;

    if( pixels->x_size > 0 && pixels->y_size > 0 )
        old_size = pixels->x_size * pixels->y_size;
    else
        old_size = 0;
    x_size = x_right - x_left + 1;

    y_size = y_top - y_bottom + 1;

    pixels->x_position = x_left;
    pixels->y_position = y_bottom;
    pixels->x_size = x_size;
    pixels->y_size = y_size;

    dx = 1.0 / x_scale;
    dy = 1.0 / y_scale;

    modify_pixels_size( &old_size, pixels, x_size, y_size, RGB_PIXEL );
    
    voxel_indices[X] = start_indices[X];
    voxel_indices[Y] = start_indices[Y];
    voxel_indices[Z] = start_indices[Z];

    start_volume_index = IJK( voxel_indices[X],
                              voxel_indices[Y],
                              voxel_indices[Z],
                              sizes[Y], sizes[Z] );

    for_less( x, 0, x_size )
    {
        voxel_indices[x_index] = start_indices[x_index] + (int) (x * dx);
        temporary_indices[x] = IJK( voxel_indices[X],
                                    voxel_indices[Y],
                                    voxel_indices[Z],
                                    sizes[Y], sizes[Z] ) -
                               start_volume_index;
    }

    voxel_indices[x_index] = start_indices[x_index];

    GET_VOXEL_PTR_3D( void_ptr, volume, 0, 0, 0 );

    if( volume->data_type == UNSIGNED_BYTE )
        byte_data = void_ptr;
    else if( volume->data_type == UNSIGNED_SHORT )
        short_data = void_ptr;
    else
    {
        HANDLE_INTERNAL_ERROR( "Invalid data type for rendering.\n" );
    }

    label = ACTIVE_BIT;

    if( fast_lookup_present )
        lookup = fast_lookup[label];

    if( Display_activities && volume->labels != (unsigned char ***) NULL )
    {
        label_ptr = volume->labels[0][0];
        display_activity = TRUE;
    }
    else
    {
        display_activity = FALSE;
    }

    prev_y_offset = -1000000;

    for_less( y, 0, y_size )
    {
        pixel_ptr = &pixels->data.pixels_rgb[y * x_size];

        y_offset = start_indices[y_index] + (int) (y * dy);

        if( y_offset == prev_y_offset )
        {
            for_less( x, 0, x_size )
            {
                *pixel_ptr = *(pixel_ptr-x_size);
                ++pixel_ptr;
            }
        }
        else
        {
            prev_y_offset = y_offset;

            voxel_indices[y_index] = y_offset;
            start_volume_index = IJK( voxel_indices[X],
                                      voxel_indices[Y],
                                      voxel_indices[Z],
                                      sizes[Y], sizes[Z] );

            prev_x_offset = -100000;

            for_less( x, 0, x_size )
            {
                x_offset = temporary_indices[x];

                if( x_offset != prev_x_offset )
                {
                    prev_x_offset = x_offset;

                    volume_index = start_volume_index + x_offset;

                    if( display_activity )
                    {
                        new_label = label_ptr[volume_index];
                        if( label != new_label && fast_lookup_present )
                        {
                            lookup = fast_lookup[new_label];
                            if( lookup == (Colour *) 0 )
                                lookup = fast_lookup[ACTIVE_BIT];
                            label = new_label;
                        }
                    }

                    if( volume->data_type == UNSIGNED_BYTE )
                        val = (int) byte_data[volume_index];
                    else
                        val = (int) short_data[volume_index];

                    if( fast_lookup_present )
                        col = lookup[val-min_value];
                    else
                    {
                        col = get_slice_colour_coding( slice_window, val,
                                                       label );
                    }
                }

                *pixel_ptr = col;
                ++pixel_ptr;
            }
        }
    }

    {
        blend_in_atlas( &slice_window->slice.atlas, pixels->data.pixels_rgb,
                        x_size, y_size,
                        start_indices, x_index, y_index, axis_index, dx, dy,
                        sizes[x_index], sizes[y_index] );
    }
}
