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
