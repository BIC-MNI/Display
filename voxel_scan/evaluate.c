
#include  <minimization.h>
#include  <display.h>

private  int  get_n_samples(
    int     n_samples_for_whole_surface,
    double  u_min,
    double  u_max,
    double  v_min,
    double  v_max );
private  double  get_parameter_in_range(
    int      i,
    int      n,
    double   min,
    double   max );
private  BOOLEAN  inside_hole(
    double      u,
    double      v,
    BOOLEAN     hole_present,
    double      u_min_hole,
    double      u_max_hole,
    double      v_min_hole,
    double      v_max_hole );
private  double   evaluate_fit_at_uv(
    Volume                  volume,
    surface_fitting_struct  *fit_data,
    double                  parameters[],
    double                  u,
    double                  v );
private  void  apply_surface_point_to_distances(
    double                  x,
    double                  y,
    double                  z,
    surface_fitting_struct  *fit_data,
    Real                    surface_point_distances[] );
private  double  distance_measure(
    int     n_surface_points,
    Real    surface_point_distances[],
    Real    distance_threshold );
private  double  get_radius_of_curvature(
    double   dx,
    double   dy,
    double   dz,
    double   ddx,
    double   ddy,
    double   ddz );
private  void   cross_product(
    double  x1,
    double  y1,
    double  z1,
    double  x2,
    double  y2,
    double  z2,
    double  *x_cross,
    double  *y_cross,
    double  *z_cross );

#define  DARTS

private  const  double  BIG_NUMBER = 1.0e30;

public  double   evaluate_fit_in_volume(
    Volume                  volume,
    surface_fitting_struct  *fit_data,
    double                  parameters[] )
{
    int       i;
    double    measure_of_fit;

    for_less( i, 0, fit_data->n_surface_points )
        fit_data->surface_point_distances[i] = -1.0;

    measure_of_fit = evaluate_fit_in_volume_with_distances( volume, fit_data,
                             parameters, 0.0, 1.0, 0.0, 1.0,
                             fit_data->surface_point_distances );

    return( measure_of_fit );
}

public  double   evaluate_fit_in_volume_with_distances(
    Volume                  volume,
    surface_fitting_struct  *fit_data,
    double                  parameters[],
    double                  u_min,
    double                  u_max,
    double                  v_min,
    double                  v_max,
    Real                    surface_point_distances[] )
{
    int                     i, j, ni, nj, n_samples, n_fitting_samples;
    double                  used_u_min, used_u_max, used_v_min, used_v_max;
    double                  fit, measure_of_fit, sum;
    double                  u, v;

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

public  void   evaluate_distances_to_surface(
    surface_fitting_struct  *fit_data,
    double                  parameters[],
    double                  u_min,
    double                  u_max,
    double                  v_min,
    double                  v_max,
    Real                    surface_point_distances[],
    BOOLEAN                 hole_present,
    double                  u_min_hole,
    double                  u_max_hole,
    double                  v_min_hole,
    double                  v_max_hole )
{
    int      i, j, ni, nj, n_fitting_samples;
    double   u, v, x, y, z;

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

private  int  get_n_samples(
    int     n_samples_for_whole_surface,
    double  u_min,
    double  u_max,
    double  v_min,
    double  v_max )
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

private  double  get_parameter_in_range(
    int      i,
    int      n,
    double   min,
    double   max )
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

private  BOOLEAN  inside_hole(
    double      u,
    double      v,
    BOOLEAN     hole_present,
    double      u_min_hole,
    double      u_max_hole,
    double      v_min_hole,
    double      v_max_hole )
{
    BOOLEAN  in_hole;

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

private  double   evaluate_fit_at_uv(
    Volume                  volume,
    surface_fitting_struct  *fit_data,
    double                  parameters[],
    double                  u,
    double                  v )
{
    double   surface_estimate, curvature, fit, u_curvature, v_curvature;
    double   x, y, z, sign_normal;
    Real     dx, dy, dz;
    double   dxuu, dyuu, dzuu, dxvv, dyvv, dzvv;
    double   dxu, dyu, dzu, dxv, dyv, dzv;
    Vector   surface_normal, function_deriv;
    Real     val;

    fit_data->surface_representation->evaluate_surface_at_uv( u, v,
                                 fit_data->descriptors, parameters, &x, &y, &z,
                                 &dxu, &dyu, &dzu, &dxv, &dyv, &dzv,
                                 &dxuu, &dyuu, &dzuu, &dxvv, &dyvv, &dzvv );

    fit = 0.0;

    if( fit_data->isovalue_factor > 0.0 )
    {
        if( volume != (Volume) NULL )
        {
#ifdef afd
            if( get_volume_voxel_activity( label_volume,
            if( evaluate_volume_in_world( volume, x, y, z, Volume_continuity,
                                          TRUE, &val,
                                          (Real *) 0, (Real *) 0, (Real *) 0,
                                          (Real *) 0, (Real *) 0, (Real *) 0,
                                          (Real *) 0, (Real *) 0, (Real *) 0 ) )
            {
                surface_estimate = ABS( val - fit_data->isovalue );
            }
            else
                surface_estimate = BIG_NUMBER;

            fit += surface_estimate * fit_data->isovalue_factor;
#endif
        }
    }

    if( fit_data->gradient_strength_factor > 0.0 )
    {
        if( volume != (Volume) NULL )
        {
#ifdef adsf
            if( evaluate_volume_in_world( volume, x, y, z, Volume_continuity,
                                          TRUE, &val, &dx, &dy, &dz,
                                          (Real *) NULL, (Real *) NULL,
                                          (Real *) NULL, (Real *) NULL,
                                          (Real *) NULL, (Real *) NULL ) )
            {
                get_surface_normal_from_derivs( dxu, dyu, dzu, dxv, dyv, dzv,
                                                &surface_normal );

                fill_Vector( function_deriv, dx, dy, dz );

                surface_estimate = DOT_VECTORS( function_deriv,
                                                surface_normal );

                if( surface_estimate < 0.0 )
                {
                    surface_estimate = -surface_estimate;
                    sign_normal = 1.0;
                }
                else
                {
                    sign_normal = -1.0;
                }

                if( fit_data->gradient_strength_exponent != 1.0 )
                {
                    if( fit_data->gradient_strength_exponent == 2.0 )
                        surface_estimate = surface_estimate * surface_estimate;
                    else
                        surface_estimate = pow( surface_estimate,
                           (double) fit_data->gradient_strength_exponent );
                }

                surface_estimate = - sign_normal * surface_estimate;
            }
            else
                surface_estimate = BIG_NUMBER;
#endif
        }
        else
        {
            surface_estimate = 0.0;
        }

        fit += surface_estimate * fit_data->gradient_strength_factor;
    }

    if( fit_data->curvature_factor > 0.0 )
    {
#define CURVATURE
#ifdef  CURVATURE
        u_curvature = get_radius_of_curvature( dxu, dyu, dzu, dxuu, dyuu, dzuu);
        v_curvature = get_radius_of_curvature( dxv, dyv, dzv, dxvv, dyvv, dzvv);

/*
        curvature = -MIN( u_curvature, v_curvature );
*/
        if( u_curvature < fit_data->curvature_factor ||
            v_curvature < fit_data->curvature_factor )
            curvature = BIG_NUMBER;
        else
            curvature = 0.0;
#else
#ifdef NORMAL
        Vector   du, dv;

        fill_Vector( du, dxu, dyu, dzu );
        fill_Vector( dv, dxv, dyv, dzv );
        NORMALIZE_VECTOR( du, du );
        NORMALIZE_VECTOR( dv, dv );

        curvature = DOT_VECTORS( du, dv );
#else
        Vector   du, dv, duu, dvv;
        Real     mag_du, mag_dv, mag_duu, mag_dvv;

        fill_Vector( du, dxu, dyu, dzu );
        fill_Vector( dv, dxv, dyv, dzv );
        fill_Vector( duu, dxuu, dyuu, dzuu );
        fill_Vector( dvv, dxvv, dyvv, dzvv );
        mag_du = MAGNITUDE( du );
        mag_dv = MAGNITUDE( dv );
        mag_duu = MAGNITUDE( duu );
        mag_dvv = MAGNITUDE( dvv );

        if( mag_du == 0.0 )
            mag_du = 1.0;

        if( mag_dv == 0.0 )
            mag_dv = 1.0;

        curvature = mag_duu / mag_du + mag_dvv / mag_dv;
#endif
#endif

        fit += fit_data->curvature_factor * curvature;
    }

    apply_surface_point_to_distances( x, y, z, fit_data,
                                      fit_data->surface_point_distances );

    return( fit );
}

private  void  apply_surface_point_to_distances(
    double                  x,
    double                  y,
    double                  z,
    surface_fitting_struct  *fit_data,
    Real                    surface_point_distances[] )
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

private  double  distance_measure(
    int     n_surface_points,
    Real    surface_point_distances[],
    Real    distance_threshold )
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

private  double  get_radius_of_curvature(
    double   dx,
    double   dy,
    double   dz,
    double   ddx,
    double   ddy,
    double   ddz )
{
    double  radius_of_curvature;
    double  x_cross, y_cross, z_cross, mag_cross, mag_deriv;

    cross_product( dx, dy, dz, ddx, ddy, ddz, &x_cross, &y_cross, &z_cross );

    mag_cross = sqrt( x_cross * x_cross + y_cross * y_cross + z_cross*z_cross );

    mag_deriv = sqrt( dx * dx + dy * dy + dz * dz );

    radius_of_curvature = mag_deriv * mag_deriv * mag_deriv;

    if( mag_cross > 0.0 )
        radius_of_curvature /= mag_cross;

    return( radius_of_curvature );
}

private  void   cross_product(
    double  x1,
    double  y1,
    double  z1,
    double  x2,
    double  y2,
    double  z2,
    double  *x_cross,
    double  *y_cross,
    double  *z_cross )
{
    *x_cross = y1 * z2 - y2 * z1;
    *y_cross = z1 * x2 - z2 * x1;
    *z_cross = x1 * y2 - x2 * y1;
}
