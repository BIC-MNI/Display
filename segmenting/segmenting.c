
#include  <def_graphics.h>
#include  <def_alloc.h>

public  void  initialize_segmenting( segmenting )
    segmenting_struct  *segmenting;
{
    segmenting->n_labels = 0;
    segmenting->labels = (label_struct *) 0;
    segmenting->min_threshold = -1;
    segmenting->max_threshold = -1;
}

public  Status  delete_all_labels( segmenting )
    segmenting_struct  *segmenting;
{
    Status   status;

    status = OK;

    if( segmenting->n_labels > 0 )
    {
        FREE1( status, segmenting->labels );
        segmenting->n_labels = 0;
    }

    return( status );
}

public  Status  reset_segmentation( slice_window )
    graphics_struct   *slice_window;
{
    Status   status;
    void     set_all_voxel_activities();
    void     set_all_voxel_inactivities();

    status = delete_all_labels( &slice_window->slice.segmenting );

    set_all_voxel_activities( slice_window->slice.volume, TRUE );
    set_all_voxel_inactivities( slice_window->slice.volume, FALSE );

    return( status );
}

public  Status  add_point_label( slice_window, x, y, z, id )
    graphics_struct  *slice_window;
{
    Status        status;
    label_struct  label;
    void          set_voxel_activity();
    void          set_voxel_inactivity();

    label.voxel_indices[X_AXIS] = x;
    label.voxel_indices[Y_AXIS] = y;
    label.voxel_indices[Z_AXIS] = z;
    label.id = id;

    ADD_ELEMENT_TO_ARRAY( status,
                          slice_window->slice.segmenting.n_labels,
                          slice_window->slice.segmenting.labels,
                          label, label_struct, DEFAULT_CHUNK_SIZE );

    if( id == 1 )
        set_voxel_activity( slice_window->slice.volume, x, y, z, TRUE );
    else
        set_voxel_inactivity( slice_window->slice.volume, x, y, z, TRUE );

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
