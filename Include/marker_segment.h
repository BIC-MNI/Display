#ifndef  DEF_MARKER_SEGMENT
#define  DEF_MARKER_SEGMENT

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
