#include  <display.h>

private  DEF_EVENT_FUNCTION( right_mouse_down );
private  DEF_EVENT_FUNCTION( end_painting );
private  DEF_EVENT_FUNCTION( handle_update_painting );
private  void  paint_labels(
    display_struct   *slice_window,
    int              view_index,
    Real             start_voxel[],
    Real             end_voxel[] );
private  void  update_paint_labels(
    display_struct  *display );
private  void  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2 );
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
    int           x_pixel, y_pixel;

    push_action_table( &display->action_table, NO_EVENT );

    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update_painting );

    add_action_table_function( &display->action_table,
                               RIGHT_MOUSE_UP_EVENT,
                               end_painting );

    (void) G_get_mouse_position( display->window, &x_pixel, &y_pixel );
    sweep_paint_labels( display, x_pixel, y_pixel, x_pixel, y_pixel );

    if( Draw_brush_outline )
    {
        display->slice.brush_outline = create_object( LINES );
        initialize_lines( get_lines_ptr(display->slice.brush_outline),
                          Brush_outline_colour );

        add_object_to_model( get_graphics_model( display, SLICE_MODEL ),
                             display->slice.brush_outline );
        update_brush( display, x_pixel, y_pixel );
    }
    
    record_mouse_pixel_position( display );

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
    display_struct  *display )
{
    int  x, y, x_prev, y_prev;

    if( pixel_mouse_moved(display,&x,&y,&x_prev,&y_prev) )
    {
        sweep_paint_labels( display, x_prev, y_prev, x, y );
        if( Draw_brush_outline )
            update_brush( display, x, y );
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
    int               y2 )
{
    int         view_index;
    Real        start_voxel[MAX_DIMENSIONS], end_voxel[MAX_DIMENSIONS];

    if( get_brush_voxel_centre( slice_window, x1, y1, start_voxel,
                                &view_index ) &&
        get_brush_voxel_centre( slice_window, x2, y2, end_voxel,
                                &view_index ) )
    {
        paint_labels( slice_window, view_index, start_voxel, end_voxel );
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
    Real             end_voxel[] )
{
    Volume   volume, label_volume;
    int      a1, a2, axis, label, value, c, sizes[N_DIMENSIONS];
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

        label = get_current_paint_label( slice_window );

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
    }
}    
