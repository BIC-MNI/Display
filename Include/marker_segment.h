#ifndef  DEF_MARKER_SEGMENT
#define  DEF_MARKER_SEGMENT

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
static char marker_segment_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/Attic/marker_segment.h,v 1.6 1995-07-31 19:53:44 david Exp $";
#endif

#include  <volume_io.h>

typedef struct
{
    Real             threshold_distance;
    BOOLEAN          must_be_reinitialized;
    model_struct     *model;
    int              n_objects;
    float            **distances;
}
marker_segment_struct;

#endif
