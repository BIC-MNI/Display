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
static char atlas_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/atlas.h,v 1.9 2001-05-26 23:01:37 stever Exp $";
#endif

#include   <volume_io.h>

typedef  struct
{
    int             axis;
    Real            axis_position;
    Volume          image;
} atlas_image_struct;

typedef  struct
{
    BOOLEAN                input;
    BOOLEAN                enabled;
    Real                   opacity;
    int                    transparent_threshold;
    Real                   slice_tolerance[N_DIMENSIONS];
    BOOLEAN                flipped[3];

    int                    n_images;
    atlas_image_struct     *images;
} atlas_struct;

#endif
