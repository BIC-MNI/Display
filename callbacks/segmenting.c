
#include  <display.h>

private  void  get_min_max(
    display_struct   *display );
private  BOOLEAN  min_max_present(
    display_struct   *display );
private  void  set_slice_activity(
    display_struct     *display,
    BOOLEAN            activity );
private  void   set_connected_activity(
    display_struct   *display,
    BOOLEAN          desired_activity );

public  DEF_MENU_FUNCTION( label_point )   /* ARGSUSED */
{
    Status   status;
    int      id, axis_index, int_voxel[MAX_DIMENSIONS];
    Real     voxel[MAX_DIMENSIONS];

    status = OK;

    if( get_voxel_under_mouse( display, voxel, &axis_index ) )
    {
        print( "Enter id: " );

        status = input_int( stdin, &id );
        (void) input_newline( stdin );

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        if( status == OK )
            add_point_label( display->associated[SLICE_WINDOW], int_voxel, id );
    }

    return( status );
}

public  DEF_MENU_UPDATE(label_point )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_voxel_inactive )   /* ARGSUSED */
{
    Volume         volume;
    Real           voxel[MAX_DIMENSIONS];
    int            axis_index, int_voxel[MAX_DIMENSIONS];

    if( get_voxel_under_mouse( display, voxel, &axis_index ) &&
        get_slice_window_volume( display, &volume ) )
    {
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_voxel_activity_flag( volume, int_voxel, FALSE );
        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_voxel_inactive )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_voxel_active )   /* ARGSUSED */
{
    Volume         volume;
    Real           voxel[MAX_DIMENSIONS];
    int            axis_index, int_voxel[MAX_DIMENSIONS];

    if( get_voxel_under_mouse( display, voxel, &axis_index ) &&
        get_slice_window_volume( display, &volume ) )
    {
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_voxel_activity_flag( volume, int_voxel, TRUE );
        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_voxel_active )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( generate_regions )   /* ARGSUSED */
{
    Real     voxel[3];
    int      voxel_axes[3], int_voxel[N_DIMENSIONS];

    if( !min_max_present(display) )
        get_min_max( display );

    if( min_max_present(display) &&
        get_voxel_under_mouse( display, voxel, &voxel_axes[2] ) )
    {
        voxel_axes[0] = (voxel_axes[2] + 1) % N_DIMENSIONS;
        voxel_axes[1] = (voxel_axes[2] + 2) % N_DIMENSIONS;

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        generate_segmentation( display->associated[SLICE_WINDOW],
                               int_voxel, voxel_axes );

        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(generate_regions )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_segmenting )   /* ARGSUSED */
{
    reset_segmentation( display->associated[SLICE_WINDOW] );

    set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
    set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
    set_slice_window_update( display->associated[SLICE_WINDOW], 2 );

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
    int      min, max;

    print( "Enter min and max threshold: " );

    if( input_int( stdin, &min ) != OK || input_int( stdin, &max ) != OK )
    {
        min = -1;
        max = -1;
    }

    (void) input_newline( stdin );

    display->associated[SLICE_WINDOW]->slice.segmenting.min_threshold = min;
    display->associated[SLICE_WINDOW]->slice.segmenting.max_threshold = max;
}

private  BOOLEAN  min_max_present(
    display_struct   *display )
{
    return( 
     display->associated[SLICE_WINDOW]->slice.segmenting.min_threshold >= 0 &&
     display->associated[SLICE_WINDOW]->slice.segmenting.max_threshold >= 0 &&
     display->associated[SLICE_WINDOW]->slice.segmenting.min_threshold <=
     display->associated[SLICE_WINDOW]->slice.segmenting.max_threshold );
}

public  DEF_MENU_FUNCTION(save_labeled_voxels)   /* ARGSUSED */
{
    FILE               *file;
    Status             status;
    STRING             filename;
    display_struct     *slice_window;

    status = OK;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "lbl",
                                          WRITE_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_label_bit( file, WRITE_FILE,
                                          get_label_volume(slice_window),
                                          get_label_bit() );

        if( status == OK )
            status = close_file( file );

        print( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(save_labeled_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(load_labeled_voxels)   /* ARGSUSED */
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
            status = open_file_with_default_suffix( filename, "lbl",
                                             READ_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_label_bit( file, READ_FILE,
                                          get_label_volume(slice_window),
                                          get_label_bit() );

        if( status == OK )
            status = close_file( file );

        if( status == OK )
        {
            set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
        }

        print( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_labeled_voxels )   /* ARGSUSED */
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
            set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
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

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_activities )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_slice_active)   /* ARGSUSED */
{
    set_slice_activity( display, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_slice_active )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_slice_inactive)   /* ARGSUSED */
{
    set_slice_activity( display, FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_slice_inactive )   /* ARGSUSED */
{
    return( OK );
}

private  void  set_slice_activity(
    display_struct     *display,
    BOOLEAN            activity )
{
    Volume           volume;
    Real             voxel[MAX_DIMENSIONS];
    int              axis_index, int_voxel[MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &axis_index ) &&
        get_slice_window_volume( display, &volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        set_activity_for_slice( volume, axis_index, int_voxel[axis_index],
                                activity );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }
}

public  DEF_MENU_FUNCTION(set_connected_inactive)   /* ARGSUSED */
{
    set_connected_activity( display, FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_connected_inactive )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_connected_active)   /* ARGSUSED */
{
    set_connected_activity( display, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_connected_active )   /* ARGSUSED */
{
    return( OK );
}

private  void   set_connected_activity(
    display_struct   *display,
    BOOLEAN          desired_activity )
{
    Volume           volume;
    Real             voxel[MAX_DIMENSIONS];
    int              axis_index, int_voxel[MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &axis_index ) &&
        get_slice_window_volume( display, &volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        set_connected_voxels_activity( volume, axis_index, int_voxel,
                          slice_window->slice.segmenting.min_threshold,
                          slice_window->slice.segmenting.max_threshold,
                          slice_window->slice.segmenting.connectivity,
                          desired_activity );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }
}

public  DEF_MENU_FUNCTION(label_connected_3d)   /* ARGSUSED */
{
    Volume           volume;
    Real             voxel[MAX_DIMENSIONS];
    int              axis_index, int_voxel[MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, voxel, &axis_index ) &&
        get_slice_window_volume( display, &volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );

        print( "Filling 3d from %d %d %d\n",
               int_voxel[X], int_voxel[Y], int_voxel[Z] );

        fill_connected_voxels_3d( volume, int_voxel,
                                  slice_window->slice.segmenting.min_threshold,
                                  slice_window->slice.segmenting.max_threshold);

        print( "Done\n" );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
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

        expand_labeled_voxels_3d( volume,
                 slice_window->slice.segmenting.min_threshold,
                 slice_window->slice.segmenting.max_threshold,
                 N_expansion_voxels );

        print( "Done\n" );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
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
    Volume           volume;
    BOOLEAN          activity;
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
                    activity = get_voxel_activity_flag( volume, voxel );
                    set_voxel_activity_flag( volume, voxel, !activity );
                }
            }
        }

        print( "Done\n" );

        slice_window = display->associated[SLICE_WINDOW];
        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(invert_activity )   /* ARGSUSED */
{
    return( OK );
}
