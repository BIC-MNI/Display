
#include  <display.h>

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
#define  N_SLICE_MODELS                10

#define  X_VOXEL_PROBE_INDEX           0
#define  Y_VOXEL_PROBE_INDEX           1
#define  Z_VOXEL_PROBE_INDEX           2
#define  X_WORLD_PROBE_INDEX           3
#define  Y_WORLD_PROBE_INDEX           4
#define  Z_WORLD_PROBE_INDEX           5
#define  VOXEL_PROBE_INDEX             6
#define  VAL_PROBE_INDEX               7
#define  LABEL_PROBE_INDEX             8
#define  N_READOUT_MODELS              9

private  void  render_slice_to_pixels(
    display_struct        *slice_window,
    int                   view_index,
    pixels_struct         *pixels );

public  void  initialize_slice_models(
    display_struct    *slice_window )
{
    int            i;
    lines_struct   *lines;
    object_struct  *object;
    model_struct   *model;
    Colour         colour;

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

    /* --- initialize readout values */

    model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );

    if( get_model_bitplanes(model) == OVERLAY_PLANES )
        colour = Readout_text_colour;
    else
        colour = Readout_text_rgb_colour;

    for_inclusive( i, 0, N_READOUT_MODELS )
    {
        object = create_object( TEXT );

        get_text_ptr(object)->colour = colour;
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
    BOOLEAN        active;
    Volume         volume;
    Real           voxel[MAX_DIMENSIONS];
    int            int_voxel[MAX_DIMENSIONS];
    int            label, i, view_index;
    Real           x_world, y_world, z_world;
    text_struct    *text;
    int            sizes[N_DIMENSIONS];
    Real           value, voxel_value;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;

    active = get_voxel_in_slice_window( slice_window, voxel, &view_index );

    get_slice_viewport( slice_window, -1, &x_min, &x_max, &y_min, &y_max );

    if( get_slice_window_volume( slice_window, &volume ) )
        get_volume_sizes( volume, sizes );

    convert_voxel_to_world( volume, voxel,
                            &x_world, &y_world, &z_world );

    if( active )
    {
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        GET_VOXEL_3D( voxel_value, volume,
                      int_voxel[X], int_voxel[Y], int_voxel[Z] );

        value = CONVERT_VOXEL_TO_VALUE( get_volume(slice_window), voxel_value );

        label = get_volume_label_data( get_label_volume(slice_window),
                                       int_voxel );

        label = label & get_max_label();
    }

    /* --- do slice readout models */

    model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );

    for_less( i, 0, N_READOUT_MODELS )
    {
        x_pos = x_min + Probe_x_pos + (i - X_VOXEL_PROBE_INDEX)
                                       * Probe_x_delta;
        y_pos = y_max - Probe_y_pos - (i - X_VOXEL_PROBE_INDEX)
                                       * Probe_y_delta;

        text = get_text_ptr( model->objects[i] );

        if( active )
        {
            switch( i )
            {
            case X_VOXEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_x_voxel_format,
                                voxel[X]+1.0 );
                break;
            case Y_VOXEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_y_voxel_format,
                                voxel[Y]+1.0 );
                break;
            case Z_VOXEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_z_voxel_format,
                                voxel[Z]+1.0 );
                break;

            case X_WORLD_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_x_world_format,
                                x_world );
                break;
            case Y_WORLD_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_y_world_format,
                                y_world );
                break;
            case Z_WORLD_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_z_world_format,
                                z_world );
                break;
            case VOXEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_voxel_format,
                                voxel_value );
                break;
            case VAL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_val_format, value );
                break;
            case LABEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_label_format, label );
                break;
            }
        }
        else
        {
            text->string[0] = (char) 0;
        }

        fill_Point( text->origin, x_pos, y_pos, 0.0 );
    }

    set_update_required( slice_window, (Bitplane_types) Slice_readout_plane );
}

private  void  get_cursor_size(
    int    slice_index,
    Real   *hor_start,
    Real   *hor_end,
    Real   *vert_start,
    Real   *vert_end )
{
    switch( slice_index )
    {
    case 0:
        *hor_start = Cursor_hor_start_0;
        *hor_end = Cursor_hor_end_0;
        *vert_start = Cursor_vert_start_0;
        *vert_end = Cursor_vert_end_0;
        break;

    case 1:
        *hor_start = Cursor_hor_start_1;
        *hor_end = Cursor_hor_end_1;
        *vert_start = Cursor_vert_start_1;
        *vert_end = Cursor_vert_end_1;
        break;

    case 2:
        *hor_start = Cursor_hor_start_2;
        *hor_end = Cursor_hor_end_2;
        *vert_start = Cursor_vert_start_2;
        *vert_end = Cursor_vert_end_2;
        break;
    }
}

public  void  rebuild_cursor(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    int            c, x_left, y_left, x_right, y_right;
    int            x_bottom, y_bottom, x_top, y_top;
    int            x_centre, y_centre, dx, dy;
    int            hor_dx, hor_dy, vert_dx, vert_dy, len_hor, len_vert;
    lines_struct   *lines;
    Real           current_voxel[N_DIMENSIONS];
    int            x_min, x_max, y_min, y_max;
    Real           tmp_voxel[MAX_DIMENSIONS];
    Real           origin[MAX_DIMENSIONS];
    Real           x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    Real           hor_pixel_start, hor_pixel_end;
    Real           vert_pixel_start, vert_pixel_end;

    model = get_graphics_model(slice_window,SLICE_MODEL);

    get_slice_plane( slice_window, view_index, origin, x_axis, y_axis );

    lines = get_lines_ptr( model->objects[CURSOR1_INDEX+view_index] );

    get_current_voxel( slice_window, current_voxel );

    convert_voxel_to_pixel( slice_window, view_index, current_voxel,
                            &x_centre, &y_centre );

    for_less( c, 0, N_DIMENSIONS )
        tmp_voxel[c] = current_voxel[c] - 0.5 * x_axis[c];
    convert_voxel_to_pixel( slice_window, view_index, tmp_voxel,
                            &x_left, &y_left);

    for_less( c, 0, N_DIMENSIONS )
        tmp_voxel[c] = current_voxel[c] + 0.5 * x_axis[c];
    convert_voxel_to_pixel( slice_window, view_index, tmp_voxel,
                            &x_right, &y_right);

    for_less( c, 0, N_DIMENSIONS )
        tmp_voxel[c] = current_voxel[c] - 0.5 * y_axis[c];
    convert_voxel_to_pixel( slice_window, view_index, tmp_voxel,
                            &x_bottom, &y_bottom);

    for_less( c, 0, N_DIMENSIONS )
        tmp_voxel[c] = current_voxel[c] + 0.5 * y_axis[c];
    convert_voxel_to_pixel( slice_window, view_index, tmp_voxel,
                            &x_top, &y_top);

    get_slice_viewport( slice_window, view_index,
                        &x_min, &x_max, &y_min, &y_max );

    if( x_centre < x_min )
    {
        dx = x_min - x_centre;
        x_centre = x_min;
        x_left += dx;
        x_right += dx;
        x_top += dx;
        x_bottom += dx;
    }
    else if( x_centre > x_max )
    {
        dx = x_max - x_centre;
        x_centre = x_max;
        x_left += dx;
        x_right += dx;
        x_top += dx;
        x_bottom += dx;
    }

    if( y_centre < y_min )
    {
        dy = y_min - y_centre;
        y_centre = y_min;
        y_left += dy;
        y_right += dy;
        y_top += dy;
        y_bottom += dy;
    }
    else if( y_centre > y_max )
    {
        dy = y_max - y_centre;
        y_centre = y_max;
        y_left += dy;
        y_right += dy;
        y_top += dy;
        y_bottom += dy;
    }

    hor_dx = x_right - x_centre;
    hor_dy = y_right - y_centre;
    len_hor = sqrt( hor_dx * hor_dx + hor_dy * hor_dy );
    if( len_hor != 0.0 )
    {
        hor_dx /= len_hor;
        hor_dy /= len_hor;
    }

    vert_dx = x_top - x_centre;
    vert_dy = y_top - y_centre;
    len_vert = sqrt( vert_dx * vert_dx + vert_dy * vert_dy );
    if( len_vert != 0.0 )
    {
        vert_dx /= len_vert;
        vert_dy /= len_vert;
    }

    get_cursor_size( view_index, &hor_pixel_start, &hor_pixel_end,
                     &vert_pixel_start, &vert_pixel_end );

    fill_Point( lines->points[0], x_right + hor_pixel_start * hor_dx,
                                  y_right + hor_pixel_start * hor_dy, 0.0 );
    fill_Point( lines->points[1], x_right + hor_pixel_end * hor_dx,
                                  y_right + hor_pixel_end * hor_dy, 0.0 );

    fill_Point( lines->points[2], x_left - hor_pixel_start * hor_dx,
                                  y_left - hor_pixel_start * hor_dy, 0.0 );
    fill_Point( lines->points[3], x_left - hor_pixel_end * hor_dx,
                                  y_left - hor_pixel_end * hor_dy, 0.0 );

    fill_Point( lines->points[4], x_bottom - vert_pixel_start * vert_dx,
                                  y_bottom - vert_pixel_start * vert_dy, 0.0 );
    fill_Point( lines->points[5], x_bottom - vert_pixel_end * vert_dx,
                                  y_bottom - vert_pixel_end * vert_dy, 0.0 );

    fill_Point( lines->points[6], x_top + vert_pixel_start * vert_dx,
                                  y_top + vert_pixel_start * vert_dy, 0.0 );
    fill_Point( lines->points[7], x_top + vert_pixel_end * vert_dx,
                                  y_top + vert_pixel_end * vert_dy, 0.0 );
}

public  void  rebuild_slice_pixels(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    pixels_struct  *pixels;
    int            axis_index, x_index, y_index;
    int            x_min, x_max, y_min, y_max;
    text_struct    *text;
    char           *format;
    int            x_pos, y_pos;
    Real           current_voxel[N_DIMENSIONS];

    model = get_graphics_model(slice_window,SLICE_MODEL);

    pixels = get_pixels_ptr( model->objects[SLICE1_INDEX+view_index] );

    render_slice_to_pixels( slice_window, view_index, pixels );

    if( slice_has_ortho_axes( slice_window, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        set_object_visibility( model->objects[TEXT1_INDEX+view_index], TRUE );

        text = get_text_ptr( model->objects[TEXT1_INDEX+view_index] );

        switch( axis_index )
        {
        case X:  format = Slice_index_x_format;  break;
        case Y:  format = Slice_index_y_format;  break;
        case Z:  format = Slice_index_z_format;  break;
        }

        get_current_voxel( slice_window, current_voxel );

        (void) sprintf( text->string, format, current_voxel[axis_index] + 1.0 );

        get_slice_viewport( slice_window,
                            view_index, &x_min, &x_max, &y_min, &y_max );

        x_pos = x_min + (int) Point_x(Slice_index_offset);
        y_pos = y_min + (int) Point_y(Slice_index_offset);

        fill_Point( text->origin, x_pos, y_pos, 0.0 );
    }
    else
        set_object_visibility( model->objects[TEXT1_INDEX+view_index], FALSE );

    rebuild_cursor( slice_window, 0 );
    rebuild_cursor( slice_window, 1 );
    rebuild_cursor( slice_window, 2 );
}

#define  MAX_LABELS   256

private  void  render_slice_to_pixels(
    display_struct        *slice_window,
    int                   view_index,
    pixels_struct         *pixels )
{
    Volume                volume;
    int                   n_label_alloced;
    unsigned short        label_table[MAX_LABELS];
    unsigned short        *label_ptr;
    pixels_struct         label_pixels;
    Colour                *colour_table;
    int                   i;
    Real                  min_voxel, max_voxel;
    int                   x_size, y_size;
    int                   label, n_alloced;
    Real                  x_trans, y_trans, x_scale, y_scale;
    Real                  origin[MAX_DIMENSIONS];
    Real                  x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    Colour                *pixel_ptr;
    int                   x_min, x_max, y_min, y_max;

    volume = get_volume( slice_window );

    if( pixels->x_size > 0 && pixels->y_size > 0 )
        delete_pixels( pixels );

    n_alloced = 0;

    x_trans = slice_window->slice.slice_views[view_index].x_trans;
    y_trans = slice_window->slice.slice_views[view_index].y_trans;
    x_scale = slice_window->slice.slice_views[view_index].x_scaling;
    y_scale = slice_window->slice.slice_views[view_index].y_scaling;

    get_slice_viewport( slice_window, view_index,
                        &x_min, &x_max, &y_min, &y_max );

    get_volume_voxel_range( volume, &min_voxel, &max_voxel );
    colour_table = slice_window->slice.colour_tables[0];

    if( (int) min_voxel > 0 )
        colour_table -= (int) min_voxel;

    get_slice_plane( slice_window, view_index, origin, x_axis, y_axis );

    create_volume_slice(
                    volume,
                    slice_window->slice.slice_views[view_index].filter_type,
                    slice_window->slice.slice_views[view_index].filter_width,
                    origin, x_axis, y_axis,
                    x_trans, y_trans, x_scale, y_scale,
                    (Volume) NULL, NEAREST_NEIGHBOUR, 0.0,
                    (Real *) 0, (Real *) 0, (Real *) 0,
                    0.0, 0.0, 0.0, 0.0,
                    x_max - x_min + 1, y_max - y_min + 1,
                    RGB_PIXEL, FALSE, (unsigned short **) NULL,
                    &colour_table, make_rgba_Colour( 0, 0, 0, 0 ),
                    &n_alloced, pixels );

    pixels->x_position += x_min;
    pixels->y_position += y_min;

    x_size = pixels->x_size;
    y_size = pixels->y_size;

    /* now do the label colour compositing */

    if( slice_window->slice.display_labels &&
        get_label_volume(slice_window)->data != (void *) NULL &&
        x_size > 0 && y_size > 0 )
    {
        for_less( i, 0, MAX_LABELS )
            label_table[i] = i;

        label_ptr = label_table;

        n_label_alloced = 0;

        create_volume_slice( get_label_volume(slice_window),
                    slice_window->slice.slice_views[view_index].filter_type,
                    slice_window->slice.slice_views[view_index].filter_width,
                    origin, x_axis, y_axis,
                    x_trans, y_trans, x_scale, y_scale,
                    (Volume) NULL, NEAREST_NEIGHBOUR, 0.0,
                    (Real *) 0, (Real *) 0, (Real *) 0,
                    0.0, 0.0, 0.0, 0.0,
                    x_max - x_min + 1, y_max - y_min + 1,
                    COLOUR_INDEX_16BIT_PIXEL, FALSE, &label_ptr,
                    (Colour **) NULL, 0,
                    &n_label_alloced, &label_pixels );

        label_ptr = label_pixels.data.pixels_16bit_colour_index;
        pixel_ptr = pixels->data.pixels_rgb;

        for_less( i, 0, x_size * y_size )
        {
            label = *label_ptr;
            ++label_ptr;

            if( label != 0 )
            {
                *pixel_ptr = apply_label_colour( slice_window, *pixel_ptr,
                                                 label );
            }

            ++pixel_ptr;
        }

        if( n_label_alloced > 0 )
            delete_pixels( &label_pixels );
    }

    /* --- now blend in the talaiarch atlas */

    if( x_size > 0 && y_size > 0 )
    {
        Real  v1[N_DIMENSIONS], v2[N_DIMENSIONS];
        Real  dx, dy;
        int   sizes[N_DIMENSIONS];
        int   x_index, y_index, axis_index;

        if( !slice_has_ortho_axes( slice_window, view_index,
                                   &x_index, &y_index, &axis_index ) )
            return;

        (void) convert_slice_pixel_to_voxel( volume,
                        pixels->x_position - x_min, pixels->y_position - y_min,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v1 );
        (void) convert_slice_pixel_to_voxel( volume,
                        pixels->x_position+1 - x_min, pixels->y_position-y_min,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v2 );

        dx = v2[x_index] - v1[x_index];

        (void) convert_slice_pixel_to_voxel( volume,
                        pixels->x_position - x_min, pixels->y_position+1-y_min,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v2 );

        dy = v2[y_index] - v1[y_index];

        get_volume_sizes( volume, sizes );

        blend_in_atlas( &slice_window->slice.atlas,
                        pixels->data.pixels_rgb,
                        x_size, y_size,
                        v1, x_index, y_index, axis_index,
                        dx, dy,
                        sizes[x_index], sizes[y_index] );
    }
}
