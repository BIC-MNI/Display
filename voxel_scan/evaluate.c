
#include  <def_graphics.h>
#include  <def_minimization.h>

const  double  BIG_NUMBER = 1.0e30;

public  double   evaluate_fit( evaluation_ptr, parameters )
    void            *evaluation_ptr;
    double          parameters[];
{
    int                     i, j, ni, nj, n_samples, n_fitting_samples;
    double                  fit, measure_of_fit, sum;
    double                  u, v;
    double                  distance_measure();
    double                  evaluate_fit_at_uv();
    graphics_struct         *graphics;
    surface_fitting_struct  *fit_data;
    Real                    get_random_0_to_1();

    graphics = (graphics_struct *) evaluation_ptr;

    fit_data = &graphics->three_d.surface_fitting;

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

                fit = evaluate_fit_at_uv( graphics, fit_data,
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

private  double   evaluate_fit_at_uv( graphics, fit_data, parameters, u, v )
    graphics_struct         *graphics;
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
    const    double  PI_SQUARED = PI * PI;
    const    double  FOUR_PI_SQUARED = 4.0 * PI_SQUARED;
    int      i;
    volume_struct  *volume;
    Vector   surface_normal, function_deriv;
    Real     evaluate_volume_at_point();
    void     get_surface_normal_from_derivs();

    fit_data->surface_representation->evaluate_surface_at_uv( u, v,
                                 fit_data->descriptors,
                                 parameters, &x, &y, &z,
                                 &dxu, &dyu, &dzu, &dxv, &dyv, &dzv,
                                 &dxuu, &dyuu, &dzuu, &dxvv, &dyvv, &dzvv );

    if( get_slice_window_volume( graphics, &volume ) &&
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
        surface_estimate = BIG_NUMBER;
        surface_estimate = 0.0;
    }

    if( fit_data->curvature_factor > 0.0 )
    {
        u_curvature = PI_SQUARED / sqrt( dxuu*dxuu + dyuu*dyuu + dzuu*dzuu );
        v_curvature = FOUR_PI_SQUARED /
                      sqrt( dxvv*dxvv + dyvv*dyvv + dzvv*dzvv );

        curvature = sqrt( u_curvature * v_curvature );

        fit = surface_estimate + fit_data->curvature_factor * curvature;
    }
    else
        fit = surface_estimate;

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
