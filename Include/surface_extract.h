
#ifndef  DEF_SURFACE_EXTRACT
#define  DEF_SURFACE_EXTRACT

#include  <def_hash.h>
#include  <def_bitlist.h>
#include  <def_queue.h>

typedef  struct
{
    short   i[N_DIMENSIONS];
} voxel_index_struct;

typedef  struct
{
    int   point_index;
} edge_point_struct;

#define  VOXEL_COMPLETELY_DONE    15

typedef  struct
{
    Boolean                              extraction_in_progress;
    Real                                 isovalue;
    Boolean                              isovalue_selected;

    hash_table_struct                    edge_points;

    int                                  n_voxels_with_surface;

    int                                  n_voxels_alloced;
    bitlist_struct                       voxels_queued;
    unsigned_byte                        *voxel_done_flags;

    QUEUE_STRUCT( voxel_index_struct )   voxels_to_do;
    

    polygons_struct                      *polygons;
} surface_extraction_struct;


#endif
