
#ifndef  DEF_CONNECT
#define  DEF_CONNECT

#include  <mni.h>

typedef  struct
{
    BOOLEAN  inside;
    int      label;
    BOOLEAN  queued;
    int      dist_transform;
    int      dist_from_region;
    int      cutoff;

    BOOLEAN  on_path;
    int      path_dist_transform;
} pixel_struct;

#define  INVALID_LABEL   0

#endif
