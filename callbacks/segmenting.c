
#include  <def_display.h>

private  void  get_min_max(
    display_struct   *display );
private  Boolean  min_max_present(
    display_struct   *display );
private  void  set_slice_activity(
    display_struct     *display,
    Boolean            activity );
private  void   set_connected_activity(
    display_struct   *display,
    Boolean          desired_activity );

public  DEF_MENU_FUNCTION( label_point )   /* ARGSUSED */
{
    Status   status;
    int      id, axis_index;
    Real     x, y, z;

    status = OK;

    if( get_voxel_under_mouse( display, &x, &y, &z, &axis_index ) )
    {
        print( "Enter id: " );

        status = input_int( stdin, &id );

        (void) input_newline( stdin );

        if( status == OK )
            add_point_label( display->associated[SLICE_WINDOW],
                             ROUND(x), ROUND(y), ROUND(z), id);
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
    Real           x, y, z;
    int            axis_index;

    if( get_voxel_under_mouse( display, &x, &y, &z, &axis_index ) &&
        get_slice_window_volume( display, &volume ) )
    {
        set_voxel_activity_flag( volume, ROUND(x), ROUND(y), ROUND(z), FALSE );
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
    Real           x, y, z;
    int            axis_index;

    if( get_voxel_under_mouse( display, &x, &y, &z, &axis_index ) &&
        get_slice_window_volume( display, &volume ) )
    {
        set_voxel_activity_flag( volume, ROUND(x), ROUND(y), ROUND(z), TRUE );
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
    Real     voxel_index[3];
    int      voxel_axes[3], int_index[N_DIMENSIONS];

    if( !min_max_present(display) )
        get_min_max( display );

    if( min_max_present(display) &&
        get_voxel_under_mouse( display, &voxel_index[X],
                               &voxel_index[Y],
                               &voxel_index[Z], &voxel_axes[2] ) )
    {
        voxel_axes[0] = (voxel_axes[2] + 1) % N_DIMENSIONS;
        voxel_axes[1] = (voxel_axes[2] + 2) % N_DIMENSIONS;

        int_index[X] = ROUND( voxel_index[X] );
        int_index[Y] = ROUND( voxel_index[Y] );
        int_index[Z] = ROUND( voxel_index[Z] );
        generate_segmentation( display->associated[SLICE_WINDOW],
                               int_index, voxel_axes );

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

private  Boolean  min_max_present(
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
    Volume             volume;
    String             filename;

    status = OK;

    if( get_slice_window_volume( display, &volume ) )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "lbl",
                                          WRITE_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_auxiliary_bit( file, WRITE_FILE, volume,
                                              LABEL_BIT );

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
    Volume           volume;
    String           filename;

    status = OK;

    if( get_slice_window_volume( display, &volume ) )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "lbl",
                                             READ_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_auxiliary_bit( file, READ_FILE, volume,
                                              LABEL_BIT );

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
    Volume           volume;
    String           filename;

    status = OK;

    if( get_slice_window_volume( display, &volume ) )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "act",
                                            WRITE_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_auxiliary_bit( file, WRITE_FILE, volume,
                                              ACTIVE_BIT );

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
    Volume           volume;
    String           filename;

    status = OK;

    if( get_slice_window_volume( display, &volume ) )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "act", READ_FILE,
                                                    BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_auxiliary_bit( file, READ_FILE, volume,
                                              ACTIVE_BIT );

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

        set_all_voxel_activity_flags( volume, TRUE );

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
    Boolean            activity )
{
    Volume           volume;
    Real             slice_index[N_DIMENSIONS];
    int              axis_index;
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, &slice_index[0], &slice_index[1],
                               &slice_index[2], &axis_index ) &&
        get_slice_window_volume( display, &volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        set_activity_for_slice( volume, axis_index,
                                ROUND(slice_index[axis_index]), activity );

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
    Boolean          desired_activity )
{
    Volume           volume;
    Real             slice_index[N_DIMENSIONS];
    int              axis_index, int_index[N_DIMENSIONS];
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, &slice_index[0], &slice_index[1],
                               &slice_index[2], &axis_index ) &&
        get_slice_window_volume( display, &volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        int_index[X] = ROUND( slice_index[X] );
        int_index[Y] = ROUND( slice_index[Y] );
        int_index[Z] = ROUND( slice_index[Z] );
        set_connected_voxels_activity( volume, axis_index, int_index,
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
    Real             x, y, z;
    int              axis_index;
    display_struct   *slice_window;

    if( get_voxel_under_mouse( display, &x, &y, &z, &axis_index ) &&
        get_slice_window_volume( display, &volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        print( "Filling 3d from %d %d %d\n", ROUND(x), ROUND(y), ROUND(z) );

        fill_connected_voxels_3d( volume, ROUND(x), ROUND(y), ROUND(z),
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
    int              x, y, z, sizes[N_DIMENSIONS];
    Volume           volume;
    Boolean          activity;
    display_struct   *slice_window;

    if( get_slice_window_volume( display, &volume) )
    {
        print( "Inverting activity\n" );
        get_volume_sizes( volume, sizes );

        for_less( x, 0, sizes[X] )
        {
            for_less( y, 0, sizes[Y] )
            {
                for_less( z, 0, sizes[Z] )
                {
                    activity = get_voxel_activity_flag( volume, x, y, z );
                    set_voxel_activity_flag( volume, x, y, z, !activity );
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
