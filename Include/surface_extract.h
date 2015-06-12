#ifndef  DEF_SURFACE_EXTRACT
#define  DEF_SURFACE_EXTRACT

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

#include  <bicpl.h>
#include  <bicpl/queue.h>

typedef  struct
{
    short   i[VIO_N_DIMENSIONS];
} voxel_index_struct;

typedef  struct
{
    int   point_index;
} edge_point_struct;

#define  VOXEL_COMPLETELY_DONE    15

typedef QUEUE_STRUCT( voxel_index_struct )   voxel_queue_struct;

#define  SURFACE_BLOCK_SIZE   8

typedef  struct
{
    VIO_Volume                               volume;
    VIO_Volume                               label_volume;

    VIO_BOOL                              extraction_in_progress;
    VIO_BOOL                              binary_flag;
    VIO_BOOL                              voxellate_flag;
    VIO_Real                                 min_value;
    VIO_Real                                 max_value;
    VIO_Real                                 min_invalid_label;
    VIO_Real                                 max_invalid_label;

    hash_table_struct                    edge_points;
    hash_table_struct                    faces_done;
    QUEUE_STRUCT( int )                  deleted_faces;

    int                                  n_voxels_with_surface;

    int                                  n_voxels_alloced;

    bitlist_3d_struct                    voxel_state;
    VIO_UCHAR                            *voxel_done_flags;
    voxel_queue_struct                   voxels_to_do;

    int                                  current_voxel[VIO_N_DIMENSIONS];
    int                                  min_block[VIO_N_DIMENSIONS];
    int                                  max_block[VIO_N_DIMENSIONS];

    int                                  min_limits[VIO_N_DIMENSIONS];
    int                                  max_limits[VIO_N_DIMENSIONS];
    int                                  min_changed_limits[VIO_N_DIMENSIONS];
    int                                  max_changed_limits[VIO_N_DIMENSIONS];
    int                                  min_modified[VIO_N_DIMENSIONS];
    int                                  max_modified[VIO_N_DIMENSIONS];
    int                                  not_changed_since[VIO_N_DIMENSIONS];

    VIO_BOOL                              inside_flags[SURFACE_BLOCK_SIZE+2]
                                                     [SURFACE_BLOCK_SIZE+2]
                                                     [SURFACE_BLOCK_SIZE+2];
    VIO_BOOL                              valid_flags[SURFACE_BLOCK_SIZE+2]
                                                    [SURFACE_BLOCK_SIZE+2]
                                                    [SURFACE_BLOCK_SIZE+2];

    polygons_struct                      *polygons;
} surface_extraction_struct;


#endif
