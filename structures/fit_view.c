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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/structures/fit_view.c,v 1.19 1996-04-19 13:25:36 david Exp $";
#endif


#include  <display.h>

#define  FACTOR  1.05

private  void   fit_view_to_points(
    view_struct   *view,
    int           n_points,
    Point         points[] );
private  void  orthogonal_fit_points(
    view_struct   *view,
    Point         *centre,
    Vector        *range );
private  void  perspective_fit_points(
    view_struct   *view,
    Point         *centre,
    int           n_points,
    Point         points[] );

public  void  fit_view_to_domain(
    view_struct   *view,
    Point         *min_limit,
    Point         *max_limit )
{
    Point     points[8];
    Real      x_min, y_min, z_min;
    Real      x_max, y_max, z_max;

    x_min = (Real) Point_x( *min_limit );
    y_min = (Real) Point_y( *min_limit );
    z_min = (Real) Point_z( *min_limit );

    x_max = (Real) Point_x( *max_limit );
    y_max = (Real) Point_y( *max_limit );
    z_max = (Real) Point_z( *max_limit );

    fill_Point( points[0], x_min, y_min, z_min );
    fill_Point( points[1], x_min, y_min, z_max );
    fill_Point( points[2], x_min, y_max, z_min );
    fill_Point( points[3], x_min, y_max, z_max );
    fill_Point( points[4], x_max, y_min, z_min );
    fill_Point( points[5], x_max, y_min, z_max );
    fill_Point( points[6], x_max, y_max, z_min );
    fill_Point( points[7], x_max, y_max, z_max );

    fit_view_to_points( view, 8, points );
}

private  void   fit_view_to_points(
    view_struct   *view,
    int           n_points,
    Point         points[] )
{
    int    i, c;
    Real   size, centre_z;
    Point  min_coord, max_coord, centre;
    Vector range;

    for_less( i, 0, n_points )
    {
        transform_point_to_view_space( view, &points[i], &points[i] );
    }

    min_coord = points[0];
    max_coord = points[0];

    for_less( i, 1, n_points )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            if( Point_coord(min_coord,c) > Point_coord( points[i], c ) )
            {
                Point_coord(min_coord,c) = Point_coord( points[i], c );
            }
            if( Point_coord(max_coord,c) < Point_coord( points[i], c ) )
            {
                Point_coord(max_coord,c) = Point_coord( points[i], c );
            }
        }
    }

    SUB_POINTS( range, max_coord, min_coord );

    size = MAGNITUDE( range );

    if( size == 0.0 )  size = 1.0;

    for_less( c, 0, 2 )
    {
        if( Vector_coord(range,c) == 0.0f )
        {
            Point_coord(min_coord,c) -= (Point_coord_type) (size / 2.0);
            Point_coord(max_coord,c) += (Point_coord_type) (size / 2.0);
            Vector_coord(range,c) = (Point_coord_type) (size);
        }
    }

    centre_z = ((Real) Point_z(min_coord) + (Real) Point_z(max_coord)) / 2.0;
    Point_z(min_coord) = (Point_coord_type) (centre_z - size / 2.0);
    Point_z(max_coord) = (Point_coord_type) (centre_z + size / 2.0);
    Vector_z(range) = (Point_coord_type) size;

    INTERPOLATE_POINTS( centre, min_coord, max_coord, 0.5 );

    if( view->perspective_flag )
    {
        perspective_fit_points( view, &centre, n_points, points );
    }
    else
    {
        orthogonal_fit_points( view, &centre, &range );
    }

    view->desired_aspect = view->window_height / view->window_width;
}

private  void  orthogonal_fit_points(
    view_struct   *view,
    Point         *centre,
    Vector        *range )
{
    Real    dx, dy, dz;
    Point   eye;
    Vector  x_axis, y_axis, line_of_sight;
    Vector  delta_x, delta_y, delta_z;
    Real    x_scale, y_scale, scale_factor;

    eye = view->origin;
    x_axis = view->x_axis;
    y_axis = view->y_axis;
    line_of_sight = view->line_of_sight;

    dx = (Real) Point_x(*centre);
    dy = (Real) Point_y(*centre);
    dz = (Real) Point_z(*centre) - (Real) Vector_z(*range);

    SCALE_VECTOR( delta_x, x_axis, dx );
    SCALE_VECTOR( delta_y, y_axis, dy );
    SCALE_VECTOR( delta_z, line_of_sight, dz );

    ADD_POINT_VECTOR( eye, eye, delta_x );
    ADD_POINT_VECTOR( eye, eye, delta_y );
    ADD_POINT_VECTOR( eye, eye, delta_z );

    view->origin = eye;

    x_scale = (Real) Vector_x(*range) * FACTOR / view->window_width;
    y_scale = (Real) Vector_y(*range) * FACTOR / view->window_height;

    if( x_scale == 0.0 )
    {
        view->desired_aspect = 1.0;
    }
    else
    {
        view->desired_aspect = y_scale / x_scale;
    }

    scale_factor = MAX( x_scale, y_scale );

    view->window_width *= scale_factor;
    view->window_height *= scale_factor;
    view->perspective_distance = (Real) Vector_z(*range);

    view->front_distance = 0.0;
    view->back_distance = 2.0 * ((Real) Point_z(*centre) - dz);
}

private  void  perspective_fit_points(
    view_struct   *view,
    Point         *centre,
    int           n_points,
    Point         points[] )
{
    int     i, c;
    Real    z_min, z_pos, dist, ratio, new_persp_dist;
    Real    dx, dy, dz, off_centre, width;
    Point   eye;
    Vector  x_axis, y_axis, line_of_sight;
    Vector  delta_x, delta_y, delta_z;

    z_min = 0.0;

    eye = view->origin;
    x_axis = view->x_axis;
    y_axis = view->y_axis;
    line_of_sight = view->line_of_sight;

    for_less( i, 0, n_points )
    {
        for_inclusive( c, X, Y )
        {
            if( c == X )
            {
                off_centre = (Real) Point_x(points[i]) - (Real)Point_x(*centre);
                width = view->window_width;
            }
            else
            {
                off_centre = (Real) Point_y(points[i]) - (Real)Point_y(*centre);
                width = view->window_height;
            }

            dist = FACTOR * off_centre * view->perspective_distance /
                                         (width/2.0);

            if( dist < 0.0 )
                dist = -dist;

            z_pos = (Real) Point_z(points[i]) - dist;

            if( (i == 0 && c == X) || z_pos < z_min )
                z_min = z_pos;
        }
    }

    dx = (Real) Point_x(*centre);
    dy = (Real) Point_y(*centre);
    dz = z_min;

    SCALE_VECTOR( delta_x, x_axis, dx );
    SCALE_VECTOR( delta_y, y_axis, dy );
    SCALE_VECTOR( delta_z, line_of_sight, dz );

    ADD_POINT_VECTOR( eye, eye, delta_x );
    ADD_POINT_VECTOR( eye, eye, delta_y );
    ADD_POINT_VECTOR( eye, eye, delta_z );

    view->origin = eye;

    new_persp_dist = (Real) Point_z(*centre) - z_min;

    ratio = new_persp_dist / view->perspective_distance;

    view->window_width *= ratio;
    view->window_height *= ratio;
    view->perspective_distance = new_persp_dist;

    view->back_distance = 2.0 * ((Real) Point_z(*centre) - dz);
    view->front_distance = 0.0;
}
