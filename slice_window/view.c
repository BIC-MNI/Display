
#include  <display.h>

private  void  update_all_slice_axes(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index );

public  void  initialize_slice_window_view(
    display_struct    *slice_window,
    int               volume_index )
{
    int      axis, view, x_min, x_max, y_min, y_max;
    Real     cosine, sine;
    Real     separations[MAX_DIMENSIONS];

    get_volume_separations( get_nth_volume(slice_window,volume_index),
                            separations );

    for_less( view, 0, N_SLICE_VIEWS )
    {
        slice_window->slice.volumes[volume_index].views[view].visibility = TRUE;
        for_less( axis, 0, N_DIMENSIONS )
        {
            slice_window->slice.volumes[volume_index].views[view].x_axis[axis]
                                                               = 0.0;
            slice_window->slice.volumes[volume_index].views[view].y_axis[axis]
                                                               = 0.0;
        }

        get_slice_viewport( slice_window, view,
                            &x_min, &x_max, &y_min, &y_max);

        slice_window->slice.slice_views[view].used_viewport_x_size =
                                                              x_max - x_min + 1;
        slice_window->slice.slice_views[view].used_viewport_y_size =
                                                              y_max - y_min + 1;
        slice_window->slice.slice_views[view].prev_viewport_x_size =
                                                              x_max - x_min + 1;
        slice_window->slice.slice_views[view].prev_viewport_y_size =
                                                              y_max - y_min + 1;
    }

    slice_window->slice.volumes[volume_index].views[0].
              x_axis[Slice_view1_axis1] = SIGN(separations[Slice_view1_axis1]);
    slice_window->slice.volumes[volume_index].views[0].
              y_axis[Slice_view1_axis2] = SIGN(separations[Slice_view1_axis2]);

    slice_window->slice.volumes[volume_index].views[1].
              x_axis[Slice_view2_axis1] = SIGN(separations[Slice_view2_axis1]);
    slice_window->slice.volumes[volume_index].views[1].
              y_axis[Slice_view2_axis2] = SIGN(separations[Slice_view2_axis2]);

    slice_window->slice.volumes[volume_index].views[2].
              x_axis[Slice_view3_axis1] = SIGN(separations[Slice_view3_axis1]);
    slice_window->slice.volumes[volume_index].views[2].
              y_axis[Slice_view3_axis2] = SIGN(separations[Slice_view3_axis2]);

    cosine = cos( 45.0 * DEG_TO_RAD );
    sine = sin( 45.0 * DEG_TO_RAD );

    slice_window->slice.volumes[volume_index].
                                views[OBLIQUE_VIEW_INDEX].visibility = FALSE;
    slice_window->slice.volumes[volume_index].
            views[OBLIQUE_VIEW_INDEX].x_axis[X] = cosine * SIGN(separations[X]);
    slice_window->slice.volumes[volume_index].
            views[OBLIQUE_VIEW_INDEX].x_axis[Y] = sine * SIGN(separations[Y]);
    slice_window->slice.volumes[volume_index].
                                views[OBLIQUE_VIEW_INDEX].x_axis[Z] = 0.0;

    slice_window->slice.volumes[volume_index].
            views[OBLIQUE_VIEW_INDEX].y_axis[X] = -sine * SIGN(separations[X]);
    slice_window->slice.volumes[volume_index].
            views[OBLIQUE_VIEW_INDEX].y_axis[Y] = cosine * SIGN(separations[Y]);
    slice_window->slice.volumes[volume_index].
                                views[OBLIQUE_VIEW_INDEX].y_axis[Z] = 0.0;
}

public  void  set_slice_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view,
    BOOLEAN           visibility )
{
    if( visibility != slice_window->slice.volumes[volume_index].
                                   views[view].visibility )
    {
        slice_window->slice.volumes[volume_index].views[view].visibility =
                                                       visibility;

        set_slice_window_update( slice_window, volume_index, view, UPDATE_BOTH);
    }
}

public  BOOLEAN  get_slice_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view )
{
    return( slice_window->slice.volumes[volume_index].views[view].visibility );
}

private  void  match_view_scale_and_translation(
    display_struct    *slice_window,
    int               view )
{
    Volume  volume;
    int     current_volume_index;
    int     volume_index;
    Real    separations[MAX_DIMENSIONS];
    Real    x_axis_x, x_axis_y, x_axis_z, y_axis_x, y_axis_y, y_axis_z;
    Real    x_len, y_len, current_x_len, current_y_len;
    Real    x_offset, y_offset;
    Real    xw, yw, zw;
    Real    current_voxel[MAX_DIMENSIONS];
    Real    x_scale, y_scale, x_trans, y_trans;
    Real    origin[MAX_DIMENSIONS];
    Real    x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    Real    scaled_x_axis[MAX_DIMENSIONS], scaled_y_axis[MAX_DIMENSIONS];
    Real    current_x_scale, current_y_scale;

    if( get_n_volumes(slice_window) == 0 )
        return;

    current_volume_index = get_current_volume_index( slice_window );

    current_x_scale = slice_window->slice.volumes[current_volume_index].
                                              views[view].x_scaling;
    current_y_scale = slice_window->slice.volumes[current_volume_index].
                                              views[view].y_scaling;

    get_volume_separations( get_volume(slice_window), separations );

    scaled_x_axis[0] = slice_window->slice.volumes[current_volume_index].
                           views[view].x_axis[0] / ABS(separations[0]);
    scaled_x_axis[1] = slice_window->slice.volumes[current_volume_index].
                           views[view].x_axis[1] / ABS(separations[1]);
    scaled_x_axis[2] = slice_window->slice.volumes[current_volume_index].
                           views[view].x_axis[2] / ABS(separations[2]);
    convert_voxel_vector_to_world( get_nth_volume(slice_window,
                                                  current_volume_index),
                                   scaled_x_axis,
                                   &x_axis_x, &x_axis_y, &x_axis_z );

    scaled_y_axis[0] = slice_window->slice.volumes[current_volume_index].
                           views[view].y_axis[0] / ABS(separations[0]);
    scaled_y_axis[1] = slice_window->slice.volumes[current_volume_index].
                           views[view].y_axis[1] / ABS(separations[1]);
    scaled_y_axis[2] = slice_window->slice.volumes[current_volume_index].
                           views[view].y_axis[2] / ABS(separations[2]);

    convert_voxel_vector_to_world( get_nth_volume(slice_window,
                                                  current_volume_index),
                                   scaled_y_axis,
                                   &y_axis_x, &y_axis_y, &y_axis_z );

    current_x_len = sqrt( x_axis_x * x_axis_x + x_axis_y * x_axis_y +
                          x_axis_z * x_axis_z );
    if( current_x_len == 0.0 )
        current_x_len = 1.0;
    current_y_len = sqrt( y_axis_x * y_axis_x + y_axis_y * y_axis_y +
                          y_axis_z * y_axis_z );
    if( current_y_len == 0.0 )
        current_y_len = 1.0;

    for_less( volume_index, 0, slice_window->slice.n_volumes )
    {
        if( volume_index == current_volume_index )
            continue;

        get_volume_separations( get_nth_volume(slice_window,volume_index),
                                separations );

        scaled_x_axis[0] = slice_window->slice.volumes[volume_index].
                               views[view].x_axis[0] / ABS(separations[0]);
        scaled_x_axis[1] = slice_window->slice.volumes[volume_index].
                               views[view].x_axis[1] / ABS(separations[1]);
        scaled_x_axis[2] = slice_window->slice.volumes[volume_index].
                               views[view].x_axis[2] / ABS(separations[2]);

        convert_voxel_vector_to_world( get_nth_volume(slice_window,
                                                      volume_index),
                                       scaled_x_axis,
                                       &x_axis_x, &x_axis_y, &x_axis_z );

        scaled_y_axis[0] = slice_window->slice.volumes[volume_index].
                               views[view].y_axis[0] / ABS(separations[0]);
        scaled_y_axis[1] = slice_window->slice.volumes[volume_index].
                               views[view].y_axis[1] / ABS(separations[1]);
        scaled_y_axis[2] = slice_window->slice.volumes[volume_index].
                               views[view].y_axis[2] / ABS(separations[2]);

        convert_voxel_vector_to_world( get_nth_volume(slice_window,
                                                      volume_index),
                                       scaled_y_axis,
                                       &y_axis_x, &y_axis_y, &y_axis_z );

        x_len = sqrt( x_axis_x * x_axis_x + x_axis_y * x_axis_y +
                      x_axis_z * x_axis_z );
        y_len = sqrt( y_axis_x * y_axis_x + y_axis_y * y_axis_y +
                      y_axis_z * y_axis_z );

        x_scale = current_x_scale * x_len / current_x_len;
        y_scale = current_y_scale * y_len / current_y_len;

        get_slice_plane( slice_window, volume_index, view,
                         origin, x_axis, y_axis );

        volume = get_nth_volume( slice_window, volume_index );

        convert_voxel_to_world( volume, origin, &xw, &yw, &zw );
        convert_world_to_voxel( get_volume(slice_window), xw, yw, zw,
                                current_voxel );
        convert_voxel_to_pixel( slice_window, current_volume_index,
                                view, current_voxel,
                                &x_offset, &y_offset );

        x_trans = x_offset;
        y_trans = y_offset;

        slice_window->slice.volumes[volume_index].views[view].x_trans = x_trans;
        slice_window->slice.volumes[volume_index].views[view].y_trans = y_trans;
        slice_window->slice.volumes[volume_index].views[view].x_scaling =
                                                                x_scale;
        slice_window->slice.volumes[volume_index].views[view].y_scaling =
                                                                y_scale;
    }

    slice_view_has_changed( slice_window, view );
}

public  void  reset_slice_view(
    display_struct    *slice_window,
    int               view )
{
    BOOLEAN found_one;
    Volume  volume;
    Real    x1, x2, y1, y2, x_offset, y_offset;
    int     int_x1, int_x2, int_y1, int_y2, current_volume_index;
    int     x_min, x_max, y_min, y_max;
    int     x_min_vp, x_max_vp, y_min_vp, y_max_vp;
    int     volume_index;
    Real    xw, yw, zw;
    Real    current_voxel[MAX_DIMENSIONS];
    Real    x_scale, y_scale, x_trans, y_trans;
    Real    current_origin[MAX_DIMENSIONS];
    Real    current_x_axis[MAX_DIMENSIONS], current_y_axis[MAX_DIMENSIONS];
    Real    origin[MAX_DIMENSIONS];
    Real    x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];

    if( get_n_volumes(slice_window) == 0 )
        return;

    get_slice_viewport( slice_window, view,
                        &x_min_vp, &x_max_vp, &y_min_vp, &y_max_vp );

    current_volume_index = get_current_volume_index( slice_window );
    get_slice_plane( slice_window, current_volume_index, view,
                     current_origin, current_x_axis, current_y_axis );

    found_one = FALSE;

    for_less( volume_index, 0, slice_window->slice.n_volumes )
    {
        if( !get_slice_visibility( slice_window, volume_index, view ) )
            continue;

        get_slice_plane( slice_window, volume_index, view,
                         origin, x_axis, y_axis );

        volume = get_nth_volume( slice_window, volume_index );

        get_volume_mapping_range( volume, origin, x_axis, y_axis,
                                  0.0, 0.0, 1.0, 1.0,
                                  &x1, &x2, &y1, &y2 );

        if( volume_index != current_volume_index )
        {
            convert_voxel_to_world( volume, origin, &xw, &yw, &zw );
            convert_world_to_voxel( get_volume(slice_window), xw, yw, zw,
                                    current_voxel );
            convert_voxel_to_slice_pixel( get_volume(slice_window),
                                          current_voxel, current_origin,
                                          current_x_axis, current_y_axis,
                                          0.0, 0.0, 1.0, 1.0,
                                          &x_offset, &y_offset );


            x1 += x_offset;
            x2 += x_offset;
            y1 += y_offset;
            y2 += y_offset;
        }

        int_x1 = FLOOR( x1 );
        int_x2 = CEILING( x2 );
        int_y1 = FLOOR( y1 );
        int_y2 = CEILING( y2 );

        if( !found_one )
        {
            x_min = int_x1;
            x_max = int_x2;
            y_min = int_y1;
            y_max = int_y2;
        }
        else
        {
            if( int_x1 < x_min )
                x_min = int_x1;
            if( int_x2 > x_max )
                x_max = int_x2;
            if( int_y1 < y_min )
                y_min = int_y1;
            if( int_y2 > y_max )
                y_max = int_y2;
        }

        found_one = TRUE;
    }

    if( !found_one || x_min == x_max || y_min == y_max )
    {
        x_trans = 0.0;
        y_trans = 0.0;
        x_scale = 1.0;
        y_scale = 1.0;
    }
    else
    {
        x_scale = (Real) (x_max_vp - x_min_vp + 1) / (x_max - x_min) /
                             (1.0 + Slice_fit_oversize);
        y_scale = (Real) (y_max_vp - y_min_vp + 1) / (y_max - y_min) /
                             (1.0 + Slice_fit_oversize);

        if( x_scale < y_scale )
            y_scale = x_scale;
        else
            x_scale = y_scale;

        slice_window->slice.slice_views[view].used_viewport_x_size =
                 ABS( x_scale * (x_max - x_min) * (1.0 + Slice_fit_oversize) );
        slice_window->slice.slice_views[view].used_viewport_y_size =
                 ABS( y_scale * (y_max - y_min) * (1.0 + Slice_fit_oversize) );

        x_trans = ((Real) (x_max_vp - x_min_vp + 1) - x_scale *
                          (x_max - x_min))/2.0 - x_scale * x_min;
        y_trans = ((Real) (y_max_vp - y_min_vp + 1) - y_scale *
                          (y_max - y_min))/2.0 - y_scale * y_min;
    }

    slice_window->slice.volumes[current_volume_index].views[view].x_trans =
                                                              x_trans;
    slice_window->slice.volumes[current_volume_index].views[view].y_trans =
                                                              y_trans;
    slice_window->slice.volumes[current_volume_index].views[view].x_scaling =
                                                              x_scale;
    slice_window->slice.volumes[current_volume_index].views[view].y_scaling =
                                                              y_scale;

    match_view_scale_and_translation( slice_window, view );

    slice_window->slice.slice_views[view].prev_viewport_x_size =
                                       (x_max_vp - x_min_vp + 1);
    slice_window->slice.slice_views[view].prev_viewport_y_size =
                                       (y_max_vp - y_min_vp + 1);

    slice_view_has_changed( slice_window, view );
}

public  void  resize_slice_view(
    display_struct    *slice_window,
    int               view )
{
    int            volume_index;
    int            x_min, x_max, y_min, y_max;
    int            used_viewport_x_size, used_viewport_y_size;

    get_slice_viewport( slice_window, view, &x_min, &x_max, &y_min, &y_max );

    for_less( volume_index, 0, slice_window->slice.n_volumes )
    {
        resize_volume_slice(
             slice_window->slice.slice_views[view].prev_viewport_x_size,
             slice_window->slice.slice_views[view].prev_viewport_y_size,
             slice_window->slice.slice_views[view].used_viewport_x_size,
             slice_window->slice.slice_views[view].used_viewport_y_size,
             x_max - x_min + 1, y_max - y_min + 1,
             &slice_window->slice.volumes[volume_index].views[view]
                                                  .x_trans,
             &slice_window->slice.volumes[volume_index].views[view]
                                                  .y_trans,
             &slice_window->slice.volumes[volume_index].views[view]
                                                  .x_scaling,
             &slice_window->slice.volumes[volume_index].views[view]
                                                  .y_scaling,
             &used_viewport_x_size, &used_viewport_y_size );
    }

    slice_window->slice.slice_views[view].used_viewport_x_size =
                                          used_viewport_x_size;
    slice_window->slice.slice_views[view].used_viewport_y_size =
                                          used_viewport_y_size;
    slice_window->slice.slice_views[view].prev_viewport_x_size =
                                          (x_max - x_min + 1);
    slice_window->slice.slice_views[view].prev_viewport_y_size =
                                          (y_max - y_min + 1);

    slice_view_has_changed( slice_window, view );
}

public  void  scale_slice_view(
    display_struct    *slice_window,
    int               view,
    Real              scale_factor )
{
    int     volume_index;
    int     x_min, x_max, y_min, y_max;

    get_slice_viewport( slice_window, view, &x_min, &x_max, &y_min, &y_max );

    for_less( volume_index, 0, slice_window->slice.n_volumes )
    {
        scale_slice_about_viewport_centre( scale_factor,
             x_max - x_min + 1, y_max - y_min + 1,
             &slice_window->slice.volumes[volume_index].views[view].x_trans,
             &slice_window->slice.volumes[volume_index].views[view].y_trans,
             &slice_window->slice.volumes[volume_index].views[view].x_scaling,
             &slice_window->slice.volumes[volume_index].views[view].y_scaling );
    }

    slice_view_has_changed( slice_window, view );
}

public  void  translate_slice_view(
    display_struct    *slice_window,
    int               view,
    Real              dx,
    Real              dy )
{
    int        volume_index;

    for_less( volume_index, 0, slice_window->slice.n_volumes )
    {
        slice_window->slice.volumes[volume_index].views[view].x_trans += dx;
        slice_window->slice.volumes[volume_index].views[view].y_trans += dy;
    }

    slice_view_has_changed( slice_window, view );
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
    int               volume_index,
    int               x_pixel,
    int               y_pixel,
    Real              voxel[],
    int               *view_index )
{
    BOOLEAN           found;
    display_struct    *slice_window;
    int               x_min, x_max, y_min, y_max;
    Real              origin[MAX_DIMENSIONS];
    Real              x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];;

    found = FALSE;

    if( get_slice_window( display, &slice_window ) &&
        find_slice_view_mouse_is_in( slice_window, x_pixel, y_pixel,
                                     view_index ) )
    {
        get_slice_viewport( slice_window, *view_index,
                            &x_min, &x_max, &y_min, &y_max );
        get_slice_plane( slice_window, volume_index,
                         *view_index, origin, x_axis, y_axis );

        x_pixel -= x_min;
        y_pixel -= y_min;

        found = convert_slice_pixel_to_voxel( get_nth_volume(slice_window,
                                volume_index), x_pixel, y_pixel,
         origin, x_axis, y_axis,
         slice_window->slice.volumes[volume_index].views[*view_index].x_trans,
         slice_window->slice.volumes[volume_index].views[*view_index].y_trans,
         slice_window->slice.volumes[volume_index].views[*view_index].x_scaling,
         slice_window->slice.volumes[volume_index].views[*view_index].y_scaling,
         voxel );
    }

    return( found );
}

public  void  convert_voxel_to_pixel(
    display_struct    *display,
    int               volume_index,
    int               view_index,
    Real              voxel[],
    Real              *x_pixel,
    Real              *y_pixel )
{
    Volume            volume;
    display_struct    *slice_window;
    Real              x_real_pixel, y_real_pixel;
    Real              origin[MAX_DIMENSIONS];
    Real              x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];;

    if( get_slice_window( display, &slice_window ) )
    {
        volume = get_nth_volume( slice_window, volume_index );

        get_slice_plane( slice_window, volume_index,
                         view_index, origin, x_axis, y_axis );

        convert_voxel_to_slice_pixel( volume, voxel, origin, x_axis, y_axis,
          slice_window->slice.volumes[volume_index].views[view_index].x_trans,
          slice_window->slice.volumes[volume_index].views[view_index].y_trans,
          slice_window->slice.volumes[volume_index].views[view_index].x_scaling,
          slice_window->slice.volumes[volume_index].views[view_index].y_scaling,
          &x_real_pixel, &y_real_pixel );

        *x_pixel = x_real_pixel;
        *y_pixel = y_real_pixel;
    }
    else
    {
        HANDLE_INTERNAL_ERROR( "convert_voxel_to_pixel" );
    }
}

public  void  get_voxel_to_pixel_transform(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    int               *x_index,
    int               *y_index,
    Real              *x_scale,
    Real              *x_trans,
    Real              *y_scale,
    Real              *y_trans )
{
    int     axis;
    Real    voxel[MAX_DIMENSIONS], x, y;

    if( !slice_has_ortho_axes( slice_window, volume_index,
                               view_index, x_index, y_index, &axis ) )
    {
        handle_internal_error( "get_voxel_to_pixel_transform" );
        return;
    }

    voxel[axis] = 0.0;
    voxel[*x_index] = 0.0;
    voxel[*y_index] = 0.0;
    convert_voxel_to_pixel( slice_window, volume_index, view_index, voxel,
                            x_trans, y_trans );

    voxel[*x_index] = 1.0;
    voxel[*y_index] = 1.0;
    convert_voxel_to_pixel( slice_window, volume_index, view_index, voxel,
                            &x, &y );

    *x_scale = x - *x_trans;
    *y_scale = y - *y_trans;
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

    if( *x_max < *x_min )
        *x_max = *x_min;
    if( *y_max < *y_min )
        *y_max = *y_min;
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
    *y_min = 0;
    *y_max = text_panel_height - 1;
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

        update_all_slice_models( slice_window );
        set_slice_window_all_update( slice_window, -1, UPDATE_BOTH );
    }
}

public  BOOLEAN  get_volume_corresponding_to_pixel(
    display_struct    *slice_window,
    int               x,
    int               y,
    int               *volume_index,
    int               *view_index,
    Real              voxel[] )
{
    if( !get_slice_window( slice_window, &slice_window ) )
        return( FALSE );

    for( *volume_index = slice_window->slice.n_volumes-1;  *volume_index >= 0;
         --(*volume_index) )
    {
        if( slice_window->slice.volumes[*volume_index].opacity > 0.0 &&
            convert_pixel_to_voxel( slice_window, *volume_index, x, y,
                                    voxel, view_index ) &&
            get_slice_visibility( slice_window, *volume_index, *view_index ) )
        {
            break;
        }
    }

    return( *volume_index >= 0 );
}

public  BOOLEAN  get_voxel_in_slice_window(
    display_struct    *display,
    Real              voxel[],
    int               *volume_index,
    int               *view_index )
{
    display_struct    *slice_window;
    int               x_mouse, y_mouse;

    if( !get_slice_window( display, &slice_window ) )
        return( FALSE );

    (void) G_get_mouse_position( slice_window->window, &x_mouse, &y_mouse );

    return( get_volume_corresponding_to_pixel( slice_window, x_mouse, y_mouse,
                                               volume_index, view_index,voxel));
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

    if( get_mouse_scene_intersection( display, (Object_types) -1,
                                      &object, &object_index,
                                      &intersection_point ) &&
        get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        found = get_voxel_corresponding_to_point( slice_window,
                                                  &intersection_point,
                                                  voxel );
    }

    return( found );
}

public  BOOLEAN  get_voxel_under_mouse(
    display_struct    *display,
    int               *volume_index,
    int               *view_index,
    Real              voxel[] )
{
    display_struct    *three_d, *slice_window;
    BOOLEAN           found;

    three_d = display->associated[THREE_D_WINDOW];

    if( !get_slice_window(three_d,&slice_window) )
        found = FALSE;
    else if( G_is_mouse_in_window( slice_window->window ) )
    {
        found = get_voxel_in_slice_window( slice_window, voxel,
                                           volume_index, view_index );
    }
    else if( G_is_mouse_in_window( three_d->window ) )
    {
        found = get_voxel_in_three_d_window( three_d, voxel );
        *volume_index = get_current_volume_index( slice_window );
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
    int               volume_index,
    Real              voxel[] )
{
    int              c;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            voxel[c] = slice_window->slice.volumes[volume_index].
                                             current_voxel[c];
        }
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
    int               this_volume_index,
    Real              voxel[] )
{
    BOOLEAN           changed;
    int               i, view, volume_index;
    int               axis, x_index, y_index;
    Real              xw, yw, zw, used_voxel[N_DIMENSIONS];

    changed = FALSE;

    convert_voxel_to_world( get_nth_volume(slice_window,this_volume_index),
                            voxel, &xw, &yw, &zw );

    for_less( volume_index, 0, slice_window->slice.n_volumes )
    {
        if( volume_index == this_volume_index )
        {
            for_less( i, 0, N_DIMENSIONS )
                used_voxel[i] = voxel[i];
        }
        else
            convert_world_to_voxel( get_nth_volume(slice_window,volume_index),
                                    xw, yw, zw, used_voxel );

        for_less( i, 0, N_DIMENSIONS )
        {
            if( used_voxel[i] != slice_window->slice.volumes[volume_index].
                                             current_voxel[i] )
            {
                slice_window->slice.volumes[volume_index].current_voxel[i] =
                                                     used_voxel[i];

                for_less( view, 0, N_SLICE_VIEWS )
                {
                    if( !slice_has_ortho_axes( slice_window, volume_index, view,
                                               &x_index, &y_index, &axis ) ||
                        axis == i )
                    {
                        set_slice_window_update( slice_window, volume_index,
                                                 view, UPDATE_BOTH );

                        if( volume_index ==
                            get_current_volume_index(slice_window) )
                        {
                            set_crop_box_update( slice_window, view );
                            set_atlas_update( slice_window, view );
                            set_slice_text_update( slice_window, view );
                        }
                    }
                }
            }

            changed = TRUE;
        }
    }

    set_slice_cross_section_update( slice_window, -1 );
    set_slice_cursor_update( slice_window, -1 );
    set_probe_update( slice_window );

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
    int              volume_index,
    int              view_index,
    Real             perp_axis[N_DIMENSIONS] )
{
    get_voxel_axis_perpendicular( get_nth_volume(slice_window,volume_index),
        slice_window->slice.volumes[volume_index].views[view_index].x_axis,
        slice_window->slice.volumes[volume_index].views[view_index].y_axis,
        perp_axis );
}

public  void  set_slice_plane_perp_axis(
    display_struct   *slice_window,
    int              volume_index,
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
    int      x_index, y_index, view;
    int      c, max_axis;

    get_volume_separations( get_nth_volume(slice_window,volume_index),
                            separations );

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
        slice_window->slice.volumes[volume_index].views[view_index].x_axis[c] =
                                                                used_x_axis[c];
        slice_window->slice.volumes[volume_index].views[view_index].y_axis[c] =
                                                                used_y_axis[c];
    }

    update_all_slice_axes( slice_window, volume_index, view_index );

    rebuild_volume_cross_section( slice_window );

    for_less( view, 0, N_SLICE_VIEWS )
        slice_view_has_changed( slice_window, view );
}

public  void  set_slice_plane(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Real             x_axis[],
    Real             y_axis[] )
{
    Real     perp[MAX_DIMENSIONS];

    get_voxel_axis_perpendicular( get_nth_volume(slice_window,volume_index),
                                  x_axis, y_axis, perp );

    set_slice_plane_perp_axis( slice_window, volume_index, view_index, perp );
}

public  void  get_slice_plane(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Real             origin[],
    Real             x_axis[],
    Real             y_axis[] )
{
    int    c;
    Real   separations[MAX_DIMENSIONS];
    Real   voxel[MAX_DIMENSIONS], perp_axis[MAX_DIMENSIONS];
    Real   voxel_dot_perp, perp_dot_perp, factor;

    get_volume_separations( get_nth_volume(slice_window,volume_index),
                            separations );

    get_current_voxel( slice_window, volume_index, voxel );
    get_slice_perp_axis( slice_window, volume_index, view_index, perp_axis );

    for_less( c, 0, N_DIMENSIONS )
    {
        separations[c] = ABS( separations[c] );
        perp_axis[c] *= separations[c];
        x_axis[c] = slice_window->slice.volumes[volume_index].
                                    views[view_index].x_axis[c];
        y_axis[c] = slice_window->slice.volumes[volume_index].
                                    views[view_index].y_axis[c];
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

public  BOOLEAN  get_slice_view_index_under_mouse(
    display_struct   *display,
    int              *view_index )
{
    BOOLEAN          found;
    Volume           volume;
    display_struct   *slice_window;
    int              x, y;

    found = FALSE;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        if( G_get_mouse_position( slice_window->window, &x, &y ) &&
            find_slice_view_mouse_is_in( slice_window, x, y, view_index ) )
        {
            found = TRUE;
        }
    }

    return( found );
}

public  BOOLEAN  get_axis_index_under_mouse(
    display_struct   *display,
    int              *volume_index,
    int              *axis_index )
{
    BOOLEAN          found;
    Real             voxel[MAX_DIMENSIONS];
    int              view_index, x_index, y_index;
    display_struct   *slice_window;

    found = get_slice_window( display, &slice_window ) &&
            get_voxel_in_slice_window( slice_window, voxel, volume_index,
                                       &view_index ) &&
            slice_has_ortho_axes( slice_window, *volume_index,
                                  view_index, &x_index, &y_index, axis_index );

    return( found );
}

public  BOOLEAN  slice_has_ortho_axes(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    int              *x_index,
    int              *y_index,
    int              *axis_index )
{
    Real     origin[N_DIMENSIONS];
    Real     x_axis[N_DIMENSIONS];
    Real     y_axis[N_DIMENSIONS];
    int      c;

    get_slice_plane( slice_window, volume_index, view_index,
                     origin, x_axis, y_axis );

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

public  void  get_slice_model_viewport(
    display_struct   *slice_window,
    int              model,
    int              *x_min,
    int              *x_max,
    int              *y_min,
    int              *y_max )
{
    int   x_size, y_size;

    switch( model )
    {
    case FULL_WINDOW_MODEL:
        G_get_window_size( slice_window->window, &x_size, &y_size );
        *x_min = 0;
        *x_max = x_size-1;
        *y_min = 0;
        *y_max = y_size-1;
        break;

    case SLICE_MODEL1:
    case SLICE_MODEL2:
    case SLICE_MODEL3:
    case SLICE_MODEL4:
        get_slice_viewport( slice_window, model - SLICE_MODEL1,
                            x_min, x_max, y_min, y_max );
        break;

    case COLOUR_BAR_MODEL:
        get_colour_bar_viewport( slice_window, x_min, x_max, y_min, y_max );
        break;

    case SLICE_READOUT_MODEL:
        get_text_display_viewport( slice_window, x_min, x_max, y_min, y_max );
        break;
    }
}

public  BOOLEAN  update_cursor_from_voxel(
    display_struct    *slice_window )
{
    Real              voxel[MAX_DIMENSIONS];
    Real              x_w, y_w, z_w;
    BOOLEAN           changed;
    Point             new_origin;
    display_struct    *display;

    if( get_n_volumes(slice_window) == 0 )
        return( FALSE );

    display = get_three_d_window( slice_window );

    get_current_voxel( slice_window, get_current_volume_index(slice_window),
                       voxel );

    convert_voxel_to_world( get_volume(slice_window), voxel, &x_w, &y_w, &z_w );
    fill_Point( new_origin, x_w, y_w, z_w );

    if( !EQUAL_POINTS( new_origin, display->three_d.cursor.origin ) )
    {
        display->three_d.cursor.origin = new_origin;

        update_cursor( display );

        changed = TRUE;
    }
    else
    {
        changed = FALSE;
    }

    return( changed );
}

public  BOOLEAN  update_voxel_from_cursor(
    display_struct    *slice_window )
{
    int               volume_index;
    Real              voxel[MAX_DIMENSIONS];
    BOOLEAN           changed;
    display_struct    *display;

    changed = FALSE;

    if( get_slice_window(slice_window,&slice_window) &&
        get_n_volumes(slice_window) > 0 )
    {
        display = get_three_d_window( slice_window );

        (void) get_voxel_corresponding_to_point( slice_window,
                                    &display->three_d.cursor.origin,
                                    voxel );
        volume_index = get_current_volume_index( slice_window );
        changed = set_current_voxel( slice_window, volume_index, voxel );
    }

    return( changed );
}

private  void  update_all_slice_axes(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    int    v;
    Real   x_axis_x, x_axis_y, x_axis_z, y_axis_x, y_axis_y, y_axis_z;
    Real   mag, x_axis[N_DIMENSIONS], y_axis[N_DIMENSIONS];

    convert_voxel_vector_to_world( get_nth_volume(slice_window,volume_index),
                     slice_window->slice.volumes[volume_index].
                                    views[view_index].x_axis,
                                   &x_axis_x, &x_axis_y, &x_axis_z );

    convert_voxel_vector_to_world( get_nth_volume(slice_window,volume_index),
                     slice_window->slice.volumes[volume_index].
                                    views[view_index].y_axis,
                                   &y_axis_x, &y_axis_y, &y_axis_z );

    for_less( v, 0, slice_window->slice.n_volumes )
    {
        if( v == volume_index )
            continue;

        convert_world_vector_to_voxel(get_nth_volume(slice_window,v),
                                      x_axis_x, x_axis_y, x_axis_z, x_axis );

        convert_world_vector_to_voxel(get_nth_volume(slice_window,v),
                                      y_axis_x, y_axis_y, y_axis_z, y_axis );

        mag = sqrt( x_axis[0] * x_axis[0] + x_axis[1] * x_axis[1] +
                    x_axis[2] * x_axis[2] );

        if( mag == 0.0 )
            mag = 1.0;

        slice_window->slice.volumes[v].views[view_index].x_axis[0] =
                                                              x_axis[0] / mag;
        slice_window->slice.volumes[v].views[view_index].x_axis[1] =
                                                              x_axis[1] / mag;
        slice_window->slice.volumes[v].views[view_index].x_axis[2] =
                                                              x_axis[2] / mag;

        mag = sqrt( y_axis[0] * y_axis[0] + y_axis[1] * y_axis[1] +
                    y_axis[2] * y_axis[2] );

        if( mag == 0.0 )
            mag = 1.0;

        slice_window->slice.volumes[v].views[view_index].y_axis[0] =
                                                              y_axis[0] / mag;
        slice_window->slice.volumes[v].views[view_index].y_axis[1] =
                                                              y_axis[1] / mag;
        slice_window->slice.volumes[v].views[view_index].y_axis[2] =
                                                              y_axis[2] / mag;
    }

    match_view_scale_and_translation( slice_window, view_index );
}

public  void  update_all_slice_axes_views(
    display_struct    *slice_window,
    int               volume_index )
{
    int   view;

    for_less( view, 0, N_SLICE_VIEWS )
        update_all_slice_axes( slice_window, volume_index, view );
}

public  void  slice_view_has_changed(
    display_struct   *display,
    int              view )
{
    display_struct   *slice_window;

    if( !get_slice_window( display, &slice_window ) )
        return;

    set_slice_cursor_update( slice_window, view );
    set_slice_cross_section_update( slice_window, view );
    set_crop_box_update( slice_window, view );
    set_probe_update( slice_window );
    set_atlas_update( slice_window, view );

    set_slice_window_update( slice_window, -1, view, UPDATE_BOTH );
}
