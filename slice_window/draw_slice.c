
#include  <display.h>

typedef  enum  {
                 ATLAS_SLICE_INDEX,
                 COMPOSITE_SLICE_INDEX,
                 CROSS_SECTION_INDEX,
                 CROP_BOX_INDEX,
                 CURSOR_INDEX1,
                 CURSOR_INDEX2,
                 TEXT_INDEX
               } Slice_model_indices;

typedef  enum  { DIVIDER_INDEX } Full_window_indices;

typedef enum { 
               VOLUME_INDEX,
               X_VOXEL_PROBE_INDEX,
               Y_VOXEL_PROBE_INDEX,
               Z_VOXEL_PROBE_INDEX,
               X_WORLD_PROBE_INDEX,
               Y_WORLD_PROBE_INDEX,
               Z_WORLD_PROBE_INDEX,
               VOXEL_PROBE_INDEX,
               VAL_PROBE_INDEX,
               LABEL_PROBE_INDEX,
               N_READOUT_MODELS     } Slice_readout_indices;

public  void  initialize_slice_models(
    display_struct    *slice_window )
{
    int            i, view;
    Point          point;
    lines_struct   *lines;
    object_struct  *object;
    model_struct   *model;
    Colour         colour;

    model = get_graphics_model( slice_window, FULL_WINDOW_MODEL );

    object = create_object( LINES );
    lines = get_lines_ptr( object );

    initialize_lines( lines, Slice_divider_colour );

    fill_Point( point, 0.0, 0.0, 0.0 );
    add_point_to_line( lines, &point );
    add_point_to_line( lines, &point );

    start_new_line( lines );
    add_point_to_line( lines, &point );
    add_point_to_line( lines, &point );

    start_new_line( lines );
    add_point_to_line( lines, &point );
    add_point_to_line( lines, &point );

    add_object_to_model( model, object );

    slice_window->slice.using_transparency = Use_transparency_hardware &&
                                             G_has_transparency_mode();

    for_less( view, 0, N_SLICE_VIEWS )
    {
        model = get_graphics_model( slice_window, SLICE_MODEL1 + view );

        /* --- make atlas pixels */

        object = create_object( PIXELS );
        initialize_pixels( get_pixels_ptr(object), 0, 0, 0, 0, 1.0, 1.0,
                           RGB_PIXEL );
        add_object_to_model( model, object );

        /* --- make composite pixels */

        object = create_object( PIXELS );
        initialize_pixels( get_pixels_ptr(object), 0, 0, 0, 0, 1.0, 1.0,
                           RGB_PIXEL );
        add_object_to_model( model, object );

        /* --- make cross section */

        object = create_object( LINES );
        lines = get_lines_ptr( object );
        initialize_lines( lines, Slice_cross_section_colour );

        lines->n_points = 2;
        lines->n_items = 1;

        ALLOC( lines->points, lines->n_points );
        ALLOC( lines->end_indices, lines->n_items );
        ALLOC( lines->indices, lines->n_points );

        lines->end_indices[0] = 2;

        for_less( i, 0, 2 )
            lines->indices[i] = i;

        set_object_visibility( object, FALSE );
        add_object_to_model( model, object );

        /* --- make crop section */

        object = create_object( LINES );
        lines = get_lines_ptr( object );
        initialize_lines( lines, Slice_crop_box_colour );

        lines->n_points = 4;
        lines->n_items = 1;

        ALLOC( lines->points, lines->n_points );
        ALLOC( lines->end_indices, lines->n_items );
        ALLOC( lines->indices, lines->n_points+1 );

        lines->end_indices[0] = 5;

        for_less( i, 0, 5 )
            lines->indices[i] = i % 4;

        set_object_visibility( object, FALSE );
        add_object_to_model( model, object );

        /* --- make inner cursor */

        object = create_object( LINES );
        lines = get_lines_ptr( object );
        initialize_lines( lines, Slice_cursor_colour1 );

        lines->n_points = 8;
        lines->n_items = 4;

        ALLOC( lines->points, lines->n_points );
        ALLOC( lines->end_indices, lines->n_items );
        ALLOC( lines->indices, lines->n_points );

        for_less( i, 0, lines->n_items )
            lines->end_indices[i] = 2 * i + 2;

        for_less( i, 0, lines->n_points )
            lines->indices[i] = i;

        add_object_to_model( model, object );

        /* --- make outer cursor */

        object = create_object( LINES );
        lines = get_lines_ptr( object );
        initialize_lines( lines, Slice_cursor_colour2 );

        lines->n_points = 16;
        lines->n_items = 8;

        ALLOC( lines->points, lines->n_points );
        ALLOC( lines->end_indices, lines->n_items );
        ALLOC( lines->indices, lines->n_points );

        for_less( i, 0, lines->n_items )
            lines->end_indices[i] = 2 * i + 2;

        for_less( i, 0, lines->n_points )
            lines->indices[i] = i;

        add_object_to_model( model, object );

        object = create_object( TEXT );

        initialize_text( get_text_ptr(object), NULL,
                         Slice_text_colour,
                         (Font_types) Slice_text_font, Slice_text_font_size );

        add_object_to_model( model, object );
    }

    /* --- initialize readout values */

    model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );

    if( get_model_bitplanes(model) == OVERLAY_PLANES )
        colour = Readout_text_colour;
    else
        colour = Readout_text_rgb_colour;

    for_less( i, 0, N_READOUT_MODELS )
    {
        object = create_object( TEXT );

        initialize_text( get_text_ptr(object), NULL,
                         colour,
                         (Font_types) Slice_readout_text_font,
                         Slice_readout_text_font_size );

        add_object_to_model( model, object );
    }
}

public  void  initialize_slice_models_for_volume(
    display_struct    *slice_window,
    int               volume_index )
{
    int            view, p;
    object_struct  *object;
    model_struct   *model;

    for_less( view, 0, N_SLICE_VIEWS )
    {
        model = get_graphics_model( slice_window, SLICE_MODEL1 + view );

        for_less( p, 0, 2 )
        {
            object = create_object( PIXELS );
            initialize_pixels( get_pixels_ptr(object), 0, 0, 0, 0, 1.0, 1.0,
                               RGB_PIXEL );
            insert_object_in_model( model, object, 2 * volume_index + p );
        }
    }
}

public  void  delete_slice_models_for_volume(
    display_struct    *slice_window,
    int               volume_index )
{
    int            view, p;
    model_struct   *model;

    for_less( view, 0, N_SLICE_VIEWS )
    {
        model = get_graphics_model( slice_window, SLICE_MODEL1 + view );

        for_less( p, 0, 2 )
        {
            delete_object( model->objects[2*volume_index] );
            remove_ith_object_from_model( model, 2 * volume_index );
        }
    }
}

public  void  rebuild_slice_divider(
    display_struct    *slice_window )
{
    model_struct   *model;
    Point          *points;
    int            left_panel_width, left_slice_width, right_slice_width;
    int            bottom_slice_height, top_slice_height, text_panel_height;
    int            colour_bar_height;
    int            x_size, y_size;

    model = get_graphics_model( slice_window, FULL_WINDOW_MODEL );
    points = get_lines_ptr(model->objects[DIVIDER_INDEX])->points;
    G_get_window_size( slice_window->window, &x_size, &y_size );

    get_slice_window_partitions( slice_window,
                                 &left_panel_width, &left_slice_width,
                                 &right_slice_width,
                                 &bottom_slice_height, &top_slice_height,
                                 &text_panel_height, &colour_bar_height );

    fill_Point( points[0], (Real) left_panel_width,               0.0, 0.0 );
    fill_Point( points[1], (Real) left_panel_width, (Real) (y_size-1), 0.0 );

    fill_Point( points[2], (Real) (left_panel_width + left_slice_width),
                                         0.0, 0.0 );
    fill_Point( points[3], (Real) (left_panel_width + left_slice_width),
                           (Real) (y_size-1), 0.0 );

    fill_Point( points[4], (Real) left_panel_width, (Real) bottom_slice_height,
                           0.0 );
    fill_Point( points[5], (Real) (x_size-1),       (Real) bottom_slice_height,
                           0.0 );

    set_slice_viewport_update( slice_window, FULL_WINDOW_MODEL );
}

public  Bitplane_types  get_slice_readout_bitplanes()
{
    if( G_has_overlay_planes() )
        return( (Bitplane_types) Slice_readout_plane );
    else
        return( NORMAL_PLANES );
}

public  void  rebuild_probe(
    display_struct    *slice_window )
{
    model_struct   *model;
    BOOLEAN        active;
    Volume         volume;
    Real           voxel[MAX_DIMENSIONS];
    int            int_voxel[MAX_DIMENSIONS];
    int            label, i, view_index, volume_index;
    Real           x_world, y_world, z_world;
    text_struct    *text;
    int            sizes[N_DIMENSIONS];
    Real           value, voxel_value;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;

    active = get_voxel_in_slice_window( slice_window, voxel, &volume_index,
                                        &view_index );

    if( active )
    {
        get_slice_model_viewport( slice_window, SLICE_READOUT_MODEL,
                                  &x_min, &x_max, &y_min, &y_max );

        volume = get_nth_volume( slice_window, volume_index );

        get_volume_sizes( volume, sizes );

        convert_voxel_to_world( volume, voxel,
                                &x_world, &y_world, &z_world );

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        voxel_value = get_volume_voxel_value( volume,
                      int_voxel[X], int_voxel[Y], int_voxel[Z], 0, 0 );

        value = CONVERT_VOXEL_TO_VALUE( get_volume(slice_window), voxel_value );

        label = get_volume_label_data( get_nth_label_volume(
                                                    slice_window,volume_index),
                                       int_voxel );
    }

    /* --- do slice readout models */

    model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );

    for_less( i, 0, N_READOUT_MODELS )
    {
        x_pos = Probe_x_pos + i * Probe_x_delta;
        y_pos = Probe_y_pos + (N_READOUT_MODELS-1-i) * Probe_y_delta +
                ((N_READOUT_MODELS-1-i) / 3) * Probe_y_pos;

        text = get_text_ptr( model->objects[i] );

        if( active )
        {
            switch( i )
            {
            case VOLUME_INDEX:
                (void) sprintf( text->string, Slice_probe_volume_index_format,
                                volume_index + 1 );
                break;
            case X_VOXEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_x_voxel_format,
                                voxel[X] );
                break;
            case Y_VOXEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_y_voxel_format,
                                voxel[Y] );
                break;
            case Z_VOXEL_PROBE_INDEX:
                (void) sprintf( text->string, Slice_probe_z_voxel_format,
                                voxel[Z] );
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

    set_slice_viewport_update( slice_window, SLICE_READOUT_MODEL );
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

    case 3:
        *hor_start = Cursor_hor_start_3;
        *hor_end = Cursor_hor_end_3;
        *vert_start = Cursor_vert_start_3;
        *vert_end = Cursor_vert_end_3;
        break;
    }
}

public  void  get_slice_cross_section_direction(
    display_struct    *slice_window,
    int               view_index,
    int               section_index,
    Vector            *in_plane_axis )
{
    int            c, volume_index;
    Real           perp_axis[N_DIMENSIONS], separations[N_DIMENSIONS];
    Real           plane_axis[N_DIMENSIONS];
    Vector         plane_normal, perp_normal;

    volume_index = get_current_volume_index( slice_window );

    get_volume_separations( get_volume(slice_window), separations );
    get_slice_perp_axis( slice_window, volume_index, section_index, perp_axis );
    get_slice_perp_axis( slice_window, volume_index, view_index, plane_axis );

    for_less( c, 0, N_DIMENSIONS )
    {
        separations[c] = ABS( separations[c] );
        Vector_coord( plane_normal, c ) = plane_axis[c] * separations[c];
        Vector_coord( perp_normal, c ) = perp_axis[c] * separations[c];
    }

    CROSS_VECTORS( *in_plane_axis, plane_normal, perp_normal );

    for_less( c, 0, N_DIMENSIONS )
        Vector_coord( *in_plane_axis, c ) /= separations[c];
}

#define  EXTRA_PIXELS   10

public  void  rebuild_slice_cross_section(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    int            sizes[N_DIMENSIONS];
    int            c, section_index, x_min, x_max, y_min, y_max;
    Real           x1, y1, x2, y2, dx, dy, len, t_min, t_max;
    Real           separations[N_DIMENSIONS];
    Real           voxel1[N_DIMENSIONS], voxel2[N_DIMENSIONS];
    Point          origin, v1, v2, p1, p2;
    Vector         in_plane_axis, direction;
    object_struct  *object;
    lines_struct   *lines;
    Real           current_voxel[N_DIMENSIONS];

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+
                            CROSS_SECTION_INDEX];

    section_index = slice_window->slice.cross_section_index;

    if( view_index == section_index || get_n_volumes( slice_window ) == 0 )
    {
        set_object_visibility( object, FALSE );
        return;
    }

    set_object_visibility( object,
                           slice_window->slice.cross_section_visibility );

    if( !slice_window->slice.cross_section_visibility )
        return;

    lines = get_lines_ptr( object );

    get_current_voxel( slice_window,
                      get_current_volume_index(slice_window), current_voxel );
    get_volume_separations( get_volume(slice_window), separations );

    for_less( c, 0, N_DIMENSIONS )
    {
        separations[c] = ABS( separations[c] );
        Point_coord( origin, c ) = current_voxel[c];
    }

    get_slice_cross_section_direction( slice_window, view_index, section_index,
                                       &in_plane_axis );

    if( null_Vector( &in_plane_axis ) )
    {
        set_object_visibility( object, FALSE );
        return;
    }

    get_volume_sizes( get_volume(slice_window), sizes );

    if( !clip_line_to_box( &origin, &in_plane_axis,
                           -0.5, (Real) sizes[X]-0.5,
                           -0.5, (Real) sizes[Y]-0.5,
                           -0.5, (Real) sizes[Z]-0.5,
                           &t_min, &t_max ) )
    {
        set_object_visibility( object, FALSE );
        return;
    }

    GET_POINT_ON_RAY( v1, origin, in_plane_axis, t_min );
    GET_POINT_ON_RAY( v2, origin, in_plane_axis, t_max );

    for_less( c, 0, N_DIMENSIONS )
    {
        voxel1[c] = Point_coord(v1,c);
        voxel2[c] = Point_coord(v2,c);
    }

    convert_voxel_to_pixel( slice_window,get_current_volume_index(slice_window),
                            view_index, voxel1, &x1, &y1 );
    convert_voxel_to_pixel( slice_window,get_current_volume_index(slice_window),
                            view_index, voxel2, &x2, &y2 );

    dx = x2 - x1;
    dy = y2 - y1;

    len = sqrt( dx * dx + dy * dy );

    if( len >= 0.0 )
    {
        x1 -= EXTRA_PIXELS * dx / len;
        y1 -= EXTRA_PIXELS * dy / len;
        x2 += EXTRA_PIXELS * dx / len;
        y2 += EXTRA_PIXELS * dy / len;
    }

    get_slice_model_viewport( slice_window, SLICE_MODEL1 + view_index,
                              &x_min, &x_max, &y_min, &y_max );

    fill_Point( origin, x1, y1, 0.0 );
    fill_Vector( direction, x2 - x1, y2 - y1, 0.0 );

    if( !clip_line_to_box( &origin, &direction, 
                           0.0, (Real) (x_max - x_min),
                           0.0, (Real) (y_max - y_min),
                           -1.0, 1.0, &t_min, &t_max ) )
    {
        t_min = 0.0;
        t_max = 0.0;
    }

    if( t_min < 0.0 )
        t_min = 0.0;
    else if( t_min > 1.0 )
        t_min = 1.0;

    if( t_max < 0.0 )
        t_max = 0.0;
    else if( t_max > 1.0 )
        t_max = 1.0;

    GET_POINT_ON_RAY( p1, origin, direction, t_min );
    GET_POINT_ON_RAY( p2, origin, direction, t_max );

    fill_Point( lines->points[0], Point_x(p1), Point_y(p1), 0.0 );
    fill_Point( lines->points[1], Point_x(p2), Point_y(p2), 0.0 );
}

public  void  rebuild_slice_crop_box(
    display_struct    *slice_window,
    int               view_index )
{
    int            volume_index, x_index, y_index, axis;
    model_struct   *model;
    object_struct  *object;
    lines_struct   *lines;
    Real           voxel[N_DIMENSIONS], x, y;
    BOOLEAN        visibility;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+
                            CROP_BOX_INDEX];

    volume_index = get_current_volume_index( slice_window );

    visibility = OFF;

    if( slice_window->slice.crop.crop_visible &&
        volume_index >= 0 &&
        slice_has_ortho_axes( slice_window, volume_index,
                              view_index, &x_index, &y_index, &axis ) )
    {
        lines = get_lines_ptr( object );

        get_current_voxel( slice_window, volume_index, voxel );

        if( slice_window->slice.crop.limits[0][axis] <= voxel[axis] &&
            voxel[axis] <= slice_window->slice.crop.limits[1][axis] )
        {
            visibility = ON;
        }
    }

    set_object_visibility( object, visibility );
    if( !visibility )
        return;

    voxel[x_index] = slice_window->slice.crop.limits[0][x_index];
    voxel[y_index] = slice_window->slice.crop.limits[0][y_index];
    convert_voxel_to_pixel( slice_window, volume_index, view_index,
                            voxel, &x, &y );
    fill_Point( lines->points[0], x, y, 0.0 );

    voxel[x_index] = slice_window->slice.crop.limits[1][x_index];
    voxel[y_index] = slice_window->slice.crop.limits[0][y_index];
    convert_voxel_to_pixel( slice_window, volume_index, view_index,
                            voxel, &x, &y );
    fill_Point( lines->points[1], x, y, 0.0 );

    voxel[x_index] = slice_window->slice.crop.limits[1][x_index];
    voxel[y_index] = slice_window->slice.crop.limits[1][y_index];
    convert_voxel_to_pixel( slice_window, volume_index, view_index,
                            voxel, &x, &y );
    fill_Point( lines->points[2], x, y, 0.0 );

    voxel[x_index] = slice_window->slice.crop.limits[0][x_index];
    voxel[y_index] = slice_window->slice.crop.limits[1][y_index];
    convert_voxel_to_pixel( slice_window, volume_index, view_index,
                            voxel, &x, &y );
    fill_Point( lines->points[3], x, y, 0.0 );
}

public  void  rebuild_slice_cursor(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    object_struct  *obj1, *obj2;
    int            c, x_index, y_index, axis, volume_index;
    Real           x_left, x_right, y_bottom, y_top, dx, dy;
    Real           x_centre, y_centre, tmp;
    Real           tmp_voxel[N_DIMENSIONS];
    lines_struct   *lines1, *lines2;
    Real           current_voxel[N_DIMENSIONS];
    int            x_min, x_max, y_min, y_max;
    Real           hor_pixel_start, hor_pixel_end;
    Real           vert_pixel_start, vert_pixel_end;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    obj1 = model->objects[2*slice_window->slice.n_volumes+CURSOR_INDEX1];
    obj2 = model->objects[2*slice_window->slice.n_volumes+CURSOR_INDEX2];

    if( get_n_volumes( slice_window ) == 0 )
    {    
        set_object_visibility( obj1, FALSE );
        set_object_visibility( obj2, FALSE );
        return;
    }

    set_object_visibility( obj1, TRUE );
    set_object_visibility( obj2, TRUE );

    lines1 = get_lines_ptr( model->objects
                             [2*slice_window->slice.n_volumes+CURSOR_INDEX1] );
    lines2 = get_lines_ptr( model->objects
                             [2*slice_window->slice.n_volumes+CURSOR_INDEX2] );

    volume_index = get_current_volume_index( slice_window );
    get_current_voxel( slice_window, volume_index, current_voxel );

    if( slice_has_ortho_axes( slice_window, volume_index,
                              view_index, &x_index, &y_index, &axis ) )
    {
        for_less( c, 0, N_DIMENSIONS )
            tmp_voxel[c] = ROUND( current_voxel[c] );

        current_voxel[x_index] += 0.5;
        convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                current_voxel, &x_right, &y_centre );
        current_voxel[x_index] -= 0.5;

        current_voxel[x_index] -= 0.5;
        convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                current_voxel, &x_left, &y_centre );
        current_voxel[x_index] += 0.5;

        current_voxel[y_index] += 0.5;
        convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                current_voxel, &x_centre, &y_top );
        current_voxel[y_index] -= 0.5;

        current_voxel[y_index] -= 0.5;
        convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                current_voxel, &x_centre, &y_bottom );
        current_voxel[y_index] += 0.5;

        if( x_left > x_right )
        {
            tmp = x_left;
            x_left = x_right;
            x_right = tmp;
        }
        if( y_bottom > y_top )
        {
            tmp = y_top;
            y_top = y_bottom;
            y_bottom = tmp;
        }
    }
    else
    {
        convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                current_voxel,
                                &x_centre, &y_centre );

        x_left = x_centre;
        x_right = x_centre;
        y_bottom = y_centre;
        y_top = y_centre;
    }

    get_cursor_size( view_index, &hor_pixel_start, &hor_pixel_end,
                     &vert_pixel_start, &vert_pixel_end );

    get_slice_model_viewport( slice_window, SLICE_MODEL1 + view_index,
                              &x_min, &x_max, &y_min, &y_max );

    if( x_centre < 0 )
    {
        dx = - x_centre;
        x_centre = 0;
        x_left += dx;
        x_right += dx;
    }
    else if( x_centre > x_max - x_min )
    {
        dx = x_max - x_min - x_centre;
        x_centre = x_max - x_min;
        x_left += dx;
        x_right += dx;
    }

    if( y_centre < 0 )
    {
        dy = - y_centre;
        y_centre = 0;
        y_top += dy;
        y_bottom += dy;
    }
    else if( y_centre > y_max - y_min )
    {
        dy = y_max - y_min - y_centre;
        y_centre = y_max - y_min;
        y_top += dy;
        y_bottom += dy;
    }

    fill_Point( lines1->points[0], x_right + hor_pixel_start, y_centre, 0.0 );
    fill_Point( lines1->points[1], x_right + hor_pixel_end, y_centre, 0.0 );
    fill_Point( lines1->points[2], x_left - hor_pixel_start, y_centre, 0.0 );
    fill_Point( lines1->points[3], x_left - hor_pixel_end, y_centre, 0.0 );
    fill_Point( lines1->points[4], x_centre, y_top + vert_pixel_start, 0.0 );
    fill_Point( lines1->points[5], x_centre, y_top + vert_pixel_end, 0.0 );
    fill_Point( lines1->points[6], x_centre, y_bottom - vert_pixel_start, 0.0 );
    fill_Point( lines1->points[7], x_centre, y_bottom - vert_pixel_end, 0.0 );

    fill_Point( lines2->points[0], x_right + hor_pixel_start, y_centre-1.0,0.0);
    fill_Point( lines2->points[1], x_right + hor_pixel_end, y_centre-1.0, 0.0 );
    fill_Point( lines2->points[2], x_right + hor_pixel_start, y_centre+1.0,0.0);
    fill_Point( lines2->points[3], x_right + hor_pixel_end, y_centre+1.0, 0.0 );

    fill_Point( lines2->points[4], x_left - hor_pixel_start, y_centre-1.0, 0.0);
    fill_Point( lines2->points[5], x_left - hor_pixel_end, y_centre-1.0, 0.0 );
    fill_Point( lines2->points[6], x_left - hor_pixel_start, y_centre+1.0, 0.0);
    fill_Point( lines2->points[7], x_left - hor_pixel_end, y_centre+1.0, 0.0 );

    fill_Point( lines2->points[8], x_centre-1.0, y_top + vert_pixel_start, 0.0);
    fill_Point( lines2->points[9], x_centre-1.0, y_top + vert_pixel_end, 0.0 );
    fill_Point( lines2->points[10],x_centre+1.0, y_top + vert_pixel_start, 0.0);
    fill_Point( lines2->points[11],x_centre+1.0, y_top + vert_pixel_end, 0.0 );

    fill_Point( lines2->points[12],x_centre-1.0, y_bottom-vert_pixel_start,0.0);
    fill_Point( lines2->points[13],x_centre-1.0, y_bottom-vert_pixel_end,0.0);
    fill_Point( lines2->points[14],x_centre+1.0, y_bottom-vert_pixel_start,0.0);
    fill_Point( lines2->points[15],x_centre+1.0, y_bottom-vert_pixel_end,0.0);
}

public  object_struct  *get_slice_pixels_object(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    return( model->objects[2*volume_index] );
}

public  object_struct  *get_label_slice_pixels_object(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    return( model->objects[2*volume_index+1] );
}

private  object_struct  *get_atlas_slice_pixels_object(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    return( model->objects[2*slice_window->slice.n_volumes+
                           ATLAS_SLICE_INDEX] );
}

public  object_struct  *get_composite_slice_pixels_object(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    return( model->objects[2*slice_window->slice.n_volumes+
                           COMPOSITE_SLICE_INDEX] );
}

private  void  render_slice_to_pixels(
    display_struct        *slice_window,
    int                   volume_index,
    int                   view_index,
    pixels_struct         *pixels )
{
    Volume                volume;
    int                   n_alloced;
    Real                  x_trans, y_trans, x_scale, y_scale;
    Real                  origin[MAX_DIMENSIONS];
    Real                  x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    int                   x_min, x_max, y_min, y_max;
    Colour                **colour_map;

    if( pixels->x_size > 0 && pixels->y_size > 0 )
        delete_pixels( pixels );

    x_trans = slice_window->slice.volumes[volume_index].views[view_index]
                                                                    .x_trans;
    y_trans = slice_window->slice.volumes[volume_index].views[view_index]
                                                                    .y_trans;
    x_scale = slice_window->slice.volumes[volume_index].views[view_index]
                                                                    .x_scaling;
    y_scale = slice_window->slice.volumes[volume_index].views[view_index]
                                                                    .y_scaling;

    get_slice_model_viewport( slice_window, SLICE_MODEL1 + view_index,
                              &x_min, &x_max, &y_min, &y_max );

    volume = get_nth_volume( slice_window, volume_index );

    get_slice_plane( slice_window, volume_index, view_index,
                     origin, x_axis, y_axis );

    if( is_an_rgb_volume(volume ) )
        colour_map = NULL;
    else
        colour_map = &slice_window->slice.volumes[volume_index].colour_table;

    n_alloced = 0;
    create_volume_slice(
                    volume,
                    slice_window->slice.volumes[volume_index].views[view_index]
                                                        .filter_type,
                    slice_window->slice.volumes[volume_index].views[view_index]
                                                        .filter_width,
                    origin, x_axis, y_axis,
                    x_trans, y_trans, x_scale, y_scale,
                    (Volume) NULL, NEAREST_NEIGHBOUR, 0.0,
                    (Real *) 0, (Real *) 0, (Real *) 0,
                    0.0, 0.0, 0.0, 0.0,
                    x_max - x_min + 1, y_max - y_min + 1,
                    RGB_PIXEL, FALSE, (unsigned short **) NULL,
                    colour_map,
                    make_rgba_Colour( 0, 0, 0, 0 ),
                    slice_window->slice.render_storage,
                    &n_alloced, pixels );
}

public  void  rebuild_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    object_struct  *pixels_object;
    pixels_struct  *pixels;

    if( get_slice_visibility( slice_window, volume_index, view_index ) )
    {
        pixels_object = get_slice_pixels_object( slice_window, volume_index,
                                                 view_index );
        pixels = get_pixels_ptr( pixels_object );

        render_slice_to_pixels( slice_window, volume_index, view_index,
                                pixels );
    }
}

public  void  rebuild_slice_text(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    int            axis_index, x_index, y_index;
    object_struct  *text_object;
    text_struct    *text;
    char           *format;
    int            x_pos, y_pos;
    Real           current_voxel[N_DIMENSIONS];

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    text_object = model->objects[2*slice_window->slice.n_volumes+TEXT_INDEX];

    if( get_n_volumes( slice_window ) != 0 &&
        slice_has_ortho_axes( slice_window,
                              get_current_volume_index( slice_window ),
                              view_index, &x_index, &y_index, &axis_index ) )
    {
        set_object_visibility( text_object, TRUE );

        text = get_text_ptr( text_object );

        switch( axis_index )
        {
        case X:  format = Slice_index_x_format;  break;
        case Y:  format = Slice_index_y_format;  break;
        case Z:  format = Slice_index_z_format;  break;
        }

        get_current_voxel( slice_window,
                      get_current_volume_index(slice_window), current_voxel );

        (void) sprintf( text->string, format, current_voxel[axis_index] + 1.0 );

        x_pos = (int) Point_x(Slice_index_offset);
        y_pos = (int) Point_y(Slice_index_offset);

        fill_Point( text->origin, x_pos, y_pos, 0.0 );
    }
    else
        set_object_visibility( text_object, FALSE );
}

public  void  rebuild_atlas_slice_pixels(
    display_struct    *slice_window,
    int               view_index )
{
    BOOLEAN        visible;
    object_struct  *pixels_object;
    pixels_struct  *pixels, *volume_pixels;
    Volume         volume;
    Real           v1[N_DIMENSIONS], v2[N_DIMENSIONS], dx, dy;
    int            sizes[N_DIMENSIONS];
    int            x_index, y_index, axis_index, volume_index;
    Real           x_trans, y_trans, x_scale, y_scale;
    Real           origin[MAX_DIMENSIONS];
    Real           x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];

    pixels_object = get_atlas_slice_pixels_object( slice_window, view_index );
    pixels = get_pixels_ptr( pixels_object );

    volume_index = get_current_volume_index( slice_window );

    if( volume_index >= 0 &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        volume = get_volume( slice_window );

        volume_pixels = get_pixels_ptr(
            get_slice_pixels_object( slice_window, volume_index,
                                                 view_index ) );

        if( pixels->x_size != volume_pixels->x_size ||
            pixels->y_size != volume_pixels->y_size )
        {
            delete_pixels( pixels );
            initialize_pixels( pixels, volume_pixels->x_position,
                               volume_pixels->y_position,
                               volume_pixels->x_size,
                               volume_pixels->y_size, 1.0, 1.0, RGB_PIXEL );
        }

        pixels->x_position = volume_pixels->x_position;
        pixels->y_position = volume_pixels->y_position;

        x_trans = slice_window->slice.volumes[volume_index].views[view_index]
                                                                   .x_trans;
        y_trans = slice_window->slice.volumes[volume_index].views[view_index]
                                                                   .y_trans;
        x_scale = slice_window->slice.volumes[volume_index].views[view_index]
                                                                   .x_scaling;
        y_scale = slice_window->slice.volumes[volume_index].views[view_index]
                                                                   .y_scaling;

        get_slice_plane( slice_window, volume_index, view_index,
                         origin, x_axis, y_axis );

        (void) convert_slice_pixel_to_voxel( volume,
                        volume_pixels->x_position, volume_pixels->y_position,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v1 );
        (void) convert_slice_pixel_to_voxel( volume,
                        volume_pixels->x_position+1, volume_pixels->y_position,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v2 );

        dx = v2[x_index] - v1[x_index];

        (void) convert_slice_pixel_to_voxel( volume,
                        volume_pixels->x_position, volume_pixels->y_position+1,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v2 );

        dy = v2[y_index] - v1[y_index];

        get_volume_sizes( volume, sizes );

        visible = render_atlas_slice_to_pixels( &slice_window->slice.atlas,
                        pixels->data.pixels_rgb,
                        pixels->x_size, pixels->y_size,
                        v1, x_index, y_index, axis_index,
                        dx, dy, sizes[x_index], sizes[y_index] );
    }
    else
        visible = FALSE;

    set_object_visibility( pixels_object, visible );
}

private  void  create_composite(
    int             n_slices,
    pixels_struct   *slices[],
    Colour          background_colour,
    pixels_struct   *composite )
{
    Colour   *src, *dest, empty, c1, c2;
    Real     r1, g1, b1, a1, r2, g2, b2, a2, alpha, one_minus_alpha;
    int      i, n_pixels, slice, x_min, x_max, y_min, y_max, x, y;

    if( n_slices == 0 )
    {
        delete_pixels( composite );
        initialize_pixels( composite, 0, 0, 0, 0, 1.0, 1.0, RGB_PIXEL );
        return;
    }

    x_min = slices[0]->x_position;
    y_min = slices[0]->y_position;
    x_max = slices[0]->x_position + slices[0]->x_size - 1;
    y_max = slices[0]->y_position + slices[0]->y_size - 1;

    for_less( slice, 0, n_slices )
    {
        if( slices[slice]->x_position < x_min )
            x_min = slices[slice]->x_position;
        if( slices[slice]->y_position < y_min )
            y_min = slices[slice]->y_position;
        if( slices[slice]->x_position + slices[0]->x_size - 1 > x_max )
            x_max = slices[slice]->x_position + slices[0]->x_size - 1;
        if( slices[slice]->y_position + slices[0]->y_size - 1 > y_max )
            y_max = slices[slice]->y_position + slices[0]->y_size - 1;
    }

    if( x_max - x_min + 1 != composite->x_size ||
        y_max - y_min + 1 != composite->y_size )
    {
        delete_pixels( composite );
        initialize_pixels( composite, 0, 0,
                           x_max - x_min + 1, y_max - y_min + 1,
                           1.0, 1.0, RGB_PIXEL );
    }

    composite->x_position = x_min;
    composite->y_position = y_min;

    n_pixels = composite->x_size * composite->y_size;
    dest = composite->data.pixels_rgb;

    for_less( i, 0, n_pixels )
        dest[i] = background_colour;

    empty = make_rgba_Colour( 0, 0, 0, 0 );

    for_less( slice, 0, n_slices )
    {
        for_less( y, 0, slices[slice]->y_size )
        {
            src = &PIXEL_RGB_COLOUR( *slices[slice], 0, y );
            dest = &PIXEL_RGB_COLOUR( *composite,
                   slices[slice]->x_position - composite->x_position,
                   y + slices[slice]->y_position- composite->y_position );

            for_less( x, 0, slices[slice]->x_size )
            {
                c1 = *dest;
                c2 = *src;
                if( c2 != empty &&
                    (a2 = (Real) get_Colour_a( c2 )) != 0.0 )
                {
                    if( a2 == 255.0 )
                        *dest = *src;
                    else
                    {
                        r1 = (Real) get_Colour_r( c1 );
                        g1 = (Real) get_Colour_g( c1 );
                        b1 = (Real) get_Colour_b( c1 );
                        a1 = (Real) get_Colour_a( c1 );

                        r2 = (Real) get_Colour_r( c2 );
                        g2 = (Real) get_Colour_g( c2 );
                        b2 = (Real) get_Colour_b( c2 );

                        alpha = a2 / 255.0;
                        one_minus_alpha = 1.0 - alpha;

                        *dest = make_rgba_Colour(
                                (int) (one_minus_alpha * r1 + alpha * r2),
                                (int) (one_minus_alpha * g1 + alpha * g2),
                                (int) (one_minus_alpha * b1 + alpha * b2),
                                (int) (one_minus_alpha * a1 + alpha * a2));
                    }
                }

                ++src;
                ++dest;
            }
        }
    }
}

private  BOOLEAN  composite_is_visible(
    display_struct    *slice_window,
    int               view )
{
    int     i;

    if( slice_window->slice.using_transparency )
        return( FALSE );

    for_less( i, 0, slice_window->slice.n_volumes )
    {
        if( slice_window->slice.volumes[i].views[view].visibility )
        {
            if( slice_window->slice.volumes[i].opacity < 1.0 )
                return( TRUE );

            if( get_label_visibility( slice_window, i, view ) )
                return( TRUE );
        }
    }

    return( FALSE );
}

public  void  composite_volume_and_labels(
    display_struct        *slice_window,
    int                   view_index )
{
    int                   v, n_slices;
    pixels_struct         **slices, *composite_pixels, *label_pixels;

    if( !composite_is_visible( slice_window, view_index ) ||
        get_n_volumes( slice_window ) == 0 )
    {
        return;
    }

    ALLOC( slices, 2 * slice_window->slice.n_volumes );
    n_slices = 0;

    for_less( v, 0, slice_window->slice.n_volumes )
    {
        if( slice_window->slice.volumes[v].views[view_index].visibility )
        {
            slices[n_slices] = get_pixels_ptr( get_slice_pixels_object(
                                               slice_window, v, view_index ) );
            ++n_slices;

            label_pixels = get_pixels_ptr(
                 get_label_slice_pixels_object( slice_window, v, view_index ) );

            if( label_pixels->x_size > 0 && label_pixels->y_size > 0 )
            {
                slices[n_slices] = label_pixels;
                ++n_slices;
            }
        }
    }

    composite_pixels = get_pixels_ptr(
           get_composite_slice_pixels_object( slice_window, view_index ) );

    create_composite( n_slices, slices,
                      G_get_background_colour(slice_window->window),
                      composite_pixels );

    FREE( slices );
}

private  void  render_label_slice_to_pixels(
    display_struct        *slice_window,
    int                   volume_index,
    int                   view_index )
{
    Volume                label_volume;
    int                   n_alloced;
    Real                  x_trans, y_trans, x_scale, y_scale;
    Real                  origin[MAX_DIMENSIONS];
    Real                  x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    int                   x_min, x_max, y_min, y_max;
    pixels_struct         *label_pixels;

    label_pixels = get_pixels_ptr( get_label_slice_pixels_object(
                                   slice_window, volume_index, view_index ) );

    if( label_pixels->x_size > 0 && label_pixels->y_size > 0 )
        delete_pixels( label_pixels );

    x_trans = slice_window->slice.volumes[volume_index].views[view_index]
                                                       .x_trans;
    y_trans = slice_window->slice.volumes[volume_index].views[view_index]
                                                       .y_trans;
    x_scale = slice_window->slice.volumes[volume_index].views[view_index]
                                                       .x_scaling;
    y_scale = slice_window->slice.volumes[volume_index].views[view_index]
                                                       .y_scaling;

    get_slice_model_viewport( slice_window, SLICE_MODEL1 + view_index,
                              &x_min, &x_max, &y_min, &y_max );

    label_volume = get_nth_label_volume( slice_window, volume_index );

    get_slice_plane( slice_window, volume_index, view_index,
                     origin, x_axis, y_axis );

    n_alloced = 0;
    create_volume_slice(
                label_volume, NEAREST_NEIGHBOUR, 0.0,
                origin, x_axis, y_axis,
                x_trans, y_trans, x_scale, y_scale,
                (Volume) NULL, NEAREST_NEIGHBOUR, 0.0,
                (Real *) 0, (Real *) 0, (Real *) 0,
                0.0, 0.0, 0.0, 0.0,
                x_max - x_min + 1, y_max - y_min + 1,
                RGB_PIXEL, FALSE, (unsigned short **) NULL,
                &slice_window->slice.volumes[volume_index].label_colour_table,
                make_rgba_Colour( 0, 0, 0, 0 ),
                slice_window->slice.render_storage,
                &n_alloced, label_pixels );
}

public  void  rebuild_label_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    BOOLEAN        visibility;

    visibility = get_label_visibility( slice_window, volume_index, view_index );

    if( visibility )
        render_label_slice_to_pixels( slice_window, volume_index, view_index );
}

public  void  update_slice_pixel_visibilities(
    display_struct    *slice_window,
    int               view )
{
    int      volume_index;
    BOOLEAN  visibility, composite_visibility;

    composite_visibility = composite_is_visible( slice_window, view );

    for_less( volume_index, 0, slice_window->slice.n_volumes )
    {
        visibility = slice_window->slice.volumes[volume_index].
                                        views[view].visibility;

        set_object_visibility( get_slice_pixels_object(
                                              slice_window,volume_index,view ),
                               visibility && !composite_visibility );

        set_object_visibility( get_label_slice_pixels_object(
                                              slice_window,volume_index,view),
                               get_label_visibility( slice_window,
                                              volume_index,view ) &&
                               !composite_visibility );
    }

    set_object_visibility( get_composite_slice_pixels_object(slice_window,view),
                           composite_visibility );
}
