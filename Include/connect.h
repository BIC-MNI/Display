
#ifndef  DEF_CONNECT
#define  DEF_CONNECT

#include  <def_mni.h>

typedef  struct
{
    Boolean  inside;
    int      label;
    Boolean  queued;
    int      dist_transform;
    int      dist_from_region;
    int      cutoff;

    Boolean  on_path;
    int      path_dist_transform;
} pixel_struct;

#define  INVALID_LABEL   0

#endif
