
#include  <def_globals.h>
#include  <def_graphics.h>

#define  FACTOR  1.2

public  void  fit_view_to_domain( view, min_limit, max_limit )
    view_struct   *view;
    Point         *min_limit;
    Point         *max_limit;
{
    Point     points[8];
    void      fit_view_to_points();
    Real      x_min, y_min, z_min;
    Real      x_max, y_max, z_max;

    x_min = Point_x( *min_limit );
    y_min = Point_y( *min_limit );
    z_min = Point_z( *min_limit );

    x_max = Point_x( *max_limit );
    y_max = Point_y( *max_limit );
    z_max = Point_z( *max_limit );

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

private  void   fit_view_to_points( view, n_points, points )
    view_struct   *view;
    int           n_points;
    Point         points[];
{
    int    i, c;
    Real   size, centre_z;
    Point  min_coord, max_coord, centre, range;
    void   perspective_fit_points();
    void   orthogonal_fit_points();
    void   transform_point_to_screen();

    for_less( i, 0, n_points )
    {
        transform_point_to_screen( view, &points[i], &points[i] );
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
        if( Point_coord(range,c) == 0.0 )
        {
            Point_coord(min_coord,c) -= size / 2.0;
            Point_coord(max_coord,c) += size / 2.0;
            Point_coord(range,c) = size;
        }
    }

    centre_z = (Point_z(min_coord) + Point_z(max_coord)) / 2.0;
    Point_z(min_coord) = centre_z - size / 2.0;
    Point_z(max_coord) = centre_z + size / 2.0;
    Point_z(range) = size;

    INTERPOLATE_POINTS( centre, min_coord, max_coord, 0.5 );

    if( view->perspective_flag )
    {
        perspective_fit_points( view, &max_coord, &centre, n_points, points );
    }
    else
    {
        orthogonal_fit_points( view, &centre, &range );
    }

    view->desired_aspect = view->window_height / view->window_width;
}

private  void  orthogonal_fit_points( view, centre, range )
    view_struct   *view;
    Point         *centre;
    Vector        *range;
{
    Real    dx, dy, dz;
    Point   eye;
    Vector  x_axis, y_axis, line_of_sight;
    Vector  delta_x, delta_y, delta_z;
    Real    x_scale, y_scale, scale;

    eye = view->origin;
    x_axis = view->x_axis;
    y_axis = view->y_axis;
    line_of_sight = view->line_of_sight;

    dx = Point_x(*centre);
    dy = Point_y(*centre);
    dz = Point_z(*centre) - Point_z(*range);

    SCALE_VECTOR( delta_x, x_axis, dx );
    SCALE_VECTOR( delta_y, y_axis, dy );
    SCALE_VECTOR( delta_z, line_of_sight, dz );

    ADD_POINT_VECTOR( eye, eye, delta_x );
    ADD_POINT_VECTOR( eye, eye, delta_y );
    ADD_POINT_VECTOR( eye, eye, delta_z );

    view->origin = eye;

    x_scale = Point_x(*range) * FACTOR / view->window_width;
    y_scale = Point_y(*range) * FACTOR / view->window_height;

    if( x_scale == 0.0 )
    {
        view->desired_aspect = 1.0;
    }
    else
    {
        view->desired_aspect = y_scale / x_scale;
    }

    scale = MAX( x_scale, y_scale );

    view->window_width *= scale;
    view->window_height *= scale;
    view->perspective_distance = Point_z(*range);

    view->front_distance = 0.0;
    view->back_distance = 2.5 * Vector_z(*range);
}

private  void  perspective_fit_points( view, max_coord, centre,
                                       n_points, points )
    view_struct   *view;
    Point         *max_coord;
    Point         *centre;
    int           n_points;
    Point         points[];
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
        for_inclusive( c, X_AXIS, Y_AXIS )
        {
            if( c == X_AXIS )
            {
                off_centre = Point_x(points[i]) - Point_x(*centre);
                width = view->window_width;
            }
            else
            {
                off_centre = Point_y(points[i]) - Point_y(*centre);
                width = view->window_height;
            }

            dist = FACTOR * off_centre * view->perspective_distance /
                                         (width/2.0);

            if( dist < 0.0 )
            {
                dist = -dist;
            }

            z_pos = Point_z(points[i]) - dist;

            if( (i == 0 && c == X_AXIS) || z_pos < z_min )
            {
                z_min = z_pos;
            }
        }
    }

    dx = Point_x(*centre);
    dy = Point_y(*centre);
    dz = z_min;

    SCALE_VECTOR( delta_x, x_axis, dx );
    SCALE_VECTOR( delta_y, y_axis, dy );
    SCALE_VECTOR( delta_z, line_of_sight, dz );

    ADD_POINT_VECTOR( eye, eye, delta_x );
    ADD_POINT_VECTOR( eye, eye, delta_y );
    ADD_POINT_VECTOR( eye, eye, delta_z );

    view->origin = eye;

    new_persp_dist = Point_z(*centre) - z_min;

    ratio = new_persp_dist / view->perspective_distance;

    view->window_width *= ratio;
    view->window_height *= ratio;
    view->perspective_distance = new_persp_dist;

    view->back_distance = FACTOR * (Point_z(*max_coord) - dz);
    view->front_distance = 0.0;
}
