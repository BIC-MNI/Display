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

#ifndef lint
static char surface_extract_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/surface_extract.h,v 1.27 2001-05-26 23:01:38 stever Exp $";
#endif

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

#define  SURFACE_BLOCK_SIZE   8

typedef  struct
{
    Volume                               volume;
    Volume                               label_volume;

    BOOLEAN                              extraction_in_progress;
    BOOLEAN                              binary_flag;
    BOOLEAN                              voxellate_flag;
    Real                                 min_value;
    Real                                 max_value;
    Real                                 min_invalid_label;
    Real                                 max_invalid_label;

    hash_table_struct                    edge_points;
    hash_table_struct                    faces_done;
    QUEUE_STRUCT( int )                  deleted_faces;

    int                                  n_voxels_with_surface;

    int                                  n_voxels_alloced;

    bitlist_3d_struct                    voxel_state;
    unsigned_byte                        *voxel_done_flags;
    voxel_queue_struct                   voxels_to_do;

    int                                  current_voxel[N_DIMENSIONS];
    int                                  min_block[N_DIMENSIONS];
    int                                  max_block[N_DIMENSIONS];

    int                                  min_limits[N_DIMENSIONS];
    int                                  max_limits[N_DIMENSIONS];
    int                                  min_changed_limits[N_DIMENSIONS];
    int                                  max_changed_limits[N_DIMENSIONS];
    int                                  min_modified[N_DIMENSIONS];
    int                                  max_modified[N_DIMENSIONS];
    int                                  not_changed_since[N_DIMENSIONS];

    BOOLEAN                              inside_flags[SURFACE_BLOCK_SIZE+2]
                                                     [SURFACE_BLOCK_SIZE+2]
                                                     [SURFACE_BLOCK_SIZE+2];
    BOOLEAN                              valid_flags[SURFACE_BLOCK_SIZE+2]
                                                    [SURFACE_BLOCK_SIZE+2]
                                                    [SURFACE_BLOCK_SIZE+2];

    polygons_struct                      *polygons;
} surface_extraction_struct;


#endif
