#include  <display.h>

private  DEF_EVENT_FUNCTION( right_mouse_down );
private  DEF_EVENT_FUNCTION( end_painting );
private  DEF_EVENT_FUNCTION( handle_update_painting );
private  void  paint_labels(
    display_struct   *slice_window,
    int              view_index,
    Real             start_voxel[],
    Real             end_voxel[],
    int              label );
private  void  update_paint_labels(
    display_struct  *display );
private  void  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2,
    int               label );
private  void   update_brush(
    display_struct    *slice_window,
    int               x,
    int               y );

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
}

private  DEF_EVENT_FUNCTION( right_mouse_down )    /* ARGSUSED */
{
    int             x_pixel, y_pixel, label;
    display_struct  *slice_window;

    if( !get_slice_window( display, &slice_window ) )
        return( OK );

    push_action_table( &slice_window->action_table, NO_EVENT );

    add_action_table_function( &slice_window->action_table,
                               NO_EVENT,
                               handle_update_painting );

    add_action_table_function( &slice_window->action_table,
                               RIGHT_MOUSE_UP_EVENT,
                               end_painting );

    (void) G_get_mouse_position( slice_window->window, &x_pixel, &y_pixel );

    record_slice_under_mouse( slice_window );

    if( is_shift_key_pressed( slice_window ) )
        label = 0;
    else
        label = get_current_paint_label( slice_window );

    sweep_paint_labels( slice_window, x_pixel, y_pixel, x_pixel, y_pixel,
                        label );

    if( Draw_brush_outline )
    {
        slice_window->slice.brush_outline = create_object( LINES );
        initialize_lines( get_lines_ptr(slice_window->slice.brush_outline),
                          Brush_outline_colour );

        add_object_to_model( get_graphics_model( slice_window, SLICE_MODEL ),
                             slice_window->slice.brush_outline );
        update_brush( slice_window, x_pixel, y_pixel );
    }
    
    record_mouse_pixel_position( slice_window );

    return( OK );
}

private  DEF_EVENT_FUNCTION( end_painting )     /* ARGSUSED */
{
    remove_action_table_function( &display->action_table,
                                  RIGHT_MOUSE_UP_EVENT, end_painting );
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  handle_update_painting );
    pop_action_table( &display->action_table, NO_EVENT );

    update_paint_labels( display );

    if( Draw_brush_outline )
    {
        remove_object_from_model( get_graphics_model( display, SLICE_MODEL ),
                                  display->slice.brush_outline );
        delete_object( display->slice.brush_outline );
    }

    set_slice_window_all_update( display );

    return( OK );
}

private  void  update_paint_labels(
    display_struct  *slice_window )
{
    int  x, y, x_prev, y_prev, label;

    if( pixel_mouse_moved(slice_window,&x,&y,&x_prev,&y_prev) )
    {
        if( is_shift_key_pressed( slice_window ) )
            label = 0;
        else
            label = get_current_paint_label( slice_window );

        sweep_paint_labels( slice_window, x_prev, y_prev, x, y, label );
        if( Draw_brush_outline )
            update_brush( slice_window, x, y );
    }
}

private  DEF_EVENT_FUNCTION( handle_update_painting )     /* ARGSUSED */
{
    update_paint_labels( display );

    return( OK );
}

private  BOOLEAN  get_brush_voxel_centre(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel,
    Real              voxel[],
    int               *view_index )
{
    BOOLEAN  inside;

    inside = convert_pixel_to_voxel( slice_window, x_pixel, y_pixel,
                                     voxel, view_index );

    if( inside && Snap_brush_to_centres )
    {
        voxel[X] = ROUND( voxel[X] );
        voxel[Y] = ROUND( voxel[Y] );
        voxel[Z] = ROUND( voxel[Z] );
    }

    return( inside );
}

private  void  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2,
    int               label )
{
    int         view_index;
    Real        start_voxel[MAX_DIMENSIONS], end_voxel[MAX_DIMENSIONS];

    if( get_brush_voxel_centre( slice_window, x1, y1, start_voxel,
                                &view_index ) &&
        get_brush_voxel_centre( slice_window, x2, y2, end_voxel,
                                &view_index ) )
    {
        paint_labels( slice_window, view_index, start_voxel, end_voxel,
                      label );
    }
}

private  BOOLEAN  get_brush(
    display_struct   *slice_window,
    int              view_index,
    int              *a1,
    int              *a2,
    int              *axis,
    Real             radius[] )
{
    BOOLEAN  okay;
    Volume   volume;
    Real     separations[MAX_DIMENSIONS];

    okay = FALSE;

    if( get_slice_window_volume( slice_window, &volume ) &&
        slice_window->slice.x_brush_radius > 0.0 &&
        slice_window->slice.y_brush_radius > 0.0 &&
        slice_has_ortho_axes( slice_window, view_index, a1, a2, axis ) )
    {
        get_volume_separations( volume, separations );

        radius[*a1] = slice_window->slice.x_brush_radius /
                      ABS( separations[*a1] );
        radius[*a2] = slice_window->slice.y_brush_radius /
                      ABS( separations[*a2] );
        radius[*axis] = slice_window->slice.z_brush_radius /
                      ABS( separations[*axis] );

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
    int    c;
    Real   d, mag, t;
    Point  voxel_offset;

    for_less( c, 0, N_DIMENSIONS )
    {
        if( radius[c] == 0.0 )
            Vector_coord(voxel_offset,c) = 0.0;
        else
            Vector_coord(voxel_offset,c) =
                                ((Real) voxel[c] - origin[c]) / radius[c];
    }

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

    return( mag <= 1.0 );
}

private  void  paint_labels(
    display_struct   *slice_window,
    int              view_index,
    Real             start_voxel[],
    Real             end_voxel[],
    int              label )
{
    Volume   volume, label_volume;
    int      a1, a2, axis, value, c, sizes[N_DIMENSIONS];
    int      min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    Real     min_limit, max_limit;
    Real     min_threshold, max_threshold, volume_value;
    Real     radius[N_DIMENSIONS];
    Vector   scaled_delta;
    int      ind[N_DIMENSIONS];
    BOOLEAN  update_required;

    if( get_slice_window_volume( slice_window, &volume ) &&
        get_brush( slice_window, view_index, &a1, &a2, &axis, radius ) )
    {
        label_volume = get_label_volume( slice_window );
        min_threshold = slice_window->slice.segmenting.min_threshold;
        max_threshold = slice_window->slice.segmenting.max_threshold;
        update_required = FALSE;
        get_volume_sizes( volume, sizes );

        for_less( c, 0, N_DIMENSIONS )
        {
            if( radius[c] == 0.0 )
                Vector_coord(scaled_delta,c) = 0.0;
            else
                Vector_coord(scaled_delta,c) = (end_voxel[c] - start_voxel[c])
                                               / radius[c];
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

        for_inclusive( ind[X], min_voxel[X], max_voxel[X] )
        {
            for_inclusive( ind[Y], min_voxel[Y], max_voxel[Y] )
            {
                for_inclusive( ind[Z], min_voxel[Z], max_voxel[Z] )
                {
                    if( inside_swept_brush( start_voxel, &scaled_delta,
                                            radius, ind ) )
                    {
                        value = get_volume_label_data( label_volume, ind );
                        if( value == label )
                            continue;

                        if( min_threshold < max_threshold )
                        {
                            GET_VALUE_3D( volume_value, volume,
                                          ind[X], ind[Y], ind[Z] );

                            if( volume_value < min_threshold ||
                                volume_value > max_threshold )
                                continue;
                        }

                        set_volume_label_data( label_volume, ind, label );
                        update_required = TRUE;
                    }
                }
            }
        }

        if( update_required )
            set_slice_window_all_update( slice_window );
    }
}

public  void  copy_labels_slice_to_slice(
    Volume           volume,
    Volume           label_volume,
    int              axis,
    int              src_voxel,
    int              dest_voxel,
    Real             min_threshold,
    Real             max_threshold )
{
    int   x, y, a1, a2, value;
    int   sizes[N_DIMENSIONS], src_indices[N_DIMENSIONS];
    int   dest_indices[N_DIMENSIONS];
    Real  volume_value;

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

            value = get_volume_label_data( label_volume, src_indices );

            if( min_threshold < max_threshold )
            {
                GET_VALUE_3D( volume_value, volume,
                            dest_indices[X], dest_indices[Y], dest_indices[Z] );
                if( volume_value < min_threshold ||
                    volume_value > max_threshold )
                    value = 0;
            }

            set_volume_label_data( label_volume, dest_indices, value );
        }
    }
}

typedef  enum  { POSITIVE_X, POSITIVE_Y, NEGATIVE_X, NEGATIVE_Y,
                 N_DIRECTIONS } Directions;

static  int  dx[N_DIRECTIONS] = { 1,  0, -1,  0 };
static  int  dy[N_DIRECTIONS] = { 0,  1,  0, -1 };

private   void    add_point_to_contour(
    display_struct   *slice_window,
    int              x_centre_pixel,
    int              y_centre_pixel,
    int              a1,
    int              a2,
    int              axis,
    int              voxel[],
    Directions       dir,
    lines_struct     *lines )
{
    int     c, x_pixel, y_pixel;
    Real    real_x_pixel, real_y_pixel;
    Real    real_voxel[N_DIMENSIONS];
    Point   point;

    for_less( c, 0, N_DIMENSIONS )
        real_voxel[c] = (Real) voxel[c];

    real_voxel[a1] += (Real) dx[dir] / 2.0;
    real_voxel[a2] += (Real) dy[dir] / 2.0;

    dir = (dir + 1) % N_DIRECTIONS;

    real_voxel[a1] += (Real) dx[dir] / 2.0;
    real_voxel[a2] += (Real) dy[dir] / 2.0;

    convert_voxel_to_pixel( slice_window, axis, real_voxel,
                            &real_x_pixel, &real_y_pixel );

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
    int               a1,
    int               a2,
    int               axis,
    Real              centre[N_DIMENSIONS],
    Real              radius[N_DIMENSIONS],
    int               start_voxel[N_DIMENSIONS],
    Directions        start_dir,
    lines_struct      *lines )
{
    int          current_voxel[N_DIMENSIONS];
    Directions   dir;

    current_voxel[X] = start_voxel[X];
    current_voxel[Y] = start_voxel[Y];
    current_voxel[Z] = start_voxel[Z];
    dir = start_dir;

    do
    {
        add_point_to_contour( slice_window, x_centre_pixel, y_centre_pixel,
                              a1, a2, axis, current_voxel, dir, lines );

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

private  void   update_brush(
    display_struct    *slice_window,
    int               x,
    int               y )
{
    Real          centre[N_DIMENSIONS];
    int           view, axis, a1, a2, start_voxel[N_DIMENSIONS];
    Real          radius[N_DIMENSIONS];
    lines_struct  *lines;

    lines = get_lines_ptr( slice_window->slice.brush_outline );
    delete_lines( lines );
    initialize_lines( lines, Brush_outline_colour );

    if( slice_window->slice.x_brush_radius > 0.0 &&
        slice_window->slice.y_brush_radius > 0.0 &&
        get_brush_voxel_centre( slice_window, x, y, centre, &view ) &&
        get_brush( slice_window, view, &a1, &a2, &axis, radius ) )
    {
        start_voxel[a1] = ROUND( centre[a1] );
        start_voxel[a2] = ROUND( centre[a2] );
        start_voxel[axis] = ROUND( centre[axis] );

        while( inside_swept_brush( centre, (Vector *) NULL, radius,
                                   start_voxel ) )
            ++start_voxel[a1];

        if( start_voxel[a1] > ROUND( centre[a1] ) )
            --start_voxel[a1];

        get_brush_contour( slice_window, x, y, a1, a2, axis, centre, radius,
                           start_voxel, POSITIVE_X, lines );

        set_slice_window_update( slice_window, view );
    }
}    

public  void  flip_labels_around_zero(
    Volume    label_volume )
{
    int             label_x, label_x_opp;
    int             int_voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    int             int_voxel_opp[MAX_DIMENSIONS];
    Real            voxel[MAX_DIMENSIONS], flip_voxel;

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

                label_x = get_volume_label_data( label_volume, int_voxel );
                label_x_opp = get_volume_label_data( label_volume,
                                                     int_voxel_opp );

                set_volume_label_data( label_volume, int_voxel_opp,
                                       label_x );
                set_volume_label_data( label_volume, int_voxel,
                                       label_x_opp );
            }
        }
    }
}

public  void  translate_labels(
    Volume    label_volume,
    int       delta[] )
{
    int               c, label;
    int               src_voxel[MAX_DIMENSIONS], dest_voxel[MAX_DIMENSIONS];
    int               sizes[MAX_DIMENSIONS];
    int               first[MAX_DIMENSIONS], last[MAX_DIMENSIONS];
    int               increment[MAX_DIMENSIONS];
    progress_struct   progress;

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
                    label = get_volume_label_data( label_volume, src_voxel );
                else
                    label = 0;

                set_volume_label_data( label_volume, dest_voxel, label );
            }

            update_progress_report( &progress, dest_voxel[X] * sizes[Y] +
                                               dest_voxel[Y] + 1 );
        }
    }

    terminate_progress_report( &progress );
}
