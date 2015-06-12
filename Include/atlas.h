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

#include   <volume_io.h>

typedef  struct
{
    int             axis;
    VIO_Real            axis_position;
    VIO_Volume          image;
} atlas_image_struct;

typedef  struct
{
    VIO_BOOL                input;
    VIO_BOOL                enabled;
    VIO_Real                   opacity;
    int                    transparent_threshold;
    VIO_Real                   slice_tolerance[VIO_N_DIMENSIONS];
    VIO_BOOL                flipped[3];

    int                    n_images;
    atlas_image_struct     *images;
} atlas_struct;

#endif
