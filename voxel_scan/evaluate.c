
#include  <def_objects.h>
#include  <def_minimization.h>
#include  <def_surface_fitting.h>

#define  DARTS

private  const  double  BIG_NUMBER = 1.0e30;

public  double   evaluate_fit_in_volume( volume, fit_data, parameters )
    volume_struct           *volume;
    surface_fitting_struct  *fit_data;
    double                  parameters[];
{
    int       i;
    double    measure_of_fit;
    double    evaluate_fit_in_volume_with_distances();

    for_less( i, 0, fit_data->n_surface_points )
        fit_data->surface_point_distances[i] = -1.0;

    measure_of_fit = evaluate_fit_in_volume_with_distances( volume, fit_data,
                             parameters, 0.0, 1.0, 0.0, 1.0,
                             fit_data->surface_point_distances );

    return( measure_of_fit );
}

public  double   evaluate_fit_in_volume_with_distances( volume, fit_data,
                          parameters, u_min, u_max, v_min, v_max,
                          surface_point_distances )
    volume_struct           *volume;
    surface_fitting_struct  *fit_data;
    double                  parameters[];
    double                  u_min, u_max;
    double                  v_min, v_max;
    Real                    surface_point_distances[];
{
    int                     i, j, ni, nj, n_samples, n_fitting_samples;
    double                  used_u_min, used_u_max, used_v_min, used_v_max;
    double                  fit, measure_of_fit, sum;
    double                  u, v;
    double                  distance_measure();
    double                  evaluate_fit_at_uv();
    double                  get_parameter_in_range();

#ifdef  DARTS
    used_u_min = 0.0;
    used_u_max = 1.0;
    used_v_min = 0.0;
    used_v_max = 1.0;
#else
    used_u_min = u_min;
    used_u_max = u_max;
    used_v_min = v_min;
    used_v_max = v_max;
#endif

    n_fitting_samples = get_n_samples( fit_data->n_samples,
                                       used_u_min, used_u_max,
                                       used_v_min, used_v_max );

    if( !fit_data->surface_representation->
              are_parameters_valid(fit_data->descriptors, parameters) )
    {
        measure_of_fit = BIG_NUMBER;
    }
    else
    {
        for_less( i, 0, fit_data->n_surface_points )
        {
            if( surface_point_distances != (Real *) 0 )
            {
                fit_data->surface_point_distances[i] =
                                        surface_point_distances[i];
            }
            else
            {
                fit_data->surface_point_distances[i] = -1.0;
            }
        }

        sum = 0.0;
        n_samples = 0;

        ni = (int) sqrt((double) n_fitting_samples) + 1;
        nj = ni;
        for_less( i, 0, ni )
        {
            u = get_parameter_in_range( i, ni, used_u_min, used_u_max);
            for_less( j, 0, nj )
            {
                v = get_parameter_in_range( j, nj, used_v_min, used_v_max);

#ifdef DARTS
                if( inside_hole( u, v, TRUE, u_min, u_max, v_min, v_max ) )
                {
#endif
                fit = evaluate_fit_at_uv( volume, fit_data, parameters, u, v );
                sum += fit;
                ++n_samples;
#ifdef DARTS
                }
#endif
            }
        }

        measure_of_fit = sum / (double) fit_data->n_samples +
                         fit_data->surface_point_distance_factor *
                         distance_measure( fit_data->n_surface_points,
                               fit_data->surface_point_distances,
                               fit_data->surface_point_distance_threshold );
    }

    return( measure_of_fit );
}

public  void   evaluate_distances_to_surface( fit_data,
                          parameters, u_min, u_max, v_min, v_max,
                          surface_point_distances,
                          hole_present, u_min_hole, u_max_hole,
                          v_min_hole, v_max_hole )
    surface_fitting_struct  *fit_data;
    double                  parameters[];
    double                  u_min, u_max;
    double                  v_min, v_max;
    Real                    surface_point_distances[];
    Boolean                 hole_present;
    double                  u_min_hole, u_max_hole;
    double                  v_min_hole, v_max_hole;
{
    int      i, j, ni, nj, n_fitting_samples;
    double   u, v, x, y, z;
    void     apply_surface_point_to_distances();
    double   get_parameter_in_range();

    n_fitting_samples = get_n_samples( fit_data->n_samples, u_min, u_max,
                                       v_min, v_max );

    if( fit_data->surface_representation->
              are_parameters_valid(fit_data->descriptors, parameters) )
    {
        for_less( i, 0, fit_data->n_surface_points )
            surface_point_distances[i] = -1.0;

        ni = (int) sqrt((double) n_fitting_samples) + 1;
        nj = ni;

        for_less( i, 0, ni )
        {
            u = get_parameter_in_range( i, ni, u_min, u_max);

            for_less( j, 0, nj )
            {
                v = get_parameter_in_range( j, nj, v_min, v_max);

                if( !inside_hole( u, v, hole_present, u_min_hole, u_max_hole,
                                  v_min_hole, v_max_hole ) )
                {
                    fit_data->surface_representation->evaluate_surface_at_uv(
                                 u, v,
                                 fit_data->descriptors, parameters, &x, &y, &z,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0 );

                    apply_surface_point_to_distances( x, y, z, fit_data,
                                                  surface_point_distances );
                }
            }
        }
    }
}

private  int  get_n_samples( n_samples_for_whole_surface, u_min, u_max,
                             v_min, v_max )
    int     n_samples_for_whole_surface;
    double  u_min;
    double  u_max;
    double  v_min;
    double  v_max;
{
    int     n_samples;
    double  du, dv;

    if( u_min <= u_max )
        du = u_max - u_min;
    else
        du = 1.0 - u_min + u_max;

    if( v_min <= v_max )
        dv = v_max - v_min;
    else
        dv = 1.0 - v_min + v_max;

    n_samples = ROUND( (double) n_samples_for_whole_surface * du * dv );

    if( n_samples < 10 )
        n_samples = 10;

    return( n_samples );
}

private  double  get_parameter_in_range( i, n, min, max )
    int      i;
    int      n;
    double   min;
    double   max;
{
    double  alpha;

    alpha = ((double) i + 0.5) / (double) n;

    if( min <= max )
        return( min + alpha * (max - min) );
    else if( alpha <= (1.0 - min) / (1.0 - min + max) )
        return( min + alpha * (1.0 + max - min) );
    else
        return( min - 1.0 + alpha * (max - min + 1.0) );
}

private  Boolean  inside_hole( u, v, hole_present, u_min_hole, u_max_hole,
                               v_min_hole, v_max_hole )
    double      u, v;
    Boolean     hole_present;
    double      u_min_hole, u_max_hole;
    double      v_min_hole, v_max_hole;
{
    Boolean  in_hole;

    in_hole = hole_present;

    if( hole_present )
    {
        if( u_min_hole <= u_max_hole )
            in_hole = (u >= u_min_hole && u <= u_max_hole);
        else
            in_hole = (u >= u_min_hole || u <= u_max_hole);


        if( v_min_hole <= v_max_hole )
            in_hole = in_hole && (v >= v_min_hole && v <= v_max_hole);
        else
            in_hole = in_hole && (v >= v_min_hole || v <= v_max_hole);
    }

    return( in_hole );
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
    double   get_radius_of_curvature();
    Vector   surface_normal, function_deriv;
    Real     evaluate_volume_at_point();
    void     get_surface_normal_from_derivs();
    void     apply_surface_point_to_distances();

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

    apply_surface_point_to_distances( x, y, z, fit_data,
                                      fit_data->surface_point_distances );

    return( fit );
}

private  void  apply_surface_point_to_distances( x, y, z, fit_data,
                                                 surface_point_distances )
    double                  x, y, z;
    surface_fitting_struct  *fit_data;
    Real                    surface_point_distances[];
{
    int     i;
    double  dx, dy, dz, dist;

    if( fit_data->surface_point_distance_factor > 0.0 )
    {
        for_less( i, 0, fit_data->n_surface_points )
        {
            dx = x - Point_x(fit_data->surface_points[i]);
            dy = y - Point_y(fit_data->surface_points[i]);
            dz = z - Point_z(fit_data->surface_points[i]);
            dist = dx * dx + dy * dy + dz * dz;
            if( surface_point_distances[i] < 0.0 ||
                dist < surface_point_distances[i] )
            {
                surface_point_distances[i] = dist;
            }
        }
    }
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
        if( surface_point_distances[i] > 0.0 )
        {
            dist = sqrt( surface_point_distances[i] );

            if( dist > distance_threshold )
                sum += dist;
        }
    }

#ifdef AVERAGE_NOT_SUM
    if( n_surface_points > 0 )
        sum /= (double) n_surface_points;
#endif

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
