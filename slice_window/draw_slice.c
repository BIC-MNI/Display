/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/draw_slice.c,v 1.115 2001/05/27 00:19:53 stever Exp $";
#endif

#include  <display.h>

typedef  enum  {
                 ATLAS_SLICE_INDEX,
                 COMPOSITE_SLICE_INDEX,
                 CROSS_SECTION_INDEX,
                 CROP_BOX_INDEX,
                 CURSOR_INDEX1,
                 CURSOR_INDEX2,
                 UNFINISHED_BAR,
                 TEXT_INDEX
               } Slice_model_indices;

typedef  enum  { DIVIDER_INDEX } Full_window_indices;



public  void  initialize_slice_models(
    display_struct    *slice_window )
{
    int               i, view;
    Point             point;
    lines_struct      *lines;
    polygons_struct   *polygons;
    object_struct     *object;
    model_struct      *model;
    Colour            colour;

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

        /* --- make cross section */

        object = create_object( POLYGONS );
        polygons = get_polygons_ptr( object );
        initialize_polygons( polygons, Unfinished_flag_colour, NULL );

        polygons->n_points = 8;
        polygons->n_items = 4;

        ALLOC( polygons->points, polygons->n_points );
        polygons->normals = NULL;
        ALLOC( polygons->end_indices, polygons->n_items );
        ALLOC( polygons->indices, 16 );

        polygons->end_indices[0] = 4;
        polygons->end_indices[1] = 8;
        polygons->end_indices[2] = 12;
        polygons->end_indices[3] = 16;

        polygons->indices[0] = 0;
        polygons->indices[1] = 1;
        polygons->indices[2] = 5;
        polygons->indices[3] = 4;

        polygons->indices[4] = 1;
        polygons->indices[5] = 2;
        polygons->indices[6] = 6;
        polygons->indices[7] = 5;

        polygons->indices[8] = 2;
        polygons->indices[9] = 3;
        polygons->indices[10] = 7;
        polygons->indices[11] = 6;

        polygons->indices[12] = 3;
        polygons->indices[13] = 0;
        polygons->indices[14] = 4;
        polygons->indices[15] = 7;

        set_object_visibility( object, FALSE );
        add_object_to_model( model, object );

        /*--- make text */

        object = create_object( TEXT );

        initialize_text( get_text_ptr(object), NULL,
                         Slice_text_colour,
                         (Font_types) Slice_text_font, Slice_text_font_size );

        add_object_to_model( model, object );
    }

    /* --- initialize readout values */

    model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );

    if( get_model_bitplanes(model) == OVERLAY_PLANES )
        colour = (Colour) Readout_text_colour;
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

public  Bitplane_types  get_slice_readout_bitplanes( void )
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
    Volume         volume_ratio_num, volume_ratio_den;
    Real           voxel[MAX_DIMENSIONS];
    int            int_voxel[MAX_DIMENSIONS];
    int            label, i, view_index, volume_index;
    Real           x_world, y_world, z_world;
    text_struct    *text;
    int            sizes[N_DIMENSIONS];
    Real           value, voxel_value;
    Real           value_ratio_num, value_ratio_den;
    int            x_pos, y_pos, x_min, x_max, y_min, y_max;
    char           buffer[EXTREMELY_LARGE_STRING_SIZE];
    Real 		   ratio;
    int            ratio_num_index, ratio_den_index;

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

        (void) evaluate_volume( volume, voxel, NULL,
                                slice_window->slice.degrees_continuity,
                                FALSE, 0.0, &value, NULL, NULL );

        voxel_value = convert_value_to_voxel( volume, value );

        label = get_voxel_label( slice_window, volume_index,
                                 int_voxel[X], int_voxel[Y], int_voxel[Z] );

        if( slice_window->slice.print_probe_ratio)
		{
        	int ratio_num_index =
        			slice_window->slice.ratio_volume_index_numerator;
        	int ratio_den_index =
        	        slice_window->slice.ratio_volume_index_denominator;

			volume_ratio_num = get_nth_volume( slice_window, ratio_num_index );
			volume_ratio_den = get_nth_volume( slice_window, ratio_den_index );

			(void) evaluate_volume( volume_ratio_num, voxel, NULL,
                                    slice_window->slice.degrees_continuity,
	                                FALSE, 0.0, &value_ratio_num, NULL, NULL );
			(void) evaluate_volume( volume_ratio_den, voxel, NULL,
                                    slice_window->slice.degrees_continuity,
	                                FALSE, 0.0, &value_ratio_den, NULL, NULL );
			ratio = value_ratio_num / value_ratio_den;
		}

    }

    /* --- do slice readout models */

    model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );

    for_less( i, 0, N_READOUT_MODELS )
    {
    	text = get_text_ptr( model->objects[i] );
    	if( active )
        {
            switch( i )
            {
            case VOLUME_INDEX:
                (void) sprintf( buffer, Slice_probe_volume_index_format,
                                volume_index + 1 );
                break;
            case X_VOXEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_x_voxel_format,
                                voxel[X] );
                break;
            case Y_VOXEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_y_voxel_format,
                                voxel[Y] );
                break;
            case Z_VOXEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_z_voxel_format,
                                voxel[Z] );
                break;

            case X_WORLD_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_x_world_format,
                                x_world );
                break;
            case Y_WORLD_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_y_world_format,
                                y_world );
                break;
            case Z_WORLD_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_z_world_format,
                                z_world );
                break;
            case VOXEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_voxel_format,
                                voxel_value );
                break;
            case VAL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_val_format, value );
                break;
            case LABEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_label_format, label );
                break;
            case RATIO_PROBE_INDEX:
            	if( slice_window->slice.print_probe_ratio )
            		(void) sprintf( buffer, Slice_probe_ratio_format, ratio );
            	else
            		(void) sprintf( buffer, "" );
                break;
            }
        }
        else
        {
            buffer[0] = END_OF_STRING;
        }

        x_pos = Probe_x_pos + i * Probe_x_delta;
        y_pos = Probe_y_pos + (N_READOUT_MODELS-i-1) * Probe_y_delta +
                ((N_READOUT_MODELS-i+1) / 3) * Probe_y_pos;



        delete_string( text->string );
        text->string = create_string( buffer );

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
        separations[c] = FABS( separations[c] );
        Vector_coord( plane_normal, c ) = (Point_coord_type)
                                             (plane_axis[c] * separations[c]);
        Vector_coord( perp_normal, c ) = (Point_coord_type)
                                             (perp_axis[c] * separations[c]);
    }

    CROSS_VECTORS( *in_plane_axis, plane_normal, perp_normal );

    for_less( c, 0, N_DIMENSIONS )
        Vector_coord( *in_plane_axis, c ) /= (Point_coord_type) separations[c];
}

public  void  rebuild_slice_unfinished_flag(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct      *model;
    Real              x_size, y_size, width;
    Point             *points;
    object_struct     *object;
    polygons_struct   *polygons;
    int               x_min, x_max, y_min, y_max;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+UNFINISHED_BAR];
    polygons = get_polygons_ptr( object );
    points = polygons->points;

    get_slice_model_viewport( slice_window, SLICE_MODEL1 + view_index,
                              &x_min, &x_max, &y_min, &y_max );

    x_size = (Real) (x_max - x_min + 1);
    y_size = (Real) (y_max - y_min + 1);
    width = Unfinished_flag_width;

    fill_Point( points[0], 0.0, 0.0, 0.0 );
    fill_Point( points[1], x_size, 0.0, 0.0 );
    fill_Point( points[2], x_size, y_size, 0.0 );
    fill_Point( points[3], 0.0, y_size, 0.0 );

    fill_Point( points[4], width, width, 0.0 );
    fill_Point( points[5], x_size-1.0-width, width, 0.0 );
    fill_Point( points[6], x_size-1.0-width, y_size-1.0-width, 0.0 );
    fill_Point( points[7], width, y_size-1.0-width, 0.0 );
}

public  BOOLEAN  get_slice_unfinished_flag_visibility(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct      *model;
    object_struct     *object;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+UNFINISHED_BAR];
    return( get_object_visibility( object ) );
}

public  void  set_slice_unfinished_flag_visibility(
    display_struct    *slice_window,
    int               view_index,
    BOOLEAN           state )
{
    model_struct      *model;
    object_struct     *object;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+UNFINISHED_BAR];
    set_object_visibility( object, state );
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
        separations[c] = FABS( separations[c] );
        Point_coord( origin, c ) = (Point_coord_type) current_voxel[c];
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
        voxel1[c] = (Real) Point_coord(v1,c);
        voxel2[c] = (Real) Point_coord(v2,c);
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
        x1 -= (Real) EXTRA_PIXELS * dx / len;
        y1 -= (Real) EXTRA_PIXELS * dy / len;
        x2 += (Real) EXTRA_PIXELS * dx / len;
        y2 += (Real) EXTRA_PIXELS * dy / len;
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
    int            x_index, y_index, axis, volume_index;
    Real           x_left, x_right, y_bottom, y_top, dx, dy;
    Real           x_centre, y_centre, tmp;
    lines_struct   *lines1, *lines2;
    Real           current_voxel[N_DIMENSIONS];
    int            x_min, x_max, y_min, y_max;
    Real           hor_pixel_start, hor_pixel_end;
    Real           vert_pixel_start, vert_pixel_end;
    int            volume, n_volumes;
    BOOLEAN        visible;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    obj1 = model->objects[2*slice_window->slice.n_volumes+CURSOR_INDEX1];
    obj2 = model->objects[2*slice_window->slice.n_volumes+CURSOR_INDEX2];

    n_volumes = get_n_volumes( slice_window );
    visible = FALSE;
    for_less( volume, 0, n_volumes )
    {
        if( get_slice_visibility(slice_window,volume,view_index) )
        {
            visible = TRUE;
            break;
        }
    }

    if( !visible || !slice_window->slice.cursor_visibility )
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

    if( x_centre < 0.0 )
    {
        dx = - x_centre;
        x_centre = 0.0;
        x_left += dx;
        x_right += dx;
    }
    else if( x_centre > (Real) (x_max - x_min) )
    {
        dx = (Real) x_max - (Real) x_min - x_centre;
        x_centre = (Real) x_max - (Real) x_min;
        x_left += dx;
        x_right += dx;
    }

    if( y_centre < 0.0 )
    {
        dy = - y_centre;
        y_centre = 0.0;
        y_top += dy;
        y_bottom += dy;
    }
    else if( y_centre > (Real) (y_max - y_min) )
    {
        dy = (Real) y_max - (Real) y_min - y_centre;
        y_centre = (Real) y_max - (Real) y_min;
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

private  int  render_slice_to_pixels(
    display_struct        *slice_window,
    int                   volume_index,
    int                   view_index,
    int                   which_volume,
    Colour                colour_table[],
    Filter_types          filter_type,
    int                   continuity,
    pixels_struct         *pixels,
    BOOLEAN               incremental_flag,
    BOOLEAN               interrupted,
    BOOLEAN               continuing_flag,
    BOOLEAN               *finished )
{
    int                   n_pixels_drawn, n_pixels_redraw;
    int                   width, x_min, x_max, y_min, y_max;
    int                   x_centre, y_centre, edge_index;
    int                   x_sub_min, x_sub_max, y_sub_min, y_sub_max;
    int                   x, y, height, *n_alloced_ptr;
    int                   r, g, b, opacity;
    Colour                empty, colour;
    Real                  x_pixel, y_pixel;
    Real                  x_trans, y_trans, x_scale, y_scale;
    Real                  current_voxel[MAX_DIMENSIONS];
    Real                  origin[MAX_DIMENSIONS];
    Real                  x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    BOOLEAN               first_flag, force_update_limits;
    Colour                **colour_map;
    loaded_volume_struct  *vol_info;
    Volume                volume;

    if( !continuing_flag )
    {
        if( pixels->x_size > 0 && pixels->y_size > 0 )
        {
            pixels->x_size = 0;
            pixels->y_size = 0;
        }
    }

    vol_info = &slice_window->slice.volumes[volume_index];

    x_trans = slice_window->slice.volumes[volume_index].views[view_index]
                                                                    .x_trans;
    y_trans = slice_window->slice.volumes[volume_index].views[view_index]
                                                                    .y_trans;
    x_scale = slice_window->slice.volumes[volume_index].views[view_index]
                                                                    .x_scaling;
    y_scale = slice_window->slice.volumes[volume_index].views[view_index]
                                                                    .y_scaling;

    (void) get_slice_subviewport( slice_window, view_index,
                                  &x_sub_min, &x_sub_max,
                                  &y_sub_min, &y_sub_max );

    x_trans -= (Real) x_sub_min;
    y_trans -= (Real) y_sub_min;

    get_slice_plane( slice_window, volume_index, view_index,
                     origin, x_axis, y_axis );

    if( which_volume == 0 )
        volume = get_nth_volume( slice_window, volume_index );
    else
        volume = get_nth_label_volume( slice_window, volume_index );

    if( is_an_rgb_volume(volume ) )
        colour_map = NULL;
    else
        colour_map = &colour_table;

    first_flag = !continuing_flag;

    if( which_volume == 0 )
    {
        n_alloced_ptr = &slice_window->slice.volumes[volume_index].
                                      views[view_index].n_pixels_alloced;
    }
    else
    {
        n_alloced_ptr = &slice_window->slice.volumes[volume_index].
                                      views[view_index].n_label_pixels_alloced;
    }

    if( first_flag )
    {
        set_volume_slice_pixel_range(
                    volume, filter_type,
                    slice_window->slice.volumes[volume_index].views[view_index]
                                                        .filter_width,
                    origin, x_axis, y_axis,
                    x_trans, y_trans, x_scale, y_scale,
                    (Volume) NULL, NEAREST_NEIGHBOUR, 0.0,
                    (Real *) 0, (Real *) 0, (Real *) 0,
                    0.0, 0.0, 0.0, 0.0,
                    x_sub_max - x_sub_min + 1, y_sub_max - y_sub_min + 1,
                    RGB_PIXEL,
                    n_alloced_ptr, pixels );
    }

    *finished = TRUE;

    if( pixels->x_size <= 0 || pixels->y_size <= 0 )
        return( 0 );

    n_pixels_redraw = vol_info->views[view_index].n_pixels_redraw;
    edge_index = vol_info->views[view_index].edge_index[which_volume];

    n_pixels_drawn = 0;


    do
    {
        x_min = vol_info->views[view_index].x_min_update[which_volume];
        x_max = vol_info->views[view_index].x_max_update[which_volume];
        y_min = vol_info->views[view_index].y_min_update[which_volume];
        y_max = vol_info->views[view_index].y_max_update[which_volume];

        if( continuing_flag && x_min <= x_max && y_min <= y_max )
        {
            edge_index = (edge_index + 1) % 4;

            if( edge_index == 0 || edge_index == 2 )
            {
                height = (x_max - x_min + 1);
                if( incremental_flag )
                    width = n_pixels_redraw / height;
                else
                    width = pixels->y_size;
            }
            else
            {
                height = (y_max - y_min + 1);
                if( incremental_flag )
                    width = n_pixels_redraw / height;
                else
                    width = pixels->x_size;
            }

            if( width < 1 )
                width = 1;

            switch( edge_index )
            {
            case 0:
                y_max = y_min - 1;
                y_min -= width;
                break;

            case 1:
                x_min = x_max + 1;
                x_max += width;
                break;

            case 2:
                y_min = y_max + 1;
                y_max += width;
                break;

            case 3:
                x_max = x_min - 1;
                x_min -= width;
                break;
            }

            force_update_limits = FALSE;
        }
        else
        {
            if( interrupted )
            {
                x_min = 0;
                x_max = -1;
                y_min = 0;
                y_max = -1;
            }
            else if( !incremental_flag )
            {
                x_min = 0;
                x_max = pixels->x_size-1;
                y_min = 0;
                y_max = pixels->y_size-1;
            }
            else
            {
                width = (int) sqrt( (Real) n_pixels_redraw ) + 1;
                if( width < 1 )
                    width = 1;

                get_current_voxel( slice_window, volume_index, current_voxel );
                convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                    current_voxel, &x_pixel, &y_pixel );

                x_centre = ROUND( x_pixel ) - x_sub_min - pixels->x_position;
                y_centre = ROUND( y_pixel ) - y_sub_min - pixels->y_position;

                x_min = x_centre - width / 2;
                x_max = x_min + width - 1;
                y_min = y_centre - width / 2;
                y_max = y_min + width - 1;

                if( x_min < 0 )
                {
                    x_max = x_max - x_min;
                    x_min = 0;
                }
                else if( x_max >= pixels->x_size )
                {
                    x_min = pixels->x_size - 1 - (x_max - x_min);
                    x_max = pixels->x_size - 1;
                }

                if( y_min < 0 )
                {
                    y_max = y_max - y_min;
                    y_min = 0;
                }
                else if( y_max >= pixels->y_size )
                {
                    y_min = pixels->y_size - 1 - (y_max - y_min);
                    y_max = pixels->y_size - 1;
                }
            }

            edge_index = 0;

            force_update_limits = TRUE;
        }

        if( x_min < 0 )
            x_min = 0;
        if( x_max >= pixels->x_size )
            x_max = pixels->x_size-1;

        if( y_min < 0 )
            y_min = 0;
        if( y_max >= pixels->y_size )
            y_max = pixels->y_size-1;

        if( x_min <= x_max && y_min <= y_max )
        {
            create_volume_slice(
                    volume, filter_type,
                    slice_window->slice.volumes[volume_index].views[view_index]
                                                        .filter_width,
                    origin, x_axis, y_axis,
                    x_trans, y_trans, x_scale, y_scale,
                    (Volume) NULL, NEAREST_NEIGHBOUR, 0.0,
                    (Real *) 0, (Real *) 0, (Real *) 0,
                    0.0, 0.0, 0.0, 0.0,
                    x_sub_max - x_sub_min + 1, y_sub_max - y_sub_min + 1,
                    x_min, x_max, y_min, y_max,
                    RGB_PIXEL, continuity,
                    (unsigned short **) NULL, colour_map,
                    make_rgba_Colour( 0, 0, 0, 0 ),
                    slice_window->slice.render_storage,
                    TRUE, n_alloced_ptr, pixels );

            pixels->x_position += x_sub_min;
            pixels->y_position += y_sub_min;

            if( is_an_rgb_volume( volume ) &&
                slice_window->slice.volumes[volume_index].opacity != 1.0 )
            {
                opacity = ROUND( 255.0 *
                            slice_window->slice.volumes[volume_index].opacity );

                for_inclusive( x, x_min, x_max )
                {
                    for_inclusive( y, y_min, y_max )
                    {
                        colour = PIXEL_RGB_COLOUR( *pixels, x, y );
                        r = get_Colour_r( colour );
                        g = get_Colour_g( colour );
                        b = get_Colour_b( colour );
                        colour = make_rgba_Colour( r, g, b, opacity );
                        PIXEL_RGB_COLOUR( *pixels, x, y ) = colour;
                    }
                }
            }

            n_pixels_drawn = (x_max - x_min + 1) * (y_max - y_min + 1);
        }

        if( force_update_limits ||
            x_min < vol_info->views[view_index].x_min_update[which_volume] )
            vol_info->views[view_index].x_min_update[which_volume] = x_min;
        if( force_update_limits ||
            x_max > vol_info->views[view_index].x_max_update[which_volume] )
            vol_info->views[view_index].x_max_update[which_volume] = x_max;
        if( force_update_limits ||
            y_min < vol_info->views[view_index].y_min_update[which_volume] )
            vol_info->views[view_index].y_min_update[which_volume] = y_min;
        if( force_update_limits ||
            y_max > vol_info->views[view_index].y_max_update[which_volume] )
            vol_info->views[view_index].y_max_update[which_volume] = y_max;

        continuing_flag = TRUE;

        *finished = (vol_info->views[view_index].x_min_update[which_volume] ==
                                                 0 &&
                 vol_info->views[view_index].x_max_update[which_volume] ==
                                                 pixels->x_size-1 &&
                 vol_info->views[view_index].y_min_update[which_volume] ==
                                                 0 &&
                 vol_info->views[view_index].y_max_update[which_volume] ==
                                                 pixels->y_size-1);
    }
    while( !interrupted && n_pixels_drawn == 0 && !(*finished) );

    vol_info->views[view_index].edge_index[which_volume] = edge_index;

    if( first_flag && !(*finished) )
    {
        empty = make_rgba_Colour( 0, 0, 0, 0 );
        for_less( y, 0, pixels->y_size )
        {
            for_less( x, 0, vol_info->views[view_index].x_min_update
                                [which_volume] )
                PIXEL_RGB_COLOUR( *pixels, x, y ) = empty;

            for_less( x, vol_info->views[view_index].x_max_update
                                [which_volume]+1,
                      pixels->x_size )
                PIXEL_RGB_COLOUR( *pixels, x, y ) = empty;
        }

        for_inclusive( x, vol_info->views[view_index].x_min_update
                                [which_volume],
                          vol_info->views[view_index].x_max_update
                                [which_volume] )
        {
            for_less( y, 0, vol_info->views[view_index].y_min_update
                                [which_volume] )
                PIXEL_RGB_COLOUR( *pixels, x, y ) = empty;

            for_less( y, vol_info->views[view_index].y_max_update
                                [which_volume]+1,
                      pixels->y_size )
                PIXEL_RGB_COLOUR( *pixels, x, y ) = empty;
        }
    }

    return( n_pixels_drawn );
}

public  int  rebuild_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    BOOLEAN           incremental_flag,
    BOOLEAN           interrupted,
    BOOLEAN           continuing_flag,
    BOOLEAN           *finished )
{
    object_struct  *pixels_object;
    pixels_struct  *pixels;

    pixels_object = get_slice_pixels_object( slice_window, volume_index,
                                             view_index );
    pixels = get_pixels_ptr( pixels_object );

    return( render_slice_to_pixels( slice_window, volume_index, view_index, 0,
                            slice_window->slice.volumes[volume_index].
                                                      colour_table,
                            slice_window->slice.volumes[volume_index].
                                      views[view_index].filter_type,
                            slice_window->slice.degrees_continuity,
                            pixels, incremental_flag,
                            interrupted, continuing_flag, finished ) );
}

public  void  rebuild_slice_text(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    int            axis_index, x_index, y_index;
    object_struct  *text_object;
    text_struct    *text;
    char           buffer[EXTREMELY_LARGE_STRING_SIZE];
    STRING         format;
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

        (void) sprintf( buffer, format, current_voxel[axis_index] );

        replace_string( &text->string, create_string(buffer) );

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
    Real           v1[N_DIMENSIONS], v2[N_DIMENSIONS];
    int            sizes[N_DIMENSIONS];
    int            x_index, y_index, axis_index, volume_index;
    Real           x_trans, y_trans, x_scale, y_scale;
    Real           origin[MAX_DIMENSIONS];
    Real           x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    Real           world_origin[MAX_DIMENSIONS];
    Real           world_x_axis[MAX_DIMENSIONS], world_y_axis[MAX_DIMENSIONS];
    Real           x1, y1, z1, x2, y2, z2;

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
            modify_pixels_size( &slice_window->slice.slice_views[view_index].
                                       n_atlas_pixels_alloced,
                                pixels,
                                volume_pixels->x_size,
                                volume_pixels->y_size, RGB_PIXEL );
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
                        (Real) volume_pixels->x_position,
                        (Real) volume_pixels->y_position,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v1 );
        (void) convert_slice_pixel_to_voxel( volume,
                        (Real) volume_pixels->x_position+1.0,
                        (Real) volume_pixels->y_position,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v2 );

        convert_voxel_to_world( volume, v1, &x1, &y1, &z1 );
        convert_voxel_to_world( volume, v2, &x2, &y2, &z2 );

        world_origin[X] = x1;
        world_origin[Y] = y1;
        world_origin[Z] = z1;

        world_x_axis[X] = x2 - x1;
        world_x_axis[Y] = y2 - y1;
        world_x_axis[Z] = z2 - z1;

        (void) convert_slice_pixel_to_voxel( volume,
                        (Real) volume_pixels->x_position,
                        (Real) volume_pixels->y_position+1.0,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v2 );

        convert_voxel_to_world( volume, v2, &x2, &y2, &z2 );

        world_y_axis[X] = x2 - x1;
        world_y_axis[Y] = y2 - y1;
        world_y_axis[Z] = z2 - z1;

        get_volume_sizes( volume, sizes );

        visible = render_atlas_slice_to_pixels( &slice_window->slice.atlas,
                        pixels->data.pixels_rgb,
                        pixels->x_size, pixels->y_size,
                        world_origin, world_x_axis, world_y_axis );
    }
    else
        visible = FALSE;

    set_object_visibility( pixels_object, visible );
}

private  void  create_composite(
    int             n_slices,
    pixels_struct   *slices[],
    Colour          background_colour,
    int             *n_alloced,
    pixels_struct   *composite )
{
    Colour   *src, *dest, empty, c1, c2;
    int      r1, g1, b1, a1, r2, g2, b2, a2, weight;
    int      r, g, b, a;
    int      i, n_pixels, slice, x_min, x_max, y_min, y_max, x, y;

    if( n_slices == 0 )
    {
        if( *n_alloced == 0 )
            initialize_pixels( composite, 0, 0, 0, 0, 1.0, 1.0, RGB_PIXEL );
        else
        {
            composite->x_size = 0;
            composite->y_size = 0;
        }
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
        if( *n_alloced == 0 )
        {
            initialize_pixels( composite, 0, 0,
                               x_max - x_min + 1, y_max - y_min + 1,
                               1.0, 1.0, RGB_PIXEL );
            *n_alloced = composite->x_size * composite->y_size;
        }
        else
        {
            modify_pixels_size( n_alloced, composite,
                                x_max - x_min + 1, y_max - y_min + 1,
                                RGB_PIXEL );
        }
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
                    (a2 = get_Colour_a( c2 )) != 0 )
                {
                    a1 = get_Colour_a( c1 );
                    if( a2 == 255 || a1 == 0 )
                        *dest = *src;
                    else
                    {
                        r1 = get_Colour_r( c1 );
                        g1 = get_Colour_g( c1 );
                        b1 = get_Colour_b( c1 );

                        r2 = get_Colour_r( c2 );
                        g2 = get_Colour_g( c2 );
                        b2 = get_Colour_b( c2 );

                        weight = (255 - a2) * a1;
                        a2 *= 255;
                        r = a2 * r2 + weight * r1;
                        g = a2 * g2 + weight * g1;
                        b = a2 * b2 + weight * b1;
                        a = a2 + weight;
                        if( a > 0 )
                        {
                            r /= a;
                            g /= a;
                            b /= a;
                            a /= 255;
                        }

                        *dest = make_rgba_Colour( r, g, b, a );
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

            if( get_Colour_a_0_1( get_colour_coding_under_colour(
                     &slice_window->slice.volumes[i].colour_coding ) ) < 1.0 )
                return( TRUE );

            if( get_Colour_a_0_1( get_colour_coding_over_colour(
                     &slice_window->slice.volumes[i].colour_coding ) ) < 1.0 )
                return( TRUE );
        }
    }

    return( FALSE );
}

private  void  create_volume_and_label_composite(
    display_struct        *slice_window,
    int                   view_index,
    pixels_struct         *composite_pixels )
{
    int                   v, n_slices;
    pixels_struct         **slices, *label_pixels;

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

    create_composite( n_slices, slices,
                      G_get_background_colour(slice_window->window),
                      &slice_window->slice.slice_views[view_index].
                                                    n_composite_pixels_alloced,
                      composite_pixels );

    FREE( slices );
}

public  void  composite_volume_and_labels(
    display_struct        *slice_window,
    int                   view_index )
{
    pixels_struct         *composite_pixels;

    if( !composite_is_visible( slice_window, view_index ) ||
        get_n_volumes( slice_window ) == 0 )
    {
        return;
    }

    composite_pixels = get_pixels_ptr(
           get_composite_slice_pixels_object( slice_window, view_index ) );

    create_volume_and_label_composite( slice_window, view_index,
                                       composite_pixels );
}

public  int  rebuild_label_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    BOOLEAN           incremental_flag,
    BOOLEAN           interrupted,
    BOOLEAN           continuing_flag,
    BOOLEAN           *finished )
{
    pixels_struct   *pixels;

    pixels = get_pixels_ptr( get_label_slice_pixels_object(
                                   slice_window, volume_index, view_index ) );

    return( render_slice_to_pixels( slice_window, volume_index, view_index, 1,
                            slice_window->slice.volumes[volume_index].
                                                   label_colour_table,
                            NEAREST_NEIGHBOUR,
                            -1, pixels, incremental_flag,
                            interrupted, continuing_flag, finished ) );
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
