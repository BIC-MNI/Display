
#ifndef  DEF_SURFACE_FITTING
#define  DEF_SURFACE_FITTING

#include  <def_mni.h>
#include  <def_geometry.h>
#include  <def_surface_rep.h>

typedef  struct
{
    double              *parameters;
    double              *descriptors;
    int                 n_samples;
    int                 n_surface_points;
    Point               *surface_points;
    Real                *surface_point_distances;
    Real                gradient_strength_factor;
    Real                curvature_factor;
    Real                surface_point_distance_factor;
    Real                surface_point_distance_threshold;
    surface_rep_struct  *surface_representation;
} surface_fitting_struct;


#endif
