
#include  <display.h>

public  void  initialize_slice_window_view(
    display_struct    *slice_window )
{
    Volume           volume;
    int              axis, view;
    int              size[N_DIMENSIONS];
    Real             separations[N_DIMENSIONS];
    Real             cosine, sine;

    for_less( view, 0, N_SLICE_VIEWS )
    {
        slice_window->slice.slice_views[view].visibility = TRUE;
        for_less( axis, 0, N_DIMENSIONS )
        {
            slice_window->slice.slice_views[view].x_axis[axis] = 0.0;
            slice_window->slice.slice_views[view].y_axis[axis] = 0.0;
        }
    }

    slice_window->slice.slice_views[0].x_axis[Slice_view1_axis1] = 1.0;
    slice_window->slice.slice_views[0].y_axis[Slice_view1_axis2] = 1.0;

    slice_window->slice.slice_views[1].x_axis[Slice_view2_axis1] = 1.0;
    slice_window->slice.slice_views[1].y_axis[Slice_view2_axis2] = 1.0;

    slice_window->slice.slice_views[2].x_axis[Slice_view3_axis1] = 1.0;
    slice_window->slice.slice_views[2].y_axis[Slice_view3_axis2] = 1.0;

    cosine = cos( 45.0 * DEG_TO_RAD );
    sine = sin( 45.0 * DEG_TO_RAD );

    slice_window->slice.slice_views[OBLIQUE_VIEW_INDEX].visibility = FALSE;
    slice_window->slice.slice_views[OBLIQUE_VIEW_INDEX].x_axis[X] = cosine;
    slice_window->slice.slice_views[OBLIQUE_VIEW_INDEX].x_axis[Y] = sine;
    slice_window->slice.slice_views[OBLIQUE_VIEW_INDEX].x_axis[Z] = 0.0;

    slice_window->slice.slice_views[OBLIQUE_VIEW_INDEX].y_axis[X] = -sine;
    slice_window->slice.slice_views[OBLIQUE_VIEW_INDEX].y_axis[Y] = cosine;
    slice_window->slice.slice_views[OBLIQUE_VIEW_INDEX].y_axis[Z] = 0.0;

    slice_window->slice.x_split = Slice_divider_x_position;
    slice_window->slice.y_split = Slice_divider_y_position;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_sizes( volume, size );
        get_volume_separations( volume, separations );

        for_less( axis, 0, N_DIMENSIONS )
            slice_window->slice.current_voxel[axis] = (size[axis] - 1) / 2.0;

        for_less( view, 0, N_SLICE_VIEWS )
            reset_slice_view( slice_window, view );

        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[X] =
                          ABS( separations[X] );
        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Y] =
                          ABS( separations[Y] );
        slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Z] =
                          ABS( separations[Z] );

        update_cursor_size( slice_window->associated[THREE_D_WINDOW] );
    }
}

public  void  set_slice_visibility(
    display_struct    *slice_window,
    int               view,
    BOOLEAN           visibility )
{
    if( visibility != slice_window->slice.slice_views[view].visibility )
    {
        slice_window->slice.slice_views[view].visibility = visibility;

        set_slice_window_update( slice_window, view, UPDATE_BOTH );
    }
}

public  BOOLEAN  get_slice_visibility(
    display_struct    *slice_window,
    int               view )
{
    return( slice_window->slice.slice_views[view].visibility );
}

public  void  reset_slice_view(
    display_struct    *slice_window,
    int               view )
{
    Volume  volume;
    int     x_min, x_max, y_min, y_max;
    Real    origin[MAX_DIMENSIONS];
    Real    x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        get_slice_viewport( slice_window, view,
                            &x_min, &x_max, &y_min, &y_max );

        get_slice_plane( slice_window, view, origin, x_axis, y_axis );

        fit_volume_slice_to_viewport( volume, origin, x_axis, y_axis,
                x_max - x_min + 1, y_max - y_min + 1,
                Slice_fit_oversize,
                &slice_window->slice.slice_views[view].x_trans,
                &slice_window->slice.slice_views[view].y_trans,
                &slice_window->slice.slice_views[view].x_scaling,
                &slice_window->slice.slice_views[view].y_scaling,
                &slice_window->slice.slice_views[view].used_viewport_x_size,
                &slice_window->slice.slice_views[view].used_viewport_y_size );

         slice_window->slice.slice_views[view].prev_viewport_x_size =
                                                          (x_max - x_min + 1);
         slice_window->slice.slice_views[view].prev_viewport_y_size =
                                                          (y_max - y_min + 1);
    }
}

public  void  resize_slice_view(
    display_struct    *slice_window,
    int               view )
{
    int            x_min, x_max, y_min, y_max;
    Volume         volume;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        get_slice_viewport( slice_window, view,
                            &x_min, &x_max, &y_min, &y_max );

        resize_volume_slice(
             slice_window->slice.slice_views[view].prev_viewport_x_size,
             slice_window->slice.slice_views[view].prev_viewport_y_size,
             slice_window->slice.slice_views[view].used_viewport_x_size,
             slice_window->slice.slice_views[view].used_viewport_y_size,
             x_max - x_min + 1, y_max - y_min + 1,
             &slice_window->slice.slice_views[view].x_trans,
             &slice_window->slice.slice_views[view].y_trans,
             &slice_window->slice.slice_views[view].x_scaling,
             &slice_window->slice.slice_views[view].y_scaling,
             &slice_window->slice.slice_views[view].used_viewport_x_size,
             &slice_window->slice.slice_views[view].used_viewport_y_size );

         slice_window->slice.slice_views[view].prev_viewport_x_size =
                                                          (x_max - x_min + 1);
         slice_window->slice.slice_views[view].prev_viewport_y_size =
                                                          (y_max - y_min + 1);
    }
}

public  void  scale_slice_view(
    display_struct    *slice_window,
    int               view,
    Real              scale_factor )
{
    Volume volume;
    int    x_min, x_max, y_min, y_max;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        get_slice_viewport( slice_window, view,
                            &x_min, &x_max, &y_min, &y_max );

        scale_slice_about_viewport_centre( scale_factor,
             x_max - x_min + 1, y_max - y_min + 1,
             &slice_window->slice.slice_views[view].x_trans,
             &slice_window->slice.slice_views[view].y_trans,
             &slice_window->slice.slice_views[view].x_scaling,
             &slice_window->slice.slice_views[view].y_scaling );
    }
}

public  void  translate_slice_view(
    display_struct    *slice_window,
    int               view,
    Real              dx,
    Real              dy )
{
    Volume volume;

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        slice_window->slice.slice_views[view].x_trans += dx;
        slice_window->slice.slice_views[view].y_trans += dy;
    }
}

public  BOOLEAN  find_slice_view_mouse_is_in(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *view_index )
{
    BOOLEAN          found;
    int              view;
    int              x_min, x_max, y_min, y_max;
    display_struct   *slice_window;

    found = FALSE;

    if( get_slice_window(display,&slice_window) )
    {
        for_less( view, 0, N_SLICE_VIEWS )
        {
            get_slice_viewport( slice_window, view,
                                &x_min, &x_max, &y_min, &y_max );

            if( x_pixel >= x_min && x_pixel <= x_max &&
                y_pixel >= y_min && y_pixel <= y_max )
            {
                *view_index = view;
                found = TRUE;

                break;
            }
        }
    }

    return( found );
}

public  BOOLEAN  convert_pixel_to_voxel(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    Real              voxel[],
    int               *view_index )
{
    BOOLEAN           found;
    Volume            volume;
    display_struct    *slice_window;
    int               x_min, x_max, y_min, y_max;
    Real              origin[MAX_DIMENSIONS];
    Real              x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];;

    found = FALSE;

    if( find_slice_view_mouse_is_in( display, x_pixel, y_pixel, view_index ) &&
        get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        get_slice_viewport( slice_window, *view_index,
                            &x_min, &x_max, &y_min, &y_max );
        get_slice_plane( slice_window, *view_index, origin, x_axis, y_axis );

        x_pixel -= x_min;
        y_pixel -= y_min;

        found = convert_slice_pixel_to_voxel( volume, x_pixel, y_pixel,
                    origin, x_axis, y_axis,
                    slice_window->slice.slice_views[*view_index].x_trans,
                    slice_window->slice.slice_views[*view_index].y_trans,
                    slice_window->slice.slice_views[*view_index].x_scaling,
                    slice_window->slice.slice_views[*view_index].y_scaling,
                    voxel );
    }

    return( found );
}

public  void  convert_voxel_to_pixel(
    display_struct    *display,
    int               view_index,
    Real              voxel[],
    Real              *x_pixel,
    Real              *y_pixel )
{
    Volume            volume;
    display_struct    *slice_window;
    int               x_min, x_max, y_min, y_max;
    Real              x_real_pixel, y_real_pixel;
    Real              origin[MAX_DIMENSIONS];
    Real              x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( display, &volume ) )
    {
        get_slice_plane( slice_window, view_index, origin, x_axis, y_axis );

        convert_voxel_to_slice_pixel( volume, voxel, origin, x_axis, y_axis,
                        slice_window->slice.slice_views[view_index].x_trans,
                        slice_window->slice.slice_views[view_index].y_trans,
                        slice_window->slice.slice_views[view_index].x_scaling,
                        slice_window->slice.slice_views[view_index].y_scaling,
                        &x_real_pixel, &y_real_pixel );

        get_slice_viewport( display, view_index,
                            &x_min, &x_max, &y_min, &y_max );

        *x_pixel = x_real_pixel + (Real) x_min;
        *y_pixel = y_real_pixel + (Real) y_min;
    }
    else
    {
        HANDLE_INTERNAL_ERROR( "convert_voxel_to_pixel" );
    }
}

public  BOOLEAN  get_voxel_corresponding_to_point(
    display_struct    *display,
    Point             *point,
    Real              voxel[] )
{
    Volume          volume;
    BOOLEAN         converted;

    converted = FALSE;

    if( get_slice_window_volume( display, &volume ) )
    {
        convert_world_to_voxel( volume,
                            Point_x(*point), Point_y(*point), Point_z(*point),
                            voxel );

        converted = voxel_is_within_volume( volume, voxel );
    }

    return( converted );
}

public  void   get_slice_window_partitions(
    display_struct    *slice_window,
    int               *left_panel_width,
    int               *left_slice_width,
    int               *right_slice_width,
    int               *bottom_slice_height,
    int               *top_slice_height,
    int               *text_panel_height,
    int               *colour_bar_panel_height )
{
    int     x_size, y_size;

    G_get_window_size( slice_window->window, &x_size, &y_size );

    *left_panel_width = Left_panel_width;
    *text_panel_height = Text_panel_height;
    *colour_bar_panel_height = y_size - *text_panel_height;
    *left_slice_width = ROUND( slice_window->slice.x_split *
                               (x_size - *left_panel_width) );
    *right_slice_width = x_size - *left_panel_width - *left_slice_width;
    *bottom_slice_height = ROUND( slice_window->slice.y_split * y_size );
    *top_slice_height = y_size - *bottom_slice_height;
}

public  void  get_slice_viewport(
    display_struct    *slice_window,
    int               view_index,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max )
{
    int  left_panel_width, left_slice_width, right_slice_width;
    int  bottom_slice_height, top_slice_height, text_panel_height;
    int  colour_bar_panel_height;

    get_slice_window_partitions( slice_window,
                                 &left_panel_width, &left_slice_width,
                                 &right_slice_width,
                                 &bottom_slice_height, &top_slice_height,
                                 &text_panel_height, &colour_bar_panel_height );


    switch( view_index )
    {
    case 0:
        *x_min = left_panel_width + Slice_divider_left;
        *x_max = left_panel_width + left_slice_width - 1 - Slice_divider_right;
        *y_min = bottom_slice_height + Slice_divider_bottom;
        *y_max = bottom_slice_height + top_slice_height - 1 - Slice_divider_top;
        break;

    case 1:
        *x_min = left_panel_width + left_slice_width + Slice_divider_left;
        *x_max = left_panel_width + left_slice_width + right_slice_width -
                 Slice_divider_right;
        *y_min = bottom_slice_height + Slice_divider_bottom;
        *y_max = bottom_slice_height + top_slice_height - 1 - Slice_divider_top;
        break;

    case 2:
        *x_min = left_panel_width + Slice_divider_left;
        *x_max = left_panel_width + left_slice_width - 1 - Slice_divider_right;
        *y_min = Slice_divider_bottom;
        *y_max = bottom_slice_height - 1 - Slice_divider_top;
        break;

    case 3:
        *x_min = left_panel_width + left_slice_width + Slice_divider_left;
        *x_max = left_panel_width + left_slice_width + right_slice_width -
                 Slice_divider_right;
        *y_min = Slice_divider_bottom;
        *y_max = bottom_slice_height - 1 - Slice_divider_top;
        break;
    }
}

public  void  get_colour_bar_viewport(
    display_struct    *slice_window,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max )
{
    int  left_panel_width, left_slice_width, right_slice_width;
    int  bottom_slice_height, top_slice_height, text_panel_height;
    int  colour_bar_panel_height;

    get_slice_window_partitions( slice_window,
                                 &left_panel_width, &left_slice_width,
                                 &right_slice_width,
                                 &bottom_slice_height, &top_slice_height,
                                 &text_panel_height, &colour_bar_panel_height );

    *x_min = 0;
    *x_max = left_panel_width-1;
    *y_min = text_panel_height + Slice_divider_bottom;
    *y_max = text_panel_height + colour_bar_panel_height - 1 -
             Slice_divider_top;
}

public  void  get_text_display_viewport(
    display_struct    *slice_window,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max )
{
    int  left_panel_width, left_slice_width, right_slice_width;
    int  bottom_slice_height, top_slice_height, text_panel_height;
    int  colour_bar_panel_height;

    get_slice_window_partitions( slice_window,
                                 &left_panel_width, &left_slice_width,
                                 &right_slice_width,
                                 &bottom_slice_height, &top_slice_height,
                                 &text_panel_height, &colour_bar_panel_height );

    *x_min = 0;
    *x_max = left_panel_width-1;
    *y_min = Slice_divider_bottom;
    *y_max = text_panel_height - 1 - Slice_divider_top;
}

public  void  get_slice_divider_intersection(
    display_struct    *slice_window,
    int               *x,
    int               *y )
{
    int  left_panel_width, left_slice_width, right_slice_width;
    int  bottom_slice_height, top_slice_height, text_panel_height;
    int  colour_bar_panel_height;

    get_slice_window_partitions( slice_window,
                                 &left_panel_width, &left_slice_width,
                                 &right_slice_width,
                                 &bottom_slice_height, &top_slice_height,
                                 &text_panel_height, &colour_bar_panel_height );

    *x = left_panel_width + left_slice_width;
    *y = bottom_slice_height;
}

public  void  set_slice_divider_position(
    display_struct    *slice_window,
    int               x,
    int               y )
{
    int  view;
    int  left_panel_width, left_slice_width, right_slice_width;
    int  bottom_slice_height, top_slice_height, text_panel_height;
    int  colour_bar_panel_height;
    Real x_split, y_split;

    get_slice_window_partitions( slice_window,
                                 &left_panel_width, &left_slice_width,
                                 &right_slice_width,
                                 &bottom_slice_height, &top_slice_height,
                                 &text_panel_height, &colour_bar_panel_height );

    x_split = (Real) (x - left_panel_width) /
              (Real) (left_slice_width + right_slice_width);
    y_split = (Real) y /
              (Real) (bottom_slice_height + top_slice_height);

    if( x_split > 0.0 && x_split < 1.0 && y_split > 0.0 && y_split < 1.0 )
    {
        slice_window->slice.x_split = x_split;
        slice_window->slice.y_split = y_split;

        for_less( view, 0, N_SLICE_VIEWS )
            resize_slice_view( slice_window, view );

        rebuild_slice_models( slice_window );
    }
}

public  BOOLEAN  get_voxel_in_slice_window(
    display_struct    *display,
    Real              voxel[],
    int               *view_index )
{
    display_struct    *slice_window;
    int               x_mouse, y_mouse;
    BOOLEAN           found;

    slice_window = display->associated[SLICE_WINDOW];

    (void) G_get_mouse_position( slice_window->window, &x_mouse, &y_mouse );

    found = convert_pixel_to_voxel( slice_window, x_mouse, y_mouse, voxel,
                                    view_index );

    return( found );
}

public  BOOLEAN  get_voxel_in_three_d_window(
    display_struct    *display,
    Real              voxel[] )
{
    BOOLEAN          found;
    object_struct    *object;
    int              object_index;
    Point            intersection_point;
    display_struct   *slice_window;

    found = FALSE;

    if( get_mouse_scene_intersection( display, -1, &object, &object_index,
                                      &intersection_point ) &&
        get_slice_window( display, &slice_window ) )
    {
        found = get_voxel_corresponding_to_point( slice_window,
                                                  &intersection_point,
                                                  voxel );
    }

    return( found );
}

public  BOOLEAN  get_voxel_under_mouse(
    display_struct    *display,
    Real              voxel[],
    int               *view_index )
{
    display_struct    *three_d, *slice_window;
    BOOLEAN           found;

    three_d = display->associated[THREE_D_WINDOW];

    if( !get_slice_window(three_d,&slice_window) )
        found = FALSE;
    else if( G_is_mouse_in_window( slice_window->window ) )
    {
        found = get_voxel_in_slice_window( display, voxel, view_index );
    }
    else if( G_is_mouse_in_window( three_d->window ) )
    {
        found = get_voxel_in_three_d_window( three_d, voxel );
        *view_index = 2;
    }
    else
    {
        found = FALSE;
    }

    return( found );
}

public  void  get_current_voxel(
    display_struct    *display,
    Real              voxel[] )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        voxel[X] = slice_window->slice.current_voxel[X];
        voxel[Y] = slice_window->slice.current_voxel[Y];
        voxel[Z] = slice_window->slice.current_voxel[Z];
    }
    else
    {
        voxel[X] = 0.0;
        voxel[Y] = 0.0;
        voxel[Z] = 0.0;
    }
}

public  BOOLEAN  set_current_voxel(
    display_struct    *slice_window,
    Real              voxel[] )
{
    BOOLEAN           changed;
    int               i, view, sizes[MAX_DIMENSIONS];
    int               axis, x_index, y_index;

    changed = FALSE;

    get_volume_sizes( get_volume(slice_window), sizes );

    for_less( i, 0, N_DIMENSIONS )
    {
        if( voxel[i] != slice_window->slice.current_voxel[i] )
        {
            slice_window->slice.current_voxel[i] = voxel[i];

            for_less( view, 0, N_SLICE_VIEWS )
            {
                if( !slice_has_ortho_axes( slice_window, view,
                                           &x_index, &y_index, &axis ) ||
                    axis == i )
                {
                    set_slice_window_update( slice_window, view, UPDATE_BOTH );
                }
            }

            changed = TRUE;
        }
    }

    if( changed )
    {
        rebuild_volume_cross_section( slice_window );
        rebuild_slice_cross_sections( slice_window );
        rebuild_probe( slice_window );
        rebuild_cursors( slice_window );
    }

    return( changed );
}

private  void  get_voxel_axis_perpendicular(
    Volume   volume,
    Real     x_axis[],
    Real     y_axis[],
    Real     perp_axis[] )
{
    int      c, a1, a2;
    Real     len, separations[MAX_DIMENSIONS];

    get_volume_separations( volume, separations );

    len = 0.0;
    for_less( c, 0, N_DIMENSIONS )
    {
        a1 = (c + 1) % N_DIMENSIONS;
        a2 = (c + 2) % N_DIMENSIONS;
        perp_axis[c] = x_axis[a1] * y_axis[a2] - x_axis[a2] * y_axis[a1];

        perp_axis[c] *= ABS( separations[a1] * separations[a2] /
                             separations[c] );

        len += perp_axis[c] * perp_axis[c];
    }

    if( len != 0.0 )
    {
        len = sqrt( len );
        for_less( c, 0, N_DIMENSIONS )
            perp_axis[c] /= len;
    }
}

public  void  get_slice_perp_axis(
    display_struct   *slice_window,
    int              view_index,
    Real             perp_axis[N_DIMENSIONS] )
{
    get_voxel_axis_perpendicular( get_volume(slice_window),
        slice_window->slice.slice_views[view_index].x_axis,
        slice_window->slice.slice_views[view_index].y_axis, perp_axis );
}

public  void  set_slice_plane_perp_axis(
    display_struct   *slice_window,
    int              view_index,
    Real             voxel_perp[] )
{
    Real     max_value, len, sign;
    Real     len_x_axis, len_y_axis, factor;
    Real     used_x_axis[MAX_DIMENSIONS];
    Real     used_y_axis[MAX_DIMENSIONS];
    Real     separations[MAX_DIMENSIONS];
    Real     perp[MAX_DIMENSIONS];
    Vector   axis, vect, new_axis, tmp;
    int      x_index, y_index;
    int      c, max_axis;

    get_volume_separations( get_volume(slice_window), separations );

    for_less( c, 0, N_DIMENSIONS )
        separations[c] = ABS( separations[c] );

    for_less( c, 0, N_DIMENSIONS )
        perp[c] = voxel_perp[c] * separations[c];

    if( slice_window->slice.cross_section_vector_present )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            Vector_coord(axis,c) = perp[c];
            Vector_coord(vect,c) =
                 slice_window->slice.cross_section_vector[c] * separations[c];
        }

        CROSS_VECTORS( tmp, vect, axis );
        CROSS_VECTORS( new_axis, vect, tmp );
        if( DOT_VECTORS( new_axis, axis ) < 0.0 )
            SCALE_VECTOR( new_axis, new_axis, -1.0 );

        for_less( c, 0, N_DIMENSIONS )
            perp[c] = Vector_coord( new_axis, c );
    }

    max_value = 0.0;
    for_less( c, 0, N_DIMENSIONS )
    {
        if( c == 0 || ABS(perp[c]) > max_value )
        {
            max_value = ABS(perp[c]);
            max_axis = c;
        }
    }

    switch( max_axis )
    {
    case X: x_index = Y;   y_index = Z;  break;
    case Y: x_index = X;   y_index = Z;  break;
    case Z: x_index = X;   y_index = Y;  break;
    }

    used_x_axis[X] = 0.0;
    used_x_axis[Y] = 0.0;
    used_x_axis[Z] = 0.0;
    used_x_axis[x_index] = 1.0;

    len = perp[X] * perp[X] + perp[Y] * perp[Y] + perp[Z] * perp[Z];
    if( len == 0.0 )
        return;

    factor = used_x_axis[x_index] * perp[x_index] / len;

    for_less( c, 0, N_DIMENSIONS )
        used_x_axis[c] -= factor * perp[c];

    used_y_axis[X] = perp[Y] * used_x_axis[Z] - used_x_axis[Y] * perp[Z];
    used_y_axis[Y] = perp[Z] * used_x_axis[X] - used_x_axis[Z] * perp[X];
    used_y_axis[Z] = perp[X] * used_x_axis[Y] - used_x_axis[X] * perp[Y];

    len_x_axis = 0.0;
    len_y_axis = 0.0;
    for_less( c, 0, N_DIMENSIONS )
    {
        used_x_axis[c] /= separations[c];
        used_y_axis[c] /= separations[c];
        len_x_axis += used_x_axis[c] * used_x_axis[c];
        len_y_axis += used_y_axis[c] * used_y_axis[c];
    }

    if( len_x_axis == 0.0 || len_y_axis == 0.0 )
        return;

    len_x_axis = sqrt( len_x_axis );
    len_y_axis = sqrt( len_y_axis );

    if( used_y_axis[y_index] < 0.0 )
        sign = -1.0;
    else
        sign = 1.0;

    for_less( c, 0, N_DIMENSIONS )
    {
        used_x_axis[c] /= len_x_axis;
        used_y_axis[c] /= sign * len_y_axis;
    }

    for_less( c, 0, N_DIMENSIONS )
    {
        slice_window->slice.slice_views[view_index].x_axis[c] = used_x_axis[c];
        slice_window->slice.slice_views[view_index].y_axis[c] = used_y_axis[c];
    }

    rebuild_volume_cross_section( slice_window );
    rebuild_slice_cross_sections( slice_window );
}

public  void  set_slice_plane(
    display_struct   *slice_window,
    int              view_index,
    Real             x_axis[],
    Real             y_axis[] )
{
    Real     perp[MAX_DIMENSIONS];

    get_voxel_axis_perpendicular( get_volume(slice_window), x_axis, y_axis,
                                  perp );

    set_slice_plane_perp_axis( slice_window, view_index, perp );
}

public  void  get_slice_plane(
    display_struct   *slice_window,
    int              view_index,
    Real             origin[],
    Real             x_axis[],
    Real             y_axis[] )
{
    int    c;
    Real   separations[MAX_DIMENSIONS];
    Real   voxel[MAX_DIMENSIONS], perp_axis[MAX_DIMENSIONS];
    Real   voxel_dot_perp, perp_dot_perp, factor;

    get_volume_separations( get_volume(slice_window), separations );

    get_current_voxel( slice_window, voxel );
    get_slice_perp_axis( slice_window, view_index, perp_axis );

    for_less( c, 0, N_DIMENSIONS )
    {
        separations[c] = ABS( separations[c] );
        perp_axis[c] *= separations[c];
        x_axis[c] = slice_window->slice.slice_views[view_index].x_axis[c];
        y_axis[c] = slice_window->slice.slice_views[view_index].y_axis[c];
    }

    voxel_dot_perp = 0.0;
    for_less( c, 0, N_DIMENSIONS )
        voxel_dot_perp += voxel[c] * separations[c] * perp_axis[c];

    perp_dot_perp = 0.0;
    for_less( c, 0, N_DIMENSIONS )
       perp_dot_perp += perp_axis[c] * perp_axis[c];

    if( perp_dot_perp == 0.0 )
    {
        for_less( c, 0, N_DIMENSIONS )
            origin[c] = 0.0;
    }
    else
    {
        factor = voxel_dot_perp / perp_dot_perp;
        for_less( c, 0, N_DIMENSIONS )
            origin[c] = factor * perp_axis[c] / separations[c];
    }
}

public  BOOLEAN  get_axis_index_under_mouse(
    display_struct   *display,
    int              *axis_index )
{
    BOOLEAN          found;
    int              view_index, x_index, y_index;
    display_struct   *slice_window;

    found = get_slice_view_index_under_mouse( display, &view_index ) &&
            get_slice_window( display, &slice_window ) &&
            slice_has_ortho_axes( slice_window, view_index,
                                  &x_index, &y_index, axis_index );

    return( found );
}

public  BOOLEAN  slice_has_ortho_axes(
    display_struct   *slice_window,
    int              view_index,
    int              *x_index,
    int              *y_index,
    int              *axis_index )
{
    Real     origin[N_DIMENSIONS];
    Real     x_axis[N_DIMENSIONS];
    Real     y_axis[N_DIMENSIONS];
    int      c;

    get_slice_plane( slice_window, view_index, origin, x_axis, y_axis );

    *x_index = -1;
    *y_index = -1;
    for_less( c, 0, N_DIMENSIONS )
    {
        if( x_axis[c] != 0.0 )
        {
            if( *x_index != -1 )
                return( FALSE );
            *x_index = c;
        }
        if( y_axis[c] != 0.0 )
        {
            if( *y_index != -1 )
                return( FALSE );
            *y_index = c;
        }
    }

    if( *x_index == *y_index )
        return( FALSE );

    *axis_index = N_DIMENSIONS - *x_index - *y_index;

    return( TRUE );
}

public  int  get_arbitrary_view_index(
    display_struct   *display )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        return( slice_window->slice.cross_section_index );
    else
        return( 0 );
}
