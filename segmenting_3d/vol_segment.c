#include  <display.h>

public  void  initialize_3d_segmenting(
    display_struct   *slice_window )
{
    slice_window->slice.segmenting.segmenting_started = FALSE;
}

public  void  delete_3d_segmenting(
    display_struct   *slice_window )
{
    if( slice_window->slice.segmenting.segmenting_started )
    {
        FREE3D( slice_window->slice.segmenting.distance_transform );
        FREE3D( slice_window->slice.segmenting.cuts );
        delete_bitlist_3d( &slice_window->slice.segmenting.to_do );
        slice_window->slice.segmenting.segmenting_started = FALSE;
    }
}

public  void  restart_segmenting_3d(
    display_struct   *slice_window,
    int              n_dimensions,
    int              voxel_pos,
    int              axis )
{
    delete_3d_segmenting( slice_window );

    slice_window->slice.segmenting.segmenting_started = TRUE;
    slice_window->slice.segmenting.n_dimensions = n_dimensions;
    slice_window->slice.segmenting.voxel_pos = voxel_pos;
    slice_window->slice.segmenting.axis = axis;
    initialize_segmenting_3d( get_volume(slice_window),
                          get_label_volume(slice_window),
                          n_dimensions, voxel_pos, axis,
                          slice_window->slice.segmenting.min_threshold,
                          slice_window->slice.segmenting.max_threshold,
                          &slice_window->slice.segmenting.distance_transform,
                          &slice_window->slice.segmenting.cuts,
                          &slice_window->slice.segmenting.to_do );
    print( "Segmenting Reset.\n" );
    set_slice_window_all_update( slice_window );
}

public  void  one_iteration_segmenting(
    display_struct   *slice_window )
{
    if( !slice_window->slice.segmenting.segmenting_started )
    {
        print( "No segmenting to do.\n" );
        return;
    }

    if( !expand_labels_3d( get_label_volume(slice_window),
                           slice_window->slice.segmenting.distance_transform,
                           slice_window->slice.segmenting.cuts,
                           &slice_window->slice.segmenting.to_do,
                           slice_window->slice.segmenting.n_dimensions,
                           slice_window->slice.segmenting.voxel_pos,
                           slice_window->slice.segmenting.axis ) )
    {
        print( "Done expanding\n" );

        delete_3d_segmenting( slice_window );
    }
    else
        set_slice_window_all_update( slice_window );
}
