#include  <display.h>

public  void  initialize_3d_segmenting(
    display_struct   *slice_window )
{
    slice_window->slice.segmenting.volumes_alloced = FALSE;
}

public  void  delete_3d_segmenting(
    display_struct   *slice_window )
{
    if( slice_window->slice.segmenting.volumes_alloced )
    {
        delete_volume( slice_window->slice.segmenting.distance_transform );
        delete_volume( slice_window->slice.segmenting.cuts );
        slice_window->slice.segmenting.volumes_alloced = FALSE;
    }
}

public  void  restart_segmenting_3d(
    display_struct   *slice_window )
{
    delete_3d_segmenting( slice_window );

    slice_window->slice.segmenting.volumes_alloced = TRUE;
    initialize_segmenting_3d( get_volume(slice_window),
                          get_label_volume(slice_window),
                          slice_window->slice.segmenting.min_threshold,
                          slice_window->slice.segmenting.max_threshold,
                          &slice_window->slice.segmenting.distance_transform,
                          &slice_window->slice.segmenting.cuts );
    print( "Segmenting Reset.\n" );
}

public  void  one_iteration_segmenting(
    display_struct   *slice_window )
{
    if( !expand_labels_3d( get_label_volume(slice_window),
                           slice_window->slice.segmenting.distance_transform,
                           slice_window->slice.segmenting.cuts ) )
    {
        print( "Done expanding\n" );
    }
    else
        set_slice_window_all_update( slice_window );
}
