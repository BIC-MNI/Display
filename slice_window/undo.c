#include  <display.h>

public  void  initialize_slice_undo(
    slice_undo_struct  *undo )
{
    undo->axis_index = -1;
    undo->slice_index = -1;
    undo->saved_labels = (int **) NULL;
}

public  void  delete_slice_undo(
    slice_undo_struct  *undo )
{
    if( undo->axis_index >= 0 )
    {
        FREE2D( undo->saved_labels );
        initialize_slice_undo( undo );
    }
}

public  void  record_slice_labels(
    display_struct  *display,
    int             axis_index,
    int             slice_index )
{
    int             voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    int             x_index, y_index;
    Volume          label_volume;
    display_struct  *slice_window;

    if( Undo_enabled &&
        get_slice_window( display, &slice_window ) )
    {
        delete_slice_undo( &slice_window->slice.undo );

        slice_window->slice.undo.axis_index = axis_index;
        slice_window->slice.undo.slice_index = slice_index;

        label_volume = get_label_volume( slice_window );
        get_volume_sizes( label_volume, sizes );

        x_index = (axis_index + 1) % N_DIMENSIONS;
        y_index = (axis_index + 2) % N_DIMENSIONS;

        ALLOC2D( slice_window->slice.undo.saved_labels,
                 sizes[x_index], sizes[y_index] );

        voxel[axis_index] = slice_index;
        for_less( voxel[x_index], 0, sizes[x_index] )
        {
            for_less( voxel[y_index], 0, sizes[y_index] )
            {
                slice_window->slice.undo.
                    saved_labels[voxel[x_index]][voxel[y_index]] =
                         get_volume_label_data( label_volume, voxel );
            }
        }
    }
}

public  void  record_slice_under_mouse(
    display_struct  *display )
{
    int             axis_index;
    display_struct  *slice_window;
    Real            voxel[MAX_DIMENSIONS];

    if( get_slice_window( display, &slice_window ) &&
        get_axis_index_under_mouse( display, &axis_index ) )
    {
        get_current_voxel( slice_window, voxel );
        if( voxel_is_within_volume( get_volume(slice_window), voxel ) )
            record_slice_labels( slice_window, axis_index,
                                 ROUND( voxel[axis_index] ) );
    }
}

public  BOOLEAN  undo_slice_labels_if_any(
    display_struct  *display )
{
    int             voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    int             axis_index, slice_index;
    int             x_index, y_index;
    Volume          label_volume;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        slice_window->slice.undo.axis_index >= 0 )
    {
        axis_index = slice_window->slice.undo.axis_index;
        slice_index = slice_window->slice.undo.slice_index;

        label_volume = get_label_volume( slice_window );
        get_volume_sizes( label_volume, sizes );

        x_index = (axis_index + 1) % N_DIMENSIONS;
        y_index = (axis_index + 2) % N_DIMENSIONS;

        voxel[axis_index] = slice_index;
        for_less( voxel[x_index], 0, sizes[x_index] )
        {
            for_less( voxel[y_index], 0, sizes[y_index] )
            {
                set_volume_label_data( label_volume, voxel,
                            slice_window->slice.undo.
                                saved_labels[voxel[x_index]][voxel[y_index]] );
            }
        }

        delete_slice_undo( &slice_window->slice.undo );
    }
    else
        print( "Nothing to undo.\n" );
}
