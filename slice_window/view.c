/**
 * \file slice_window/view.c
 * \brief Create and maintain views of the slice window.
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
#include  <assert.h>

/**
 * Sets the maximum size of a direction cosine before it is considered to
 * contribute too much to the vector to be considered "orthogonal."
 */
#define ORTHOGONAL_SLICE_EPSILON 1.5e-2f

static  void  update_all_slice_axes(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index );

static void  slice_view_has_changed(
    display_struct   *slice_window,
    int              view_index );

/**
 * Tiny helper function to take the magnitude of
 * an array (its dot product with itself).
 * \param array The array for which we want the magnitude.
 * \param n The length of the array.
 * \returns The magnitude of the array.
 */
static VIO_Real array_magnitude(const VIO_Real array[], int n)
{
    VIO_Real sum = 0.0;
    int      i;
    for_less(i, 0, n)
        sum += array[i] * array[i];
    return sqrt(sum);
}

/**
 * Tiny helper function to normalize an array, such that
 * its length will be one.
 *
 * \param array The array which we want to normalize.
 * \param n The length of the array.
 * \returns The magnitude of the array.
 */
static VIO_Real array_normalize(VIO_Real array[], int n)
{
    VIO_Real len = array_magnitude(array, n);
    if (len != 0.0)
    {
        int i;
        for_less(i, 0, n)
            array[i] /= len;
    }
    return len;
}

/**
 * Initialize the view axes of the views for a particular volume.
 *
 * \param slice_window The display_struct of the slice window.
 * \param view_index The index of the desired view.
 * \param volume_index The zero-based index of the desired volume.
 */
static  void  set_orthogonal_slice_window_view(
    display_struct    *slice_window,
    int               view_index,
    int               volume_index )
{
    int             axis;
    VIO_Real        separations[VIO_MAX_DIMENSIONS];
    volume_view_ptr view_ptr;

    view_ptr = &slice_window->slice.volumes[volume_index].views[view_index];

    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        view_ptr->x_axis[axis] = 0.0;
        view_ptr->y_axis[axis] = 0.0;
    }

    get_volume_separations( get_nth_volume(slice_window, volume_index),
                            separations );

    switch( view_index )
    {
    case 0:
        view_ptr->x_axis[Slice_view1_axis1] =
          VIO_FSIGN(separations[Slice_view1_axis1]);
        view_ptr->y_axis[Slice_view1_axis2] =
          VIO_FSIGN(separations[Slice_view1_axis2]);
        break;

    case 1:
        view_ptr->x_axis[Slice_view2_axis1] =
          VIO_FSIGN(separations[Slice_view2_axis1]);
        view_ptr->y_axis[Slice_view2_axis2] =
          VIO_FSIGN(separations[Slice_view2_axis2]);
        break;

    case 2:
    case 3:
        /** \todo Should we really handle both views the same way? */
        view_ptr->x_axis[Slice_view3_axis1] =
          VIO_FSIGN(separations[Slice_view3_axis1]);
        view_ptr->y_axis[Slice_view3_axis2] =
          VIO_FSIGN(separations[Slice_view3_axis2]);
        break;
    }
}

/**
 * Initialize the views of a volume.
 *
 * \param slice_window The display_struct of the slice window.
 * \param volume_index The zero-based index of the desired volume.
 */
void  initialize_slice_window_view(
    display_struct    *slice_window,
    int               volume_index )
{
    int      axis, view, x_min, x_max, y_min, y_max;

    for_less( view, 0, N_SLICE_VIEWS )
    {
        slice_window->slice.volumes[volume_index].views[view].visibility = TRUE;
        for_less( axis, 0, VIO_N_DIMENSIONS )
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

        slice_window->slice.volumes[volume_index].views[view].
                                                     n_pixels_alloced = 0;
        slice_window->slice.volumes[volume_index].views[view].
                                                     n_label_pixels_alloced = 0;

        set_orthogonal_slice_window_view( slice_window, view, volume_index );
    }

    slice_window->slice.volumes[volume_index].
                                views[OBLIQUE_VIEW_INDEX].visibility = FALSE;
}

/**
 * Set the visibility of a given volume in a view, and update the
 * display accordingly.
 *
 * \param slice_window The display_struct of the slice window.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The index of the desired view.
 * \param visibility The desired visibility state.
 */
void  set_slice_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    VIO_BOOL          visibility )
{
    loaded_volume_struct *lv_ptr = &slice_window->slice.volumes[volume_index];
    if( visibility != lv_ptr->views[view_index].visibility )
    {
        lv_ptr->views[view_index].visibility = visibility;
        set_slice_outline_update( slice_window, -1 );
        set_slice_window_update( slice_window, volume_index, view_index,
                                 UPDATE_BOTH);
    }
}

/**
 * Predicate to test slice visibility.
 *
 * \param slice_window The display_struct of the slice window.
 * \param volume_index The zero-based index of the desired volume. If a
 * negative value is passed for the volume index, all volumes will be
 * checked for visibility.
 * \param view_index The index of the desired view.
 * \returns TRUE if the volume is visible in the view.
 */
VIO_BOOL get_slice_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    if ( volume_index >= 0 )
    {
        return( slice_window->slice.volumes[volume_index].views[view_index].
                visibility );
    }
    else
    {
        int n_volumes = get_n_volumes( slice_window );
        int i;
        for_less( i, 0, n_volumes )
        {
            if ( get_slice_visibility( slice_window, i, view_index ) )
            {
                return TRUE;
            }
        }
        return FALSE;
    }
}

/**
 * \brief See if the chosen volume is currently visible.
 *
 * \param slice_window A pointer to the slice window's main data structure.
 * \param volume_index The zero-based index of the desired volume.
 * \returns TRUE if the volume is visible.
 */
VIO_BOOL
get_volume_visibility( display_struct *slice_window, int volume_index )
{
    int view_index;

    if( slice_window->slice.volumes[volume_index].opacity == 0.0 )
        return FALSE;

    for_less( view_index, 0, N_SLICE_VIEWS )
    {
        if( get_slice_visibility( slice_window, volume_index, view_index ))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * Align the scaling and translation of all volumes in a particular view,
 * using the given volume index to specify a reference volume.
 *
 * \param slice_window The display_struct of the slice window.
 * \param view_index The index of the affected view.
 * \param ref_volume_index The zero-based index of the reference volume.
 */
static  void  match_view_scale_and_translation(
    display_struct    *slice_window,
    int               view_index,
    int               ref_volume_index )
{
    VIO_Volume  volume;
    VIO_Volume  ref_volume;
    int         volume_index, axis;
    VIO_Real    separations[VIO_MAX_DIMENSIONS];
    VIO_Real    ref_x_axis[VIO_MAX_DIMENSIONS], ref_y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real    x_axis_x, x_axis_y, x_axis_z, y_axis_x, y_axis_y, y_axis_z;
    VIO_Real    x_len, y_len, current_x_len, current_y_len;
    VIO_Real    x_offset, y_offset;
    VIO_Real    xw, yw, zw;
    VIO_Real    current_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real    x_scale, y_scale, x_trans, y_trans;
    VIO_Real    origin[VIO_MAX_DIMENSIONS];
    VIO_Real    x_axis[VIO_MAX_DIMENSIONS], y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real    current_x_scale, current_y_scale;

    if( get_n_volumes( slice_window ) == 0 )
        return;

    ref_volume = get_nth_volume( slice_window, ref_volume_index );

    current_x_scale = slice_window->slice.volumes[ref_volume_index].
                                              views[view_index].x_scaling;
    current_y_scale = slice_window->slice.volumes[ref_volume_index].
                                              views[view_index].y_scaling;

    get_volume_separations( ref_volume, separations );

    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        x_axis[axis] = slice_window->slice.volumes[ref_volume_index].
                           views[view_index].x_axis[axis];
        y_axis[axis] = slice_window->slice.volumes[ref_volume_index].
                           views[view_index].y_axis[axis];
    }

    x_len = 0.0;
    y_len = 0.0;
    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        x_len += x_axis[axis] * x_axis[axis] *
                 separations[axis] * separations[axis];
        y_len += y_axis[axis] * y_axis[axis] *
                 separations[axis] * separations[axis];
    }

    x_len = sqrt( x_len );
    if( x_len == 0.0 )
        x_len = 1.0;

    y_len = sqrt( y_len );
    if( y_len == 0.0 )
        y_len = 1.0;

    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        x_axis[axis] /= x_len;
        y_axis[axis] /= y_len;
    }

    convert_voxel_vector_to_world( ref_volume,
                                   x_axis,
                                   &ref_x_axis[VIO_X], &ref_x_axis[VIO_Y],
                                   &ref_x_axis[VIO_Z] );

    convert_voxel_vector_to_world( ref_volume,
                                   y_axis,
                                   &ref_y_axis[VIO_X], &ref_y_axis[VIO_Y],
                                   &ref_y_axis[VIO_Z] );

    current_x_len = sqrt( ref_x_axis[VIO_X] * ref_x_axis[VIO_X] +
                          ref_x_axis[VIO_Y] * ref_x_axis[VIO_Y] +
                          ref_x_axis[VIO_Z] * ref_x_axis[VIO_Z] );
    if( current_x_len == 0.0 )
        current_x_len = 1.0;

    current_y_len = sqrt( ref_y_axis[VIO_X] * ref_y_axis[VIO_X] +
                          ref_y_axis[VIO_Y] * ref_y_axis[VIO_Y] +
                          ref_y_axis[VIO_Z] * ref_y_axis[VIO_Z] );
    if( current_y_len == 0.0 )
        current_y_len = 1.0;

    for_less( volume_index, 0, get_n_volumes( slice_window ) )
    {
        if( volume_index == ref_volume_index )
            continue;

        volume = get_nth_volume( slice_window, volume_index );

        get_volume_separations( volume, separations );

        for_less( axis, 0, VIO_N_DIMENSIONS )
        {
            x_axis[axis] = slice_window->slice.volumes[volume_index].
                           views[view_index].x_axis[axis];
            y_axis[axis] = slice_window->slice.volumes[volume_index].
                           views[view_index].y_axis[axis];
        }

        x_len = 0.0;
        y_len = 0.0;
        for_less( axis, 0, VIO_N_DIMENSIONS )
        {
            x_len += x_axis[axis] * x_axis[axis] *
                     separations[axis] * separations[axis];
            y_len += y_axis[axis] * y_axis[axis] *
                     separations[axis] * separations[axis];
        }

        x_len = sqrt( x_len );
        if( x_len == 0.0 )
            x_len = 1.0;

        y_len = sqrt( y_len );
        if( y_len == 0.0 )
            y_len = 1.0;

        for_less( axis, 0, VIO_N_DIMENSIONS )
        {
            x_axis[axis] /= x_len;
            y_axis[axis] /= y_len;
        }

        convert_voxel_vector_to_world( volume, x_axis,
                                       &x_axis_x, &x_axis_y, &x_axis_z );
        convert_voxel_vector_to_world( volume, y_axis,
                                       &y_axis_x, &y_axis_y, &y_axis_z );

        x_len = sqrt( x_axis_x * x_axis_x + x_axis_y * x_axis_y +
                      x_axis_z * x_axis_z );
        if( x_len == 0.0 )
            x_len = 1.0;
        y_len = sqrt( y_axis_x * y_axis_x + y_axis_y * y_axis_y +
                      y_axis_z * y_axis_z );
        if( y_len == 0.0 )
            y_len = 1.0;

        x_scale = current_x_scale * x_len / current_x_len;
        y_scale = current_y_scale * y_len / current_y_len;

#ifdef DEBUG
#define DEBUG
#define  TOL 1.0e-3

        if( !numerically_close( current_x_scale * ref_x_axis[VIO_X], x_scale * x_axis_x, TOL ) ||
            !numerically_close( current_x_scale * ref_x_axis[VIO_Y], x_scale * x_axis_y, TOL ) ||
            !numerically_close( current_x_scale * ref_x_axis[VIO_Z], x_scale * x_axis_z, TOL ) ||
            !numerically_close( current_y_scale * ref_y_axis[VIO_X], y_scale * y_axis_x, TOL ) ||
            !numerically_close( current_y_scale * ref_y_axis[VIO_Y], y_scale * y_axis_y, TOL ) ||
            !numerically_close( current_y_scale * ref_y_axis[VIO_Z], y_scale * y_axis_z, TOL ) )
        {
            print( "Error:  %g %g %g\n", current_x_scale * ref_x_axis[VIO_X], current_x_scale * ref_x_axis[VIO_Y],
                   current_x_scale * ref_x_axis[VIO_Z] );
            print( "     :  %g %g %g\n", x_scale * x_axis_x,
                   x_scale * x_axis_y, x_scale * x_axis_z );
            print( "Error:  %g %g %g\n", current_y_scale * ref_y_axis[VIO_X], current_y_scale * ref_y_axis[VIO_Y],
                   current_y_scale * ref_y_axis[VIO_Z] );
            print( "     :  %g %g %g\n", y_scale * y_axis_x,
                   y_scale * y_axis_y, y_scale * y_axis_z );
        }
#endif

        get_slice_plane( slice_window, volume_index, view_index,
                         origin, x_axis, y_axis );

        convert_voxel_to_world( volume, origin, &xw, &yw, &zw );
        convert_world_to_voxel( ref_volume,
                                xw, yw, zw, current_voxel );
        convert_voxel_to_pixel( slice_window, ref_volume_index,
                                view_index, current_voxel,
                                &x_offset, &y_offset );

        x_trans = x_offset;
        y_trans = y_offset;

        slice_window->slice.volumes[volume_index].views[view_index].x_trans = x_trans;
        slice_window->slice.volumes[volume_index].views[view_index].y_trans = y_trans;
        slice_window->slice.volumes[volume_index].views[view_index].x_scaling =
                                                                x_scale;
        slice_window->slice.volumes[volume_index].views[view_index].y_scaling =
                                                                y_scale;

#ifdef DEBUG
{
    VIO_Real  test_x_offset, test_y_offset;

    get_slice_plane( slice_window, volume_index, view,
                     origin, x_axis, y_axis );
    convert_voxel_to_world( volume, origin, &xw, &yw, &zw );
    convert_world_to_voxel( volume, xw, yw, zw, current_voxel );
    convert_voxel_to_pixel( slice_window, volume_index,
                            view, current_voxel,
                            &test_x_offset, &test_y_offset );

    if( !numerically_close( test_x_offset, x_offset, TOL ) ||
        !numerically_close( test_y_offset, y_offset, TOL ) )
    {
        print( "Offsets: %g\t%g\n", x_offset, y_offset );
        print( "         %g\t%g\n", test_x_offset, test_y_offset );
    }
}
#endif
    }

    slice_view_has_changed( slice_window, view_index );
}

/**
 * Completely reset the slice view back to "normal", whatever that is.
 *
 * \param slice_window The display_struct of the slice window.
 * \param view_index The index of the affected view.
 */
void  reset_slice_view(
    display_struct    *slice_window,
    int               view_index )
{
    VIO_Volume volume;
    VIO_Real   x1, x2, y1, y2, x_offset, y_offset;
    int        int_x1, int_x2, int_y1, int_y2;
    int        current_volume_index;
    VIO_Volume current_volume;
    int        x_min, x_max, y_min, y_max;
    int        x_min_vp, x_max_vp, y_min_vp, y_max_vp;
    int        volume_index;
    VIO_Real   xw, yw, zw;
    VIO_Real   current_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real   x_scale, y_scale, x_trans, y_trans;
    VIO_Real   current_origin[VIO_MAX_DIMENSIONS];
    VIO_Real   current_x_axis[VIO_MAX_DIMENSIONS];
    VIO_Real   current_y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real   origin[VIO_MAX_DIMENSIONS];
    VIO_Real   x_axis[VIO_MAX_DIMENSIONS];
    VIO_Real   y_axis[VIO_MAX_DIMENSIONS];

    slice_window_struct *slice_ptr = &slice_window->slice;

    if( get_n_volumes(slice_window) == 0 )
        return;

    get_slice_viewport( slice_window, view_index,
                        &x_min_vp, &x_max_vp, &y_min_vp, &y_max_vp );

    current_volume_index = get_current_volume_index( slice_window );
    current_volume = get_volume( slice_window );

    update_all_slice_axes( slice_window, current_volume_index, view_index );

    get_slice_plane( slice_window, current_volume_index, view_index,
                     current_origin, current_x_axis, current_y_axis );

    x_min = INT_MAX;
    x_max = -INT_MAX;
    y_min = INT_MAX;
    y_max = -INT_MAX;

    for_less( volume_index, 0, get_n_volumes( slice_window ) )
    {
        get_slice_plane( slice_window, volume_index, view_index,
                         origin, x_axis, y_axis );

        volume = get_nth_volume( slice_window, volume_index );

        get_volume_mapping_range( volume, origin, x_axis, y_axis,
                                  0.0, 0.0, 1.0, 1.0,
                                  &x1, &x2, &y1, &y2 );

        if( volume_index != current_volume_index )
        {
            convert_voxel_to_world( volume, origin, &xw, &yw, &zw );
            convert_world_to_voxel( current_volume, xw, yw, zw,
                                    current_voxel );
            convert_voxel_to_slice_pixel( current_volume,
                                          current_voxel, current_origin,
                                          current_x_axis, current_y_axis,
                                          0.0, 0.0, 1.0, 1.0,
                                          &x_offset, &y_offset );


            x1 += x_offset;
            x2 += x_offset;
            y1 += y_offset;
            y2 += y_offset;
        }

        int_x1 = VIO_FLOOR( x1 );
        int_x2 = VIO_CEILING( x2 );
        int_y1 = VIO_FLOOR( y1 );
        int_y2 = VIO_CEILING( y2 );

        if( int_x1 < x_min )
            x_min = int_x1;
        if( int_x2 > x_max )
            x_max = int_x2;
        if( int_y1 < y_min )
            y_min = int_y1;
        if( int_y2 > y_max )
            y_max = int_y2;
    }

    if( get_n_volumes( slice_window ) == 0 || x_min == x_max || y_min == y_max )
    {
        x_trans = 0.0;
        y_trans = 0.0;
        x_scale = 1.0;
        y_scale = 1.0;
    }
    else
    {
        x_scale = (VIO_Real) (x_max_vp - x_min_vp + 1) / (VIO_Real) (x_max - x_min) /
                             (1.0 + Slice_fit_oversize);
        y_scale = (VIO_Real) (y_max_vp - y_min_vp + 1) / (VIO_Real) (y_max - y_min) /
                             (1.0 + Slice_fit_oversize);

        if( x_scale < y_scale )
            y_scale = x_scale;
        else
            x_scale = y_scale;

        slice_ptr->slice_views[view_index].used_viewport_x_size =
                     VIO_ROUND( VIO_FABS( x_scale * (VIO_Real) (x_max - x_min) *
                                  (1.0 + Slice_fit_oversize) ));
        slice_ptr->slice_views[view_index].used_viewport_y_size =
                     VIO_ROUND( VIO_FABS( y_scale * (VIO_Real) (y_max - y_min) *
                                  (1.0 + Slice_fit_oversize) ));

        x_trans = ((VIO_Real) (x_max_vp - x_min_vp + 1) - x_scale *
                          (VIO_Real) (x_max - x_min))/2.0 - x_scale * (VIO_Real) x_min;
        y_trans = ((VIO_Real) (y_max_vp - y_min_vp + 1) - y_scale *
                          (VIO_Real) (y_max - y_min))/2.0 - y_scale * (VIO_Real) y_min;
    }

    slice_ptr->volumes[current_volume_index].views[view_index].x_trans =
                                                              x_trans;
    slice_ptr->volumes[current_volume_index].views[view_index].y_trans =
                                                              y_trans;
    slice_ptr->volumes[current_volume_index].views[view_index].x_scaling =
                                                              x_scale;
    slice_ptr->volumes[current_volume_index].views[view_index].y_scaling =
                                                              y_scale;

    match_view_scale_and_translation( slice_window, view_index,
                                      current_volume_index );

    slice_ptr->slice_views[view_index].prev_viewport_x_size =
                                       (x_max_vp - x_min_vp + 1);
    slice_ptr->slice_views[view_index].prev_viewport_y_size =
                                       (y_max_vp - y_min_vp + 1);

    slice_view_has_changed( slice_window, view_index );
}

/**
 * Resize a slice view, assuming that the size of the viewport changed,
 * for example.
 *
 * \param slice_window The display_struct of the slice window.
 * \param view_index The index of the affected view.
 */
void  resize_slice_view(
    display_struct    *slice_window,
    int               view_index )
{
    int            volume_index;
    int            x_min, x_max, y_min, y_max;
    int            used_viewport_x_size, used_viewport_y_size;
    slice_window_struct *slice_ptr = &slice_window->slice;

    get_slice_viewport( slice_window, view_index,
                        &x_min, &x_max, &y_min, &y_max );

    for_less( volume_index, 0, get_n_volumes( slice_window ) )
    {
        resize_volume_slice(
             slice_ptr->slice_views[view_index].prev_viewport_x_size,
             slice_ptr->slice_views[view_index].prev_viewport_y_size,
             slice_ptr->slice_views[view_index].used_viewport_x_size,
             slice_ptr->slice_views[view_index].used_viewport_y_size,
             x_max - x_min + 1, y_max - y_min + 1,
             &slice_ptr->volumes[volume_index].views[view_index].x_trans,
             &slice_ptr->volumes[volume_index].views[view_index].y_trans,
             &slice_ptr->volumes[volume_index].views[view_index].x_scaling,
             &slice_ptr->volumes[volume_index].views[view_index].y_scaling,
             &used_viewport_x_size, &used_viewport_y_size );
    }

    slice_ptr->slice_views[view_index].used_viewport_x_size =
                                          used_viewport_x_size;
    slice_ptr->slice_views[view_index].used_viewport_y_size =
                                          used_viewport_y_size;
    slice_ptr->slice_views[view_index].prev_viewport_x_size =
                                          (x_max - x_min + 1);
    slice_ptr->slice_views[view_index].prev_viewport_y_size =
                                          (y_max - y_min + 1);

    rebuild_slice_rulers( slice_window, view_index ); /*???*/

    slice_view_has_changed( slice_window, view_index );
}

/**
 * Gets the minimum absolute step size associated with a volume. This
 * is used by scale_slice_view() to make an intelligent choice about the
 * minimum allowable scaling.
 *
 * \param slice_window The display_struct associated with the slice view.
 * \param volume_index The index of the volume to use.
 * \returns The step size with the minimum magnitude (absolute value).
 */
VIO_Real
get_volume_min_step(display_struct *slice_window, int volume_index)
{
  VIO_Volume volume = get_nth_volume(slice_window, volume_index);
  VIO_Real steps[VIO_MAX_DIMENSIONS];
  int i;
  VIO_Real min_step;

  get_volume_separations(volume, steps);
  min_step = fabs(steps[0]);
  for (i = 1; i < get_volume_n_dimensions(volume); i++)
  {
    VIO_Real tmp = fabs(steps[i]);
    if (tmp < min_step)
      min_step = tmp;
  }
  return min_step;
}

/**
 * Control zooming of the slice viewport. If the scale_factor is
 * greater than one, this will increase the number of pixels per voxel
 * (the apparent voxel size). If the scale_factor is less than one, the
 * number of pixels per voxel will decrease. We impose limits to
 * prevent ridiculous scaling: Our voxels should never be bigger than
 * the viewport, and they should never be less than 10% of the full
 * size.
 *
 * \param slice_window The display_struct of the slice window.
 * \param view_index The index of the affected view.
 * \param scale_factor A scaling factor.
 */
void  scale_slice_view(
    display_struct    *slice_window,
    int               view_index,
    VIO_Real          scale_factor )
{
    int     volume_index;
    int     x_min, x_max, y_min, y_max;
    int     x_size, y_size;
    loaded_volume_struct *volumes_ptr;

    get_slice_viewport( slice_window, view_index,
                        &x_min, &x_max, &y_min, &y_max );

    x_size = x_max - x_min + 1;
    y_size = y_max - y_min + 1;

    volumes_ptr = slice_window->slice.volumes;

    /* Verify that this scaling isn't insane. I disallow scaling of
     * less than 10%, or where the scaling would create voxels that
     * are larger than the viewport!
     */
    for_less( volume_index, 0, get_n_volumes( slice_window ) )
    {

      VIO_Real x_scale = volumes_ptr[volume_index].views[view_index].x_scaling;
      VIO_Real step = get_volume_min_step(slice_window, volume_index);

      /* Make sure the voxels aren't ridiculously large...
       */
      if (scale_factor > 1.0 && x_scale * step >= x_size) {
        return;
      }

      /* Make sure the image does not get smaller than 10% of its
       * original size.
       */
      if (scale_factor < 1.0 && x_scale < 0.1) {
        return;
      }
    }

    for_less( volume_index, 0, get_n_volumes( slice_window ) )
    {
        scale_slice_about_viewport_centre( scale_factor,
             x_size, y_size,
             &volumes_ptr[volume_index].views[view_index].x_trans,
             &volumes_ptr[volume_index].views[view_index].y_trans,
             &volumes_ptr[volume_index].views[view_index].x_scaling,
             &volumes_ptr[volume_index].views[view_index].y_scaling );
    }

    slice_view_has_changed( slice_window, view_index );
}

/**
 * Move the slice relative to the the viewport.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param view_index The index of one of the four slice view panels.
 * \param dx Number of pixels to translate in the column direction.
 * \param dy Number of pixels to translate in the row direction.
 */
void  translate_slice_view(
    display_struct    *slice_window,
    int               view_index,
    VIO_Real          dx,
    VIO_Real          dy )
{
    int                 volume_index;
    slice_window_struct *slice_ptr = &slice_window->slice;

    for_less( volume_index, 0, get_n_volumes( slice_window ) )
    {
        slice_ptr->volumes[volume_index].views[view_index].x_trans += dx;
        slice_ptr->volumes[volume_index].views[view_index].y_trans += dy;
    }

    slice_view_has_changed( slice_window, view_index );
}

/**
 * Determines whether the mouse is over one of the slice views, and if so,
 * returns the index of that view.
 * 
 * \param display A pointer to a display_struct.
 * \param x_pixel The x coordinate of the mouse.
 * \param y_pixel The y coordinate of the mouse.
 * \param view_index Receives the index of the view.
 * \returns TRUE if the mouse is over a slice view.
 */
VIO_BOOL  find_slice_view_mouse_is_in(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *view_index )
{
    VIO_BOOL          found;
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

/**
 * Convert a window position in pixel coordinates to a voxel coordinate
 * relative to the given volume, also returning the view index associated
 * with the position.
 *
 * \param display A pointer to a display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param x_pixel The x pixel coordinate relative to the window.
 * \param y_pixel The y pixel coordinate relative to the window.
 * \param voxel Receives the voxel coordinate.
 * \param view_index Receives the index of the view corresponding to this
 * position.
 * \returns TRUE if the pixel coordinate can be transformed to 
 * a valid voxel coordinate.
 */
VIO_BOOL  convert_pixel_to_voxel(
    display_struct    *display,
    int               volume_index,
    int               x_pixel,
    int               y_pixel,
    VIO_Real          voxel[],
    int               *view_index )
{
    VIO_BOOL          found;
    display_struct    *slice_window;
    int               x_min, x_max, y_min, y_max;
    VIO_Real          origin[VIO_MAX_DIMENSIONS];
    VIO_Real          x_axis[VIO_MAX_DIMENSIONS], y_axis[VIO_MAX_DIMENSIONS];

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
                                volume_index), (VIO_Real) x_pixel, (VIO_Real) y_pixel,
         origin, x_axis, y_axis,
         slice_window->slice.volumes[volume_index].views[*view_index].x_trans,
         slice_window->slice.volumes[volume_index].views[*view_index].y_trans,
         slice_window->slice.volumes[volume_index].views[*view_index].x_scaling,
         slice_window->slice.volumes[volume_index].views[*view_index].y_scaling,
         voxel );
         voxel[VIO_T] = origin[VIO_T];
         voxel[VIO_V] = 0.0;
    }

    return( found );
}

/**
 * Convert a voxel coordinate to a pixel coordinate. The returned
 * pixel coordinate is given _relative to the viewport_! Therefore
 * this function is not exactly the inverse of convert_pixel_to_voxel().
 * That function takes a pixel coordinate _relative to the window_.
 *
 * \param display A pointer to a display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The index of one of the four slice view panels.
 * \param voxel The voxel coordinate to transform.
 * \param x_pixel Receives the x pixel coordinate relative to the viewport.
 * \param y_pixel Receives the y pixel coordinate relative to the viewport.
 */
void  convert_voxel_to_pixel(
    display_struct    *display,
    int               volume_index,
    int               view_index,
    VIO_Real          voxel[],
    VIO_Real          *x_pixel,
    VIO_Real          *y_pixel )
{
    VIO_Volume        volume;
    display_struct    *slice_window;
    VIO_Real          x_real_pixel, y_real_pixel;
    VIO_Real          origin[VIO_MAX_DIMENSIONS];
    VIO_Real          x_axis[VIO_MAX_DIMENSIONS], y_axis[VIO_MAX_DIMENSIONS];

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
        *x_pixel = 0;
        *y_pixel = 0;
    }
}

/**
 * Get the parameters needed to transform voxel coordinates to pixel
 * coordinates for the given volume and view.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The index of one of the four slice view panels.
 * \param x_index Receives the index of the column axis.
 * \param y_index Receives the index of the row axis.
 * \param x_scale Receives the scaling to apply in the column direction.
 * \param x_trans Receives the offset to apply in the column direction.
 * \param y_scale Receives the scaling to apply in the row direction.
 * \param y_trans Receives the offset to apply in the row direction.
 */
void  get_voxel_to_pixel_transform(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    int               *x_index,
    int               *y_index,
    VIO_Real          *x_scale,
    VIO_Real          *x_trans,
    VIO_Real          *y_scale,
    VIO_Real          *y_trans )
{
    int      axis;
    VIO_Real voxel[VIO_MAX_DIMENSIONS];
    VIO_Real x, y;

    if( !slice_has_ortho_axes( slice_window, volume_index,
                               view_index, x_index, y_index, &axis ) )
    {
        handle_internal_error( "get_voxel_to_pixel_transform" );
        return;
    }

    /* Get the position of the origin.
     */
    for_less( axis, 0, VIO_MAX_DIMENSIONS )
      voxel[axis] = 0.0;
    convert_voxel_to_pixel( slice_window, volume_index, view_index, voxel,
                            x_trans, y_trans );

    voxel[*x_index] = 1.0;
    voxel[*y_index] = 1.0;
    convert_voxel_to_pixel( slice_window, volume_index, view_index, voxel,
                            &x, &y );

    *x_scale = x - *x_trans;
    *y_scale = y - *y_trans;
}

/** 
 * Gets the voxel coordinate corresponding to a particular point in world
 * coordinates. The voxel coordinates are calculated with respect to the
 * current volume.
 *
 * \param display A pointer to any display_struct.
 * \param point A pointer to the point to be converted.
 * \param voxel Receives the voxel coordinates of the point.
 * \returns TRUE if the voxel coordinates are valid and lie within the
 * current volume.
 */
VIO_BOOL  get_voxel_corresponding_to_point(
    display_struct  *display,
    const VIO_Point *point,
    VIO_Real        voxel[] )
{
    VIO_Volume     volume;
    VIO_BOOL       converted = FALSE;
    int i;

    /* Properly initialize voxel[i].
     */
    for_less( i, 0, VIO_MAX_DIMENSIONS )
    {
        voxel[i] = 0.0;
    }
    if( get_slice_window_volume( display, &volume ) )
    {
        convert_world_to_voxel( volume,
                                (VIO_Real) Point_x(*point),
                                (VIO_Real) Point_y(*point),
                                (VIO_Real) Point_z(*point), voxel );

        converted = voxel_is_within_volume( volume, voxel );
    }

    return( converted );
}

/**
 * Gets the various size parameters that control the layout of the
 * slice view window. All of the returned values are expressed in
 * pixel units.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param left_panel_width Receives the width of the left panel 
 * that contains the colour bar and text readout.
 * \param left_slice_width Receives the width of the left-hand slice views.
 * \param right_slice_width Receives he width of the right-hand slice views.
 * \param bottom_slice_height Receives the height of the bottom slice views.
 * \param top_slice_height Receives the height of the upper slice views.
 * \param text_panel_height Receives the height of the text readout panel.
 * \param colour_bar_panel_height Receives the height of the colour bar.
 */
void   get_slice_window_partitions(
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
    *left_slice_width = VIO_ROUND( slice_window->slice.x_split *
                               (VIO_Real) (x_size - *left_panel_width) );
    *right_slice_width = x_size - *left_panel_width - *left_slice_width;
    *bottom_slice_height = VIO_ROUND( slice_window->slice.y_split * (VIO_Real) y_size );
    *top_slice_height = y_size - *bottom_slice_height;
}

/**
 * Gets the rectangular region associated with one of the four slice
 * views.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param view_index The zero-based index of the view.
 * \param x_min Receives the minimum x coordinate of the viewport.
 * \param x_max Receives the maximum x coordinate of the viewport.
 * \param y_min Receives the minimum y coordinate of the viewport.
 * \param y_max Receives the maximum y coordinate of the viewport.
 */
void  get_slice_viewport(
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

/**
 * Gets the rectangular region associated with the colour bar, the
 * widget used to set the lower and upper limits of the volume colour
 * coding.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param x_min Receives the minimum x coordinate of the viewport.
 * \param x_max Receives the maximum x coordinate of the viewport.
 * \param y_min Receives the minimum y coordinate of the viewport.
 * \param y_max Receives the maximum y coordinate of the viewport.
 */
void  get_colour_bar_viewport(
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

/**
 * Gets the rectangular region associated with the text display (also
 * referred to as the "probe" or "readout" in other contexts).
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param x_min Receives the minimum x coordinate of the viewport.
 * \param x_max Receives the maximum x coordinate of the viewport.
 * \param y_min Receives the minimum y coordinate of the viewport.
 * \param y_max Receives the maximum y coordinate of the viewport.
 */
void  get_text_display_viewport(
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
    *x_max = left_panel_width - 2;
    *y_min = 0;
    *y_max = text_panel_height - 2;
}

/**
 * Gets the position of the intersection of the slice dividers that define the
 * relative sizes of the four view panels.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param x Receives the x position.
 * \param y Receives the y position.
 */
void  get_slice_divider_intersection(
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

/**
 * Sets the position of the intersection of the slice dividers that define the
 * relative sizes of the four view panels. Updates the display accordingly.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param x The new x position.
 * \param y The new y position.
 */
void  set_slice_divider_intersection(
    display_struct    *slice_window,
    int               x,
    int               y )
{
    int  view;
    int  left_panel_width, left_slice_width, right_slice_width;
    int  bottom_slice_height, top_slice_height, text_panel_height;
    int  colour_bar_panel_height;
    VIO_Real x_split, y_split;

    get_slice_window_partitions( slice_window,
                                 &left_panel_width, &left_slice_width,
                                 &right_slice_width,
                                 &bottom_slice_height, &top_slice_height,
                                 &text_panel_height, &colour_bar_panel_height );

    x_split = (VIO_Real) (x - left_panel_width) /
              (VIO_Real) (left_slice_width + right_slice_width);
    y_split = (VIO_Real) y /
              (VIO_Real) (bottom_slice_height + top_slice_height);

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

/**
 * Calculate the volume index, view index, and voxel position
 * corresponding to a pixel position in the slice window.  Always
 * starts from the last (or top) volume and checks whether the current
 * position overlaps that volume, ignoring volumes that are inactive,
 * either because their opacity is zero or they are currently
 * invisible.
 *
 * \param slice_window The slice window's display_struct
 * \param x The pixel x coordinate.
 * \param y The pixel y coordinate.
 * \param volume_index The volume index at this cursor position.
 * \param view_index The view index at this cursor position.
 * \param voxel The voxel coordinates corresponding to this cursor position.
 * \returns TRUE if an active volume and view can be identified.
 */
VIO_BOOL  get_volume_corresponding_to_pixel(
    display_struct    *slice_window,
    int               x,
    int               y,
    int               *volume_index,
    int               *view_index,
    VIO_Real          voxel[] )
{
    if( !get_slice_window( slice_window, &slice_window ) )
        return( FALSE );

    *volume_index = get_current_volume_index( slice_window );
    if (*volume_index < 0)
        return( FALSE );

    if( slice_window->slice.volumes[*volume_index].opacity > 0.0 &&
        convert_pixel_to_voxel( slice_window, *volume_index, x, y,
                                voxel, view_index ) &&
        get_slice_visibility( slice_window, *volume_index, *view_index ) )
    {
        return (*volume_index >= 0);
    }

    for( *volume_index = get_n_volumes( slice_window ) - 1;  *volume_index >= 0;
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

/**
 * If the mouse cursor is over any of the slice views, get the voxel
 * position, the volume index, and the view index corresponding to
 * the mouse cursor position.
 *
 * \param display Any display_struct.
 * \param voxel The voxel position corresponding to the mouse position.
 * \param volume_index Receives the index of the desired volume.
 * \param view_index Receives the index of the slice view under the mouse.
 * \returns TRUE if the mouse is over one of the the slice views.
 */
VIO_BOOL  get_voxel_in_slice_window(
    display_struct    *display,
    VIO_Real          voxel[],
    int               *volume_index,
    int               *view_index )
{
    display_struct    *slice_window;
    int               x_mouse, y_mouse;

    if( !get_slice_window( display, &slice_window ) )
        return( FALSE );

    if (!G_get_mouse_position( slice_window->window, &x_mouse, &y_mouse ))
        return( FALSE );

    return( get_volume_corresponding_to_pixel( slice_window, x_mouse, y_mouse,
                                               volume_index, view_index,
                                               voxel));
}

/**
 * Return the voxel coordinate corresponding to the mouse cursor position
 * in the 3D window, checking that the mouse is in the 3D window and there
 * is a loaded volume in the slice window.
 *
 * \param display The display_struct of the 3D window.
 * \param voxel The returned voxel coordinates.
 * \returns TRUE if the voxel coordinate was found.
 */
static VIO_BOOL
get_voxel_in_three_d_window(
    display_struct    *display,
    VIO_Real          voxel[] )
{
    VIO_BOOL         found;
    object_struct    *object;
    int              object_index;
    VIO_Point        intersection_point;
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

/**
 * Get the voxel coordinates, volume index, and view index corresponding 
 * to the current mouse cursor position. These values are calculated for
 * either the 3D view or the slice view windows.
 *
 * \param display A pointer to any display_struct.
 * \param volume_index Receives the index of the volume under the mouse.
 * \param view_index Receives the index of the view under the mouse. 
 * \param voxel Receives the voxel coordinates corresponding to the 
 * mouse position, with respect to the volume.
 * \returns TRUE if a valid voxel location can be calculated.
 */
VIO_BOOL  get_voxel_under_mouse(
    display_struct    *display,
    int               *volume_index,
    int               *view_index,
    VIO_Real          voxel[] )
{
    display_struct    *three_d, *slice_window;
    VIO_BOOL           found;

    three_d = get_display_by_type( THREE_D_WINDOW );

    if( !get_slice_window(three_d, &slice_window) )
    {
        found = FALSE;
    }
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

/**
 * Get the current voxel coordinate for a given volume.
 *
 * \param display A pointer to a display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param voxel The returned voxel position.
 */
void  get_current_voxel(
    display_struct    *display,
    int               volume_index,
    VIO_Real          voxel[] )
{
    int              c;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        for_less( c, 0, VIO_MAX_DIMENSIONS )
        {
            voxel[c] = slice_window->slice.volumes[volume_index].
                                             current_voxel[c];
        }
    }
    else
    {
        for_less( c, 0, VIO_MAX_DIMENSIONS )
        {
            voxel[c] = 0.0;
        }
    }
}

/**
 * Sets the current position for all volumes with respect to a
 * specific voxel position in a given volume. Arranges for the actual
 * display to be updated to reflect the new position.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param ref_volume_index The zero-based index of the reference volume.
 * \param ref_voxel The voxel position in the reference volume.
 * \returns TRUE if the current position was updated for any volume.
 */
VIO_BOOL  set_current_voxel(
    display_struct    *slice_window,
    int               ref_volume_index,
    VIO_Real          ref_voxel[] )
{
    VIO_BOOL          changed;
    int               i, view_index, volume_index;
    VIO_Real          ref_w[VIO_N_DIMENSIONS];
    VIO_Real          new_voxel[VIO_MAX_DIMENSIONS];
    VIO_Volume        volume;
    int               n_dimensions;
    int               cur_volume_index;

    cur_volume_index = get_current_volume_index( slice_window );

    changed = FALSE;

    volume = get_nth_volume( slice_window, ref_volume_index );

    convert_voxel_to_world( volume, ref_voxel,
                            &ref_w[VIO_X], &ref_w[VIO_Y], &ref_w[VIO_Z] );

    n_dimensions = get_volume_n_dimensions( volume );

    for_less( volume_index, 0, get_n_volumes( slice_window ) )
    {
        loaded_volume_struct *lv_ptr = &slice_window->slice.volumes[volume_index];
        if( volume_index == ref_volume_index )
        {
            /* For the reference volume, there is obviously no need to
             * transform the voxel coordinates.
             */
            for_less( i, 0, VIO_MAX_DIMENSIONS )
                new_voxel[i] = ref_voxel[i];
        }
        else
        {
            /* For all other volumes, we need to transform from the
             * world space of the reference volume to the voxel space
             * of this volume.
             */
            convert_world_to_voxel( get_nth_volume(slice_window, volume_index),
                                    ref_w[VIO_X], ref_w[VIO_Y], ref_w[VIO_Z],
                                    new_voxel );

            /* Make sure all elements of the array are initialized.
             */
            for_less( i, VIO_N_DIMENSIONS, VIO_MAX_DIMENSIONS )
                new_voxel[i] = 0.0;
        }

        for_less( i, 0, n_dimensions )
        {
            if( new_voxel[i] != lv_ptr->current_voxel[i] )
            {
                lv_ptr->current_voxel[i] = new_voxel[i];
                changed = TRUE;

                for_less( view_index, 0, N_SLICE_VIEWS )
                {
                    int axis_index, x_index, y_index;

                    /* Force an update for the slice view if it is:
                       1. Non-orthogonal (oblique).
                       2. The current (changing) axis 'i'
                       3. The time axis.
                    */
                    if( !slice_has_ortho_axes( slice_window, volume_index,
                                               view_index, &x_index,
                                               &y_index, &axis_index ) ||
                        axis_index == i || VIO_T == i )
                    {
                        set_slice_window_update( slice_window, volume_index,
                                                 view_index, UPDATE_BOTH );

                        set_slice_text_update( slice_window, -1 );

                        /* Special handling necessary if the
                         * currently selected volume is changing.
                         */
                        if( volume_index == cur_volume_index )
                        {
                            set_crop_box_update( slice_window, view_index );
                            set_atlas_update( slice_window, view_index );
                            set_slice_outline_update( slice_window, view_index );
                        }
                    }
                }
            }
        }
    }

    set_slice_cross_section_update( slice_window, -1 );
    rebuild_volume_cross_section( slice_window );
    set_slice_cursor_update( slice_window, -1 );
    set_probe_update( slice_window );
    set_slice_outline_update( slice_window, -1 ); /* Is this needed?? */

    return( changed );
}

/**
 * Compute the axis perpendicular to the given current column ("x")
 * and row ("y") axes. The returned axis will be in voxel coordinates,
 * normalized to length one.
 *
 * \param volume The volume for which this information is desired.
 * \param x_axis An array containing the three components of the column
 * direction (left-right) in the view panel, in voxel coordinates.
 * \param y_axis An array containing the three components of the row
 * direction (up-down) in the view panel, in voxel coordinates.
 * \param perp_axis The output array, which will be filled in with the
 * perpendicular axis for this view panel. Again, this is in voxel coordinates.
 */
static  void  get_voxel_axis_perpendicular(
    VIO_Volume     volume,
    const VIO_Real x_axis[],
    const VIO_Real y_axis[],
    VIO_Real       perp_axis[] )
{
    int      c, a1, a2;
    VIO_Real separations[VIO_MAX_DIMENSIONS];

    get_volume_separations( volume, separations );

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        a1 = (c + 1) % VIO_N_DIMENSIONS;
        a2 = (c + 2) % VIO_N_DIMENSIONS;
        perp_axis[c] = x_axis[a1] * y_axis[a2] - x_axis[a2] * y_axis[a1];

        perp_axis[c] *= VIO_FABS( separations[a1] * separations[a2] /
                             separations[c] );
    }

    array_normalize( perp_axis, VIO_N_DIMENSIONS );
}

/**
 * Get the axis perpendicular to the given volume and slice view. The
 * value returned is in _voxel_ coordinates.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The index of one of the four slice view panels.
 * \param voxel_perp The returned axis components.
 */
void  get_slice_perp_axis(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    VIO_Real         voxel_perp[] )
{
    get_voxel_axis_perpendicular( get_nth_volume(slice_window,volume_index),
        slice_window->slice.volumes[volume_index].views[view_index].x_axis,
        slice_window->slice.volumes[volume_index].views[view_index].y_axis,
        voxel_perp );
}

/**
 * Sets the plane of the given volume and view, from a
 * given perpendicular axis.
 *
 * Primarily used to set the axis of the arbitrary (oblique) view.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The index of one of the four slice view panels.
 * \param voxel_perp The axis perpendicular to the desired plane.
 */
void  set_slice_plane_perp_axis(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    const VIO_Real   voxel_perp[] )
{
    VIO_Real     max_value, len, sign;
    VIO_Real     len_x_axis, len_y_axis, factor;
    VIO_Real     used_x_axis[VIO_MAX_DIMENSIONS];
    VIO_Real     used_y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real     separations[VIO_MAX_DIMENSIONS];
    VIO_Real     sep[VIO_MAX_DIMENSIONS];
    VIO_Real     perp[VIO_MAX_DIMENSIONS];
    VIO_Vector   axis, vect, new_axis, tmp;
    int      x_index, y_index, view;
    int      c, max_axis;

    get_volume_separations( get_nth_volume(slice_window,volume_index),
                            separations );

    for_less( c, 0, VIO_N_DIMENSIONS )
        separations[c] = VIO_FABS( separations[c] );

    for_less( c, 0, VIO_N_DIMENSIONS )
        perp[c] = voxel_perp[c] * separations[c];

    if( slice_window->slice.cross_section_vector_present )
    {
        for_less( c, 0, VIO_N_DIMENSIONS )
        {
            Vector_coord(axis,c) = (VIO_Point_coord_type) perp[c];
            Vector_coord(vect,c) = (VIO_Point_coord_type)
                (slice_window->slice.cross_section_vector[c] * separations[c]);
        }

        CROSS_VECTORS( tmp, vect, axis );
        CROSS_VECTORS( new_axis, vect, tmp );
        if( DOT_VECTORS( new_axis, axis ) < 0.0 )
            SCALE_VECTOR( new_axis, new_axis, -1.0f );

        for_less( c, 0, VIO_N_DIMENSIONS )
            perp[c] = (VIO_Real) Vector_coord( new_axis, c );
    }

    max_value = 0.0;
    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        if( c == 0 || VIO_FABS(perp[c]) > max_value )
        {
            max_value = VIO_FABS(perp[c]);
            max_axis = c;
        }
    }

    switch( max_axis )
    {
    case VIO_X: x_index = VIO_Y;   y_index = VIO_Z;  break;
    case VIO_Y: x_index = VIO_X;   y_index = VIO_Z;  break;
    case VIO_Z: x_index = VIO_X;   y_index = VIO_Y;  break;
    }

    used_x_axis[VIO_X] = 0.0;
    used_x_axis[VIO_Y] = 0.0;
    used_x_axis[VIO_Z] = 0.0;

    get_volume_separations( get_nth_volume(slice_window,volume_index), sep );

    if( sep[x_index] < 0.0 )
        used_x_axis[x_index] = -1.0;
    else
        used_x_axis[x_index] = 1.0;

    len = perp[VIO_X] * perp[VIO_X] + perp[VIO_Y] * perp[VIO_Y] + perp[VIO_Z] * perp[VIO_Z];
    if( len == 0.0 )
        return;

    factor = used_x_axis[x_index] * perp[x_index] / len;

    for_less( c, 0, VIO_N_DIMENSIONS )
        used_x_axis[c] -= factor * perp[c];

    used_y_axis[VIO_X] = perp[VIO_Y] * used_x_axis[VIO_Z] - used_x_axis[VIO_Y] * perp[VIO_Z];
    used_y_axis[VIO_Y] = perp[VIO_Z] * used_x_axis[VIO_X] - used_x_axis[VIO_Z] * perp[VIO_X];
    used_y_axis[VIO_Z] = perp[VIO_X] * used_x_axis[VIO_Y] - used_x_axis[VIO_X] * perp[VIO_Y];

    len_x_axis = 0.0;
    len_y_axis = 0.0;
    for_less( c, 0, VIO_N_DIMENSIONS )
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

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        used_x_axis[c] /= len_x_axis;
        used_y_axis[c] /= sign * len_y_axis;
    }

    for_less( c, 0, VIO_N_DIMENSIONS )
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

/**
 * Sets the plane of the given volume and view, from a
 * given pair of row and column axes.
 *
 * Primarily used to set the axis of the arbitrary (oblique) view.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The index of one of the four slice view panels.
 * \param x_axis The unit vector of the column direction.
 * \param y_axis The unit vector of the row direction.
 */
void  set_slice_plane(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    const VIO_Real   x_axis[],
    const VIO_Real   y_axis[] )
{
    VIO_Real     perp[VIO_MAX_DIMENSIONS];

    get_voxel_axis_perpendicular( get_nth_volume( slice_window, volume_index ),
                                  x_axis, y_axis, perp );

    set_slice_plane_perp_axis( slice_window, volume_index, view_index, perp );
}

/**
 * Get the plane and origin of the given volume and view.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The index of one of the four slice view panels.
 * \param origin Receives the current origin of the view in voxel coordinates.
 * \param x_axis Receives the vector corresponding to the column direction.
 * \param y_axis Receives the vector corresponding to the row direction.
 */
void  get_slice_plane(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    VIO_Real         origin[],
    VIO_Real         x_axis[],
    VIO_Real         y_axis[] )
{
    int        c, axis;
    VIO_Real   separations[VIO_MAX_DIMENSIONS];
    VIO_Real   voxel[VIO_MAX_DIMENSIONS], perp_axis[VIO_MAX_DIMENSIONS];
    VIO_Real   voxel_dot_perp, perp_dot_perp, factor;

    get_volume_separations( get_nth_volume( slice_window, volume_index ),
                            separations );

    get_current_voxel( slice_window, volume_index, voxel );
    get_slice_perp_axis( slice_window, volume_index, view_index, perp_axis );

    axis = -1;

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        separations[c] = VIO_FABS( separations[c] );
        perp_axis[c] *= separations[c];
        x_axis[c] = slice_window->slice.volumes[volume_index].
                                    views[view_index].x_axis[c];
        y_axis[c] = slice_window->slice.volumes[volume_index].
                                    views[view_index].y_axis[c];

        if( perp_axis[c] != 0.0 )
        {
            if( axis == -1 )
                axis = c;
            else
                axis = -2;
        }
    }

    /*--- if one of cardinal axes, avoid floating point roundoff, and just
          assign all zeros, and the current voxel position */

    if( axis >= 0 )
    {
        for_less( c, 0, VIO_N_DIMENSIONS )
            origin[c] = 0.0;
        origin[axis] = voxel[axis];
    }
    else
    {
        voxel_dot_perp = 0.0;
        for_less( c, 0, VIO_N_DIMENSIONS )
            voxel_dot_perp += voxel[c] * separations[c] * perp_axis[c];

        perp_dot_perp = 0.0;
        for_less( c, 0, VIO_N_DIMENSIONS )
            perp_dot_perp += perp_axis[c] * perp_axis[c];

        if( perp_dot_perp == 0.0 )
        {
            for_less( c, 0, VIO_N_DIMENSIONS )
                origin[c] = 0.0;
        }
        else
        {
            factor = voxel_dot_perp / perp_dot_perp;
            for_less( c, 0, VIO_N_DIMENSIONS )
                origin[c] = factor * perp_axis[c] / separations[c];
        }
    }
    /* Make sure time information is properly brought through.
     */
    origin[VIO_T] = voxel[VIO_T];
    x_axis[VIO_T] = 0.0;
    y_axis[VIO_T] = 0.0;
    origin[VIO_V] = 0.0;
    x_axis[VIO_V] = 0.0;
    y_axis[VIO_V] = 0.0;
}

/**
 * Gets the slice view index corresponding to the current mouse position.
 *
 * The slice view index is a value from 0 to N_SLICE_VIEWS - 1 that
 * corresponds to one of the four slice view panels.
 *
 * \param display A pointer to a top-level display_struct.
 * \param view_index Receives the index of the view under the mouse, if any.
 * \returns TRUE if the mouse is over one of the slice view panels.
 */
VIO_BOOL  get_slice_view_index_under_mouse(
    display_struct   *display,
    int              *view_index )
{
    VIO_Volume     volume;
    display_struct *slice_window;
    int            x, y;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        if( G_get_mouse_position( slice_window->window, &x, &y ) &&
            find_slice_view_mouse_is_in( slice_window, x, y, view_index ) )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * Get the volume index and index of the slice axis for the slice
 * view under the mouse.
 *
 * If the mouse is over the current sagittal plane, for example, this
 * function will return the index of the X axis of the volume, and also
 * returns the volume_index of the volume under the mouse.
 *
 * \param display A pointer to a top-level display_struct.
 * \param volume_index Receives the zero-based index of the volume.
 * \param axis_index Receives the slice axis index.
 * \returns TRUE if the mouse is over one of the orthogonal slice views.
 */
VIO_BOOL  get_axis_index_under_mouse(
    display_struct   *display,
    int              *volume_index,
    int              *axis_index )
{
    VIO_BOOL         found;
    VIO_Real         voxel[VIO_MAX_DIMENSIONS];
    int              view_index, x_index, y_index;
    display_struct   *slice_window;

    found = get_slice_window( display, &slice_window ) &&
            get_voxel_in_slice_window( slice_window, voxel, volume_index,
                                       &view_index ) &&
            slice_has_ortho_axes( slice_window, *volume_index,
                                  view_index, &x_index, &y_index, axis_index );

    return( found );
}

/**
 * See if the given volume and view have orthogonal axes. Also returns
 * the indices of the column, row, and slice axes.
 *
 * Certain operations are disallowed if the current slice has oblique
 * axes, so this function exists to make checking for that condition
 * as easy as possible.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The view index of the desired view.
 * \param x_index Receives the index of the voxel column axis.
 * \param y_index Receives the index of the voxel row axis.
 * \param axis_index Receives the index of the voxel slice axis.
 *
 * \returns TRUE if the slice view axes are orthogonal.
 */
VIO_BOOL  slice_has_ortho_axes(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    int              *x_index,
    int              *y_index,
    int              *axis_index )
{
    VIO_Real     origin[VIO_MAX_DIMENSIONS];
    VIO_Real     x_axis[VIO_MAX_DIMENSIONS];
    VIO_Real     y_axis[VIO_MAX_DIMENSIONS];
    int      c;

    get_slice_plane( slice_window, volume_index, view_index,
                     origin, x_axis, y_axis );

    *x_index = -1;
    *y_index = -1;
    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        if( fabs(x_axis[c]) > ORTHOGONAL_SLICE_EPSILON )
        {
            if( *x_index != -1 )
                return( FALSE );
            *x_index = c;
        }
        if( fabs(y_axis[c]) > ORTHOGONAL_SLICE_EPSILON )
        {
            if( *y_index != -1 )
                return( FALSE );
            *y_index = c;
        }
    }

    if( *x_index < 0 || *y_index < 0 || *x_index == *y_index )
        return( FALSE );

    *axis_index = VIO_N_DIMENSIONS - *x_index - *y_index;

    return( TRUE );
}

/**
 * Retrieve the view index of the current arbitrary (oblique) view.
 *
 * \param display A pointer to a top-level display_struct.
 * \returns The current oblique view index (between 0 and N_SLICE_VIEWS - 1).
 */
int  get_arbitrary_view_index(
    display_struct   *display )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        return( slice_window->slice.cross_section_index );
    else
        return( 0 );
}

/**
 * Returns the rectangular viewport (i.e. window coordinates) for each of the
 * parts of the slice view window.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param model_index The number of the model whose viewport we want.
 * \param x_min Receives the minimum x-coordinate of the rectangle.
 * \param x_max Receives the maximum x-coordinate of the rectangle.
 * \param y_min Receives the minimum y-coordinate of the rectangle.
 * \param y_max Receives the maximum y-coordinate of the rectangle.
 */
void  get_slice_model_viewport(
    display_struct   *slice_window,
    int              model_index,
    int              *x_min,
    int              *x_max,
    int              *y_min,
    int              *y_max )
{
    int   x_size, y_size;

    switch( model_index )
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
        get_slice_viewport( slice_window, model_index - SLICE_MODEL1,
                            x_min, x_max, y_min, y_max );
        break;

    case INTENSITY_PLOT_MODEL:
        get_slice_viewport( slice_window,
                            get_arbitrary_view_index(slice_window),
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

/**
 * Update the 3D view window's cursor position (in world coordinates)
 * based upon the voxel cursor position in the slice view window.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \returns TRUE if the cursor was updated successfully.
 */
VIO_BOOL  update_cursor_from_voxel(
    display_struct    *slice_window )
{
    VIO_Real  voxel[VIO_MAX_DIMENSIONS];
    VIO_Real  x_w, y_w, z_w;
    int       volume_index;
    VIO_Point origin;

    volume_index = get_current_volume_index( slice_window );
    if (volume_index < 0)
        return FALSE;

    get_current_voxel( slice_window, volume_index, voxel );

    convert_voxel_to_world( get_nth_volume( slice_window, volume_index ),
                            voxel, &x_w, &y_w, &z_w );

    fill_Point( origin, x_w, y_w, z_w );

    set_cursor_origin( slice_window, &origin );

    return TRUE;
}

/**
 * Update the slice view window's voxel position based on the value of
 * the 3D view window's cursor.
 *
 * \param display A pointer to a top-level display_struct.
 * \returns TRUE if the voxel position was changed.
 */
VIO_BOOL update_voxel_from_cursor(
    display_struct    *display )
{
    display_struct    *slice_window;
    VIO_BOOL          changed = FALSE;

    if( get_slice_window(display, &slice_window) &&
        get_n_volumes(slice_window) > 0 )
    {
        VIO_Point cursor_origin;
        int       volume_index;
        VIO_Real  voxel[VIO_MAX_DIMENSIONS];
        VIO_Real  prev_voxel[VIO_MAX_DIMENSIONS];

        get_cursor_origin(slice_window, &cursor_origin);

        (void) get_voxel_corresponding_to_point( slice_window,
                                                 &cursor_origin,
                                                 voxel );
        volume_index = get_current_volume_index( slice_window );
        get_current_voxel( slice_window, volume_index, prev_voxel );

        /* Don't change the time position.
         */
        voxel[VIO_T] = prev_voxel[VIO_T];
        voxel[VIO_V] = 0.0;
        changed = set_current_voxel( slice_window, volume_index, voxel );
    }

    return( changed );
}

/**
 * Update the slice axes for a view, using a particular volume as the
 * "reference" volume. All other volumes will be aligned to the voxel
 * space of the reference volume.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param ref_volume_index The zero-based index of the reference volume.
 * \param view_index The view index to update.
 */
static  void  update_all_slice_axes(
    display_struct    *slice_window,
    int               ref_volume_index,
    int               view_index )
{
    int        volume_index, axis;
    VIO_Real   ref_x_world[VIO_N_DIMENSIONS];
    VIO_Real   ref_y_world[VIO_N_DIMENSIONS];
    VIO_Volume ref_volume;
    volume_view_ptr ref_view;

    ref_volume = get_nth_volume( slice_window, ref_volume_index );

    ref_view = &slice_window->slice.volumes[ref_volume_index].views[view_index];

    convert_voxel_vector_to_world( ref_volume, ref_view->x_axis,
                                   &ref_x_world[VIO_X],
                                   &ref_x_world[VIO_Y],
                                   &ref_x_world[VIO_Z]);

    convert_voxel_vector_to_world( ref_volume, ref_view->y_axis,
                                   &ref_y_world[VIO_X],
                                   &ref_y_world[VIO_Y],
                                   &ref_y_world[VIO_Z]);

    for_less( volume_index, 0, get_n_volumes( slice_window ) )
    {
        VIO_Volume volume = get_nth_volume( slice_window, volume_index );
        VIO_Real   x_axis[VIO_MAX_DIMENSIONS];
        VIO_Real   y_axis[VIO_MAX_DIMENSIONS];
        VIO_Real   x_mag, y_mag;
        volume_view_ptr view;

        if( volume_index == ref_volume_index )
            continue;

        if( volumes_are_same_grid( ref_volume, volume ) )
        {
            for_less( axis, 0, VIO_N_DIMENSIONS )
            {
                x_axis[axis] = ref_view->x_axis[axis];
                y_axis[axis] = ref_view->y_axis[axis];
            }
        }
        else
        {
            convert_world_vector_to_voxel( volume,
                                           ref_x_world[VIO_X],
                                           ref_x_world[VIO_Y],
                                           ref_x_world[VIO_Z],
                                           x_axis );
            convert_world_vector_to_voxel( volume,
                                           ref_y_world[VIO_X],
                                           ref_y_world[VIO_Y],
                                           ref_y_world[VIO_Z],
                                           y_axis );
        }

        x_mag = array_magnitude( x_axis, VIO_N_DIMENSIONS );
        if( x_mag == 0.0 )
            x_mag = 1.0;

        y_mag = array_magnitude( y_axis, VIO_N_DIMENSIONS );
        if( y_mag == 0.0 )
            y_mag = 1.0;

        view = &slice_window->slice.volumes[volume_index].views[view_index];

        for_less( axis, 0, VIO_N_DIMENSIONS )
        {
            view->x_axis[axis] = x_axis[axis] / x_mag;
            view->y_axis[axis] = y_axis[axis] / y_mag;
        }
    }

    match_view_scale_and_translation( slice_window, view_index,
                                      ref_volume_index );
}

/**
 * Update the axes of all four slice views, using a particular volume
 * as the reference volume. All other volumes will be aligned to the
 * reference volume.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param volume_index The zero-based index of the reference volume.
 */
void  update_all_slice_axes_views(
    display_struct    *slice_window,
    int               volume_index )
{
    int   view_index;

    for_less( view_index, 0, N_SLICE_VIEWS )
        update_all_slice_axes( slice_window, volume_index, view_index );
}

/**
 * Trigger redisplay of a particular slice view.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param view_index The view number (0-3) to update.
 */
static void  slice_view_has_changed(
    display_struct   *slice_window,
    int              view_index )
{
    assert( slice_window->window_type == SLICE_WINDOW );

    set_slice_cursor_update( slice_window, view_index );
    set_slice_cross_section_update( slice_window, view_index );
    set_crop_box_update( slice_window, view_index );
    set_probe_update( slice_window );
    set_atlas_update( slice_window, view_index );
    set_slice_outline_update( slice_window, view_index );
    set_slice_window_update( slice_window, -1, view_index, UPDATE_BOTH );
}

/**
 * Update the voxel-to-world transform of a particular volume.
 *
 * Once the new transform is set, the slice view will be updated
 * accordingly. Both the reported position and the visualized
 * alignment of loaded volumes may change. The first volume is always
 * used as the "reference" volume - all other volumes are aligned to
 * its voxel coordinates.
 *
 * TODO: Allow user to change the reference volume index on the fly?
 *
 * \param display A pointer to a top-level window's display_struct.
 * \param volume_index The zero-based index of the current volume.
 * \param transform The new transform to apply to this volume.
 */
void  set_volume_transform(
    display_struct     *display,
    int                volume_index,
    VIO_General_transform  *transform )
{
    VIO_Volume         volume;
    display_struct     *slice_window;
    VIO_General_transform  copy;
    const int          ref_volume_index = 0;

    if( !get_slice_window( display, &slice_window ) )
        return;

    volume = get_nth_volume( slice_window, volume_index );

    copy_general_transform( transform, &copy );

    set_voxel_to_world_transform( volume, &copy );

    (void) update_voxel_from_cursor( display );

    /*
     * Update all views. We use the first volume as the "reference"
     * volume, all other volumes are aligned to it.
     */
    update_all_slice_axes_views( slice_window, ref_volume_index );
}

/**
 * Append a new transform to the transform of a volume.
 *
 * \param display A pointer to a top-level window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param transform The transform to concatenate to the volume transform.
 */
void  concat_transform_to_volume(
    display_struct     *display,
    int                volume_index,
    VIO_General_transform  *transform )
{
    VIO_Volume             volume;
    VIO_General_transform  *volume_transform, concated;

    volume = get_nth_volume( display, volume_index );

    volume_transform = get_voxel_to_world_transform( volume );

    concat_general_transforms( volume_transform, transform, &concated );

    set_volume_transform( display, volume_index, &concated );

    delete_general_transform( &concated );
}

/**
 * Read a transform from the given file and append it to the transform of
 * the currently-selected volume.
 *
 * \param display A pointer to a top-level window's display_struct.
 * \param filename The path to a transform file.
 */
void  transform_current_volume_from_file(
    display_struct   *display,
    VIO_STR           filename )
{
    VIO_General_transform  file_transform;

    if( input_transform_file( filename, &file_transform ) != VIO_OK )
        return;

    concat_transform_to_volume( display,
                                get_current_volume_index(display),
                                &file_transform );

    delete_general_transform( &file_transform );
}

/**
 * Reset the transform of the currently selected volume. This is used
 * to clear any volume transform operations.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 */
void  reset_current_volume_transform(
    display_struct *slice_window )
{
    int volume_index;

    volume_index = get_current_volume_index( slice_window );

    set_volume_transform( slice_window, volume_index,
                          &(slice_window->slice.volumes[volume_index]
                            .original_transform) );
}

/**
 * \brief Get a text name for the given view index.
 *
 * Used when describing these views to the user.
 *
 * \param slice_window A pointer to the slice view window's display_struct.
 * \param volume_index The volume index.
 * \param view_index The view index we want to describe.
 * \returns A constant string like "sagittal", e.g.
 */
const char *
get_view_name( display_struct *slice_window, int volume_index, int view_index )
{
  int x_index, y_index, axis_index;

  if (slice_has_ortho_axes( slice_window, volume_index, view_index,
                            &x_index, &y_index, &axis_index ))
  {
    switch ( axis_index )
    {
    case VIO_X:
      return "sagittal";
    case VIO_Y:
      return "coronal";
    case VIO_Z:
      return "transverse";
    default:
      return "UNKNOWN";
    }
  }

  return "oblique";
}
