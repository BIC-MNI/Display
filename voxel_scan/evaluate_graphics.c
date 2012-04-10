#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

public  double   evaluate_graphics_fit(
    void            *evaluation_ptr,
    double          parameters[] )
{
    Volume                  volume;
    display_struct          *display;
    surface_fitting_struct  *fit_data;

    display = (display_struct *) evaluation_ptr;

    fit_data = &display->three_d.surface_fitting;

    (void) get_slice_window_volume( display, &volume );

    return( evaluate_fit_in_volume( volume, fit_data, parameters ) );
}

public  double   evaluate_graphics_fit_with_range(
    void            *evaluation_ptr,
    double          parameters[],
    double          u_min,
    double          u_max,
    double          v_min,
    double          v_max,
    Real            surface_point_distances[] )
{
    Volume                  volume;
    display_struct          *display;
    surface_fitting_struct  *fit_data;

    display = (display_struct  *) evaluation_ptr;

    fit_data = &display->three_d.surface_fitting;

    (void) get_slice_window_volume( display, &volume );

    return( evaluate_fit_in_volume_with_distances( volume, fit_data,
                          parameters, u_min, u_max, v_min, v_max,
                          surface_point_distances ) );
}

public  void   evaluate_graphics_surface_point_distances(
    void            *evaluation_ptr,
    double          parameters[],
    Real            distances[],
    double          u_min_parm,
    double          u_max_parm,
    double          v_min_parm,
    double          v_max_parm )
{
    display_struct          *display;
    surface_fitting_struct  *fit_data;

    display = (display_struct  *) evaluation_ptr;

    fit_data = &display->three_d.surface_fitting;

    evaluate_distances_to_surface( fit_data,
                                   parameters, 0.0, 1.0, 0.0, 1.0,
                                   distances,
                                   TRUE, u_min_parm, u_max_parm,
                                   v_min_parm, v_max_parm );
}
