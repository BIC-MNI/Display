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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

  void  initialize_segmenting(
    segmenting_struct  *segmenting )
{
    segmenting->min_threshold = 0.0;
    segmenting->max_threshold = -1.0;
    segmenting->connectivity = (Neighbour_types) Segmenting_connectivity;
}

  void  clear_all_labels(
    display_struct    *display )
{
    display_struct    *slice_window;

    if( get_slice_window( display, &slice_window ) )
        clear_labels( slice_window, get_current_volume_index(slice_window) );
}

  void  set_labels_on_slice(
    display_struct  *slice_window,
    int             volume_index,
    int             axis_index,
    int             position,
    int             label )
{
    int     voxel[VIO_N_DIMENSIONS], sizes[VIO_N_DIMENSIONS], a1, a2;

    get_volume_sizes( get_nth_label_volume(slice_window,volume_index), sizes );

    voxel[axis_index] = position;

    a1 = (axis_index + 1) % VIO_N_DIMENSIONS;
    a2 = (axis_index + 2) % VIO_N_DIMENSIONS;

    for_less( voxel[a1], 0, sizes[a1] )
    {
        for_less( voxel[a2], 0, sizes[a2] )
        {
            set_voxel_label( slice_window, volume_index,
                             voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z], label );
        }
    }
}

typedef struct
{
    int  x, y;
} slice_position;

  void  set_connected_voxels_labels(
    display_struct    *slice_window,
    int               volume_index,
    int               axis_index,
    int               position[],
    VIO_Real              min_threshold,
    VIO_Real              max_threshold,
    int               label_min_threshold,
    int               label_max_threshold,
    Neighbour_types   connectivity,
    int               label )
{
    int                             voxel[VIO_N_DIMENSIONS], sizes[VIO_N_DIMENSIONS];
    int                             a1, a2, x, y;
    int                             dir, n_dirs, *dx, *dy;
    slice_position                  entry;
    QUEUE_STRUCT( slice_position )  queue;
    VIO_Volume                          volume, label_volume;

    volume = get_nth_volume( slice_window, volume_index );
    label_volume = get_nth_label_volume( slice_window, volume_index );

    n_dirs = get_neighbour_directions( connectivity, &dx, &dy );

    get_volume_sizes( volume, sizes );

    voxel[0] = position[0];
    voxel[1] = position[1];
    voxel[2] = position[2];

    a1 = (axis_index + 1) % VIO_N_DIMENSIONS;
    a2 = (axis_index + 2) % VIO_N_DIMENSIONS;

    INITIALIZE_QUEUE( queue );

    if( should_change_this_one( volume, label_volume, voxel,
                                min_threshold, max_threshold, 
                                label_min_threshold, label_max_threshold,
                                label) )
    {
        set_voxel_label( slice_window, volume_index,
                         voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z], label );
        entry.x = voxel[a1];
        entry.y = voxel[a2];
        INSERT_IN_QUEUE( queue, entry );
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;

        for_less( dir, 0, n_dirs )
        {
            voxel[a1] = x + dx[dir];
            voxel[a2] = y + dy[dir];

            if( voxel[a1] >= 0 && voxel[a1] < sizes[a1] &&
                voxel[a2] >= 0 && voxel[a2] < sizes[a2] &&
                should_change_this_one( volume, label_volume, voxel,
                                     min_threshold, max_threshold,
                                     label_min_threshold, label_max_threshold,
                                     label))
            {
                set_voxel_label( slice_window, volume_index,
                                 voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z], label );
                entry.x = voxel[a1];
                entry.y = voxel[a2];
                INSERT_IN_QUEUE( queue, entry );
            }
        }
    }

    DELETE_QUEUE( queue );
}
