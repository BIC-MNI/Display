#ifndef  DEF_MARKER_SEGMENT
#define  DEF_MARKER_SEGMENT

#include  <def_objects.h>
#include  <def_bitlist.h>

typedef struct
{
    Real             threshold_distance;
    Boolean          must_be_reinitialized;
    model_struct     *model;
    int              n_objects;
    float            **distances;
}
marker_segment_struct;

#endif
