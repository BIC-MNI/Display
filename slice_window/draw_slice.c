/**
 * \file draw_slice.c
 * \brief Drawing the slice window and its controls
 *
 * \copyright
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
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
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
                 TEXT_INDEX,
                 FOV_INDEX,
               } Slice_model_indices;

typedef  enum  { DIVIDER_INDEX } Full_window_indices;



  void  initialize_slice_models(
    display_struct    *slice_window )
{
    int               i, view;
    VIO_Point             point;
    lines_struct      *lines;
    polygons_struct   *polygons;
    object_struct     *object;
    model_struct      *model;
    VIO_Colour            colour;

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

        /*--- make cursor position text */

        object = create_object( TEXT );

        initialize_text( get_text_ptr(object), NULL,
                         Slice_text_colour,
                         (Font_types) Slice_text_font, Slice_text_font_size );

        add_object_to_model( model, object );

        /*--- make field of view text */
        object = create_object( TEXT );

        initialize_text( get_text_ptr(object), NULL,
                         Slice_text_colour,
                         (Font_types) Slice_text_font, Slice_text_font_size );

        add_object_to_model( model, object );
    }

    /* --- initialize readout values */

    model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );

    if( get_model_bitplanes(model) == OVERLAY_PLANES )
        colour = (VIO_Colour) Readout_text_colour;
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

  void  initialize_slice_models_for_volume(
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

  void  delete_slice_models_for_volume(
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

  void  rebuild_slice_divider(
    display_struct    *slice_window )
{
    model_struct   *model;
    VIO_Point          *points;
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

    fill_Point( points[0], (VIO_Real) left_panel_width,               0.0, 0.0 );
    fill_Point( points[1], (VIO_Real) left_panel_width, (VIO_Real) (y_size-1), 0.0 );

    fill_Point( points[2], (VIO_Real) (left_panel_width + left_slice_width),
                                         0.0, 0.0 );
    fill_Point( points[3], (VIO_Real) (left_panel_width + left_slice_width),
                           (VIO_Real) (y_size-1), 0.0 );

    fill_Point( points[4], (VIO_Real) left_panel_width, (VIO_Real) bottom_slice_height,
                           0.0 );
    fill_Point( points[5], (VIO_Real) (x_size-1),       (VIO_Real) bottom_slice_height,
                           0.0 );

    set_slice_viewport_update( slice_window, FULL_WINDOW_MODEL );
}

  Bitplane_types  get_slice_readout_bitplanes( void )
{
    if( G_has_overlay_planes() )
        return( (Bitplane_types) Slice_readout_plane );
    else
        return( NORMAL_PLANES );
}


/**
 * Calculate the ratio between two selected volumes.
 * \param slice_window The slice window structure.
 * \param voxel The voxel coordinates of the mouse.
 * \returns The ratio between the two volumes at this voxel, or
 * INFINITY if the denominator is zero.
 */
#ifndef INFINITY
#include <math.h>
#endif
#ifndef INFINITY
#define INFINITY (-1.0)
#endif

static VIO_Real
calculate_volume_ratio(display_struct *slice_window, VIO_Real voxel[])
{
    VIO_Volume volume_num;      /* Volume in the numerator. */
    VIO_Volume volume_den;      /* Volume in the denominator. */
    VIO_Real value_num;         /* Real value of the numerator. */
    VIO_Real value_den;         /* Real value of the denominator. */

    volume_num = get_nth_volume( slice_window,
                                 slice_window->slice.ratio_volume_numerator);
    volume_den = get_nth_volume( slice_window,
                                 slice_window->slice.ratio_volume_denominator);

    evaluate_volume( volume_den, voxel, NULL,
                     slice_window->slice.degrees_continuity,
                     FALSE, 0.0, &value_den, NULL, NULL );

    /* If the denominator is zero, don't even bother to calculate the
     * ratio.
     */
    if (value_den == 0.0)
        return INFINITY;

    evaluate_volume( volume_num, voxel, NULL,
                     slice_window->slice.degrees_continuity,
                     FALSE, 0.0, &value_num, NULL, NULL );

    return value_num / value_den;
}

/**
 * Calculate distance from mouse to either cursor or currently
 * selected marker.
 */
static VIO_Real
calculate_user_distance(display_struct *slice_window, 
                        const VIO_Real world[],
                        char *distance_origin)
{
  VIO_Point      mouse_point;
  display_struct *display;
  object_struct  *object;
  VIO_Point      origin_point;

  display = get_three_d_window( slice_window );

  /* See if a marker is selected.
   */
  if (get_current_object(display, &object) && object->object_type == MARKER) 
  {
    marker_struct *marker = get_marker_ptr(object);
    origin_point = marker->position;
    sprintf(distance_origin, "%d", get_current_object_index(display));
  }
  else
  {
    origin_point = display->three_d.cursor.origin;
    strcpy(distance_origin, "c");
  }
              
  fill_Point( mouse_point, world[VIO_X], world[VIO_Y], world[VIO_Z] );
  return distance_between_points(&origin_point, &mouse_point);
}

/**
 * Calculate and format the values that will appear in the "probe" or
 * "slice readout" section of the slice window. These are the numeric
 * information fields that generally appear at the lower left corner
 * of the slice window.
 */
void  
rebuild_probe(display_struct *slice_window)
{
    model_struct *model;        /* Graphics model for the readout (probe). */
    VIO_BOOL     active;        /* True if mouse is over a slice. */
    int          i;             /* Loop counter. */
    int          view_index;    /* The view under the mouse. */
    int          volume_index;  /* The currently selected volume. */
    VIO_Real     voxel[VIO_MAX_DIMENSIONS]; /* Current voxel coordinates. */
    VIO_Real     world[VIO_N_DIMENSIONS];   /* Current world coordinates. */
    VIO_Real     real_value;  /* Real voxel value */
    VIO_Real     voxel_value; /* Raw voxel value */
    int          label;       /* Label of current voxel. */
    VIO_Real     ratio;       /* Ratio between given volume voxels. */
    VIO_Real     distance_value; /* Distance from mouse to another point. */
    char         distance_origin[VIO_EXTREMELY_LARGE_STRING_SIZE];

    /*
     * Get the voxel coordinates corresponding to the current mouse
     * position, if the mouse is actively over one of the visible 
     * slice views.
     */
    active = get_voxel_in_slice_window( slice_window, voxel, &volume_index,
                                        &view_index );

    if( active )
    {
        int        ivoxel[VIO_MAX_DIMENSIONS];
        VIO_Volume volume = get_nth_volume( slice_window, volume_index );

        /* Get the current world coordinates.
         */
        convert_voxel_to_world( volume, voxel,
                                &world[VIO_X], &world[VIO_Y], &world[VIO_Z]);

        /* Get the "real" (scaled) value of the voxel under the mouse.
         */
        (void) evaluate_volume( volume, voxel, NULL,
                                slice_window->slice.degrees_continuity,
                                FALSE, 0.0, &real_value, NULL, NULL );

        /* Get the "raw" value of the voxel.
         */
        voxel_value = convert_value_to_voxel( volume, real_value );

        /* Convert the voxel coordinates to integer format.
         */
        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, ivoxel );

        label = get_voxel_label( slice_window, volume_index,
                                 ivoxel[VIO_X], ivoxel[VIO_Y], ivoxel[VIO_Z] );

        if( slice_window->slice.ratio_enabled)
            ratio = calculate_volume_ratio(slice_window, voxel);

        distance_value = calculate_user_distance(slice_window, world, 
                                                 distance_origin);
    }

    /* Reformat each of the text objects in the slice readout model. */

    model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );

    for_less( i, 0, N_READOUT_MODELS )
    {
        char        buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
        text_struct *text;
        int         x_pos, y_pos;

        buffer[0] = VIO_END_OF_STRING;

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
                                voxel[VIO_X] );
                break;
            case Y_VOXEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_y_voxel_format,
                                voxel[VIO_Y] );
                break;
            case Z_VOXEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_z_voxel_format,
                                voxel[VIO_Z] );
                break;
            case X_WORLD_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_x_world_format,
                                world[VIO_X] );
                break;
            case Y_WORLD_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_y_world_format,
                                world[VIO_Y] );
                break;
            case Z_WORLD_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_z_world_format, 
                                world[VIO_Z] );
                break;
            case VOXEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_voxel_format, 
                                voxel_value );
                break;
            case VAL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_val_format, real_value );
                break;
            case LABEL_PROBE_INDEX:
                (void) sprintf( buffer, Slice_probe_label_format, label );
                break;
            case RATIO_PROBE_INDEX:
                if( slice_window->slice.ratio_enabled )
                {
                    (void) sprintf( buffer, Slice_probe_ratio_format, 
                                    slice_window->slice.ratio_volume_numerator,
                                    slice_window->slice.ratio_volume_denominator,
                                    ratio );
                }
                break;

            case DISTANCE_PROBE_INDEX:
                (void) sprintf(buffer, Slice_probe_distance_format, distance_origin, distance_value);
                break;
            }
        }

        x_pos = Probe_x_pos + i * Probe_x_delta;
        switch (i)
        {
        case VOLUME_INDEX:
          y_pos = Probe_y_pos + (N_READOUT_MODELS-i-1) * Probe_y_delta + 
            (3 * Probe_y_pos);
          break;
        case X_VOXEL_PROBE_INDEX:
        case Y_VOXEL_PROBE_INDEX:
        case Z_VOXEL_PROBE_INDEX:
          y_pos = Probe_y_pos + (N_READOUT_MODELS-i-1) * Probe_y_delta + 
            (2 * Probe_y_pos);
          break;
        case X_WORLD_PROBE_INDEX:
        case Y_WORLD_PROBE_INDEX:
        case Z_WORLD_PROBE_INDEX:
          y_pos = Probe_y_pos + (N_READOUT_MODELS-i-1) * Probe_y_delta + 
            (1 * Probe_y_pos);
          break;
        case VOXEL_PROBE_INDEX:
        case VAL_PROBE_INDEX:
        case LABEL_PROBE_INDEX:
        case RATIO_PROBE_INDEX:
        case DISTANCE_PROBE_INDEX:
          y_pos = Probe_y_pos + (N_READOUT_MODELS-i-1) * Probe_y_delta;
          break;
        }

        /* Set the text and position of the text object.
         * We should not really have to reset the origin every time, 
         * but it is probably not terribly expensive to calculate.
         */
        text = get_text_ptr( model->objects[i] );
        delete_string( text->string );
        text->string = create_string( buffer );
        fill_Point( text->origin, x_pos, y_pos, 0.0 );
    }

    set_slice_viewport_update( slice_window, SLICE_READOUT_MODEL );
}

static  void  get_cursor_size(
    int    slice_index,
    VIO_Real   *hor_start,
    VIO_Real   *hor_end,
    VIO_Real   *vert_start,
    VIO_Real   *vert_end )
{
    switch( slice_index )
    {
    case 0:
    default:
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

  void  get_slice_cross_section_direction(
    display_struct    *slice_window,
    int               view_index,
    int               section_index,
    VIO_Vector            *in_plane_axis )
{
    int            c, volume_index;
    VIO_Real       perp_axis[VIO_MAX_DIMENSIONS];
    VIO_Real       separations[VIO_MAX_DIMENSIONS];
    VIO_Real       plane_axis[VIO_MAX_DIMENSIONS];
    VIO_Vector         plane_normal, perp_normal;

    volume_index = get_current_volume_index( slice_window );

    get_volume_separations( get_volume(slice_window), separations );
    get_slice_perp_axis( slice_window, volume_index, section_index, perp_axis );
    get_slice_perp_axis( slice_window, volume_index, view_index, plane_axis );

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        separations[c] = VIO_FABS( separations[c] );
        Vector_coord( plane_normal, c ) = (VIO_Point_coord_type)
                                             (plane_axis[c] * separations[c]);
        Vector_coord( perp_normal, c ) = (VIO_Point_coord_type)
                                             (perp_axis[c] * separations[c]);
    }

    CROSS_VECTORS( *in_plane_axis, plane_normal, perp_normal );

    for_less( c, 0, VIO_N_DIMENSIONS )
        Vector_coord( *in_plane_axis, c ) /= (VIO_Point_coord_type) separations[c];
}

  void  rebuild_slice_unfinished_flag(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct      *model;
    VIO_Real              x_size, y_size, width;
    VIO_Point             *points;
    object_struct     *object;
    polygons_struct   *polygons;
    int               x_min, x_max, y_min, y_max;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+UNFINISHED_BAR];
    polygons = get_polygons_ptr( object );
    points = polygons->points;

    get_slice_model_viewport( slice_window, SLICE_MODEL1 + view_index,
                              &x_min, &x_max, &y_min, &y_max );

    x_size = (VIO_Real) (x_max - x_min + 1);
    y_size = (VIO_Real) (y_max - y_min + 1);
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

  VIO_BOOL  get_slice_unfinished_flag_visibility(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct      *model;
    object_struct     *object;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+UNFINISHED_BAR];
    return( get_object_visibility( object ) );
}

  void  set_slice_unfinished_flag_visibility(
    display_struct    *slice_window,
    int               view_index,
    VIO_BOOL           state )
{
    model_struct      *model;
    object_struct     *object;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+UNFINISHED_BAR];
    set_object_visibility( object, state );
}

#define  EXTRA_PIXELS   10

  void  rebuild_slice_cross_section(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    int            sizes[VIO_MAX_DIMENSIONS];
    int            c, section_index, x_min, x_max, y_min, y_max;
    VIO_Real           x1, y1, x2, y2, dx, dy, len, t_min, t_max;
    VIO_Real       separations[VIO_MAX_DIMENSIONS];
    VIO_Real       voxel1[VIO_MAX_DIMENSIONS], voxel2[VIO_MAX_DIMENSIONS];
    VIO_Point          origin, v1, v2, p1, p2;
    VIO_Vector         in_plane_axis, direction;
    object_struct  *object;
    lines_struct   *lines;
    VIO_Real       current_voxel[VIO_MAX_DIMENSIONS];

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

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        separations[c] = VIO_FABS( separations[c] );
        Point_coord( origin, c ) = (VIO_Point_coord_type) current_voxel[c];
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
                           -0.5, (VIO_Real) sizes[VIO_X]-0.5,
                           -0.5, (VIO_Real) sizes[VIO_Y]-0.5,
                           -0.5, (VIO_Real) sizes[VIO_Z]-0.5,
                           &t_min, &t_max ) )
    {
        set_object_visibility( object, FALSE );
        return;
    }

    GET_POINT_ON_RAY( v1, origin, in_plane_axis, t_min );
    GET_POINT_ON_RAY( v2, origin, in_plane_axis, t_max );

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        voxel1[c] = (VIO_Real) Point_coord(v1,c);
        voxel2[c] = (VIO_Real) Point_coord(v2,c);
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
        x1 -= (VIO_Real) EXTRA_PIXELS * dx / len;
        y1 -= (VIO_Real) EXTRA_PIXELS * dy / len;
        x2 += (VIO_Real) EXTRA_PIXELS * dx / len;
        y2 += (VIO_Real) EXTRA_PIXELS * dy / len;
    }

    get_slice_model_viewport( slice_window, SLICE_MODEL1 + view_index,
                              &x_min, &x_max, &y_min, &y_max );

    fill_Point( origin, x1, y1, 0.0 );
    fill_Vector( direction, x2 - x1, y2 - y1, 0.0 );

    if( !clip_line_to_box( &origin, &direction, 
                           0.0, (VIO_Real) (x_max - x_min),
                           0.0, (VIO_Real) (y_max - y_min),
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

  void  rebuild_slice_crop_box(
    display_struct    *slice_window,
    int               view_index )
{
    int            volume_index, x_index, y_index, axis;
    model_struct   *model;
    object_struct  *object;
    lines_struct   *lines;
    VIO_Real           voxel[VIO_MAX_DIMENSIONS], x, y;
    VIO_BOOL        visibility;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = model->objects[2*slice_window->slice.n_volumes+
                            CROP_BOX_INDEX];

    volume_index = get_current_volume_index( slice_window );

    visibility = FALSE;

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
            visibility = TRUE;
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

  void  rebuild_slice_cursor(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    object_struct  *obj1, *obj2;
    int            x_index, y_index, axis, volume_index;
    VIO_Real           x_left, x_right, y_bottom, y_top, dx, dy;
    VIO_Real           x_centre, y_centre, tmp;
    lines_struct   *lines1, *lines2;
    VIO_Real       current_voxel[VIO_MAX_DIMENSIONS];
    int            x_min, x_max, y_min, y_max;
    VIO_Real           hor_pixel_start, hor_pixel_end;
    VIO_Real           vert_pixel_start, vert_pixel_end;
    int            volume, n_volumes;
    VIO_BOOL        visible;

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
    else if( x_centre > (VIO_Real) (x_max - x_min) )
    {
        dx = (VIO_Real) x_max - (VIO_Real) x_min - x_centre;
        x_centre = (VIO_Real) x_max - (VIO_Real) x_min;
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
    else if( y_centre > (VIO_Real) (y_max - y_min) )
    {
        dy = (VIO_Real) y_max - (VIO_Real) y_min - y_centre;
        y_centre = (VIO_Real) y_max - (VIO_Real) y_min;
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

  object_struct  *get_slice_pixels_object(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    return( model->objects[2*volume_index] );
}

  object_struct  *get_label_slice_pixels_object(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    return( model->objects[2*volume_index+1] );
}

static  object_struct  *get_atlas_slice_pixels_object(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    return( model->objects[2*slice_window->slice.n_volumes+
                           ATLAS_SLICE_INDEX] );
}

  object_struct  *get_composite_slice_pixels_object(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;

    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    return( model->objects[2*slice_window->slice.n_volumes+
                           COMPOSITE_SLICE_INDEX] );
}

static  int  render_slice_to_pixels(
    display_struct        *slice_window,
    int                   volume_index,
    int                   view_index,
    int                   which_volume,
    VIO_Colour            colour_table[],
    colour_coding_struct  *colour_coding,
    VIO_Filter_types      filter_type,
    int                   continuity,
    pixels_struct         *pixels,
    VIO_BOOL               incremental_flag,
    VIO_BOOL               interrupted,
    VIO_BOOL               continuing_flag,
    VIO_BOOL               *finished )
{
    int                   n_pixels_drawn, n_pixels_redraw;
    int                   width, x_min, x_max, y_min, y_max;
    int                   x_centre, y_centre, edge_index;
    int                   x_sub_min, x_sub_max, y_sub_min, y_sub_max;
    int                   x, y, height, *n_alloced_ptr;
    int                   r, g, b, opacity;
    VIO_Colour                empty, colour;
    VIO_Real                  x_pixel, y_pixel;
    VIO_Real                  x_trans, y_trans, x_scale, y_scale;
    VIO_Real                  current_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real                  origin[VIO_MAX_DIMENSIONS];
    VIO_Real                  x_axis[VIO_MAX_DIMENSIONS], y_axis[VIO_MAX_DIMENSIONS];
    VIO_BOOL               first_flag, force_update_limits;
    VIO_Colour                **colour_map;
    loaded_volume_struct  *vol_info;
    VIO_Volume                volume;

    if( !continuing_flag )
    {
        if( pixels->x_size > 0 && pixels->y_size > 0 )
        {
            pixels->x_size = 0;
            pixels->y_size = 0;
        }
    }

    vol_info = &slice_window->slice.volumes[volume_index];

    x_trans = vol_info->views[view_index].x_trans;
    y_trans = vol_info->views[view_index].y_trans;
    x_scale = vol_info->views[view_index].x_scaling;
    y_scale = vol_info->views[view_index].y_scaling;
    (void) get_slice_subviewport( slice_window, view_index,
                                  &x_sub_min, &x_sub_max,
                                  &y_sub_min, &y_sub_max );

    x_trans -= (VIO_Real) x_sub_min;
    y_trans -= (VIO_Real) y_sub_min;

    get_slice_plane( slice_window, volume_index, view_index,
                     origin, x_axis, y_axis );

    if( which_volume == 0 )
        volume = get_nth_volume( slice_window, volume_index );
    else
        volume = get_nth_label_volume( slice_window, volume_index );

    if( is_an_rgb_volume(volume ) || colour_table == NULL )
        colour_map = NULL;
    else
        colour_map = &colour_table;

    first_flag = !continuing_flag;

    if( which_volume == 0 )
    {
        n_alloced_ptr = &vol_info->views[view_index].n_pixels_alloced;
    }
    else
    {
        n_alloced_ptr = &vol_info->views[view_index].n_label_pixels_alloced;
    }

    if( first_flag )
    {
        set_volume_slice_pixel_range(
                    volume, filter_type,
                    vol_info->views[view_index].filter_width,
                    origin, x_axis, y_axis,
                    x_trans, y_trans, x_scale, y_scale,
                    (VIO_Volume) NULL, NEAREST_NEIGHBOUR, 0.0,
                    (VIO_Real *) 0, (VIO_Real *) 0, (VIO_Real *) 0,
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
                width = (int) sqrt( (VIO_Real) n_pixels_redraw ) + 1;
                if( width < 1 )
                    width = 1;

                get_current_voxel( slice_window, volume_index, current_voxel );
                convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                    current_voxel, &x_pixel, &y_pixel );

                x_centre = VIO_ROUND( x_pixel ) - x_sub_min - pixels->x_position;
                y_centre = VIO_ROUND( y_pixel ) - y_sub_min - pixels->y_position;

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
            create_volume_slice_coding(
                    volume, filter_type,
                    vol_info->views[view_index].filter_width,
                    origin, x_axis, y_axis,
                    x_trans, y_trans, x_scale, y_scale,
                    (VIO_Volume) NULL, NEAREST_NEIGHBOUR, 0.0,
                    (VIO_Real *) 0, (VIO_Real *) 0, (VIO_Real *) 0,
                    0.0, 0.0, 0.0, 0.0,
                    x_sub_max - x_sub_min + 1, y_sub_max - y_sub_min + 1,
                    x_min, x_max, y_min, y_max,
                    RGB_PIXEL, continuity,
                    (unsigned short **) NULL, colour_map,
                    make_rgba_Colour( 0, 0, 0, 0 ),
                    colour_coding,
                    slice_window->slice.render_storage,
                    TRUE, n_alloced_ptr, pixels );

            pixels->x_position += x_sub_min;
            pixels->y_position += y_sub_min;

            if( is_an_rgb_volume( volume ) && vol_info->opacity != 1.0 )
            {
                opacity = VIO_ROUND( 255.0 * vol_info->opacity );

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

  int  rebuild_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    VIO_BOOL          incremental_flag,
    VIO_BOOL          interrupted,
    VIO_BOOL          continuing_flag,
    VIO_BOOL          *finished )
{
    object_struct  *pixels_object;
    pixels_struct  *pixels;
    loaded_volume_struct *vol_ptr = &slice_window->slice.volumes[volume_index];
    pixels_object = get_slice_pixels_object( slice_window, volume_index,
                                             view_index );
    pixels = get_pixels_ptr( pixels_object );

    return( render_slice_to_pixels( slice_window, volume_index, view_index, 0,
                                    vol_ptr->colour_table,
                                    &vol_ptr->colour_coding,
                                    vol_ptr->views[view_index].filter_type,
                                    slice_window->slice.degrees_continuity,
                                    pixels, incremental_flag,
                                    interrupted, continuing_flag, finished ) );
}

static VIO_BOOL
compute_slice_field_of_view(display_struct *display,
                            int volume_index,
                            int view_index, 
                            VIO_Real *fov_width_ptr,
                            VIO_Real *fov_height_ptr)
{
  VIO_Volume volume = get_nth_volume( display, volume_index );
  int      x_vp_min, x_vp_max, y_vp_min, y_vp_max;
  VIO_Real v_min[VIO_MAX_DIMENSIONS];
  VIO_Real v_max[VIO_MAX_DIMENSIONS];
  int      width_index, height_index, axis_index;
  VIO_Real w_min[VIO_N_DIMENSIONS];
  VIO_Real w_max[VIO_N_DIMENSIONS];
  int      x, y;

  if( !slice_has_ortho_axes( display, volume_index, view_index, 
                             &width_index, &height_index, &axis_index ))
  {
    return FALSE;
  }

  get_slice_viewport( display, view_index,
                      &x_vp_min, &x_vp_max, &y_vp_min, &y_vp_max );

  for (x = x_vp_min; x <= x_vp_max; x++)
  {
    for (y = y_vp_min; y <= y_vp_max; y++)
    {
      if (convert_pixel_to_voxel(display, volume_index, x, y, 
                                 v_min, &view_index))
      {
        x = x_vp_max + 1;
        break;
      }
    } 
  }

  for (x = x_vp_max; x >= x_vp_min; x--)
  {
    for (y = y_vp_max; y >= y_vp_min; y--)
    {
      if (convert_pixel_to_voxel(display, volume_index, x, y,
                                 v_max, &view_index))
      {
        x = x_vp_min - 1;
        break;
      }
    }
  }

  convert_voxel_to_world( volume, v_min,
                          &w_min[VIO_X], &w_min[VIO_Y], &w_min[VIO_Z]);
    
  convert_voxel_to_world( volume, v_max,
                          &w_max[VIO_X], &w_max[VIO_Y], &w_max[VIO_Z]);
    
  *fov_width_ptr = w_max[width_index] - w_min[width_index];
  *fov_height_ptr = w_max[height_index] - w_min[height_index];
  return TRUE;
}

/**
 * Recreates the field of view text if it is enabled.
 */
void 
rebuild_slice_field_of_view(display_struct *slice_window,
                           int view_index)
{
  int volume_index = get_current_volume_index(slice_window);
  /*
   * Get the model associated with this slice view.
   */
  model_struct *model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

  /*
   * Get the text object associated with the slice view model.
   */
  object_struct *text_object = model->objects[2 * slice_window->slice.n_volumes + FOV_INDEX];
  VIO_Real fov_w, fov_h;

  if (!Show_slice_field_of_view ||
      volume_index < 0 ||
      !get_slice_visibility(slice_window, volume_index, view_index) ||
      !compute_slice_field_of_view(slice_window, volume_index, view_index,
                                   &fov_w, &fov_h))
  {
    set_object_visibility(text_object, FALSE);
  }
  else
  {
    char buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    int x_min, x_max, y_min, y_max;
    text_struct *text_ptr;

    get_slice_viewport( slice_window, view_index,
                        &x_min, &x_max, &y_min, &y_max );

    sprintf(buffer, "%5.3gW %5.3gH", fov_w, fov_h);

    text_ptr = get_text_ptr( text_object );
    replace_string( &text_ptr->string, create_string(buffer) );
        
    fill_Point( text_ptr->origin, 
                (int) Point_x(Slice_index_offset), 
                (int) (y_max - y_min) - 2 * Point_y(Slice_index_offset),
                0.0 );

    set_object_visibility(text_object, TRUE);
  }
}

/**
 * Recreates the cursor position text for each slice view.
 */
  void  rebuild_slice_text(
    display_struct    *slice_window,
    int               view_index )
{
    model_struct   *model;
    int            axis_index, x_index, y_index;
    object_struct  *text_object;
    text_struct    *text;
    char           buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_STR        format;
    int            x_pos, y_pos;
    VIO_Real       current_voxel[VIO_MAX_DIMENSIONS];
    int            volume_index;

    /*
     * Get the model associated with this slice view.
     */
    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );

    /*
     * Get the text object associated with the slice view model.
     */
    text_object = model->objects[2 * slice_window->slice.n_volumes + TEXT_INDEX];
    volume_index = get_current_volume_index( slice_window );

    /*
     * See if we need to display a voxel position.
     * We use the X, Y, or Z axis if appropriate.
     */
    if( volume_index >= 0 &&
        get_slice_visibility(slice_window, volume_index, view_index) &&
        slice_has_ortho_axes( slice_window, volume_index,
                              view_index, &x_index, &y_index, &axis_index ))
    {
        set_object_visibility( text_object, TRUE );

        text = get_text_ptr( text_object );

        switch( axis_index )
        {
        case VIO_X:  format = Slice_index_x_format;  break;
        case VIO_Y:  format = Slice_index_y_format;  break;
        case VIO_Z:  format = Slice_index_z_format;  break;
        }

        get_current_voxel( slice_window, volume_index, current_voxel );

        (void) sprintf( buffer, format, current_voxel[axis_index] );

        replace_string( &text->string, create_string(buffer) );

        x_pos = (int) Point_x(Slice_index_offset);
        y_pos = (int) Point_y(Slice_index_offset);

        fill_Point( text->origin, x_pos, y_pos, 0.0 );
    }
    else
        set_object_visibility( text_object, FALSE );
}

  void  rebuild_atlas_slice_pixels(
    display_struct    *slice_window,
    int               view_index )
{
    VIO_BOOL        visible;
    object_struct  *pixels_object;
    pixels_struct  *pixels, *volume_pixels;
    VIO_Volume         volume;
    VIO_Real           v1[VIO_MAX_DIMENSIONS], v2[VIO_MAX_DIMENSIONS];
    int            sizes[VIO_MAX_DIMENSIONS];
    int            x_index, y_index, axis_index, volume_index;
    VIO_Real           x_trans, y_trans, x_scale, y_scale;
    VIO_Real           origin[VIO_MAX_DIMENSIONS];
    VIO_Real           x_axis[VIO_MAX_DIMENSIONS], y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real           world_origin[VIO_MAX_DIMENSIONS];
    VIO_Real           world_x_axis[VIO_MAX_DIMENSIONS], world_y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real           x1, y1, z1, x2, y2, z2;

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
                        (VIO_Real) volume_pixels->x_position,
                        (VIO_Real) volume_pixels->y_position,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v1 );
        (void) convert_slice_pixel_to_voxel( volume,
                        (VIO_Real) volume_pixels->x_position+1.0,
                        (VIO_Real) volume_pixels->y_position,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v2 );

        convert_voxel_to_world( volume, v1, &x1, &y1, &z1 );
        convert_voxel_to_world( volume, v2, &x2, &y2, &z2 );

        world_origin[VIO_X] = x1;
        world_origin[VIO_Y] = y1;
        world_origin[VIO_Z] = z1;

        world_x_axis[VIO_X] = x2 - x1;
        world_x_axis[VIO_Y] = y2 - y1;
        world_x_axis[VIO_Z] = z2 - z1;

        (void) convert_slice_pixel_to_voxel( volume,
                        (VIO_Real) volume_pixels->x_position,
                        (VIO_Real) volume_pixels->y_position+1.0,
                        origin, x_axis, y_axis,
                        x_trans, y_trans, x_scale, y_scale, v2 );

        convert_voxel_to_world( volume, v2, &x2, &y2, &z2 );

        world_y_axis[VIO_X] = x2 - x1;
        world_y_axis[VIO_Y] = y2 - y1;
        world_y_axis[VIO_Z] = z2 - z1;

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

static  void  create_composite(
    int             n_slices,
    pixels_struct   *slices[],
    VIO_Colour          background_colour,
    int             *n_alloced,
    pixels_struct   *composite )
{
    VIO_Colour   *src, *dest, empty, c1, c2;
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
        if( slices[slice]->x_position + slices[slice]->x_size - 1 > x_max )
            x_max = slices[slice]->x_position + slices[slice]->x_size - 1;
        if( slices[slice]->y_position + slices[slice]->y_size - 1 > y_max )
            y_max = slices[slice]->y_position + slices[slice]->y_size - 1;
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

static  VIO_BOOL  composite_is_visible(
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

static  void  create_volume_and_label_composite(
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

  void  composite_volume_and_labels(
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

int  rebuild_label_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    VIO_BOOL          incremental_flag,
    VIO_BOOL          interrupted,
    VIO_BOOL          continuing_flag,
    VIO_BOOL          *finished )
{
    pixels_struct   *pixels;

    pixels = get_pixels_ptr( get_label_slice_pixels_object(
                                   slice_window, volume_index, view_index ) );

    return( render_slice_to_pixels( slice_window, volume_index, view_index, 1,
                                    slice_window->slice.volumes[volume_index].
                                        label_colour_table,
                                    NULL,
                                    NEAREST_NEIGHBOUR,
                                    -1, pixels, incremental_flag,
                                    interrupted, continuing_flag, finished ) );
}

void  update_slice_pixel_visibilities(
    display_struct    *slice_window,
    int               view )
{
    int      volume_index;
    VIO_BOOL  visibility, composite_visibility;

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
