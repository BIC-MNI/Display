
#include  <display.h>

private  void  set_slice_labels(
    display_struct     *display,
    int                label );
private  void   set_connected_labels(
    display_struct   *display,
    int              label,
    BOOLEAN          use_threshold );

public  DEF_MENU_FUNCTION( label_voxel )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(label_voxel )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( clear_voxel )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(clear_voxel )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( reset_segmenting )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(reset_segmenting )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( set_segmenting_threshold )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(set_segmenting_threshold )   /* ARGSUSED */
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

public  DEF_MENU_FUNCTION(load_label_data)   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(load_label_data )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION(save_label_data)   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(save_label_data )   /* ARGSUSED */
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

public  DEF_MENU_FUNCTION( load_labels )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(load_labels )   /* ARGSUSED */
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

public  DEF_MENU_FUNCTION( save_labels )   /* ARGSUSED */
{
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        save_labels_as_tags( display, slice_window, -1 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(save_labels )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( save_current_label )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(save_current_label )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION(label_slice)   /* ARGSUSED */
{
    set_slice_labels( display, get_current_paint_label(display) );

    return( OK );
}

public  DEF_MENU_UPDATE(label_slice )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION(clear_slice)   /* ARGSUSED */
{
    set_slice_labels( display, 0 );

    return( OK );
}

public  DEF_MENU_UPDATE(clear_slice )   /* ARGSUSED */
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

public  DEF_MENU_FUNCTION(clear_connected)   /* ARGSUSED */
{
    set_connected_labels( display, 0, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(clear_connected )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION(label_connected)   /* ARGSUSED */
{
    set_connected_labels( display, get_current_paint_label(display), TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(label_connected )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION(label_connected_no_threshold)   /* ARGSUSED */
{
    set_connected_labels( display, get_current_paint_label(display), FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(label_connected_no_threshold )   /* ARGSUSED */
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

public  DEF_MENU_FUNCTION(label_connected_3d)   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(label_connected_3d )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION(dilate_labels)   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(dilate_labels )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION(erode_labels)   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(erode_labels )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION(toggle_connectivity)   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(toggle_connectivity )   /* ARGSUSED */
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
