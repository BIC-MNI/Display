
#include  <display.h>

public  void  apply_simplex_minimization(
    int             n_parameters,
    double          parameters[],
    double          (*evaluate_fit_function)( void *, double [] ),
    void            *evaluation_ptr )
{
    int                      n_fitting_evaluations;
    downhill_simplex_struct  minimization;

    initialize_amoeba( &minimization, n_parameters, parameters,
                       evaluate_fit_function, evaluation_ptr ); 

    amoeba( &minimization, n_parameters, Fitting_tolerance,
            Max_fitting_evaluations, &n_fitting_evaluations, parameters );

    terminate_amoeba( &minimization );
}

public  void  apply_one_parameter_minimization(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    int                 max_iterations,
    double              tolerance,
    int                 n_parameters,
    double              parameters[],
    double              max_parameter_deltas[],
    double              parameter_deltas[],
    void                (*evaluate_distances_function)
                                   ( void *, double [], Real [],
                                      double, double, double, double ),
    double              (*evaluate_fit_function)( void *, double [],
                                     double, double, double, double,
                                     Real [] ),
    void                *evaluation_ptr )
{
    double   prev_fit, fit, prev_parameter;
    int      n_iterations, current_parameter;
    Real     *distances_without_this_parameter;
    double   u_min, u_max, v_min, v_max, gain, total_gain;

#ifdef TESTING
    test_min( surface_rep,
              descriptors,
              max_iterations,
              tolerance,
              n_parameters, parameters,
              max_parameter_deltas,
              parameter_deltas,
              evaluate_distances_function,
              evaluate_fit_function,
              evaluation_ptr );

    return;
#endif

    ALLOC( distances_without_this_parameter, n_parameters );

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

            print( "Parameter[%d]:  %g -> %g    = gain of %g\n",
                   current_parameter + 1,
                   prev_parameter, parameters[current_parameter], gain );

#ifdef DEBUG
            if( !numerically_close( t1 - t2, gain, 0.05 ) )
                print( "Error  t1 %g, t2 %g, t1 - t2 %g, gain %g\n",
                       t1, t2, t1 - t2, gain );
#endif
        }

        ++n_iterations;

        fit = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                         0.0, 1.0, 0.0, 1.0, (Real *) 0 );

        print( "Fit has improved from %g to %g\n", prev_fit, fit );

        if( !numerically_close( prev_fit - total_gain, fit, 0.05 ) )
            print( "------------------- Error  total gain = %g, actual = %g\n",
                       total_gain, prev_fit - fit );
    }
    while( n_iterations < max_iterations &&
           (n_iterations == 1 || !numerically_close(fit,prev_fit,tolerance)) );

        FREE( distances_without_this_parameter );
}

#ifdef TESTING

private  void  test_min( surface_rep,
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
    int      which_parameter;
    double   prev_parameter;
    double   u_min, u_max, v_min, v_max, prev, next;
    double   u_min2, u_max2, v_min2, v_max2;
    double   prev_part1, next_part1, prev_part2, next_part2;
    double   t1, t2;

    which_parameter = 0;

    t1 = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                    0.0, 1.0, 0.0, 1.0, (Real *) 0 );

    t2 = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                    0.0, 1.0, 0.0, 0.999, (Real *) 0 );

    if( t1 != t2 )
    {
        print( "------ Error in test_min %g %g.\n", t1, t2 );
    }

    return;

    surface_rep->get_parameter_influence( which_parameter,
                            descriptors, &u_min, &u_max, &v_min, &v_max );

    u_min2 = 0.0;
    u_max2 = 1.0;
    v_min2 = 0.5;
    v_max2 = 1.0;

    prev_parameter = parameters[which_parameter];

    prev = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                     0.0, 1.0, 0.0, 1.0, (Real *) 0 );

    prev_part1 = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                     u_min, u_max, v_min, v_max, (Real *) 0 );

/*
    prev_part2 = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                 u_min2, u_max2, v_min2, v_max2, (Real *) 0 );
*/

    parameters[which_parameter] *= 1.05;

    next = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                     0.0, 1.0, 0.0, 1.0, (Real *) 0 );

    next_part1 = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                     u_min, u_max, v_min, v_max, (Real *) 0 );

/*
    next_part2 = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                 u_min2, u_max2, v_min2, v_max2, (Real *) 0 );
*/


    print( "prev %g, %g + %g (%g)\n", prev, prev_part1, prev_part2,
           prev_part1 + prev_part2 );

    print( "next %g, %g + %g (%g)\n", next, next_part1, next_part2,
           next_part1 + next_part2 );

    if( !numerically_close( next_part1 - prev_part1, next - prev, 1.0e-3 ) )
    {
        print( "------------- Error -----------------\n %g %g",
               next_part1 - prev_part1, next - prev );
    }

    parameters[which_parameter] = prev_parameter;
}

#endif
