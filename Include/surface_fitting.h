
#ifndef  DEF_SURFACE_FITTING
#define  DEF_SURFACE_FITTING

#include  <bicpl.h>

typedef  struct
{
    double              *parameters;
    double              *descriptors;
    int                 n_samples;
    double              *max_parameter_deltas;
    double              *parameter_deltas;
    int                 n_surface_points;
    Point               *surface_points;
    Real                *surface_point_distances;
    Real                isovalue;
    Real                isovalue_factor;
    Real                gradient_strength_factor;
    Real                gradient_strength_exponent;
    Real                curvature_factor;
    Real                surface_point_distance_factor;
    Real                surface_point_distance_threshold;
    surface_rep_struct  *surface_representation;
} surface_fitting_struct;


#endif
