
#include  <def_globals.h>
#include  <def_files.h>
#include  <def_minimization.h>

public  Status  apply_simplex_minimization( n_parameters, parameters,
                                       evaluate_fit_function, evaluation_ptr )
    int             n_parameters;
    double          parameters[];
    double          (*evaluate_fit_function)();
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

public  void  apply_one_parameter_minimization( max_iterations,
                                                tolerance,
                                                n_parameters, parameters,
                                                max_parameter_deltas,
                                                parameter_deltas,
                                                evaluate_fit_function,
                                                evaluation_ptr )
    int             max_iterations;
    double          tolerance;
    int             n_parameters;
    double          parameters[];
    double          max_parameter_deltas[];
    double          parameter_deltas[];
    double          (*evaluate_fit_function)();
    void            *evaluation_ptr;
{
    double  prev_fit, fit, prev_parameter;
    int     n_iterations, current_parameter;
    void    one_parameter_minimization();

    fit = (*evaluate_fit_function) ( evaluation_ptr, parameters );
    n_iterations = 0;

    do
    {
        prev_fit = fit;

        for_less( current_parameter, 0, n_parameters )
        {
            prev_parameter = parameters[current_parameter];
            one_parameter_minimization( &fit, parameters, current_parameter,
                                        max_parameter_deltas[current_parameter],
                                        &parameter_deltas[current_parameter],
                                        evaluate_fit_function, evaluation_ptr );

            PRINT( "%g:  parameter[%d]:  %g -> %g\n",
                   fit, current_parameter + 1,
                   prev_parameter, parameters[current_parameter] );
        }

        ++n_iterations;
    }
    while( n_iterations < max_iterations &&
           (n_iterations == 1 || !numerically_close(fit,prev_fit,tolerance)) );
}
