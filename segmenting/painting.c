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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/segmenting/painting.c,v 1.42 1996-04-17 17:50:21 david Exp $";
#endif

#include  <display.h>

private  DEF_EVENT_FUNCTION( right_mouse_down );
private  DEF_EVENT_FUNCTION( end_painting );
private  DEF_EVENT_FUNCTION( handle_update_painting );

private  int  update_paint_labels(
    display_struct  *slice_window );

private  void   update_brush(
    display_struct    *slice_window,
    int               x,
    int               y,
    BOOLEAN           erase_brush );

private  int  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2,
    int               label );

private  void  paint_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Real             start_voxel[],
    Real             end_voxel[],
    int              label );


public  int  get_current_paint_label(
    display_struct    *display )
{
    display_struct    *slice_window;

    if( get_slice_window( display, &slice_window ) )
        return( slice_window->slice.current_paint_label );
    else
        return( 0 );
}

public  void  initialize_voxel_labeling(
    display_struct    *slice_window )
{
    add_action_table_function( &slice_window->action_table,
                               RIGHT_MOUSE_DOWN_EVENT,
                               right_mouse_down );
    slice_window->slice.segmenting.n_starts_alloced = 0;
    slice_window->slice.segmenting.mouse_scale_factor =
                                               Initial_mouse_scale_factor;
    slice_window->slice.painting_view_index = -1;
    slice_window->slice.segmenting.fast_updating_allowed =
                           Default_fast_painting_flag;
    slice_window->slice.segmenting.cursor_follows_paintbrush =
                           Default_cursor_follows_paintbrush_flag;
}

public  void  delete_voxel_labeling(
    slice_window_struct    *slice )
{
    if( slice->segmenting.n_starts_alloced > 0 )
    {
        FREE( slice->segmenting.y_starts );
    }
}

private  int  scale_x_mouse(
    display_struct  *slice_window,
    int             x )
{
    Real   x_real;
    if( slice_window->slice.segmenting.mouse_scale_factor > 0.0 &&
        slice_window->slice.segmenting.mouse_scale_factor != 1.0 )
    {
        x_real = slice_window->slice.segmenting.x_mouse_start +
                 slice_window->slice.segmenting.mouse_scale_factor *
                 (x - slice_window->slice.segmenting.x_mouse_start);
        x = ROUND( x_real );
    }

    return( x );
}

private  int  scale_y_mouse(
    display_struct  *slice_window,
    int             y )
{
    Real   y_real;
    if( slice_window->slice.segmenting.mouse_scale_factor > 0.0 &&
        slice_window->slice.segmenting.mouse_scale_factor != 1.0 )
    {
        y_real = slice_window->slice.segmenting.y_mouse_start +
                 slice_window->slice.segmenting.mouse_scale_factor *
                 (y - slice_window->slice.segmenting.y_mouse_start);
        y = ROUND( y_real );
    }

    return( y );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( right_mouse_down )
{
    int             x_pixel, y_pixel, label, axis_index, volume_index;
    display_struct  *slice_window;

    if( !get_slice_window( display, &slice_window ) ||
        !get_axis_index_under_mouse( slice_window, &volume_index, &axis_index ))
        return( OK );

    push_action_table( &slice_window->action_table, NO_EVENT );

    add_action_table_function( &slice_window->action_table,
                               NO_EVENT,
                               handle_update_painting );

    add_action_table_function( &slice_window->action_table,
                               RIGHT_MOUSE_UP_EVENT,
                               end_painting );

    (void) G_get_mouse_position( slice_window->window, &x_pixel, &y_pixel );

    slice_window->slice.segmenting.x_mouse_start = x_pixel;
    slice_window->slice.segmenting.y_mouse_start = y_pixel;

    record_slice_under_mouse( slice_window, volume_index );

    if( is_shift_key_pressed( slice_window ) )
        label = 0;
    else
        label = get_current_paint_label( slice_window );

    (void) sweep_paint_labels( slice_window,
                               x_pixel, y_pixel, x_pixel, y_pixel, label );

    if( Draw_brush_outline &&
        find_slice_view_mouse_is_in( slice_window, x_pixel, y_pixel,
                                     &slice_window->slice.painting_view_index ))
    {
        slice_window->slice.brush_outline = create_object( LINES );
        initialize_lines( get_lines_ptr(slice_window->slice.brush_outline),
                          Brush_outline_colour );

        add_object_to_model( get_graphics_model( slice_window,
                       SLICE_MODEL1 + slice_window->slice.painting_view_index ),
                             slice_window->slice.brush_outline );
        update_brush( slice_window, x_pixel, y_pixel, FALSE );
    }
    else
        slice_window->slice.painting_view_index = -1;
    
    record_mouse_pixel_position( slice_window );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( end_painting )
{
    int   volume_index;

    remove_action_table_function( &display->action_table,
                                  RIGHT_MOUSE_UP_EVENT, end_painting );
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  handle_update_painting );
    pop_action_table( &display->action_table, NO_EVENT );

    volume_index = update_paint_labels( display );

    if( Draw_brush_outline &&
        display->slice.painting_view_index >= 0 )
    {
        remove_object_from_model( get_graphics_model( display,
                       SLICE_MODEL1 + display->slice.painting_view_index ),
                                  display->slice.brush_outline );

        delete_object( display->slice.brush_outline );
        display->slice.painting_view_index = -1;
    }

    set_slice_window_all_update( display, volume_index, UPDATE_LABELS );

    update_all_menu_text( display );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_update_painting )
{
    (void) update_paint_labels( display );

    return( OK );
}

private  BOOLEAN  get_brush_voxel_centre(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel,
    Real              voxel[],
    int               *volume_index,
    int               *view_index )
{
    BOOLEAN  inside;

    inside = get_volume_corresponding_to_pixel( slice_window, x_pixel, y_pixel,
                                        volume_index, view_index, voxel );

    if( inside && Snap_brush_to_centres )
    {
        voxel[X] = ROUND( voxel[X] );
        voxel[Y] = ROUND( voxel[Y] );
        voxel[Z] = ROUND( voxel[Z] );
    }

    return( inside );
}

private  int  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2,
    int               label )
{
    int         view_index, volume_index, volume_index2;
    Real        start_voxel[MAX_DIMENSIONS], end_voxel[MAX_DIMENSIONS];

    if( get_brush_voxel_centre( slice_window, x1, y1, start_voxel,
                                &volume_index, &view_index ) &&
        get_brush_voxel_centre( slice_window, x2, y2, end_voxel,
                                &volume_index2, &view_index ) &&
        volume_index == volume_index2 )
    {
        if( slice_window->slice.segmenting.cursor_follows_paintbrush )
            set_voxel_cursor_from_mouse_position( slice_window );
        paint_labels( slice_window, volume_index, view_index,
                      start_voxel, end_voxel, label );
    }
    else
        volume_index = get_current_volume_index( slice_window );

    return( volume_index );
}

private  int  update_paint_labels(
    display_struct  *slice_window )
{
    int  x, y, x_prev, y_prev, label, volume_index;

    if( pixel_mouse_moved(slice_window,&x,&y,&x_prev,&y_prev) )
    {
        if( is_shift_key_pressed( slice_window ) )
            label = 0;
        else
            label = get_current_paint_label( slice_window );

        volume_index = sweep_paint_labels( slice_window,
                          scale_x_mouse(slice_window,x_prev),
                          scale_y_mouse(slice_window,y_prev),
                          scale_x_mouse(slice_window,x),
                          scale_y_mouse(slice_window,y),
                          label );

        if( Draw_brush_outline )
        {
            update_brush( slice_window,
                          scale_x_mouse(slice_window,x),
                          scale_y_mouse(slice_window,y), TRUE );
        }
    }
    else
        volume_index = get_current_volume_index( slice_window );

    return( volume_index );
}

private  BOOLEAN  get_brush(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    int              *a1,
    int              *a2,
    int              *axis,
    Real             radius[] )
{
    int      c;
    BOOLEAN  okay;
    Real     separations[MAX_DIMENSIONS];

    okay = FALSE;

    if( slice_has_ortho_axes( slice_window, volume_index, view_index,
                              a1, a2, axis ) )
    {
        get_volume_separations( get_nth_volume(slice_window,volume_index),
                                separations );

        radius[*a1] = slice_window->slice.x_brush_radius /
                      ABS( separations[*a1] );

        radius[*a2] = slice_window->slice.y_brush_radius /
                      ABS( separations[*a2] );
        radius[*axis] = slice_window->slice.z_brush_radius /
                      ABS( separations[*axis] );

        for_less( c, 0, N_DIMENSIONS )
        {
            if( radius[c] != 0.0 && radius[c] < 0.5 )
                radius[c] = 0.5;
        }

        okay = TRUE;
    }

    return( okay );
}

private  BOOLEAN  inside_swept_brush(
    Real       origin[],
    Vector     *scaled_delta,
    Real       radius[],
    int        voxel[] )
{
    int      c, n_non_zero;
    Real     d, mag, t, t_min, t_max;
    Point    voxel_offset, voxel_origin;
    Vector   delta;
    BOOLEAN  inside;

    n_non_zero = 0;

    for_less( c, 0, N_DIMENSIONS )
    {
        if( radius[c] == 0.0 )
            Vector_coord(voxel_offset,c) = 0.0;
        else
        {
            Vector_coord(voxel_offset,c) =
                                ((Real) voxel[c] - origin[c]) / radius[c];
            ++n_non_zero;
        }
    }

    if( n_non_zero == 0 )
    {
        if( scaled_delta == NULL )
        {
            fill_Vector( delta, 0.0, 0.0, 0.0 );
            scaled_delta = &delta;
        }

        fill_Point( voxel_origin, origin[X], origin[Y], origin[Z] );
        inside = clip_line_to_box( &voxel_origin, scaled_delta,
                              (Real) voxel[X] - 0.5, (Real) voxel[X] + 0.5,
                              (Real) voxel[Y] - 0.5, (Real) voxel[Y] + 0.5,
                              (Real) voxel[Z] - 0.5, (Real) voxel[Z] + 0.5,
                              &t_min, &t_max ) &&
                 t_min <= 1.0 && t_max >= 0.0;
    }
    else
    {
        if( scaled_delta != (Vector *) NULL )
            d = DOT_VECTORS( *scaled_delta, *scaled_delta );
        else
            d = 0.0;

        if( d != 0.0 )
        {
            t = DOT_VECTORS( voxel_offset, *scaled_delta ) / d;

            if( t < 0.0 )
                t = 0.0;
            else if( t > 1.0 )
                t = 1.0;

            for_less( c, 0, N_DIMENSIONS )
            {
                Vector_coord( voxel_offset, c ) -=
                                        t * Vector_coord(*scaled_delta,c);
            }
        }

        mag = DOT_VECTORS( voxel_offset, voxel_offset );

        inside = (mag <= 1.0);
    }

    return( inside );
}

private  void  fast_paint_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    int              a1,
    int              a2,
    int              axis,
    Real             start_voxel[],
    int              min_voxel[],
    int              max_voxel[],
    Vector           *scaled_delta,
    Real             radius[],
    int              label )
{
    Volume         volume;
    int            value, sizes[N_DIMENSIONS], tmp;
    Real           min_threshold, max_threshold, volume_value;
    int            ind[N_DIMENSIONS], new_n_starts, *y_starts, y_inc, x_inc;
    int            x_min_pixel, y_min_pixel, x_max_pixel, y_max_pixel;
    pixels_struct  *pixels;
    Real           x_offset, x_scale, y_offset, y_scale;
    Real           x_trans, y_trans;
    Real           real_x_start, real_x_end, real_y_start;
    int            i, j, x_start, x_end, y_start, y_end;
    Colour         colour;
    BOOLEAN        update_required;
 
    volume = get_nth_volume( slice_window, volume_index );
    min_threshold = slice_window->slice.segmenting.min_threshold;
    max_threshold = slice_window->slice.segmenting.max_threshold;
    update_required = FALSE;

    get_volume_sizes( volume, sizes );

    pixels = get_pixels_ptr( get_label_slice_pixels_object(
                               slice_window, volume_index, view_index ) );

    colour = get_colour_of_label( slice_window, volume_index, label );

    get_voxel_to_pixel_transform( slice_window, volume_index, view_index,
                                  &a1, &a2,
                                  &x_scale, &x_trans, &y_scale, &y_trans );

    if( x_scale >= 0.0 )
    {
        x_offset = 0.5;
        x_inc = 1;
    }
    else
    {
        x_offset = -0.5;
        x_inc = -1;
    }

    if( y_scale >= 0.0 )
    {
        y_offset = 0.5;
        y_inc = 1;
    }
    else
    {
        y_offset = -0.5;
        y_inc = -1;
    }

    x_trans -= (Real) pixels->x_position;
    y_trans -= (Real) pixels->y_position;

    ind[axis] = min_voxel[axis];

    new_n_starts = max_voxel[a2] - min_voxel[a2] + 3;
    if( new_n_starts > slice_window->slice.segmenting.n_starts_alloced )
    {
        SET_ARRAY_SIZE( slice_window->slice.segmenting.y_starts,
                        slice_window->slice.segmenting.n_starts_alloced,
                        new_n_starts, DEFAULT_CHUNK_SIZE );
        slice_window->slice.segmenting.n_starts_alloced = new_n_starts;
    }

    y_starts = slice_window->slice.segmenting.y_starts;

    for_inclusive( ind[a2], min_voxel[a2]-1, max_voxel[a2]+1 )
    {
        real_y_start = y_scale * ((Real) ind[a2] - y_offset) + y_trans;

        y_start = CEILING( real_y_start );

        if( y_start < 0 )
            y_start = 0;
        else if( y_start > pixels->y_size )
            y_start = pixels->y_size;

        y_starts[ind[a2] - min_voxel[a2]+1] = y_start;
    }

    real_x_start = x_scale * ((Real) min_voxel[a1] - x_offset) + x_trans;
    x_min_pixel = CEILING( real_x_start );
    real_x_start = x_scale * ((Real) max_voxel[a1] - x_offset) + x_trans;
    x_max_pixel = CEILING( real_x_start );
    if( x_min_pixel > x_max_pixel )
    {
        tmp = x_min_pixel;
        x_min_pixel = x_max_pixel;
        x_max_pixel = tmp;
    }

    real_y_start = y_scale * ((Real) min_voxel[a2] - y_offset) + y_trans;
    y_min_pixel = CEILING( real_y_start );
    real_y_start = y_scale * ((Real) max_voxel[a2] - y_offset) + y_trans;
    y_max_pixel = CEILING( real_y_start );
    if( y_min_pixel > y_max_pixel )
    {
        tmp = y_min_pixel;
        y_min_pixel = y_max_pixel;
        y_max_pixel = tmp;
    }

    for_inclusive( ind[a1], min_voxel[a1], max_voxel[a1] )
    {
        real_x_start = x_scale * ((Real) ind[a1] - x_offset) + x_trans;
        real_x_end = x_scale * ((Real) (ind[a1]+x_inc) - x_offset) + x_trans;

        x_start = CEILING( real_x_start );
        x_end = CEILING( real_x_end );

        if( x_start < 0 )
            x_start = 0;
        if( x_end > pixels->x_size )
            x_end = pixels->x_size;

        for_inclusive( ind[a2], min_voxel[a2], max_voxel[a2] )
        {
            y_start = y_starts[ind[a2] - min_voxel[a2]+1];
            y_end = y_starts[ind[a2] - min_voxel[a2]+1 + y_inc];

            if( inside_swept_brush( start_voxel, scaled_delta,
                                    radius, ind ) )
            {
                value = get_voxel_label( slice_window, volume_index,
                                         ind[X], ind[Y], ind[Z] );

                if( value == label )
                    continue;

                if( min_threshold < max_threshold )
                {
                    volume_value = get_volume_real_value( volume,
                                               ind[X], ind[Y], ind[Z], 0, 0 );

                    if( volume_value < min_threshold ||
                        volume_value > max_threshold )
                        continue;
                }

                set_voxel_label( slice_window, volume_index,
                                 ind[X], ind[Y], ind[Z], label );

                for_less( i, x_start, x_end )
                {
                    for_less( j, y_start, y_end )
                    {
                        PIXEL_RGB_COLOUR( *pixels, i, j ) = colour;
                    }
                }

                update_required = TRUE;
            }
        }
    }

    if( update_required )
    {
        set_slice_composite_update( slice_window, view_index,
                                    pixels->x_position + x_min_pixel,
                                    pixels->x_position + x_max_pixel,
                                    pixels->y_position + y_min_pixel,
                                    pixels->y_position + y_max_pixel );
    }
}

private  void  paint_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Real             start_voxel[],
    Real             end_voxel[],
    int              label )
{
    Volume         volume, label_volume;
    int            a1, a2, axis, value, c, sizes[N_DIMENSIONS];
    int            min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    Real           min_limit, max_limit;
    Real           min_threshold, max_threshold, volume_value;
    Real           radius[N_DIMENSIONS];
    Vector         scaled_delta;
    int            ind[N_DIMENSIONS];
    BOOLEAN        update_required;
 
    if( get_brush( slice_window, volume_index, view_index,
                   &a1, &a2, &axis, radius ) )
    {
        volume = get_nth_volume( slice_window, volume_index );
        label_volume = get_nth_label_volume( slice_window, volume_index );
        min_threshold = slice_window->slice.segmenting.min_threshold;
        max_threshold = slice_window->slice.segmenting.max_threshold;
        update_required = FALSE;
        get_volume_sizes( volume, sizes );

        for_less( c, 0, N_DIMENSIONS )
        {
            Vector_coord(scaled_delta,c) = end_voxel[c] - start_voxel[c];
            if( radius[c] != 0.0 )
                Vector_coord(scaled_delta,c) /= radius[c];
        }

        for_less( c, 0, N_DIMENSIONS )
        {
            min_limit = MIN( start_voxel[c], end_voxel[c] ) - radius[c];
            max_limit = MAX( start_voxel[c], end_voxel[c] ) + radius[c];

            if( min_limit == max_limit )
            {
                min_voxel[c] = ROUND( min_limit );
                max_voxel[c] = ROUND( min_limit );
            }
            else
            {
                min_voxel[c] = FLOOR( min_limit + 0.5 );
                max_voxel[c] = CEILING( max_limit + 0.5 );
            }

            if( min_voxel[c] < 0 )
                min_voxel[c] = 0;
            if( max_voxel[c] >= sizes[c] )
                max_voxel[c] = sizes[c] - 1;
        }

        if( slice_window->slice.segmenting.fast_updating_allowed &&
            radius[axis] == 0.0 &&
            label_volume != NULL &&
            is_label_volume_initialized( label_volume ) &&
            !slice_window->slice.volumes[volume_index].
                             views[view_index].update_labels_flag )
        {
            fast_paint_labels( slice_window, volume_index, view_index,
                               a1, a2, axis,
                               start_voxel, min_voxel, max_voxel,
                               &scaled_delta, radius, label );
        }
        else
        {
            for_inclusive( ind[a1], min_voxel[a1], max_voxel[a1] )
            {
                for_inclusive( ind[a2], min_voxel[a2], max_voxel[a2] )
                {
                    for_inclusive( ind[axis], min_voxel[axis], max_voxel[axis] )
                    {
                        if( inside_swept_brush( start_voxel, &scaled_delta,
                                                radius, ind ) )
                        {
                            value = get_voxel_label( slice_window, volume_index,
                                                     ind[X], ind[Y], ind[Z] );

                            if( value == label )
                                continue;

                            if( min_threshold < max_threshold )
                            {
                                volume_value = get_volume_real_value( volume,
                                              ind[X], ind[Y], ind[Z], 0, 0 );

                                if( volume_value < min_threshold ||
                                    volume_value > max_threshold )
                                    continue;
                            }

                            set_voxel_label( slice_window, volume_index,
                                             ind[X], ind[Y], ind[Z], label );

                            update_required = TRUE;
                        }
                    }
                }
            }
        }

        if( update_required )
        {
            set_slice_window_all_update( slice_window, volume_index,
                                         UPDATE_LABELS );
        }
    }
}

typedef  enum  { POSITIVE_X, POSITIVE_Y, NEGATIVE_X, NEGATIVE_Y,
                 N_DIRECTIONS } Directions;

static  int  dx[N_DIRECTIONS] = { 1,  0, -1,  0 };
static  int  dy[N_DIRECTIONS] = { 0,  1,  0, -1 };

private   void    add_point_to_contour(
    int              x_centre_pixel,
    int              y_centre_pixel,
    int              a1,
    int              a2,
    Real             x_scale,
    Real             x_trans,
    Real             y_scale,
    Real             y_trans,
    int              voxel[],
    Directions       dir,
    lines_struct     *lines )
{
    int     x_pixel, y_pixel, next_dir;
    Real    real_x_pixel, real_y_pixel;
    Point   point;

    next_dir = (dir + 1) % N_DIRECTIONS;

    real_x_pixel = x_scale * ((Real) voxel[a1] +
                              (Real) (dx[dir] + (Real) dx[next_dir]) / 2.0) +
                   x_trans;
    real_y_pixel = y_scale * ((Real) voxel[a2] +
                              (Real) (dy[dir] + (Real) dy[next_dir]) / 2.0) +
                   y_trans;

    x_pixel = ROUND( real_x_pixel );
    y_pixel = ROUND( real_y_pixel );

    if( x_pixel < x_centre_pixel )
        x_pixel -= Brush_outline_offset;
    else if( x_pixel > x_centre_pixel )
        x_pixel += Brush_outline_offset;

    if( y_pixel < y_centre_pixel )
        y_pixel -= Brush_outline_offset;
    else if( y_pixel > y_centre_pixel )
        y_pixel += Brush_outline_offset;

    fill_Point( point, (Real) x_pixel, (Real) y_pixel, 0.0 );

    add_point_to_line( lines, &point );
}

private  BOOLEAN  neighbour_is_inside(
    Real       centre[],
    Real       radius[],
    int        a1,
    int        a2,
    int        voxel[],
    Directions dir )
{
    BOOLEAN   inside;

    voxel[a1] += dx[dir];
    voxel[a2] += dy[dir];

    inside = inside_swept_brush( centre, (Vector *) NULL, radius, voxel );

    voxel[a1] -= dx[dir];
    voxel[a2] -= dy[dir];

    return( inside );
}

private  void  get_brush_contour(
    display_struct    *slice_window,
    int               x_centre_pixel,
    int               y_centre_pixel,
    int               volume_index,
    int               view_index,
    int               a1,
    int               a2,
    Real              centre[N_DIMENSIONS],
    Real              radius[N_DIMENSIONS],
    int               start_voxel[N_DIMENSIONS],
    Directions        start_dir,
    lines_struct      *lines )
{
    int          current_voxel[N_DIMENSIONS];
    Directions   dir;
    Real         x_scale, x_trans, y_scale, y_trans;

    get_voxel_to_pixel_transform( slice_window, volume_index, view_index,
                                  &a1, &a2,
                                  &x_scale, &x_trans, &y_scale, &y_trans );

    current_voxel[X] = start_voxel[X];
    current_voxel[Y] = start_voxel[Y];
    current_voxel[Z] = start_voxel[Z];
    dir = start_dir;

    do
    {
        add_point_to_contour( x_centre_pixel, y_centre_pixel,
                              a1, a2, x_scale, x_trans, y_scale, y_trans,
                              current_voxel, dir, lines );

        dir = (dir + 1) % N_DIRECTIONS;

        while( neighbour_is_inside( centre, radius, a1, a2,
                                    current_voxel, dir ) )
        {
            current_voxel[a1] += dx[dir];
            current_voxel[a2] += dy[dir];
            dir = (dir - 1 + N_DIRECTIONS) % N_DIRECTIONS;
        }
    }
    while( current_voxel[X] != start_voxel[X] ||
           current_voxel[Y] != start_voxel[Y] ||
           current_voxel[Z] != start_voxel[Z] ||
           dir != start_dir );

    ADD_ELEMENT_TO_ARRAY( lines->indices, lines->end_indices[lines->n_items-1],
                          0, DEFAULT_CHUNK_SIZE );
}

private  BOOLEAN   get_lines_limits(
    lines_struct  *lines,
    int           *x_min,
    int           *x_max,
    int           *y_min,
    int           *y_max )
{
    int           i, x, y;

    *x_min = 0;
    *x_max = 0;
    *y_min = 0;
    *y_max = 0;

    for_less( i, 0, lines->n_points )
    {
        x = Point_x(lines->points[i]);
        y = Point_y(lines->points[i]);
        if( i == 0 )
        {
            *x_min = x;
            *x_max = x;
            *y_min = y;
            *y_max = y;
        }
        else
        {
            if( x < *x_min )
                *x_min = x;
            if( x > *x_max )
                *x_max = x;
            if( y < *y_min )
                *y_min = y;
            if( y > *y_max )
                *y_max = y;
        }
    }

    return( lines->n_points > 0 );
}

private  void   update_brush(
    display_struct    *slice_window,
    int               x,
    int               y,
    BOOLEAN           erase_brush )
{
    Real          centre[N_DIMENSIONS];
    int           view, axis, a1, a2, start_voxel[N_DIMENSIONS], volume_index;
    int           x_min, x_max, y_min, y_max;
    Real          radius[N_DIMENSIONS];
    lines_struct  *lines;


    lines = get_lines_ptr( slice_window->slice.brush_outline );

    if( erase_brush &&
        get_lines_limits( lines, &x_min, &x_max, &y_min, &y_max ) &&
        slice_window->slice.painting_view_index >= 0 )
    {
        set_slice_composite_update( slice_window,
                                    slice_window->slice.painting_view_index,
                                    x_min, x_max, y_min, y_max );
    }

    delete_lines( lines );
    initialize_lines( lines, Brush_outline_colour );

    if( get_brush_voxel_centre( slice_window, x, y, centre,
                                &volume_index, &view ) &&
        get_brush( slice_window, volume_index, view, &a1, &a2, &axis, radius ) )
    {
        start_voxel[a1] = ROUND( centre[a1] );
        start_voxel[a2] = ROUND( centre[a2] );
        start_voxel[axis] = ROUND( centre[axis] );

        while( inside_swept_brush( centre, (Vector *) NULL, radius,
                                   start_voxel ) )
            ++start_voxel[a1];

        if( start_voxel[a1] > ROUND( centre[a1] ) )
            --start_voxel[a1];

        get_brush_contour( slice_window, x, y, volume_index, view, a1, a2,
                           centre, radius, start_voxel, POSITIVE_X, lines );

        if( get_lines_limits( lines, &x_min, &x_max, &y_min, &y_max ) )
        {
            set_slice_composite_update( slice_window, view,
                                        x_min, x_max, y_min, y_max );
        }
    }
}

public  void  flip_labels_around_zero(
    display_struct  *slice_window )
{
    int             label_x, label_x_opp;
    int             int_voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    int             int_voxel_opp[MAX_DIMENSIONS];
    Real            voxel[MAX_DIMENSIONS], flip_voxel;
    Volume          label_volume;

    label_volume = get_label_volume( slice_window );

    convert_world_to_voxel( label_volume, 0.0, 0.0, 0.0, voxel );

    flip_voxel = voxel[X];

    get_volume_sizes( label_volume, sizes );

    for_less( int_voxel[X], 0, sizes[X] )
    {
        int_voxel_opp[X] = ROUND( flip_voxel +
                                  (flip_voxel - (Real) int_voxel[X]) );
        if( int_voxel_opp[X] <= int_voxel[X] ||
            int_voxel_opp[X] < 0 || int_voxel_opp[X] >= sizes[X] )
            continue;

        for_less( int_voxel[Y], 0, sizes[Y] )
        {
            int_voxel_opp[Y] = int_voxel[Y];
            for_less( int_voxel[Z], 0, sizes[Z] )
            {
                int_voxel_opp[Z] = int_voxel[Z];

                label_x = get_voxel_label( slice_window,
                                 get_current_volume_index(slice_window),
                                 int_voxel[X], int_voxel[Y], int_voxel[Z] );
                label_x_opp = get_voxel_label( slice_window,
                        get_current_volume_index(slice_window),
                        int_voxel_opp[X], int_voxel_opp[Y], int_voxel_opp[Z] );

                set_voxel_label( slice_window,
                                 get_current_volume_index(slice_window),
                                 int_voxel_opp[X], int_voxel_opp[Y],
                                 int_voxel_opp[Z], label_x );
                set_voxel_label( slice_window,
                                 get_current_volume_index(slice_window),
                                 int_voxel[X], int_voxel[Y], int_voxel[Z],
                                 label_x_opp );
            }
        }
    }
}

public  void  translate_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              delta[] )
{
    int               c, label;
    int               src_voxel[MAX_DIMENSIONS], dest_voxel[MAX_DIMENSIONS];
    int               sizes[MAX_DIMENSIONS];
    int               first[MAX_DIMENSIONS], last[MAX_DIMENSIONS];
    int               increment[MAX_DIMENSIONS];
    progress_struct   progress;
    Volume            label_volume;

    label_volume = get_nth_label_volume( slice_window, volume_index );

    get_volume_sizes( label_volume, sizes );

    for_less( c, 0, N_DIMENSIONS )
    {
        if( delta[c] > 0 )
        {
            first[c] = sizes[c]-1;
            last[c] = -1;
            increment[c] = -1;
        }
        else
        {
            first[c] = 0;
            last[c] = sizes[c];
            increment[c] = 1;
        }
    }

    initialize_progress_report( &progress, FALSE, sizes[X] * sizes[Y],
                                "Translating Labels" );

    for( dest_voxel[X] = first[X];  dest_voxel[X] != last[X];
         dest_voxel[X] += increment[X] )
    {
        src_voxel[X] = dest_voxel[X] - delta[X];

        for( dest_voxel[Y] = first[Y];  dest_voxel[Y] != last[Y];
             dest_voxel[Y] += increment[Y] )
        {
            src_voxel[Y] = dest_voxel[Y] - delta[Y];

            for( dest_voxel[Z] = first[Z];  dest_voxel[Z] != last[Z];
                 dest_voxel[Z] += increment[Z] )
            {
                src_voxel[Z] = dest_voxel[Z] - delta[Z];

                if( int_voxel_is_within_volume( label_volume, src_voxel ) )
                {
                    label = get_voxel_label( slice_window, volume_index,
                                 src_voxel[X], src_voxel[Y], src_voxel[Z] );
                }
                else
                    label = 0;

                set_voxel_label( slice_window, volume_index,
                                 dest_voxel[X], dest_voxel[Y], dest_voxel[Z],
                                 label );
            }

            update_progress_report( &progress, dest_voxel[X] * sizes[Y] +
                                               dest_voxel[Y] + 1 );
        }
    }

    terminate_progress_report( &progress );
}

public  void  copy_labels_slice_to_slice(
    display_struct   *slice_window,
    int              volume_index,
    int              axis,
    int              src_voxel,
    int              dest_voxel,
    Real             min_threshold,
    Real             max_threshold )
{
    int               x, y, a1, a2, value;
    int               sizes[N_DIMENSIONS], src_indices[N_DIMENSIONS];
    int               dest_indices[N_DIMENSIONS];
    Real              volume_value;
    Volume            volume, label_volume;

    volume = get_nth_volume( slice_window, volume_index );
    label_volume = get_nth_label_volume( slice_window, volume_index );

    get_volume_sizes( label_volume, sizes );
    a1 = (axis + 1) % N_DIMENSIONS;
    a2 = (axis + 2) % N_DIMENSIONS;

    src_indices[axis] = src_voxel;
    dest_indices[axis] = dest_voxel;

    for_less( x, 0, sizes[a1] )
    {
        src_indices[a1] = x;
        dest_indices[a1] = x;
        for_less( y, 0, sizes[a2] )
        {
            src_indices[a2] = y;
            dest_indices[a2] = y;

            value = get_voxel_label( slice_window, volume_index,
                                     src_indices[X], src_indices[Y],
                                     src_indices[Z] );

            if( min_threshold < max_threshold )
            {
                volume_value = get_volume_real_value( volume,
                            dest_indices[X], dest_indices[Y], dest_indices[Z],
                            0, 0 );
                if( volume_value < min_threshold ||
                    volume_value > max_threshold )
                    value = 0;
            }

            set_voxel_label( slice_window, volume_index,
                             dest_indices[X], dest_indices[Y], dest_indices[Z],
                             value );
        }
    }
}
