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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/segmenting.c,v 1.42 1995-07-31 19:53:51 david Exp $";
#endif


#include  <display.h>

private  void  set_slice_labels(
    display_struct     *display,
    int                label );
private  void   set_connected_labels(
    display_struct   *display,
    int              label,
    BOOLEAN          use_threshold );

/* ARGSUSED */

public  DEF_MENU_FUNCTION( label_voxel )
{
    Real           voxel[MAX_DIMENSIONS];
    int            view_index, int_voxel[MAX_DIMENSIONS], volume_index;
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index, voxel))
    {
        record_slice_under_mouse( slice_window, volume_index );
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_volume_label_data( get_nth_label_volume(slice_window,volume_index),
                               int_voxel,
                               get_current_paint_label(slice_window) );
        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(label_voxel )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( clear_voxel )
{
    Real           voxel[MAX_DIMENSIONS];
    display_struct *slice_window;
    int            view_index, int_voxel[MAX_DIMENSIONS], volume_index;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index,voxel))
    {
        record_slice_under_mouse( slice_window, volume_index );
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_volume_label_data( get_nth_label_volume(slice_window,volume_index),
                               int_voxel, 0 );
        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(clear_voxel )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( reset_segmenting )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        clear_all_labels( slice_window );
        delete_slice_undo( &slice_window->slice.undo,
                           get_current_volume_index(slice_window) );
        set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(reset_segmenting )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_segmenting_threshold )
{
    display_struct   *slice_window;
    Real             min, max;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter min and max threshold: " );

        if( input_real( stdin, &min ) == OK && input_real( stdin, &max ) == OK )
        {
            slice_window->slice.segmenting.min_threshold = min;
            slice_window->slice.segmenting.max_threshold = max;
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_segmenting_threshold )
{
    return( slice_window_exists(display) );
}

public  Status  input_label_volume_file(
    display_struct   *display,
    char             *filename )
{
    Status           status;
    display_struct   *slice_window;

    status = OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        status = load_label_volume( filename,
                                    get_label_volume(slice_window) );

        if( status == OK )
            (void) strcpy( slice_window->slice.volumes[
                      get_current_volume_index(slice_window)].labels_filename,
                      filename );

        delete_slice_undo( &slice_window->slice.undo,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(load_label_data)
{
    Status           status;
    STRING           filename;

    status = OK;

    if( get_n_volumes(display) > 0 )
    {
        print( "Enter filename to load: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        status = input_label_volume_file( display, filename );

        print( "Done\n" );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(load_label_data )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(save_label_data)
{
    Status           status;
    STRING           filename;
    display_struct   *slice_window;

    status = OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter filename to save: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = save_label_volume( filename,
                      slice_window->slice.volumes[
                      get_current_volume_index(slice_window)].labels_filename,
                      get_label_volume(slice_window) );

        print( "Done\n" );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(save_label_data )
{
    return( get_n_volumes(display) > 0 );
}

public  Status input_tag_label_file(
    display_struct   *display,
    char             filename[] )
{
    Status         status;
    BOOLEAN        landmark_format;
    FILE           *file;
    display_struct *slice_window;
    Volume         volume;

    status = OK;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        landmark_format = filename_extension_matches( filename,
                             get_default_landmark_file_suffix() );

        status = open_file_with_default_suffix( filename,
                            get_default_tag_file_suffix(),
                            READ_FILE, ASCII_FORMAT, &file );

        if( status == OK )
        {
            if( landmark_format )
                status = input_landmarks_as_labels( file, volume,
                                        get_label_volume(slice_window) );
            else
                status = input_tags_as_labels( file, volume,
                                        get_label_volume(slice_window) );
        }

        if( status == OK )
            status = close_file( file );

        delete_slice_undo( &slice_window->slice.undo,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                 get_current_volume_index(slice_window), UPDATE_LABELS  );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( load_labels )
{
    STRING         filename;

    if( get_n_volumes(display) > 0 )
    {
        print( "Enter filename: " );
        if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            (void) input_tag_label_file( display, filename );

            print( "Done loading.\n" );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(load_labels )
{
    return( get_n_volumes(display) > 0 );
}

private  void   save_labels_as_tags(
    display_struct  *display,
    display_struct  *slice_window,
    int             desired_label )
{
    Status         status;
    FILE           *file;
    STRING         filename;

    print( "Enter filename to save: " );
    if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
    {
        status = open_file_with_default_suffix( filename,
                         get_default_tag_file_suffix(),
                         WRITE_FILE, ASCII_FORMAT, &file );

        if( status == OK )
            status = output_labels_as_tags( file,
                      get_volume(slice_window),
                      get_label_volume(slice_window),
                      desired_label,
                      display->three_d.default_marker_size,
                      display->three_d.default_marker_patient_id );

        if( status == OK )
            status = close_file( file );

        print( "Done saving.\n" );
    }

    (void) input_newline( stdin );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( save_labels )
{
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        save_labels_as_tags( display, slice_window, -1 );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(save_labels )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( save_current_label )
{
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if( slice_window->slice.current_paint_label > 0 )
        {
            save_labels_as_tags( display, slice_window,
                                 slice_window->slice.current_paint_label );
        }
        else
            print( "You first have to set the current label > 0.\n" );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(save_current_label )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(label_slice)
{
    set_slice_labels( display, get_current_paint_label(display) );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(label_slice )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(clear_slice)
{
    set_slice_labels( display, 0 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(clear_slice )
{
    return( get_n_volumes(display) > 0 );
}

private  void  set_slice_labels(
    display_struct     *display,
    int                label )
{
    Real             voxel[MAX_DIMENSIONS];
    int              view_index, int_voxel[MAX_DIMENSIONS], volume_index;
    int              x_index, y_index, axis_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index,
                               voxel ) &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        record_slice_under_mouse( display, volume_index );

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_labels_on_slice( get_nth_label_volume(slice_window,volume_index),
                             axis_index, int_voxel[axis_index],
                             label );

        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );
    }
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(clear_connected)
{
    set_connected_labels( display, 0, TRUE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(clear_connected )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(label_connected)
{
    set_connected_labels( display, get_current_paint_label(display), TRUE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(label_connected )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(label_connected_no_threshold)
{
    set_connected_labels( display, get_current_paint_label(display), FALSE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(label_connected_no_threshold )
{
    return( get_n_volumes(display) > 0 );
}

private  void   set_connected_labels(
    display_struct   *display,
    int              desired_label,
    BOOLEAN          use_threshold )
{
    Real             voxel[MAX_DIMENSIONS], min_threshold, max_threshold;
    int              view_index, int_voxel[MAX_DIMENSIONS];
    int              label_under_mouse, volume_index;
    int              x_index, y_index, axis_index;
    int              min_label_threshold, max_label_threshold;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index,
                               &view_index, voxel ) &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        record_slice_under_mouse( display, volume_index );

        if( use_threshold )
        {
            min_threshold = slice_window->slice.segmenting.min_threshold;
            max_threshold = slice_window->slice.segmenting.max_threshold;
        }
        else
        {
            min_threshold = 1.0;
            max_threshold = 0.0;
        }

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        label_under_mouse = get_volume_label_data(
                  get_nth_label_volume(slice_window,volume_index), int_voxel );

        min_label_threshold = label_under_mouse;
        max_label_threshold = label_under_mouse;

        set_connected_voxels_labels( get_nth_volume(slice_window,volume_index),
                          get_nth_label_volume(slice_window,volume_index),
                          axis_index, int_voxel,
                          min_threshold, max_threshold,
                          min_label_threshold, max_label_threshold,
                          slice_window->slice.segmenting.connectivity,
                          desired_label );

        set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS);
    }
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(label_connected_3d)
{
    Real             voxel[MAX_DIMENSIONS];
    int              view_index, int_voxel[MAX_DIMENSIONS];
    int              label_under_mouse, desired_label, volume_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index, voxel))
    {
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        label_under_mouse = get_volume_label_data(
                 get_nth_label_volume(slice_window,volume_index), int_voxel );

        desired_label = get_current_paint_label( slice_window );

        print( "Filling 3d from %d %d %d, label %d becomes %d\n",
               int_voxel[X], int_voxel[Y], int_voxel[Z],
               label_under_mouse, desired_label );

        (void) fill_connected_voxels( get_nth_volume(slice_window,volume_index),
                               get_nth_label_volume(slice_window,volume_index),
                               slice_window->slice.segmenting.connectivity,
                               int_voxel,
                               label_under_mouse, label_under_mouse,
                               desired_label,
                               slice_window->slice.segmenting.min_threshold,
                               slice_window->slice.segmenting.max_threshold );

        delete_slice_undo( &slice_window->slice.undo, volume_index );

        print( "Done\n" );

        set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS);
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(label_connected_3d )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(dilate_labels)
{
    int              min_outside_label, max_outside_label;
    Volume           volume;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume) )
    {
        print( "Enter min and max outside label: " );

        if( input_int( stdin, &min_outside_label ) == OK &&
            input_int( stdin, &max_outside_label ) == OK )
        {
            dilate_labeled_voxels_3d( get_volume(display),
                                  get_label_volume(display),
                                  get_current_paint_label(display),
                                  get_current_paint_label(display),
                                  0.0, -1.0,
                                  min_outside_label, max_outside_label,
                                  slice_window->slice.segmenting.min_threshold,
                                  slice_window->slice.segmenting.max_threshold,
                                  get_current_paint_label(display),
                                  slice_window->slice.segmenting.connectivity );

            delete_slice_undo( &slice_window->slice.undo,
                               get_current_volume_index(slice_window) );

            print( "Done\n" );

            set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(dilate_labels )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(erode_labels)
{
    int              min_outside_label, max_outside_label, set_value;
    Volume           volume;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume) )
    {
        print( "Enter min and max outside label: " );

        if( input_int( stdin, &min_outside_label ) == OK &&
            input_int( stdin, &max_outside_label ) == OK )
        {
            if( min_outside_label <= max_outside_label )
                set_value = MAX( min_outside_label, 0 );
            dilate_labeled_voxels_3d( get_volume(display),
                                  get_label_volume(display),
                                  min_outside_label, max_outside_label,
                                  0.0, -1.0,
                                  get_current_paint_label(display),
                                  get_current_paint_label(display),
                                  slice_window->slice.segmenting.min_threshold,
                                  slice_window->slice.segmenting.max_threshold,
                                  set_value,
                                  slice_window->slice.segmenting.connectivity );

            delete_slice_undo( &slice_window->slice.undo,
                               get_current_volume_index(slice_window) );

            print( "Done\n" );

            set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(erode_labels )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(toggle_connectivity)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window) )
    {
        if( slice_window->slice.segmenting.connectivity == FOUR_NEIGHBOURS )
            slice_window->slice.segmenting.connectivity = EIGHT_NEIGHBOURS;
        else
            slice_window->slice.segmenting.connectivity = FOUR_NEIGHBOURS;
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_connectivity )
{
    BOOLEAN          state;
    display_struct   *slice_window;
    Neighbour_types  connectivity;
    int              n_neigh;

    state = get_slice_window( display, &slice_window );

    if( state )
        connectivity = slice_window->slice.segmenting.connectivity;
    else
        connectivity = (Neighbour_types) Segmenting_connectivity;

    switch( connectivity )
    {
    case  FOUR_NEIGHBOURS:
        n_neigh = 4; 
        break;
    case  EIGHT_NEIGHBOURS:
        n_neigh = 8; 
        break;
    }

    set_menu_text_int( menu_window, menu_entry, n_neigh );

    return( state );
}
