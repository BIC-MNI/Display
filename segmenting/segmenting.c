
#include  <def_graphics.h>
#include  <def_arrays.h>
#include  <def_queue.h>
#include  <def_globals.h>

public  void  initialize_segmenting( segmenting )
    segmenting_struct  *segmenting;
{
    segmenting->n_labels = 0;
    segmenting->labels = (label_struct *) 0;
    segmenting->min_threshold = -1;
    segmenting->max_threshold = -1;
    segmenting->connectivity = (Neighbour_types) Segmenting_connectivity;
}

public  Status  delete_all_labels( segmenting )
    segmenting_struct  *segmenting;
{
    Status   status;

    status = OK;

    if( segmenting->n_labels > 0 )
    {
        FREE( status, segmenting->labels );
        segmenting->n_labels = 0;
    }

    return( status );
}

public  Status  reset_segmentation( slice_window )
    graphics_struct   *slice_window;
{
    Status   status;
    void     set_all_voxel_activity_flags();
    void     set_all_voxel_label_flags();

    status = delete_all_labels( &slice_window->slice.segmenting );

    set_all_voxel_activity_flags( slice_window->slice.volume, TRUE );
    set_all_voxel_label_flags( slice_window->slice.volume, FALSE );

    return( status );
}

public  Status  add_point_label( slice_window, x, y, z, id )
    graphics_struct  *slice_window;
{
    Status        status;
    label_struct  label;
    void          set_voxel_label_flag();
    void          set_voxel_activity_flag();

    label.voxel_indices[X_AXIS] = x;
    label.voxel_indices[Y_AXIS] = y;
    label.voxel_indices[Z_AXIS] = z;
    label.id = id;

    ADD_ELEMENT_TO_ARRAY( status,
                          slice_window->slice.segmenting.n_labels,
                          slice_window->slice.segmenting.labels,
                          label, DEFAULT_CHUNK_SIZE );

    if( id == 1 )
        set_voxel_label_flag( slice_window->slice.volume, x, y, z, TRUE );
    else
        set_voxel_activity_flag( slice_window->slice.volume, x, y, z, FALSE );

    return( status );
}

public  Status  generate_segmentation( slice_window, voxel_indices, voxel_axes )
    graphics_struct   *slice_window;
    int               voxel_indices[3];
    int               voxel_axes[3];
{
    Status   status;
    Status   disconnect_components();

    status = disconnect_components( slice_window->slice.volume,
                        voxel_indices, voxel_axes,
                        slice_window->slice.segmenting.n_labels,
                        slice_window->slice.segmenting.labels,
                        slice_window->slice.segmenting.min_threshold,
                        slice_window->slice.segmenting.max_threshold );

    return( status );
}

public  void  reset_slice_activity( volume, axis_index, position )
    volume_struct  *volume;
    int            axis_index;
    int            position;
{
    int     voxel[3], sizes[3], a1, a2;
    void    get_volume_size();
    void    set_voxel_activity_flag();

    get_volume_size( volume, &sizes[0], &sizes[1], &sizes[2] );

    voxel[axis_index] = position;

    a1 = (axis_index + 1) % 3;
    a2 = (axis_index + 2) % 3;

    for_less( voxel[a1], 0, sizes[a1] )
    {
        for_less( voxel[a2], 0, sizes[a2] )
        {
            set_voxel_activity_flag( volume, voxel[0], voxel[1], voxel[2],
                                     TRUE );
        }
    }
}

typedef struct
{
    int  x, y;
} slice_position;

public  void  set_connected_voxels_inactive( volume, axis_index, position,
                                             min_threshold, max_threshold )
    volume_struct  *volume;
    int            axis_index;
    int            position[3];
    int            min_threshold;
    int            max_threshold;
{
    int                             voxel[3], sizes[3], a1, a2, x, y, dx, dy;
    void                            get_volume_size();
    void                            set_voxel_activity_flag();
    Status                          status;
    slice_position                  entry;
    QUEUE_STRUCT( slice_position )  queue;

    get_volume_size( volume, &sizes[0], &sizes[1], &sizes[2] );

    voxel[0] = position[0];
    voxel[1] = position[1];
    voxel[2] = position[2];

    a1 = (axis_index + 1) % 3;
    a2 = (axis_index + 2) % 3;

    INITIALIZE_QUEUE( queue );

    if( value_in_range( 
                 (int) GET_VOLUME_DATA( *volume, voxel[0], voxel[1], voxel[2]),
                 min_threshold, max_threshold ) )
    {
        set_voxel_activity_flag( volume, voxel[0], voxel[1], voxel[2], FALSE );
        entry.x = voxel[a1];
        entry.y = voxel[a2];
        INSERT_IN_QUEUE( status, queue, entry );
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;

        for_inclusive( dx, -1, 1 )
        {
            for_inclusive( dy, -1, 1 )
            {
                voxel[a1] = x + dx;
                voxel[a2] = y + dy;
                if( (dx != 0 || dy != 0) &&
                    voxel[a1] >= 0 && voxel[a1] < sizes[a1] &&
                    voxel[a2] >= 0 && voxel[a2] < sizes[a2] &&
                    get_voxel_activity_flag( volume, voxel[0], voxel[1],
                                             voxel[2] ) &&
                    value_in_range(
                         (int) GET_VOLUME_DATA( *volume, voxel[0], voxel[1],
                                                voxel[2]),
                         min_threshold, max_threshold ) )
                {
                    set_voxel_activity_flag( volume, voxel[0], voxel[1],
                                             voxel[2], FALSE );
                    entry.x = voxel[a1];
                    entry.y = voxel[a2];
                    INSERT_IN_QUEUE( status, queue, entry );
                }
            }
        }
    }

    if( status == OK )
        DELETE_QUEUE( status, queue );
}

private  Boolean  value_in_range( value, min_threshold, max_threshold )
    int   value;
    int   min_threshold;
    int   max_threshold;
{
    return( min_threshold <= value && value <= max_threshold );
}
