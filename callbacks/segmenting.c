
#include  <display.h>

private  void  get_min_max(
    display_struct   *display );
private  void  set_slice_labels(
    display_struct     *display,
    int                label );
private  void   set_connected_labels(
    display_struct   *display,
    int              label );

public  DEF_MENU_FUNCTION( label_voxel )   /* ARGSUSED */
{
    Real           voxel[MAX_DIMENSIONS];
    int            axis_index, int_voxel[MAX_DIMENSIONS];

    if( get_voxel_under_mouse( display, voxel, &axis_index ) )
    {
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_volume_label_data( get_label_volume(display), int_voxel,
                               get_current_paint_label(display) );
        set_slice_window_all_update( display->associated[SLICE_WINDOW] );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(label_voxel )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( clear_voxel )   /* ARGSUSED */
{
    Real           voxel[MAX_DIMENSIONS];
    int            axis_index, int_voxel[MAX_DIMENSIONS];

    if( get_voxel_under_mouse( display, voxel, &axis_index ) )
    {
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_volume_label_data( get_label_volume(display), int_voxel, 0 );
        set_slice_window_all_update( display->associated[SLICE_WINDOW] );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(clear_voxel )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_segmenting )   /* ARGSUSED */
{
    reset_segmentation( display->associated[SLICE_WINDOW] );
    set_slice_window_all_update( display->associated[SLICE_WINDOW] );

    return( OK );
}

public  DEF_MENU_UPDATE(reset_segmenting )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_segmenting_threshold )   /* ARGSUSED */
{
    get_min_max( display );

    return( OK );
}

public  DEF_MENU_UPDATE(set_segmenting_threshold )   /* ARGSUSED */
{
    return( OK );
}

private  void  get_min_max(
    display_struct   *display )
{
    Real      min, max;

    print( "Enter min and max threshold: " );

    if( input_real( stdin, &min ) == OK && input_real( stdin, &max ) == OK )
    {
        display->associated[SLICE_WINDOW]->slice.segmenting.min_threshold = min;
        display->associated[SLICE_WINDOW]->slice.segmenting.max_threshold = max;
    }

    (void) input_newline( stdin );
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
        set_slice_window_all_update( display->associated[SLICE_WINDOW] );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_label_data )   /* ARGSUSED */
{
    return( OK );
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
    return( OK );
}

public  DEF_MENU_FUNCTION(save_active_voxels)   /* ARGSUSED */
{
    FILE             *file;
    Status           status;
    STRING           filename;
    display_struct   *slice_window;

    status = OK;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "act",
                                            WRITE_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_label_bit( file, WRITE_FILE,
                                          get_label_volume(slice_window),
                                          get_active_bit() );

        if( status == OK )
            status = close_file( file );

        print( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(save_active_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(load_active_voxels)   /* ARGSUSED */
{
    FILE             *file;
    Status           status;
    STRING           filename;
    display_struct   *slice_window;

    status = OK;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "act", READ_FILE,
                                                    BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_label_bit( file, READ_FILE,
                                          get_label_volume(slice_window),
                                          get_active_bit() );

        if( status == OK )
            status = close_file( file );

        if( status == OK )
        {
            set_slice_window_all_update( display->associated[SLICE_WINDOW] );
        }

        print( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_active_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_activities)   /* ARGSUSED */
{
    Volume           volume;
    display_struct   *slice_window;

    if( get_slice_window_volume( display, &volume ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        set_all_voxel_activity_flags( get_label_volume(slice_window), TRUE );

        set_slice_window_all_update( slice_window );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_activities )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(label_slice)   /* ARGSUSED */
{
    set_slice_labels( display, get_current_paint_label(display) );

    return( OK );
}

public  DEF_MENU_UPDATE(label_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(clear_slice)   /* ARGSUSED */
{
    set_slice_labels( display, 0 );

    return( OK );
}

public  DEF_MENU_UPDATE(clear_slice )   /* ARGSUSED */
{
    return( OK );
}

private  void  set_slice_labels(
    display_struct     *display,
    int                label )
{
    Real             voxel[MAX_DIMENSIONS];
    int              axis_index, int_voxel[MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &axis_index ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_labels_on_slice( get_label_volume(slice_window),
                             axis_index, int_voxel[axis_index],
                             label );

        set_slice_window_all_update( slice_window );
    }
}

public  DEF_MENU_FUNCTION(clear_connected)   /* ARGSUSED */
{
    set_connected_labels( display, 0 );

    return( OK );
}

public  DEF_MENU_UPDATE(clear_connected )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(label_connected)   /* ARGSUSED */
{
    set_connected_labels( display, get_current_paint_label(display) );

    return( OK );
}

public  DEF_MENU_UPDATE(label_connected )   /* ARGSUSED */
{
    return( OK );
}

private  void   set_connected_labels(
    display_struct   *display,
    int              label )
{
    Real             voxel[MAX_DIMENSIONS];
    int              axis_index, int_voxel[MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &axis_index ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        set_connected_voxels_labels( get_volume(slice_window),
                          get_label_volume(slice_window),
                          axis_index, int_voxel,
                          slice_window->slice.segmenting.min_threshold,
                          slice_window->slice.segmenting.max_threshold,
                          slice_window->slice.segmenting.connectivity,
                          label );

        set_slice_window_all_update( slice_window );
    }
}

public  DEF_MENU_FUNCTION(label_connected_3d)   /* ARGSUSED */
{
    Real             voxel[MAX_DIMENSIONS];
    int              axis_index, int_voxel[MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &axis_index ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        print( "Filling 3d from %d %d %d\n",
               int_voxel[X], int_voxel[Y], int_voxel[Z] );

        fill_connected_voxels_3d( get_volume(slice_window),
                                  get_label_volume(slice_window), int_voxel,
                                  get_current_paint_label(slice_window),
                                  slice_window->slice.segmenting.min_threshold,
                                  slice_window->slice.segmenting.max_threshold);

        print( "Done\n" );

        set_slice_window_all_update( slice_window );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(label_connected_3d )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(expand_labeled_3d)   /* ARGSUSED */
{
    Volume           volume;
    display_struct   *slice_window;

    if( get_slice_window_volume( display, &volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        print( "Expanding 3d labeled voxels\n" );

        expand_labeled_voxels_3d( get_volume(display),
                                  get_label_volume(display),
                                  get_current_paint_label(display),
                                  slice_window->slice.segmenting.min_threshold,
                                  slice_window->slice.segmenting.max_threshold,
                                  N_expansion_voxels );

        print( "Done\n" );

        set_slice_window_all_update( slice_window );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(expand_labeled_3d )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(invert_activity)   /* ARGSUSED */
{
    int              voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    BOOLEAN          activity;
    Volume           volume;
    display_struct   *slice_window;

    if( get_slice_window_volume( display, &volume) )
    {
        print( "Inverting activity\n" );
        get_volume_sizes( volume, sizes );

        for_less( voxel[X], 0, sizes[X] )
        {
            for_less( voxel[Y], 0, sizes[Y] )
            {
                for_less( voxel[Z], 0, sizes[Z] )
                {
                    activity = get_voxel_activity_flag(
                                     get_label_volume(display), voxel );
                    set_voxel_activity_flag( get_label_volume(display),
                                     voxel, !activity );
                }
            }
        }

        print( "Done\n" );

        slice_window = display->associated[SLICE_WINDOW];
        set_slice_window_all_update( slice_window );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(invert_activity )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_3d_segmenting)   /* ARGSUSED */
{
    display_struct   *slice_window;
    int              axis_index, voxel_pos, n_dimensions;
    Real             voxel[MAX_DIMENSIONS];

    if( get_slice_window( display, &slice_window) )
    {
        if( get_voxel_under_mouse( display, voxel, &axis_index ) )
        {
            voxel_pos = ROUND( voxel[axis_index] );
            n_dimensions = 2;
        }
        else
        {
            voxel_pos = -1;
            axis_index = -1;
            n_dimensions = 3;
        }

        restart_segmenting_3d( slice_window, n_dimensions, voxel_pos,
                               axis_index );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_3d_segmenting )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(do_3d_segmenting)   /* ARGSUSED */
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window) )
        one_iteration_segmenting( slice_window );

    return( OK );
}

public  DEF_MENU_UPDATE(do_3d_segmenting )   /* ARGSUSED */
{
    return( OK );
}
