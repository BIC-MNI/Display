
#include  <def_globals.h>
#include  <def_files.h>
#include  <def_minimization.h>
#include  <def_surface_rep.h>
#include  <def_alloc.h>

#define  DEBUG

public  Status  apply_simplex_minimization( n_parameters, parameters,
                                       evaluate_fit_function, evaluation_ptr )
    int             n_parameters;
    double          parameters[];
    double          (*evaluate_fit_function)( void *, double [] );
    void            *evaluation_ptr;
{
    Status                   status;
    int                      n_fitting_evaluations;
    downhill_simplex_struct  minimization;
    Status                   initialize_amoeba();
    void                     amoeba();
    Status                   terminate_amoeba();
    void                     display_parameters();

    status = initialize_amoeba( &minimization, n_parameters, parameters,
                                evaluate_fit_function, evaluation_ptr ); 

    if( status == OK )
        amoeba( &minimization, n_parameters, Fitting_tolerance,
                Max_fitting_evaluations, &n_fitting_evaluations, parameters );

    if( status == OK )
        status = terminate_amoeba( &minimization );

    return( status );
}

public  void  apply_one_parameter_minimization( surface_rep,
                                                descriptors,
                                                max_iterations,
                                                tolerance,
                                                n_parameters, parameters,
                                                max_parameter_deltas,
                                                parameter_deltas,
                                                evaluate_distances_function,
                                                evaluate_fit_function,
                                                evaluation_ptr )
    surface_rep_struct  *surface_rep;
    double              descriptors[];
    int                 max_iterations;
    double              tolerance;
    int                 n_parameters;
    double              parameters[];
    double              max_parameter_deltas[];
    double              parameter_deltas[];
    void                (*evaluate_distances_function)
                                   ( void *, double [], Real [],
                                      double, double, double, double );
    double              (*evaluate_fit_function)( void *, double [],
                                     double, double, double, double,
                                     Real [] );
    void                *evaluation_ptr;
{
    Status   status;
    double   prev_fit, fit, prev_parameter;
    int      n_iterations, current_parameter;
    void     one_parameter_minimization();
    Real     *distances_without_this_parameter;
    double   u_min, u_max, v_min, v_max, gain, total_gain;

    ALLOC( status, distances_without_this_parameter, n_parameters );

    n_iterations = 0;

    fit = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                     0.0, 1.0, 0.0, 1.0, (Real *) 0 );

    do
    {
        prev_fit = fit;
        total_gain = 0.0;

        for_less( current_parameter, 0, n_parameters )
        {
#ifdef DEBUG
double  t1, t2;
#endif
            surface_rep->get_parameter_influence( current_parameter,
                            descriptors, &u_min, &u_max, &v_min, &v_max );

            (*evaluate_distances_function)( evaluation_ptr, parameters,
                                            distances_without_this_parameter,
                                            u_min, u_max, v_min, v_max );

            prev_parameter = parameters[current_parameter];

#ifdef DEBUG
t1 = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                         0.0, 1.0, 0.0, 1.0, (Real *) 0 );
#endif

            one_parameter_minimization( parameters, current_parameter,
                                        u_min, u_max, v_min, v_max,
                                        max_parameter_deltas[current_parameter],
                                        &parameter_deltas[current_parameter],
                                        distances_without_this_parameter,
                                        evaluate_fit_function, evaluation_ptr,
                                        &gain );

            total_gain = total_gain + gain;

#ifdef DEBUG
t2 = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                         0.0, 1.0, 0.0, 1.0, (Real *) 0 );
#endif

            PRINT( "Parameter[%d]:  %g -> %g    = gain of %g\n",
                   current_parameter + 1,
                   prev_parameter, parameters[current_parameter], gain );

#ifdef DEBUG
            if( !numerically_close( t1 - t2, gain, 1.0e-2 ) )
                PRINT( "Error  t1 %g, t2 %g, t1 - t2 %g, gain %g\n",
                       t1, t2, t1 - t2, gain );
#endif
        }

        ++n_iterations;

        fit = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                         0.0, 1.0, 0.0, 1.0, (Real *) 0 );

        PRINT( "Fit has improved from %g to %g\n", prev_fit, fit );

        if( !numerically_close( prev_fit - total_gain, fit, 1.0e-3 ) )
            PRINT( "------------------- Error  total gain = %g, actual = %g\n",
                       total_gain, prev_fit - fit );
    }
    while( n_iterations < max_iterations &&
           (n_iterations == 1 || !numerically_close(fit,prev_fit,tolerance)) );

    if( status == OK )
        FREE( status, distances_without_this_parameter );
}
