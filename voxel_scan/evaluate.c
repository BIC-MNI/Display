
#include  <def_standard.h>
#include  <def_stdio.h>
#include  <def_alloc.h>
#include  <def_math.h>
#include  <def_mr.h>
#include  <def_progress.h>
#include  <def_geometry.h>
#include  <def_minimization.h>

const  double  BIG_NUMBER                       = 1.0e30;

const  double  TOLERANCE                         = 1.0e-4;

const  double  CURVATURE_FACTOR                  =       0.0;
const  double  SURFACE_POINT_DISTANCE_FACTOR     = 1.0;
const  double  SURFACE_POINT_DISTANCE_THRESHOLD  = 1.0;

static    Status     input_data_file();
static    Status     fit_surface();
static    Status     optimize_surface();
double     evaluate_fit();
static    void       evaluate_fit_derivative();
static    double     get_magnitude_derivative();
static    void       random_step_towards_derivative();
static    void       step_towards_derivative();
static    double     evaluate_fit_partial_derivative();
static    double     evaluate_fit_at_uv();
static    void       compute_xyz();
static    Boolean    inside_dataset();
static    void       evaluate_mr_function();
static    void       evaluate_mr_function_and_derivs();

int  main( argc, argv )
    int    argc;
    char   *argv[];
{
    Status             status;
    Status             initialize_data_file_input();
    Status             terminate_data_file_input();
    unsigned short     ***slice;
    data_file_struct   data_file;
    int                i, n_parameters, n_surface_points;
    double             *parameters;
    Real               x, y, z;
    Point              point, *surface_points;
   
    if( argc < 2 )
    {
        PRINT_ERROR( "Arguments\n" );
        return( 1 );
    }

    status = initialize_data_file_input( argv[1], "", 10, 100000.0,
                                         &data_file );

    if( status == OK )
    {
        n_parameters = get_num_parameters();

        ALLOC1( status, parameters, n_parameters, double );
    }

    if( status == OK )
    {
        PRINT( "Enter %d initial parameters: ", n_parameters );
        for_less( i, 0, n_parameters )
            (void) scanf( "%lf", &parameters[i] );

        PRINT( "Enter surface point [1]: " );

        n_surface_points = 0;
        while( scanf( "%f %f %f", &x, &y, &z ) == 3 )
        {
            fill_Point( point, x, y, z );
            ADD_ELEMENT_TO_ARRAY( status, n_surface_points, surface_points,
                                  point, Point, DEFAULT_CHUNK_SIZE );

            PRINT( "Enter surface point [%d]: ", n_surface_points+1 );
        }

        PRINT( "\n" );
        PRINT( "%d surface points entered.\n", n_surface_points );
    }

    if( status == OK )
        status = input_data_file( &data_file, &slice );

    if( status == OK )
        status = terminate_data_file_input( &data_file );

    if( status == OK )
    {
        status = fit_surface( data_file.nx, data_file.ny, data_file.nz,
                              slice, n_parameters, parameters,
                              n_surface_points, surface_points );
    }

    if( status == OK )
        FREE1( status, parameters );

    return( (int) status );
}

private  Status  input_data_file( data_file, slice )
    data_file_struct   *data_file;
    unsigned  short    ****slice;
{
    int               z;
    Status            status;
    Status            alloc_slice();
    Status            input_slice_from_file();
    progress_struct   progress;

    initialize_progress_report( &progress, TRUE, data_file->nz, "Inputting" );

    ALLOC1( status, *slice, data_file->nz, unsigned short ** );

    if( status == OK )
    {
        for_less( z, 0, data_file->nz )
        {
            status = alloc_slice( &(*slice)[z], data_file->nx, data_file->ny );

            if( status == OK )
            {
                status = input_slice_from_file( data_file, (*slice)[z] );
            }

            update_progress_report( &progress, z+1 );
        }

        terminate_progress_report( &progress );
    }

    return( status );
}

private  Status   fit_surface( nx, ny, nz, slice, n_parameters,
                               init_parameters,
                               n_surface_points, surface_points )
    int                nx, ny, nz;
    unsigned short     ***slice;
    int                n_parameters;
    double             init_parameters[];
    int                n_surface_points;
    Point              surface_points[];
{
    Status   status;
    double   *parameters;
    Status   optimize_surface();

    ALLOC1( status, parameters, n_parameters, double );

    if( status == OK )
        status = optimize_surface( nx, ny, nz, slice, n_parameters,
                                   init_parameters, parameters,
                                   n_surface_points, surface_points );

    if( status == OK )
        status = optimize_surface( nx, ny, nz, slice, n_parameters,
                                   parameters, parameters,
                                   n_surface_points, surface_points );

    if( status == OK )
        FREE1( status, parameters );

    return( status );
}

typedef  struct
{
    int             nx, ny, nz;
    unsigned short  ***slice;
    int             n_surface_points;
    Point           *surface_points;
    double          *surface_point_distances;
} evaluation_data_struct;

private  Status   optimize_surface( nx, ny, nz, slice, n_parameters,
                                    init_parameters, parameters,
                                    n_surface_points, surface_points )
    int                nx, ny, nz;
    unsigned short     ***slice;
    int                n_parameters;
    double             init_parameters[];
    double             parameters[];
    int                n_surface_points;
    Point              surface_points[];
{
    Status                status;
    int                   i, total, n_done;
    double                current_fit_val;
    Status                initialize_amoeba();
    void                  amoeba();
    Status                terminate_amoeba();
    evaluation_data_struct  eval_data;
    minimization_struct   fit_data;
    const int             N_FUNCTION_EVALS = 10;
    const int             MAX_FUNCTION_EVALS = 10000;

    eval_data.nx = nx;
    eval_data.ny = ny;
    eval_data.nz = nz;
    eval_data.slice = slice;
    eval_data.n_surface_points = n_surface_points;
    eval_data.surface_points = surface_points;

    status = OK;

    if( n_surface_points > 0 )
    {
        ALLOC1( status, eval_data.surface_point_distances, n_surface_points,
                double );
    }

    if( status == OK )
        status = initialize_amoeba( &fit_data, (void *) &eval_data,
                                    init_parameters, n_parameters );

    if( status == OK )
    {
        total = 0;
        do
        {
            amoeba( &fit_data, n_parameters,
                    TOLERANCE, N_FUNCTION_EVALS, &n_done, parameters );

            current_fit_val = evaluate_fit( (void *) &eval_data, parameters );

            PRINT( "%10g:", current_fit_val );
            for_less( i, 0, n_parameters )
                PRINT( " %g", parameters[i] );

            PRINT( "\n" );
            total += n_done;
        }
        while( n_done >= N_FUNCTION_EVALS && total < MAX_FUNCTION_EVALS );

        current_fit_val = evaluate_fit( (void *) &eval_data, parameters );
    }

    if( status == OK )
    {
        PRINT( "%14g: ", current_fit_val );
        for_less( i, 0, n_parameters )
            PRINT( " %g", parameters[i] );
        PRINT( "\n" );

        status = terminate_amoeba( &fit_data );
    }

    if( status == OK && n_surface_points > 0 )
        FREE1( status, eval_data.surface_point_distances );

    return( status );
}

public  double   evaluate_fit( evaluation_ptr, parameters )
    void            *evaluation_ptr;
    double          parameters[];
{
    const    int  N_SAMPLES = 10000;
    int                     i, j, ni, nj, n_samples;
    double                  fit, measure_of_fit, sum;
    double                  u, v;
    double                  distance_measure();
    evaluation_data_struct  *eval_data;
    Real                    get_random_0_to_1();

    eval_data = (evaluation_data_struct *) evaluation_ptr;

    if( !parameters_are_valid(parameters) )
    {
        measure_of_fit = BIG_NUMBER;
    }
    else
    {
        sum = 0.0;
        n_samples = 0;

        for_less( i, 0, eval_data->n_surface_points )
            eval_data->surface_point_distances[i] = -1.0;

#ifdef RANDOMLY
        for_less( i, 0, N_SAMPLES )
        {
            u = (double) get_random_0_to_1();
            v = (double) get_random_0_to_1();

            fit = evaluate_fit_at_uv( eval_data->nx, eval_data->ny,
                           eval_data->nz, eval_data->slice, 
                           eval_data->n_surface_points,
                           eval_data->surface_points,
                           eval_data->surface_point_distances,
                           parameters, u, v );

            sum += fit;
            ++n_samples;
        }
#else
        ni = (int) sqrt((double) N_SAMPLES) + 1;
        nj = ni;
        for_less( i, 0, ni )
        {
            u = (double) i / (double) ni;
            for_less( j, 0, nj )
            {
                v = (double) j / (double) nj;

                fit = evaluate_fit_at_uv( eval_data->nx, eval_data->ny,
                           eval_data->nz, eval_data->slice, 
                           eval_data->n_surface_points,
                           eval_data->surface_points,
                           eval_data->surface_point_distances,
                           parameters, u, v );
                sum += fit;
                ++n_samples;
            }
        }
#endif

        measure_of_fit = sum / (double) n_samples +
                         SURFACE_POINT_DISTANCE_FACTOR *
                         distance_measure( eval_data->n_surface_points,
                                           eval_data->surface_point_distances );
    }

    return( measure_of_fit );
}

private  double   evaluate_fit_at_uv( nx, ny, nz, slice, n_surface_points,
                                      surface_points, surface_point_distances,
                                      parameters, u, v )
    int                nx, ny, nz;
    unsigned short     ***slice;
    int                n_surface_points;
    Point              surface_points[];
    double             surface_point_distances[];
    double             parameters[];
    double             u, v;
{
    void     evaluate_parametric_surface();
    double   surface_estimate, curvature, fit, u_curvature, v_curvature;
    double   x, y, z, fval, dx, dy, dz;
    double   dxuu, dyuu, dzuu, dxvv, dyvv, dzvv;
    double   dxu, dyu, dzu, dxv, dyv, dzv;
    double   dist;
    const    double  PI_SQUARED = PI * PI;
    const    double  FOUR_PI_SQUARED = 4.0 * PI_SQUARED;
    int      i;
    Vector   du, dv, surface_normal, function_deriv;

    evaluate_parametric_surface( u, v, parameters, &x, &y, &z,
                                 &dxu, &dyu, &dzu, &dxv, &dyv, &dzv,
                                 &dxuu, &dyuu, &dzuu, &dxvv, &dyvv, &dzvv );

    if( inside_dataset( x, y, z, nx, ny, nz ) )
    {
        evaluate_mr_function_and_derivs( z, x, y, nz, nx, ny, slice, &fval,
                                         &dz, &dx, &dy );

        Vector_x(du) = dxu;
        Vector_y(du) = dyu;
        Vector_z(du) = dzu;

        Vector_x(dv) = dxv;
        Vector_y(dv) = dyv;
        Vector_z(dv) = dzv;

        CROSS_VECTORS( surface_normal, dv, du );

        NORMALIZE_VECTOR( surface_normal, surface_normal );

        fill_Vector( function_deriv, dx, dy, dz );

        surface_estimate = -DOT_VECTORS( function_deriv, surface_normal );
    }
    else
    {
        surface_estimate = BIG_NUMBER;
    }

    if( CURVATURE_FACTOR > 0.0 )
    {
        u_curvature = PI_SQUARED / sqrt( dxuu*dxuu + dyuu*dyuu + dzuu*dzuu );
        v_curvature = FOUR_PI_SQUARED / sqrt( dxvv*dxvv + dyvv*dyvv + dzvv*dzvv );

        curvature = sqrt( u_curvature * v_curvature );

        fit = surface_estimate + CURVATURE_FACTOR * curvature;
    }
    else
        fit = surface_estimate;

    for_less( i, 0, n_surface_points )
    {
        dx = x - Point_x(surface_points[i]);
        dy = y - Point_y(surface_points[i]);
        dz = z - Point_z(surface_points[i]);
        dist = dx * dx + dy * dy + dz * dz;
        if( surface_point_distances[i] < 0.0 ||
            dist < surface_point_distances[i] )
        {
            surface_point_distances[i] = dist;
        }
    }

    return( fit );
}

private  double  distance_measure( n_surface_points,
                                   surface_point_distances )
    int     n_surface_points;
    double  surface_point_distances[];
{
    int     i;
    double  sum, dist;

    sum = 0.0;

    for_less( i, 0, n_surface_points )
    {
        dist = sqrt( surface_point_distances[i] );

        if( dist > SURFACE_POINT_DISTANCE_THRESHOLD )
            sum += dist;
    }

    if( n_surface_points > 0 )
        sum /= (double) n_surface_points;

    return( sum );
}

private  Boolean  inside_dataset( x, y, z, nx, ny, nz )
    double     x, y, z;
    int        nx, ny, nz;
{
    Boolean  inside;

    inside = (x >= 0.0 && x <= (double) nx-1 &&
              y >= 0.0 && y <= (double) ny-1 &&
              z >= 0.0 && z <= (double) nz-1);

    return( inside );
}

private  void   evaluate_mr_function( x, y, z, nx, ny, nz, slice, val )
    double             x, y, z;
    int                nx, ny, nz;
    unsigned short     ***slice;
    double             *val;
{
    int      i, j, k;
    double   u, v, w;
    double   c000, c001, c010, c011, c100, c101, c110, c111;
    double   c00, c01, c10, c11;
    double   c0, c1;
    double   dv0, dv1;
    double   du00, du01, du10, du11;

    i = (int) x;
    j = (int) y;
    k = (int) z;

    u = FRACTION( x );
    v = FRACTION( y );
    w = FRACTION( z );

    if( i == nx-1 )
    {
        --i;
        u += 1.0;
    }

    if( j == ny-1 )
    {
        --j;
        v += 1.0;
    }

    if( k == nz-1 )
    {
        --k;
        w += 1.0;
    }

    c000 = (double) slice[i  ][j  ][k  ];
    c001 = (double) slice[i  ][j  ][k+1];
    c010 = (double) slice[i  ][j+1][k  ];
    c011 = (double) slice[i  ][j+1][k+1];
    c100 = (double) slice[i+1][j  ][k  ];
    c101 = (double) slice[i+1][j  ][k+1];
    c110 = (double) slice[i+1][j+1][k  ];
    c111 = (double) slice[i+1][j+1][k+1];

    du00 = c100 - c000;
    du01 = c101 - c001;
    du10 = c110 - c010;
    du11 = c111 - c011;

    c00 = c000 + u* du00;
    c01 = c001 + u* du01;
    c10 = c010 + u* du10;
    c11 = c011 + u* du11;

    dv0 = c10 - c00;
    dv1 = c11 - c01;

    c0 = c00 + v * dv0;
    c1 = c01 + v * dv1;

    *val = INTERPOLATE( w, c0, c1 );
}

private  void   evaluate_mr_function_and_derivs( x, y, z, nx, ny, nz, slice,
                                                 val, dx, dy, dz )
    double             x, y, z;
    int                nx, ny, nz;
    unsigned short     ***slice;
    double             *val, *dx, *dy, *dz;
{
    int      i, j, k;
    double   u, v, w;
    double   c000, c001, c010, c011, c100, c101, c110, c111;
    double   c00, c01, c10, c11;
    double   c0, c1;
    double   du00, du01, du10, du11, du0, du1;
    double   dv0, dv1;

    i = (int) x;
    j = (int) y;
    k = (int) z;

    u = FRACTION( x );
    v = FRACTION( y );
    w = FRACTION( z );

    if( i == nx-1 )
    {
        --i;
        u += 1.0;
    }

    if( j == ny-1 )
    {
        --j;
        v += 1.0;
    }

    if( k == nz-1 )
    {
        --k;
        w += 1.0;
    }

    c000 = (double) slice[i  ][j  ][k  ];
    c001 = (double) slice[i  ][j  ][k+1];
    c010 = (double) slice[i  ][j+1][k  ];
    c011 = (double) slice[i  ][j+1][k+1];
    c100 = (double) slice[i+1][j  ][k  ];
    c101 = (double) slice[i+1][j  ][k+1];
    c110 = (double) slice[i+1][j+1][k  ];
    c111 = (double) slice[i+1][j+1][k+1];

    du00 = c100 - c000;
    du01 = c101 - c001;
    du10 = c110 - c010;
    du11 = c111 - c011;

    c00 = c000 + u* du00;
    c01 = c001 + u* du01;
    c10 = c010 + u* du10;
    c11 = c011 + u* du11;

    du0 = INTERPOLATE( v, du00, du10 );
    du1 = INTERPOLATE( v, du01, du11 );

    dv0 = c10 - c00;
    dv1 = c11 - c01;

    c0 = c00 + v * dv0;
    c1 = c01 + v * dv1;

    *val = INTERPOLATE( w, c0, c1 );
    *dx = INTERPOLATE( w, du0, du1 );
    *dy = INTERPOLATE( w, dv0, dv1 );
    *dz = c1 - c0;
}
