
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
    int            view_index, int_voxel[MAX_DIMENSIONS];

    if( get_voxel_under_mouse( display, voxel, &view_index ) )
    {
        record_slice_under_mouse( display );
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_volume_label_data( get_label_volume(display), int_voxel,
                               get_current_paint_label(display) );
        set_slice_window_all_update( display->associated[SLICE_WINDOW],
                                     UPDATE_LABELS );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(label_voxel )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION( clear_voxel )   /* ARGSUSED */
{
    Real           voxel[MAX_DIMENSIONS];
    int            view_index, int_voxel[MAX_DIMENSIONS];

    if( get_voxel_under_mouse( display, voxel, &view_index ) )
    {
        record_slice_under_mouse( display );
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_volume_label_data( get_label_volume(display), int_voxel, 0 );
        set_slice_window_all_update( display->associated[SLICE_WINDOW],
                                     UPDATE_LABELS );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(clear_voxel )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION( reset_segmenting )   /* ARGSUSED */
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        clear_all_labels( slice_window );
        delete_slice_undo( &slice_window->slice.undo );
        set_slice_window_all_update( slice_window, UPDATE_LABELS );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_segmenting )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
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

public  DEF_MENU_FUNCTION(load_label_data)   /* ARGSUSED */
{
    Status           status;
    STRING           filename;
    display_struct   *slice_window;

    status = OK;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename to load: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = load_label_volume( filename,
                                        get_label_volume(slice_window) );

        print( "Done\n" );
        delete_slice_undo( &display->associated[SLICE_WINDOW]->slice.undo );
        set_slice_window_all_update( display->associated[SLICE_WINDOW],
                                     UPDATE_LABELS );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_label_data )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(save_label_data)   /* ARGSUSED */
{
    Status           status;
    STRING           filename;
    display_struct   *slice_window;

    status = OK;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename to save: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = save_label_volume( filename,
                                        get_label_volume(slice_window) );

        print( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(save_label_data )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(label_slice)   /* ARGSUSED */
{
    set_slice_labels( display, get_current_paint_label(display) );

    return( OK );
}

public  DEF_MENU_UPDATE(label_slice )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(clear_slice)   /* ARGSUSED */
{
    set_slice_labels( display, 0 );

    return( OK );
}

public  DEF_MENU_UPDATE(clear_slice )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

private  void  set_slice_labels(
    display_struct     *display,
    int                label )
{
    Real             voxel[MAX_DIMENSIONS];
    int              view_index, int_voxel[MAX_DIMENSIONS];
    int              x_index, y_index, axis_index;
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &view_index ) &&
        get_slice_window( display, &slice_window ) &&
        slice_has_ortho_axes( slice_window, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        record_slice_under_mouse( display );

        slice_window = display->associated[SLICE_WINDOW];

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_labels_on_slice( get_label_volume(slice_window),
                             axis_index, int_voxel[axis_index],
                             label );

        set_slice_window_all_update( slice_window, UPDATE_LABELS );
    }
}

public  DEF_MENU_FUNCTION(clear_connected)   /* ARGSUSED */
{
    set_connected_labels( display, 0, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(clear_connected )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(label_connected)   /* ARGSUSED */
{
    set_connected_labels( display, get_current_paint_label(display), TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(label_connected )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(label_connected_no_threshold)   /* ARGSUSED */
{
    set_connected_labels( display, get_current_paint_label(display), FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(label_connected_no_threshold )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

private  void   set_connected_labels(
    display_struct   *display,
    int              desired_label,
    BOOLEAN          use_threshold )
{
    Real             voxel[MAX_DIMENSIONS], min_threshold, max_threshold;
    int              view_index, int_voxel[MAX_DIMENSIONS];
    int              label_under_mouse;
    int              x_index, y_index, axis_index;
    int              min_label_threshold, max_label_threshold;
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &view_index ) &&
        get_slice_window( display, &slice_window ) &&
        slice_has_ortho_axes( slice_window, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        record_slice_under_mouse( display );
        slice_window = display->associated[SLICE_WINDOW];

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
                                get_label_volume(slice_window), int_voxel );

        min_label_threshold = label_under_mouse;
        max_label_threshold = label_under_mouse;

        set_connected_voxels_labels( get_volume(slice_window),
                          get_label_volume(slice_window),
                          axis_index, int_voxel,
                          min_threshold, max_threshold,
                          min_label_threshold, max_label_threshold,
                          slice_window->slice.segmenting.connectivity,
                          desired_label );

        set_slice_window_all_update( slice_window, UPDATE_LABELS );
    }
}

public  DEF_MENU_FUNCTION(label_connected_3d)   /* ARGSUSED */
{
    Real             voxel[MAX_DIMENSIONS];
    int              view_index, int_voxel[MAX_DIMENSIONS];
    int              label_under_mouse, desired_label;
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &view_index ) &&
        get_slice_window( display, &slice_window ) )
    {
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        label_under_mouse = get_volume_label_data(
                                get_label_volume(slice_window), int_voxel );
        desired_label = get_current_paint_label( slice_window );

        print( "Filling 3d from %d %d %d, label %d becomes %d\n",
               int_voxel[X], int_voxel[Y], int_voxel[Z],
               label_under_mouse, desired_label );

        (void) fill_connected_voxels( get_volume(slice_window),
                               get_label_volume(slice_window),
                               slice_window->slice.segmenting.connectivity,
                               int_voxel,
                               label_under_mouse, label_under_mouse,
                               desired_label,
                               slice_window->slice.segmenting.min_threshold,
                               slice_window->slice.segmenting.max_threshold );

        delete_slice_undo( &slice_window->slice.undo );

        print( "Done\n" );

        set_slice_window_all_update( slice_window, UPDATE_LABELS );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(label_connected_3d )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(expand_labeled_3d)   /* ARGSUSED */
{
    int              orig_label;
    Volume           volume;
    display_struct   *slice_window;

    if( get_slice_window_volume( display, &volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        print( "Enter label to change from: " );

        if( input_int( stdin, &orig_label ) == OK )
        {
            expand_labeled_voxels_3d( get_volume(display),
                                  get_label_volume(display),
                                  orig_label,
                                  get_current_paint_label(display),
                                  slice_window->slice.segmenting.min_threshold,
                                  slice_window->slice.segmenting.max_threshold,
                                  slice_window->slice.segmenting.connectivity,
                                  N_expansion_voxels );

            delete_slice_undo( &slice_window->slice.undo );

            print( "Done\n" );

            set_slice_window_all_update( slice_window, UPDATE_LABELS );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(expand_labeled_3d )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
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
