#include  <display.h>

private  DEF_EVENT_FUNCTION( right_mouse_down );
private  DEF_EVENT_FUNCTION( end_painting );
private  DEF_EVENT_FUNCTION( handle_update_painting );
private  void  update_paint_labels(
    display_struct  *display );
private  void  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2 );
private  void  paint_labels_at_point(
    display_struct   *slice_window,
    int              x,
    int              y );
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
    paint_labels_at_point( display, x_pixel, y_pixel );

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

    remove_object_from_model( get_graphics_model( display, SLICE_MODEL ),
                              find_object_index_in_model(
                                  get_graphics_model(display,SLICE_MODEL),
                                  display->slice.brush_outline ) );

    update_paint_labels( display );

    if( Draw_brush_outline )
        delete_object( display->slice.brush_outline );

    set_slice_window_update( display, 0 );
    set_slice_window_update( display, 1 );
    set_slice_window_update( display, 2 );

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

private  void  sweep_paint_labels(
    display_struct    *slice_window,
    int               x1,
    int               y1,
    int               x2,
    int               y2 )
{
    int         i, dx, dy, n, x, y;

    dx = ABS( x2 - x1 );
    dy = ABS( y2 - y1 );
    n = MAX( dx, dy );

    for_inclusive( i, 1, n )
    {
        x = ROUND( x1 + (Real) i / (Real) n * (x2 - x1) );
        y = ROUND( y1 + (Real) i / (Real) n * (y2 - y1) );

        paint_labels_at_point( slice_window, x, y );
    }
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

private  void  paint_labels_at_point(
    display_struct   *slice_window,
    int              x,
    int              y )
{
    Volume   volume, label_volume;
    int      label, axis, value, c, sizes[N_DIMENSIONS];
    Real     delta, dx2, dy2, dz2;
    int      min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    Real     min_limit, max_limit;
    Real     one_over_r2[N_DIMENSIONS], radius[N_DIMENSIONS];
    Real     voxel[N_DIMENSIONS], separations[MAX_DIMENSIONS];
    int      ind[N_DIMENSIONS];
    int      x_index, y_index, view_index;
    BOOLEAN  update_required;

    if( get_slice_window_volume( slice_window, &volume ) &&
        get_brush_voxel_centre( slice_window, x, y, voxel, &view_index ) )
    {
        label_volume = get_label_volume( slice_window );

        update_required = FALSE;

        get_slice_axes( slice_window, view_index, &x_index, &y_index, &axis );

        radius[x_index] = slice_window->slice.x_brush_radius;
        radius[y_index] = slice_window->slice.y_brush_radius;
        radius[axis] = slice_window->slice.z_brush_radius;

        get_volume_separations( volume, separations );
        get_volume_sizes( volume, sizes );

        for_less( c, 0, N_DIMENSIONS )
        {
            if( radius[c] <= 0.0 )
            {
                min_voxel[c] = ROUND( voxel[c] );
                max_voxel[c] = min_voxel[c];
                one_over_r2[c] = 0.0;
            }
            else
            {
                min_limit = voxel[c] - radius[c] / separations[c];
                max_limit = voxel[c] + radius[c] / separations[c];
                min_voxel[c] = CEILING( min_limit );
                if( min_voxel[c] < 0 )
                    min_voxel[c] = 0;
                max_voxel[c] = (int) max_limit;
                if( max_voxel[c] >= sizes[c] )
                    max_voxel[c] = sizes[c] - 1;
                one_over_r2[c] = separations[c] * separations[c] /
                                 radius[c] / radius[c];
            }
        }

        label = get_current_paint_label( slice_window );

        for_inclusive( ind[X], min_voxel[X], max_voxel[X] )
        {
            delta = (Real) ind[X] - voxel[X];
            dx2 = delta * delta * one_over_r2[X];

            for_inclusive( ind[Y], min_voxel[Y], max_voxel[Y] )
            {
                delta = (Real) ind[Y] - voxel[Y];
                dy2 = delta * delta * one_over_r2[Y];

                for_inclusive( ind[Z], min_voxel[Z], max_voxel[Z] )
                {
                    delta = (Real) ind[Z] - voxel[Z];
                    dz2 = delta * delta * one_over_r2[Z];

                    if( dx2 + dy2 + dz2 <= 1.0 &&
                        int_voxel_is_within_volume( volume, ind ) )
                    {
                        value = get_volume_label_data( label_volume, ind );
                        if( value != label )
                        {
                            set_volume_label_data( label_volume, ind, label );
                            update_required = TRUE;
                        }
                    }
                }
            }
        }

        if( update_required )
        {
            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
    }
}

public  void  copy_labels_slice_to_slice(
    Volume           label_volume,
    int              axis,
    int              src_voxel,
    int              dest_voxel )
{
    int   x, y, a1, a2, value;
    int   sizes[N_DIMENSIONS], src_indices[N_DIMENSIONS];
    int   dest_indices[N_DIMENSIONS];

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

            set_volume_label_data( label_volume, dest_indices, value );
        }
    }
}

private  BOOLEAN  inside_brush(
    Real       origin[],
    Real       one_over_r2[],
    int        voxel[] )
{
    int    c;
    Real   sum, delta;

    sum = 0.0;

    for_less( c, 0, N_DIMENSIONS )
    {
        delta = (Real) voxel[c] - origin[c];
        sum += delta * delta * one_over_r2[c];
    }

    return( sum <= 1.0 );
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
    int              voxel[N_DIMENSIONS],
    Directions       dir,
    lines_struct     *lines )
{
    int     x_pixel, y_pixel;
    Real    x_voxel, y_voxel;
    Point   point;

    x_voxel = (Real) voxel[a1] + (Real) dx[dir] / 2.0;
    y_voxel = (Real) voxel[a2] + (Real) dy[dir] / 2.0;

    dir = (dir + 1) % N_DIRECTIONS;

    x_voxel += (Real) dx[dir] / 2.0;
    y_voxel += (Real) dy[dir] / 2.0;

    convert_voxel_to_pixel( slice_window, axis, x_voxel, y_voxel,
                            &x_pixel, &y_pixel );

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
    Real       origin[],
    Real       one_over_r2[],
    int        a1,
    int        a2,
    int        voxel[],
    Directions dir )
{
    BOOLEAN   inside;

    voxel[a1] += dx[dir];
    voxel[a2] += dy[dir];

    inside = inside_brush( origin, one_over_r2, voxel );

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
    Real              origin[N_DIMENSIONS],
    Real              one_over_r2[N_DIMENSIONS],
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

        while( neighbour_is_inside( origin, one_over_r2, a1, a2,
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
    Real          voxel[N_DIMENSIONS], separations[MAX_DIMENSIONS];
    int           c, view, axis, a1, a2, start_voxel[N_DIMENSIONS];
    Real          one_over_r2[N_DIMENSIONS], radius[N_DIMENSIONS];
    Volume        volume;
    lines_struct  *lines;

    lines = get_lines_ptr( slice_window->slice.brush_outline );
    delete_lines( lines );
    initialize_lines( lines, Brush_outline_colour );

    if( get_slice_window_volume( slice_window, &volume ) &&
        slice_window->slice.x_brush_radius > 0.0 &&
        slice_window->slice.y_brush_radius > 0.0 &&
        get_brush_voxel_centre( slice_window, x, y, voxel, &view ) )
    {
        get_slice_axes( slice_window, view, &a1, &a2, &axis );
        get_volume_separations( volume, separations );

        radius[a1] = slice_window->slice.x_brush_radius;
        radius[a2] = slice_window->slice.y_brush_radius;
        radius[axis] = slice_window->slice.z_brush_radius;

        for_less( c, 0, N_DIMENSIONS )
        {
            if( radius[c] <= 0.0 )
                one_over_r2[c] = 0.0;
            else
                one_over_r2[c] = separations[c] * separations[c] /
                                 radius[c] / radius[c];
        }

        start_voxel[a1] = (int) (voxel[a1] + radius[a1] / separations[a1]) + 2;
        start_voxel[a2] = ROUND( voxel[a2] );
        start_voxel[axis] = ROUND( voxel[axis] );

        while( start_voxel[a1] >= voxel[a1] &&
               !inside_brush( voxel, one_over_r2, start_voxel ) )
            --start_voxel[a1];

        if( start_voxel[a1] < voxel[a1] )
            return;

        get_brush_contour( slice_window, x, y, a1, a2, axis, voxel, one_over_r2,
                           start_voxel, POSITIVE_X, lines );
    }
}    
