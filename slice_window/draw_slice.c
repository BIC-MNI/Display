
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
#define  X_FILE_PROBE_INDEX            17
#define  Y_FILE_PROBE_INDEX            18
#define  Z_FILE_PROBE_INDEX            19
#define  MAX_MODEL_INDEX               19

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

    for_inclusive( i, X_TRANSFORMED_PROBE_INDEX, MAX_MODEL_INDEX )
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
    int            i, x_voxel, y_voxel, z_voxel, view_index, max_index;
    Real           x_tal_voxel, y_tal_voxel, z_tal_voxel;
    Real           x_talairach, y_talairach, z_talairach;
    text_struct    *text;
    int            nx, ny, nz;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;
    int            x_file, y_file, z_file;
    void           get_slice_viewport();
    Boolean        get_voxel_in_slice_window();
    void           convert_voxel_to_talairach();
    void           convert_talairach_to_mm();
    void           get_volume_size();
    void           convert_to_file_space();

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

    if( volume->mapping_present )
    {
        convert_to_file_space( volume, x_voxel, y_voxel, z_voxel,
                                &x_file, &y_file, &z_file );
        max_index = MAX_MODEL_INDEX;
    }
    else
    {
        max_index = VAL_PROBE_INDEX;
    }

    for_inclusive( i, X_FILE_PROBE_INDEX, Z_FILE_PROBE_INDEX )
    {
        model->object_list[i]->visibility = volume->mapping_present;
    }

    for_inclusive( i, X_TRANSFORMED_PROBE_INDEX, max_index )
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

            case X_FILE_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_x_file_format,
                                x_file+1 );
                break;
            case Y_FILE_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_y_file_format,
                                y_file+1 );
                break;
            case Z_FILE_PROBE_INDEX:
                (void) sprintf( text->text, Slice_probe_z_file_format,
                                z_file+1 );
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

public  void  rebuild_slice_pixels( slice_window, view_index )
    graphics_struct   *slice_window;
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

    model = get_graphics_model(slice_window,SLICE_MODEL);

    axis_index = slice_window->slice.slice_views[view_index].axis_map[Z];
    x_index = slice_window->slice.slice_views[view_index].axis_map[X];
    y_index = slice_window->slice.slice_views[view_index].axis_map[Y];

    pixels = model->object_list[SLICE1_INDEX+view_index]->ptr.pixels;

    get_slice_view( slice_window, view_index, &x_scale, &y_scale,
                    &x_pixel_start, &y_pixel_start,
                    &x_pixel_end, &y_pixel_end, voxel_indices );

    x_size = x_pixel_end - x_pixel_start + 1;

    if( x_size > slice_window->slice.temporary_indices_alloced )
    {
        SET_ARRAY_SIZE( status, slice_window->slice.temporary_indices,
                        slice_window->slice.temporary_indices_alloced,
                        x_size, DEFAULT_CHUNK_SIZE );

        if( status == OK )
        {
            slice_window->slice.temporary_indices_alloced = x_size;
        }
        else
        {
            slice_window->slice.temporary_indices_alloced = 0;
        }
    }

    render_slice_to_pixels( slice_window, pixels, x_index, y_index,
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

    text = model->object_list[TEXT1_INDEX+view_index]->ptr.text;

    if( print_cursor )
    {
        switch( axis_index )
        {
        case X:  format = Slice_index_xc_format;  break;
        case Y:  format = Slice_index_yc_format;  break;
        case Z:  format = Slice_index_zc_format;  break;
        }

        (void) sprintf( text->text, format,
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

        (void) sprintf( text->text, format,
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

    x_index = graphics->slice.slice_views[view_index].axis_map[X];
    y_index = graphics->slice.slice_views[view_index].axis_map[Y];

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

private  void  render_slice_to_pixels( slice_window, pixels,
                                       x_index, y_index, start_indices,
                                       x_left, x_right, y_bottom, y_top,
                                       x_scale, y_scale )
    graphics_struct       *slice_window;
    pixels_struct         *pixels;
    int                   x_index, y_index;
    int                   start_indices[N_DIMENSIONS];
    int                   x_left, x_right, y_bottom, y_top;
    Real                  x_scale, y_scale;
{
    volume_struct         *volume;
    int                   *temporary_indices;
    Boolean               fast_lookup_present;
    Pixel_colour          **fast_lookup;
    Status                status;
    int                   new_size, old_size;
    int                   x, y, prev_x_offset, x_offset;
    int                   prev_y_offset, y_offset;
    int                   x_size, y_size;
    int                   val, min_value;
    int                   label, new_label;
    int                   voxel_indices[3];
    unsigned char         *volume_ptr, *start_volume_ptr;
    Colour                col;
    Pixel_colour          pixel_col, *pixel_ptr;
    Real                  dx, dy;
    Pixel_colour          *lookup;
    void                  get_slice_colour_coding();

    status = OK;

    volume = slice_window->slice.volume;
    temporary_indices = slice_window->slice.temporary_indices;
    fast_lookup_present = slice_window->slice.fast_lookup_present;
    fast_lookup = slice_window->slice.fast_lookup;

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
    
    voxel_indices[X] = start_indices[X];
    voxel_indices[Y] = start_indices[Y];
    voxel_indices[Z] = start_indices[Z];

    start_volume_ptr = GET_VOLUME_PTR( *volume,
                                       voxel_indices[X],
                                       voxel_indices[Y],
                                       voxel_indices[Z] );

    for_less( x, 0, x_size )
    {
        voxel_indices[x_index] = start_indices[x_index] + (int) (x * dx);
        volume_ptr = GET_VOLUME_PTR( *volume,
                                     voxel_indices[X],
                                     voxel_indices[Y],
                                     voxel_indices[Z] );
        temporary_indices[x] = (int) (volume_ptr - start_volume_ptr);
    }

    voxel_indices[x_index] = start_indices[x_index];

    min_value = volume->min_value;

    label = ACTIVE_BIT;

    if( fast_lookup_present )
        lookup = fast_lookup[label];

    prev_y_offset = -1000000;

    for_less( y, 0, y_size )
    {
        pixel_ptr = &pixels->pixels[y * x_size];

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
            start_volume_ptr = GET_VOLUME_PTR( *volume, voxel_indices[X],
                                               voxel_indices[Y],
                                               voxel_indices[Z] );

            prev_x_offset = -100000;

            for_less( x, 0, x_size )
            {
                x_offset = temporary_indices[x];

                if( x_offset != prev_x_offset )
                {
                    prev_x_offset = x_offset;

                    volume_ptr = start_volume_ptr + x_offset;

                    if( Display_activities )
                    {
                        new_label = GET_VOLUME_AUX_DATA_AT_PTR( *volume,
                                                                volume_ptr );
                        if( label != new_label && fast_lookup_present )
                        {
                            lookup = fast_lookup[new_label];
                            if( lookup == (Pixel_colour *) 0 )
                                lookup = fast_lookup[ACTIVE_BIT];
                            label = new_label;
                        }
                    }

                    val = GET_VOLUME_DATA_AT_PTR( *volume, volume_ptr );

                    if( fast_lookup_present )
                        pixel_col = lookup[val-min_value];
                    else
                    {
                        get_slice_colour_coding( slice_window, val, label,
                                                 &col );
                        COLOUR_TO_PIXEL( col, pixel_col );
                    }
                }

                *pixel_ptr = pixel_col;
                ++pixel_ptr;
            }
        }
    }

    if( x_index == X && y_index == Y && start_indices[Z] == 26 )
    {
        void   blend_in_talairach_image();

        blend_in_talairach_image( pixels->pixels, x_size, y_size,
                                  start_indices, dx, dy, volume->sizes[X] );
    }
}

private  Boolean  images_read_in = FALSE;

#define  N_IMAGES   4

private  pixels_struct  images[N_IMAGES];

private  check_read_in()
{
    static  char  *filenames[N_IMAGES] = {
                "/nil/david/Talairach/resampled_128.obj",
                "/nil/david/Talairach/resampled_256.obj",
                "/nil/david/Talairach/resampled_512.obj",
                "/nil/david/Talairach/resampled_1024.obj" };
    Status        status;
    Status        input_object_type();
    Status        io_pixels();
    int           i;
    FILE          *file;
    File_formats  format;
    object_types  type;
    Boolean       eof;

    if( !images_read_in )
    {
        images_read_in = TRUE;

        for_less( i, 0, N_IMAGES )
        {
            status = OK;

            (void) printf( "Reading Talairach images [%d/%d].\n",i+1, N_IMAGES);

            if( status == OK )
                status = open_file( filenames[i], READ_FILE, BINARY_FORMAT,
                                    &file );

            if( status == OK )
                status = input_object_type( file, &type, &format, &eof );

            if( status == OK && !eof && type == PIXELS )
                status = io_pixels( file, READ_FILE, format, &images[i] );

            if( status == OK )
                status = close_file( file );

            if( status != OK )
                images[i].x_max = 0;
        }
    }
}


private  void  blend_in_talairach_image( pixels, x_size, y_size,
                   start_indices, dx, dy, x_volume_size )
    Pixel_colour   pixels[];
    int            x_size;
    int            y_size;
    int            start_indices[];
    Real           dx;
    Real           dy;
    int            x_volume_size;
{
    int            x, y, image_index, image_size;
    int            x_pixel, y_pixel, x_pixel_start, y_pixel_start;
    int            r_tal, g_tal, b_tal, r_vox, g_vox, b_vox;
    int            r, g, b;
    Pixel_colour   *image, voxel_pixel, tal_pixel;

    check_read_in();

    image_index = -1;

    if( x_volume_size != 128 && x_volume_size != 256 )
        return;

    x_pixel_start = start_indices[X] / dx;
    y_pixel_start = start_indices[Y] / dy;

    if( x_volume_size == 256 )
    {
        dx /= 2.0;
        dy /= 2.0;
    }

    if( dx == 1.0 )
    {
        image_index = 0;
        image_size = 128;
    }
    else if( dx == 0.5 )
    {
        image_index = 1;
        image_size = 256;
    }
    else if( dx == 0.25 )
    {
        image_index = 2;
        image_size = 512;
    }
    else if( dx == 0.125 )
    {
        image_index = 3;
        image_size = 1024;
    }
    else
        return;

    if( images[image_index].x_max == 0 )
        return;

    image = images[image_index].pixels;

    for_less( y, 0, y_size )
    {
        y_pixel = y_pixel_start + y;

        if( y_pixel < 0 )  y_pixel = 0;
        if( y_pixel >= image_size )  y_pixel = image_size-1;

        for_less( x, 0, x_size )
        {
            x_pixel = x_pixel_start + x;

            if( x_pixel < 0 )  x_pixel = 0;
            if( x_pixel >= image_size )  x_pixel = image_size-1;

            voxel_pixel = *pixels;
            r_vox = Pixel_colour_r(voxel_pixel);
            g_vox = Pixel_colour_g(voxel_pixel);
            b_vox = Pixel_colour_b(voxel_pixel);

            tal_pixel = image[IJ(y_pixel,x_pixel,image_size)];
            r_tal = Pixel_colour_r(tal_pixel);
            g_tal = Pixel_colour_g(tal_pixel);
            b_tal = Pixel_colour_b(tal_pixel);

            if( r_tal > Talairach_opacity_threshold &&
                g_tal > Talairach_opacity_threshold &&
                b_tal > Talairach_opacity_threshold )
            {
                *pixels = voxel_pixel;
            }
            else
            {
                r = ROUND( r_vox + (r_tal - r_vox) * Talairach_opacity );
                g = ROUND( g_vox + (g_tal - g_vox) * Talairach_opacity );
                b = ROUND( b_vox + (b_tal - b_vox) * Talairach_opacity );
                *pixels = RGB_255_TO_PIXEL( r, g, b );
            }

            ++pixels;
        }
    }
}
