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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/undo.c,v 1.8 1996-04-17 17:50:25 david Exp $";
#endif

#include  <display.h>

public  void  initialize_slice_undo(
    slice_undo_struct  *undo )
{
    undo->volume_index = -1;
    undo->axis_index = -1;
    undo->slice_index = -1;
    undo->saved_labels = (int **) NULL;
}

public  void  delete_slice_undo(
    slice_undo_struct  *undo,
    int                volume_index )
{
    if( undo->axis_index >= 0 &&
        (volume_index < 0 || volume_index == undo->volume_index) )
    {
        FREE2D( undo->saved_labels );
        initialize_slice_undo( undo );
    }
}

public  void  record_slice_labels(
    display_struct  *display,
    int             volume_index,
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
        delete_slice_undo( &slice_window->slice.undo, -1 );

        slice_window->slice.undo.volume_index = volume_index;
        slice_window->slice.undo.axis_index = axis_index;
        slice_window->slice.undo.slice_index = slice_index;

        label_volume = get_nth_label_volume( slice_window, volume_index );
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
                         get_voxel_label( display, volume_index,
                                          voxel[X], voxel[Y], voxel[Z] );
            }
        }
    }
}

public  void  record_slice_under_mouse(
    display_struct  *display,
    int             volume_index )
{
    int             view_index, slice, sizes[MAX_DIMENSIONS];
    int             x_index, y_index, axis_index;
    display_struct  *slice_window;
    Real            voxel[MAX_DIMENSIONS];

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( display, &view_index ) &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        get_current_voxel( slice_window, volume_index, voxel );
        slice = ROUND( voxel[axis_index] );
        get_volume_sizes( get_nth_volume(slice_window,volume_index), sizes );
        if( slice >= 0 && slice < sizes[axis_index] )
            record_slice_labels( slice_window, volume_index, axis_index, slice);
    }
}

public  BOOLEAN  slice_labels_to_undo(
    display_struct  *display )
{
    display_struct  *slice_window;

    return( get_slice_window( display, &slice_window ) &&
            slice_window->slice.undo.axis_index >= 0 );
}

public  int  undo_slice_labels_if_any(
    display_struct  *display )
{
    int             voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    int             volume_index, axis_index, slice_index;
    int             x_index, y_index;
    Volume          label_volume;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        slice_window->slice.undo.axis_index >= 0 )
    {
        volume_index = slice_window->slice.undo.volume_index;
        axis_index = slice_window->slice.undo.axis_index;
        slice_index = slice_window->slice.undo.slice_index;

        label_volume = get_nth_label_volume( slice_window, volume_index );
        get_volume_sizes( label_volume, sizes );

        x_index = (axis_index + 1) % N_DIMENSIONS;
        y_index = (axis_index + 2) % N_DIMENSIONS;

        voxel[axis_index] = slice_index;
        for_less( voxel[x_index], 0, sizes[x_index] )
        {
            for_less( voxel[y_index], 0, sizes[y_index] )
            {
                set_voxel_label( display, volume_index,
                                 voxel[X], voxel[Y], voxel[Z],
                                 slice_window->slice.undo.
                                 saved_labels[voxel[x_index]][voxel[y_index]] );
            }
        }

        delete_slice_undo( &slice_window->slice.undo, -1 );
    }
    else
    {
        print( "Nothing to undo.\n" );
        volume_index = -1;
    }

    return( volume_index );
}
