
#include  <def_graphics.h>

#define  FACTOR  1.2

public  void  fit_view_to_domain( view, x_min, y_min, z_min,
                                  x_max, y_max, z_max )
    view_struct   *view;
    Real          x_min, y_min, z_min;
    Real          x_max, y_max, z_max;
{
    Point     points[8];
    void      fit_view_to_points();

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
    void   perspective_fit_points();
    void   orthogonal_fit_points();

    if( view->perspective_flag )
    {
    }
    else
    {
        orthogonal_fit_points( view, n_points, points );
    }
}

private  void  orthogonal_fit_points( view, n_points, points )
    view_struct   *view;
    int           n_points;
    Point         points[];
{
    int     i, c;
    Real    min_coord[N_DIMENSIONS], max_coord[N_DIMENSIONS];
    Real    range[N_DIMENSIONS];
    Real    centre_x, centre_y, centre_z;
    Real    dx, dy, dz, size;
    Point   eye;
    Vector  x_axis, y_axis, line_of_sight;
    Vector  delta_x, delta_y, delta_z;
    Real    x_scale, y_scale, scale;
    void    transform_point_to_screen();

    for_less( i, 0, n_points )
    {
        transform_point_to_screen( view, &points[i], &points[i] );
    }

    min_coord[0] = Point_x(points[0]);
    min_coord[1] = Point_y(points[0]);
    min_coord[2] = Point_z(points[0]);

    max_coord[0] = Point_x(points[0]);
    max_coord[1] = Point_y(points[0]);
    max_coord[2] = Point_z(points[0]);

    for_less( i, 1, n_points )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            if( min_coord[c] > Point_coord( points[i], c ) )
            {
                min_coord[c] = Point_coord( points[i], c );
            }
            if( max_coord[c] < Point_coord( points[i], c ) )
            {
                max_coord[c] = Point_coord( points[i], c );
            }
        }
    }

    range[0] = max_coord[0] - min_coord[0];
    range[1] = max_coord[1] - min_coord[1];
    range[2] = max_coord[2] - min_coord[2];

    size = sqrt( range[0]*range[0] + range[1]*range[1] + range[2]*range[2] );

    if( size == 0.0 )  size = 1.0;

    for_less( c, 0, N_DIMENSIONS )
    {
        if( range[c] == 0.0 )
        {
            min_coord[c] -= size / 2.0;
            max_coord[c] += size / 2.0;
            range[c] = size;
        }
    }

    centre_x = (min_coord[0] + max_coord[0]) / 2.0;
    centre_y = (min_coord[1] + max_coord[1]) / 2.0;
    centre_z = (min_coord[2] + max_coord[2]) / 2.0;

    eye = view->origin;
    x_axis = view->x_axis;
    y_axis = view->y_axis;
    line_of_sight = view->line_of_sight;

    dx = centre_x - Point_x(eye);
    dy = centre_y - Point_y(eye);
    dz = centre_z - range[2] - Point_z(eye);

    SCALE_VECTOR( delta_x, x_axis, dx );
    SCALE_VECTOR( delta_y, y_axis, dy );
    SCALE_VECTOR( delta_z, line_of_sight, dz );

    ADD_POINT_VECTOR( eye, eye, delta_x );
    ADD_POINT_VECTOR( eye, eye, delta_y );
    ADD_POINT_VECTOR( eye, eye, delta_z );

    view->origin = eye;

    x_scale = range[0] * FACTOR / view->window_width;
    y_scale = range[1] * FACTOR / view->window_height;

    scale = MAX( x_scale, y_scale );

    view->window_width *= scale;
    view->window_height *= scale;
    view->perspective_distance *= scale;

    view->front_distance = 0.0;
    view->back_distance = 2.5 * range[2];
}
