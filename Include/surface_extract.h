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
static char surface_extract_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/surface_extract.h,v 1.20 1996-04-17 17:50:09 david Exp $";
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

typedef  struct
{
    Volume                               volume;
    Volume                               label_volume;

    BOOLEAN                              extraction_in_progress;
    BOOLEAN                              binary_flag;
    Real                                 min_value;
    Real                                 max_value;
    Real                                 min_invalid_label;
    Real                                 max_invalid_label;

    hash_table_struct                    edge_points;

    int                                  n_voxels_with_surface;

    int                                  n_voxels_alloced;
    bitlist_3d_struct                    voxels_queued;
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
