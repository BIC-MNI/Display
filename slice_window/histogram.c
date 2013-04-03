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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif

#include  <display.h>

  void  initialize_slice_histogram(
    display_struct   *slice_window )
{
    lines_struct   *lines;
    object_struct  *object;
    model_struct   *model;

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );

    object = create_object( LINES );
    add_object_to_model( model, object );

    lines = get_lines_ptr( object );

    initialize_lines( lines, Histogram_colour );

    slice_window->slice.histogram_object = object;
    initialize_lines( &slice_window->slice.unscaled_histogram_lines, WHITE );
}

  void  delete_slice_histogram(
    slice_window_struct   *slice )
{
    delete_lines( &slice->unscaled_histogram_lines );
}

  void  clear_histogram(
    display_struct   *slice_window )
{
    lines_struct   *lines;

    lines = get_lines_ptr( slice_window->slice.histogram_object );

    delete_lines( lines );
    initialize_lines( lines, Histogram_colour );
    delete_lines( &slice_window->slice.unscaled_histogram_lines );
    initialize_lines( &slice_window->slice.unscaled_histogram_lines, WHITE );
}

static  void  compute_histogram_lines(
    display_struct   *slice_window,
    int              volume_index,
    VIO_BOOL          labeled_only,
    int              axis_index,
    int              voxel_index,
    VIO_Real             width_ratio,
    lines_struct     *lines )
{
    int                x, y, z, sizes[VIO_MAX_DIMENSIONS];
    int                start[VIO_MAX_DIMENSIONS], end[VIO_MAX_DIMENSIONS];
    VIO_Real               min_value, max_value, value, window_width;
    histogram_struct   histogram;
    VIO_progress_struct    progress;
    VIO_Volume             volume;

    volume = get_nth_volume( slice_window, volume_index );

    get_volume_real_range( volume, &min_value, &max_value );
    get_volume_sizes( volume, sizes );

    initialize_histogram( &histogram,
                          (max_value - min_value) / 1000.0, min_value );

    start[VIO_X] = 0;
    end[VIO_X] = sizes[VIO_X];
    start[VIO_Y] = 0;
    end[VIO_Y] = sizes[VIO_Y];
    start[VIO_Z] = 0;
    end[VIO_Z] = sizes[VIO_Z];

    if( axis_index >= 0 && voxel_index >= 0 && voxel_index < sizes[axis_index] )
    {
        start[axis_index] = voxel_index;
        end[axis_index] = voxel_index+1;
    }

    if( axis_index < 0 )
    {
        initialize_progress_report( &progress, FALSE, sizes[VIO_X] * sizes[VIO_Y],
                                    "Histogramming" );
    }

    for_less( x, start[VIO_X], end[VIO_X] )
    {
        for_less( y, start[VIO_Y], end[VIO_Y] )
        {
            for_less( z, start[VIO_Z], end[VIO_Z] )
            {
                if( !labeled_only ||
                    get_voxel_label( slice_window, volume_index, x, y, z ) != 0)
                {
                    value = get_volume_real_value( volume, x, y, z, 0, 0 );
                    add_to_histogram( &histogram, value );
                }
            }

            if( axis_index < 0 )
                update_progress_report( &progress, x * sizes[VIO_Y] + y + 1 );
        }
    }

    if( axis_index < 0 )
        terminate_progress_report( &progress );

    window_width = width_ratio * (max_value - min_value);

    delete_lines( lines );
    create_histogram_line( &histogram, 200, 200, window_width, lines );

    delete_histogram( &histogram );
}

  void  resize_histogram(
    display_struct   *slice_window )
{
    int            i, start, x_min, x_max;
    VIO_Real           x, y;
    VIO_Real           max_y;
    lines_struct   *unscaled_lines, *lines;

    unscaled_lines = &slice_window->slice.unscaled_histogram_lines;
    lines = get_lines_ptr( slice_window->slice.histogram_object );

    if( unscaled_lines->n_points == 0 )
        return;

    start = VIO_ROUND( (VIO_Real) unscaled_lines->n_points * 0.05 );
    max_y = 0.0;
    for_less( i, start, unscaled_lines->n_points )
    {
        if( i == start || (VIO_Real) Point_y(unscaled_lines->points[i]) > max_y )
            max_y = (VIO_Real) Point_y(unscaled_lines->points[i]);
    }

    get_histogram_space( slice_window, &x_min, &x_max );

    for_less( i, 0, lines->n_points )
    {
        x = (VIO_Real) x_min + (VIO_Real) (x_max - x_min) * Histogram_x_scale *
            (VIO_Real) Point_y(unscaled_lines->points[i]) / (VIO_Real) max_y;

        if( x > (VIO_Real) x_max )
            x = (VIO_Real) x_max;

        y = (VIO_Real) get_colour_bar_y_pos( slice_window,
                                  (VIO_Real) Point_x(unscaled_lines->points[i]) );
        fill_Point( lines->points[i], x, y, 0.0 );
    }
}

  void  compute_histogram(
    display_struct   *slice_window,
    int              axis_index,
    int              voxel_index,
    VIO_BOOL          labeled_only )
{
    int            i;
    lines_struct   *unscaled_lines, *lines;
    VIO_Volume         volume;

    clear_histogram( slice_window );

    if( !get_slice_window_volume(slice_window,&volume) ||
        is_an_rgb_volume(volume) )
        return;

    compute_histogram_lines( slice_window,
                             get_current_volume_index(slice_window),
                             labeled_only, axis_index, voxel_index,
                             Histogram_smoothness_ratio,
                             &slice_window->slice.unscaled_histogram_lines );

    unscaled_lines = &slice_window->slice.unscaled_histogram_lines;
    lines = get_lines_ptr( slice_window->slice.histogram_object );

    for_less( i, 0, unscaled_lines->n_points )
        add_point_to_line( lines, &unscaled_lines->points[i] );

    resize_histogram( slice_window );
}
