
#ifndef  DEF_SURFACE_EXTRACT
#define  DEF_SURFACE_EXTRACT

#include  <def_hash.h>
#include  <def_queue.h>

typedef  struct
{
    int   voxel_indices[N_DIMENSIONS];
} voxel_index_struct;

typedef  struct
{
    int   point_index;
} edge_point_struct;

typedef  struct
{
    Boolean                              extraction_started;
    Real                                 isovalue;
    Boolean                              isovalue_selected;

    hash_table_struct                    edge_points;
    hash_table_struct                    voxels_done;

    QUEUE_STRUCT( voxel_index_struct )   voxels_to_do;
    

    polygons_struct                      triangles;
} surface_extraction_struct;


#endif
