#include  <def_graphics.h>

public  double   evaluate_graphics_fit( evaluation_ptr, parameters )
    void            *evaluation_ptr;
    double          parameters[];
{
    double                  evaluate_fit_in_volume();
    volume_struct           *volume;
    graphics_struct         *graphics;
    surface_fitting_struct  *fit_data;

    graphics = (graphics_struct *) evaluation_ptr;

    fit_data = &graphics->three_d.surface_fitting;

    (void) get_slice_window_volume( graphics, &volume );

    return( evaluate_fit_in_volume( volume, fit_data, parameters ) );
}

public  double   evaluate_graphics_fit_with_range( evaluation_ptr, parameters,
                                            u_min, u_max, v_min, v_max,
                                            surface_point_distances )
    void            *evaluation_ptr;
    double          parameters[];
    double          u_min;
    double          u_max;
    double          v_min;
    double          v_max;
    Real            surface_point_distances[];
{
    double                  evaluate_fit_in_volume_with_distances();
    volume_struct           *volume;
    graphics_struct         *graphics;
    surface_fitting_struct  *fit_data;

    graphics = (graphics_struct *) evaluation_ptr;

    fit_data = &graphics->three_d.surface_fitting;

    (void) get_slice_window_volume( graphics, &volume );

    return( evaluate_fit_in_volume_with_distances( volume, fit_data,
                          parameters, u_min, u_max, v_min, v_max,
                          surface_point_distances ) );
}

public  void   evaluate_graphics_surface_point_distances( evaluation_ptr,
                                           parameters, distances,
                                           u_min_parm, u_max_parm,
                                           v_min_parm, v_max_parm )
    void            *evaluation_ptr;
    double          parameters[];
    Real            distances[];
    double          u_min_parm, u_max_parm;
    double          v_min_parm, v_max_parm;
{
    graphics_struct         *graphics;
    surface_fitting_struct  *fit_data;
    void                    evaluate_distances_to_surface();

    graphics = (graphics_struct *) evaluation_ptr;

    fit_data = &graphics->three_d.surface_fitting;

    evaluate_distances_to_surface( fit_data,
                                   parameters, 0.0, 1.0, 0.0, 1.0,
                                   distances,
                                   TRUE, u_min_parm, u_max_parm,
                                   v_min_parm, v_max_parm );
}
