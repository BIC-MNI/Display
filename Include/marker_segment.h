#ifndef  DEF_MARKER_SEGMENT
#define  DEF_MARKER_SEGMENT

#include  <def_objects.h>
#include  <def_bitlist.h>

typedef struct
{
    model_struct     *model;
    int              n_objects;
    bitlist_struct   *connected;
}
marker_segment_struct;

#endif
