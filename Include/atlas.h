#ifndef  DEF_ATLAS
#define  DEF_ATLAS

#include   <def_objects.h>

typedef  struct
{
    int             axis;
    Real            axis_position;
    int             n_resolutions;
    int             *pixel_map_indices;
} atlas_position_struct;

typedef  struct
{
    Boolean                input;
    Boolean                enabled;
    Real                   opacity;
    Real                   transparent_threshold;
    Real                   slice_tolerance[3];
    Boolean                flipped[3];

    int                    n_pixel_maps;
    pixels_struct          *pixel_maps;

    int                    n_pages;
    atlas_position_struct  *pages;

    atlas_position_struct  **slice_lookup[3];
} atlas_struct;



#endif
