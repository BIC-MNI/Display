
#ifndef  DEF_SURFACE_EXTRACT
#define  DEF_SURFACE_EXTRACT

#include  <def_hash.h>
#include  <def_bitlist.h>
#include  <def_queue.h>

typedef  enum  { ON_FIRST_CORNER, ON_EDGE, ON_SECOND_CORNER } Point_classes;

typedef  struct
{
    int   i[N_DIMENSIONS];
} voxel_index_struct;

typedef  struct
{
    int   point_index;
} edge_point_struct;

typedef  struct
{
    Boolean                              extraction_in_progress;
    Real                                 isovalue;
    Boolean                              isovalue_selected;

    hash_table_struct                    edge_points;

    int                                  n_voxels_alloced;
    bitlist_struct                       voxels_done;

    QUEUE_STRUCT( voxel_index_struct )   voxels_to_do;
    

    polygons_struct                      *triangles;
} surface_extraction_struct;


#endif
