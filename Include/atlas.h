#ifndef  DEF_ATLAS
#define  DEF_ATLAS

/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char atlas_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/atlas.h,v 1.7 1996-04-19 13:24:50 david Exp $";
#endif

#include   <volume_io.h>

typedef  struct
{
    int             axis;
    Real            axis_position;
    int             n_resolutions;
    int             *pixel_map_indices;
} atlas_position_struct;

typedef  struct
{
    BOOLEAN                input;
    BOOLEAN                enabled;
    Real                   opacity;
    int                    transparent_threshold;
    Real                   slice_tolerance[N_DIMENSIONS];
    BOOLEAN                flipped[3];

    int                    n_pixel_maps;
    pixels_struct          *pixel_maps;

    int                    n_pages;
    atlas_position_struct  *pages;

    atlas_position_struct  **slice_lookup[N_DIMENSIONS];
} atlas_struct;



#endif
