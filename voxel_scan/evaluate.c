
#include  <def_objects.h>
#include  <def_minimization.h>
#include  <def_surface_fitting.h>

private  const  double  BIG_NUMBER = 1.0e30;

public  double   evaluate_fit_in_volume( volume, fit_data, parameters )
    volume_struct           *volume;
    surface_fitting_struct  *fit_data;
    double                  parameters[];
{
    int                     i, j, ni, nj, n_samples, n_fitting_samples;
    double                  fit, measure_of_fit, sum;
    double                  u, v;
    double                  distance_measure();
    double                  evaluate_fit_at_uv();

    n_fitting_samples = fit_data->n_samples;

    if( !fit_data->surface_representation->
              are_parameters_valid(fit_data->descriptors, parameters) )
    {
        measure_of_fit = BIG_NUMBER;
    }
    else
    {
        sum = 0.0;
        n_samples = 0;

        for_less( i, 0, fit_data->n_surface_points )
            fit_data->surface_point_distances[i] = -1.0;

        ni = (int) sqrt((double) n_fitting_samples) + 1;
        nj = ni;
        for_less( i, 0, ni )
        {
            u = (double) i / (double) ni;
            for_less( j, 0, nj )
            {
                v = (double) j / (double) nj;

                fit = evaluate_fit_at_uv( volume, fit_data,
                                          parameters, u, v );
                sum += fit;
                ++n_samples;
            }
        }

        measure_of_fit = sum / (double) n_samples +
                         fit_data->surface_point_distance_factor *
                         distance_measure( fit_data->n_surface_points,
                                 fit_data->surface_point_distances,
                                 fit_data->surface_point_distance_threshold );
    }

    return( measure_of_fit );
}

private  double   evaluate_fit_at_uv( volume, fit_data, parameters, u, v )
    volume_struct           *volume;
    surface_fitting_struct  *fit_data;
    double                  parameters[];
    double                  u, v;
{
    double   surface_estimate, curvature, fit, u_curvature, v_curvature;
    double   x, y, z;
    Real     dx, dy, dz;
    double   dxuu, dyuu, dzuu, dxvv, dyvv, dzvv;
    double   dxu, dyu, dzu, dxv, dyv, dzv;
    double   dist;
    double   get_radius_of_curvature();
    int      i;
    Vector   surface_normal, function_deriv;
    Real     evaluate_volume_at_point();
    void     get_surface_normal_from_derivs();

    fit_data->surface_representation->evaluate_surface_at_uv( u, v,
                                 fit_data->descriptors, parameters, &x, &y, &z,
                                 &dxu, &dyu, &dzu, &dxv, &dyv, &dzv,
                                 &dxuu, &dyuu, &dzuu, &dxvv, &dyvv, &dzvv );

    fit = 0.0;

    if( fit_data->gradient_strength_factor > 0.0 )
    {
        if( volume != (volume_struct *) 0 &&
            point_is_within_volume( volume, x, y, z ) )
        {
            (void) evaluate_volume_at_point( volume, x, y, z, &dx, &dy, &dz );

            get_surface_normal_from_derivs( dxu, dyu, dzu, dxv, dyv, dzv,
                                            &surface_normal );

            fill_Vector( function_deriv, dx, dy, dz );

            surface_estimate = -DOT_VECTORS( function_deriv, surface_normal );

            if( surface_estimate > 0.0 )  surface_estimate = -surface_estimate;
        }
        else
        {
            surface_estimate = 0.0;
        }

        fit += surface_estimate * fit_data->gradient_strength_factor;
    }

    if( fit_data->curvature_factor > 0.0 )
    {
        u_curvature = get_radius_of_curvature( dxu, dyu, dzu, dxuu, dyuu, dzuu);
        v_curvature = get_radius_of_curvature( dxv, dyv, dzv, dxvv, dyvv, dzvv);

        curvature = MIN( u_curvature, v_curvature );

        fit += -fit_data->curvature_factor * curvature;
    }

    for_less( i, 0, fit_data->n_surface_points )
    {
        dx = x - Point_x(fit_data->surface_points[i]);
        dy = y - Point_y(fit_data->surface_points[i]);
        dz = z - Point_z(fit_data->surface_points[i]);
        dist = dx * dx + dy * dy + dz * dz;
        if( fit_data->surface_point_distances[i] < 0.0 ||
            dist < fit_data->surface_point_distances[i] )
        {
            fit_data->surface_point_distances[i] = dist;
        }
    }

    return( fit );
}

private  double  distance_measure( n_surface_points,
                                   surface_point_distances, distance_threshold )
    int     n_surface_points;
    Real    surface_point_distances[];
    Real    distance_threshold;
{
    int     i;
    Real    sum, dist;

    sum = 0.0;

    for_less( i, 0, n_surface_points )
    {
        dist = sqrt( surface_point_distances[i] );

        if( dist > distance_threshold )
            sum += dist;
    }

    if( n_surface_points > 0 )
        sum /= (double) n_surface_points;

    return( sum );
}

private  double  get_radius_of_curvature( dx, dy, dz, ddx, ddy, ddz )
    double   dx;
    double   dy;
    double   dz;
    double   ddx;
    double   ddy;
    double   ddz;
{
    double  radius_of_curvature;
    double  x_cross, y_cross, z_cross, mag_cross, mag_deriv;
    void    cross_product();

    cross_product( dx, dy, dz, ddx, ddy, ddz, &x_cross, &y_cross, &z_cross );

    mag_cross = sqrt( x_cross * x_cross + y_cross * y_cross + z_cross*z_cross );

    mag_deriv = sqrt( dx * dx + dy * dy + dz * dz );

    radius_of_curvature = mag_deriv * mag_deriv * mag_deriv;

    if( mag_cross > 0.0 )
        radius_of_curvature /= mag_cross;

    return( radius_of_curvature );
}

private  void   cross_product( x1, y1, z1, x2, y2, z2,
                               x_cross, y_cross, z_cross )
    double  x1;
    double  y1;
    double  z1;
    double  x2;
    double  y2;
    double  z2;
    double  *x_cross;
    double  *y_cross;
    double  *z_cross;
{
    *x_cross = y1 * z2 - y2 * z1;
    *y_cross = z1 * x2 - z2 * x1;
    *z_cross = x1 * y2 - x2 * y1;
}
