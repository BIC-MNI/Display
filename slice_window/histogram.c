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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/histogram.c,v 1.10 1995-07-31 19:54:27 david Exp $";
#endif

#include  <display.h>

public  void  initialize_slice_histogram(
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

public  void  delete_slice_histogram(
    slice_window_struct   *slice )
{
    delete_lines( &slice->unscaled_histogram_lines );
}

public  void  clear_histogram(
    display_struct   *slice_window )
{
    lines_struct   *lines;

    lines = get_lines_ptr( slice_window->slice.histogram_object );

    delete_lines( lines );
    initialize_lines( lines, Histogram_colour );
    delete_lines( &slice_window->slice.unscaled_histogram_lines );
    initialize_lines( &slice_window->slice.unscaled_histogram_lines, WHITE );
}

private  void  compute_histogram_lines(
    Volume           volume,
    Volume           label_volume,
    BOOLEAN          labeled_only,
    int              axis_index,
    int              voxel_index,
    Real             width_ratio,
    lines_struct     *lines )
{
    int                x, y, z, sizes[MAX_DIMENSIONS];
    int                start[MAX_DIMENSIONS], end[MAX_DIMENSIONS];
    Real               min_value, max_value, value, window_width;
    histogram_struct   histogram;
    progress_struct    progress;

    get_volume_real_range( volume, &min_value, &max_value );
    get_volume_sizes( volume, sizes );

    initialize_histogram( &histogram,
                          (max_value - min_value) / 1000, min_value );

    start[X] = 0;
    end[X] = sizes[X];
    start[Y] = 0;
    end[Y] = sizes[Y];
    start[Z] = 0;
    end[Z] = sizes[Z];

    if( axis_index >= 0 && voxel_index >= 0 && voxel_index < sizes[axis_index] )
    {
        start[axis_index] = voxel_index;
        end[axis_index] = voxel_index+1;
    }

    if( axis_index < 0 )
    {
        initialize_progress_report( &progress, FALSE, sizes[X] * sizes[Y],
                                    "Histogramming" );
    }

    for_less( x, start[X], end[X] )
    {
        for_less( y, start[Y], end[Y] )
        {
            for_less( z, start[Z], end[Z] )
            {
                if( !labeled_only ||
                    get_3D_volume_label_data( label_volume, x, y, z ) != 0 )
                {
                    value = get_volume_real_value( volume, x, y, z, 0, 0 );
                    add_to_histogram( &histogram, value );
                }
            }

            if( axis_index < 0 )
                update_progress_report( &progress, x * sizes[Y] + y + 1 );
        }
    }

    if( axis_index < 0 )
        terminate_progress_report( &progress );

    window_width = width_ratio * (max_value - min_value);

    delete_lines( lines );
    create_histogram_line( &histogram, 200, 200, window_width, lines );

    delete_histogram( &histogram );
}

public  void  resize_histogram(
    display_struct   *slice_window )
{
    int            i, start, x_min, x_max;
    Real           x, y;
    Real           max_y;
    lines_struct   *unscaled_lines, *lines;

    unscaled_lines = &slice_window->slice.unscaled_histogram_lines;
    lines = get_lines_ptr( slice_window->slice.histogram_object );

    if( unscaled_lines->n_points == 0 )
        return;

    start = unscaled_lines->n_points * 0.05;
    max_y = 0.0;
    for_less( i, start, unscaled_lines->n_points )
    {
        if( i == start || Point_y(unscaled_lines->points[i]) > max_y )
            max_y = Point_y(unscaled_lines->points[i]);
    }

    get_histogram_space( slice_window, &x_min, &x_max );

    for_less( i, 0, lines->n_points )
    {
        x = x_min + (x_max - x_min) * Histogram_x_scale *
            Point_y(unscaled_lines->points[i]) / max_y;

        if( x > (Real) x_max )
            x = (Real) x_max;

        y = get_colour_bar_y_pos( slice_window,
                                  Point_x(unscaled_lines->points[i]) );
        fill_Point( lines->points[i], x, y, 0.0 );
    }
}

public  void  compute_histogram(
    display_struct   *slice_window,
    int              axis_index,
    int              voxel_index,
    BOOLEAN          labeled_only )
{
    int            i;
    lines_struct   *unscaled_lines, *lines;
    Volume         volume;

    clear_histogram( slice_window );

    if( !get_slice_window_volume(slice_window,&volume) ||
        is_an_rgb_volume(volume) )
        return;

    compute_histogram_lines( volume,
                             get_label_volume(slice_window), labeled_only,
                             axis_index, voxel_index,
                             Histogram_smoothness_ratio,
                             &slice_window->slice.unscaled_histogram_lines );

    unscaled_lines = &slice_window->slice.unscaled_histogram_lines;
    lines = get_lines_ptr( slice_window->slice.histogram_object );

    for_less( i, 0, unscaled_lines->n_points )
        add_point_to_line( lines, &unscaled_lines->points[i] );

    resize_histogram( slice_window );
}
