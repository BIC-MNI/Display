
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_files.h>

static    void     get_min_max();
static    Boolean  min_max_present();

public  DEF_MENU_FUNCTION( label_point )   /* ARGSUSED */
{
    Status   status;
    int      id, x, y, z, axis_index;
    Status   add_point_label();

    status = OK;

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) )
    {
        PRINT( "Enter id: " );

        status = input_int( stdin, &id );

        (void) input_newline( stdin );

        if( status == OK )
            status = add_point_label( graphics->associated[SLICE_WINDOW],
                                      x, y, z, id);
    }

    return( status );
}

public  DEF_MENU_UPDATE(label_point )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_voxel_inactive )   /* ARGSUSED */
{
    Status         status;
    volume_struct  *volume;
    int            x, y, z, axis_index;
    void           set_voxel_activity_flag();
    void           set_slice_window_update();

    status = OK;

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) &&
        get_slice_window_volume( graphics, &volume ) )
    {
        set_voxel_activity_flag( volume, x, y, z, FALSE );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_voxel_inactive )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_voxel_active )   /* ARGSUSED */
{
    Status         status;
    volume_struct  *volume;
    int            x, y, z, axis_index;
    void           set_voxel_activity_flag();
    void           set_slice_window_update();

    status = OK;

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) &&
        get_slice_window_volume( graphics, &volume ) )
    {
        set_voxel_activity_flag( volume, x, y, z, TRUE );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_voxel_active )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( generate_regions )   /* ARGSUSED */
{
    Status   status;
    int      voxel_index[3], voxel_axes[3];
    Status   generate_segmentation();
    void     set_slice_window_update();

    status = OK;

    if( !min_max_present(graphics) )
        get_min_max( graphics );

    if( min_max_present(graphics) &&
        get_voxel_under_mouse( graphics, &voxel_index[X],
                               &voxel_index[Y],
                               &voxel_index[Z], &voxel_axes[2] ) )
    {
        voxel_axes[0] = (voxel_axes[2] + 1) % N_DIMENSIONS;
        voxel_axes[1] = (voxel_axes[2] + 2) % N_DIMENSIONS;

        status = generate_segmentation( graphics->associated[SLICE_WINDOW],
                                        voxel_index, voxel_axes );

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }

    return( status );
}

public  DEF_MENU_UPDATE(generate_regions )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_segmenting )   /* ARGSUSED */
{
    Status   status;
    void     set_slice_window_update();
    Status   reset_segmentation();

    status = reset_segmentation( graphics->associated[SLICE_WINDOW] );

    set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
    set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
    set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );

    return( status );
}

public  DEF_MENU_UPDATE(reset_segmenting )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_segmenting_threshold )   /* ARGSUSED */
{
    get_min_max( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(set_segmenting_threshold )   /* ARGSUSED */
{
    return( OK );
}

private  void  get_min_max( graphics )
    graphics_struct  *graphics;
{
    int      min, max;

    PRINT( "Enter min and max threshold: " );

    if( input_int( stdin, &min ) != OK || input_int( stdin, &max ) != OK )
    {
        min = -1;
        max = -1;
    }

    graphics->associated[SLICE_WINDOW]->slice.segmenting.min_threshold = min;
    graphics->associated[SLICE_WINDOW]->slice.segmenting.max_threshold = max;
}

private  Boolean  min_max_present( graphics )
    graphics_struct  *graphics;
{
    return( 
     graphics->associated[SLICE_WINDOW]->slice.segmenting.min_threshold >= 0 &&
     graphics->associated[SLICE_WINDOW]->slice.segmenting.max_threshold >= 0 &&
     graphics->associated[SLICE_WINDOW]->slice.segmenting.min_threshold <=
     graphics->associated[SLICE_WINDOW]->slice.segmenting.max_threshold );
}

public  DEF_MENU_FUNCTION(save_labeled_voxels)   /* ARGSUSED */
{
    FILE               *file;
    Status             status;
    Status             io_volume_auxiliary_bit();
    volume_struct      *volume;
    String             filename;

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        PRINT( "Enter filename: " );

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

        PRINT( "Done\n" );
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
    Status           io_volume_auxiliary_bit();
    volume_struct    *volume;
    String           filename;
    void             set_slice_window_update();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        PRINT( "Enter filename: " );

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
            set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
            set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
            set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
        }

        PRINT( "Done\n" );
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
    Status           io_volume_auxiliary_bit();
    volume_struct    *volume;
    String           filename;

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        PRINT( "Enter filename: " );

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

        PRINT( "Done\n" );
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
    Status           io_volume_auxiliary_bit();
    volume_struct    *volume;
    String           filename;
    void             set_slice_window_update();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        PRINT( "Enter filename: " );

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
            set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
            set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
            set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
        }

        PRINT( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_active_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_activities)   /* ARGSUSED */
{
    volume_struct    *volume;
    void             set_all_voxel_activity_flags();
    void             set_slice_window_update();
    graphics_struct  *slice_window;
    void             set_update_required();

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

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
    void             set_slice_activity();

    set_slice_activity( graphics, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_slice_active )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_slice_inactive)   /* ARGSUSED */
{
    void             set_slice_activity();

    set_slice_activity( graphics, FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_slice_inactive )   /* ARGSUSED */
{
    return( OK );
}

private  void  set_slice_activity( graphics, activity )
    graphics_struct    *graphics;
    Boolean            activity;
{
    volume_struct    *volume;
    int              slice_index[3], axis_index;
    void             set_slice_window_update();
    void             set_activity_for_slice();
    void             reset_slice_activity();
    graphics_struct  *slice_window;
    void             set_update_required();

    if( get_voxel_under_mouse( graphics, &slice_index[0], &slice_index[1],
                               &slice_index[2], &axis_index ) &&
        get_slice_window_volume( graphics, &volume) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        set_activity_for_slice( volume, axis_index, slice_index[axis_index],
                                activity );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }
}

public  DEF_MENU_FUNCTION(set_connected_inactive)   /* ARGSUSED */
{
    void             set_connected_activity();

    set_connected_activity( graphics, FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_connected_inactive )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_connected_active)   /* ARGSUSED */
{
    void             set_connected_activity();

    set_connected_activity( graphics, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_connected_active )   /* ARGSUSED */
{
    return( OK );
}

private  void   set_connected_activity( graphics, desired_activity )
    graphics_struct  *graphics;
    Boolean          desired_activity;
{
    volume_struct    *volume;
    int              slice_index[3], axis_index;
    void             set_connected_voxels_activity();
    void             set_slice_window_update();
    graphics_struct  *slice_window;
    void             set_update_required();

    if( get_voxel_under_mouse( graphics, &slice_index[0], &slice_index[1],
                               &slice_index[2], &axis_index ) &&
        get_slice_window_volume( graphics, &volume) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        set_connected_voxels_activity( volume, axis_index, slice_index,
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
    Status           status;
    volume_struct    *volume;
    int              x, y, z, axis_index;
    Status           fill_connected_voxels_3d();
    void             set_slice_window_update();
    graphics_struct  *slice_window;
    void             set_update_required();

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) &&
        get_slice_window_volume( graphics, &volume) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        PRINT( "Filling 3d from %d %d %d\n", x, y, z );

        status = fill_connected_voxels_3d( volume, x, y, z,
                          slice_window->slice.segmenting.min_threshold,
                          slice_window->slice.segmenting.max_threshold );

        PRINT( "Done\n" );

        if( status == OK )
        {
            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(label_connected_3d )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(expand_labeled_3d)   /* ARGSUSED */
{
    volume_struct    *volume;
    void             expand_labeled_voxels_3d();
    void             set_slice_window_update();
    graphics_struct  *slice_window;
    void             set_update_required();

    if( get_slice_window_volume( graphics, &volume) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        PRINT( "Expanding 3d labeled voxels\n" );

        expand_labeled_voxels_3d( volume,
                 slice_window->slice.segmenting.min_threshold,
                 slice_window->slice.segmenting.max_threshold,
                 N_expansion_voxels );

        PRINT( "Done\n" );

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
    int              x, y, z;
    volume_struct    *volume;
    Boolean          activity;
    void             set_slice_window_update();
    graphics_struct  *slice_window;
    void             set_voxel_activity_flag();
    void             set_update_required();

    if( get_slice_window_volume( graphics, &volume) )
    {
        PRINT( "Inverting activity\n" );

        for_less( x, 0, volume->sizes[X] )
        {
            for_less( y, 0, volume->sizes[Y] )
            {
                for_less( z, 0, volume->sizes[Z] )
                {
                    activity = get_voxel_activity_flag( volume, x, y, z );
                    set_voxel_activity_flag( volume, x, y, z, !activity );
                }
            }
        }

        PRINT( "Done\n" );

        slice_window = graphics->associated[SLICE_WINDOW];
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
