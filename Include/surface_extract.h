
#ifndef  DEF_SURFACE_EXTRACT
#define  DEF_SURFACE_EXTRACT

#include  <bicpl.h>
#include  <queue.h>

typedef  struct
{
    short   i[N_DIMENSIONS];
} voxel_index_struct;

typedef  struct
{
    int   point_index;
} edge_point_struct;

#define  VOXEL_COMPLETELY_DONE    15

typedef QUEUE_STRUCT( voxel_index_struct )   voxel_queue_struct;

typedef  struct
{
    Volume                               volume;
    Volume                               label_volume;

    BOOLEAN                              extraction_in_progress;
    BOOLEAN                              binary_flag;
    Real                                 min_value;
    Real                                 max_value;
    Real                                 valid_min_label;
    Real                                 valid_max_label;
    Real                                 valid_out_min_label;
    Real                                 valid_out_max_label;

    hash_table_struct                    edge_points;

    int                                  n_voxels_with_surface;

    int                                  n_voxels_alloced;
    bitlist_struct                       voxels_queued;
    unsigned_byte                        *voxel_done_flags;

    voxel_queue_struct                   voxels_to_do;

    int                                  x_voxel_max_distance;
    int                                  y_voxel_max_distance;
    int                                  z_voxel_max_distance;
    int                                  x_starting_voxel;
    int                                  y_starting_voxel;
    int                                  z_starting_voxel;
    

    polygons_struct                      *polygons;
} surface_extraction_struct;


#endif
