
#include  <def_graphics.h>
#include  <def_math.h>
#include  <def_stdio.h>

public  Boolean  get_current_volume( graphics, volume )
    graphics_struct   *graphics;
    volume_struct     **volume;
{
    Status          status;
    Boolean         found;
    object_struct   *current_object, *object;
    Boolean         get_current_object();
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();

    found = get_slice_window_volume( graphics, volume );

    if( !found )
    {
        if( get_current_object( graphics, &current_object ) )
        {
            status = initialize_object_traverse( &object_traverse,
                                                 1, &current_object );

            if( status == OK )
            {
                while( get_next_object_traverse(&object_traverse, &object) )
                {
                    if( !found && object->object_type == VOLUME )
                    {
                        found = TRUE;
                        *volume = object->ptr.volume;
                    }
                }
            }
        }
    }

    return( found );
}

public  DEF_MENU_FUNCTION( advance_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            rebuild_selected_list();
    void            graphics_models_have_changed();
    int             nx, ny, nz;
    void            get_volume_size();

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        ++volume->current_slice;
        get_volume_size( volume, &nx, &ny, &nz );
        if( volume->current_slice >= nz )
        {
            volume->current_slice = 0;
        }
        volume->slice_visibilities[volume->current_slice] = ON;

        rebuild_selected_list( graphics, menu_window );
    }

    graphics_models_have_changed( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(advance_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            rebuild_selected_list();
    void            graphics_models_have_changed();
    int             nx, ny, nz;
    void            get_volume_size();

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        --volume->current_slice;
        if( volume->current_slice < 0 )
        {
            get_volume_size( volume, &nx, &ny, &nz );
            volume->current_slice = nz-1;
        }
        volume->slice_visibilities[volume->current_slice] = ON;

        rebuild_selected_list( graphics, menu_window );
    }

    graphics_models_have_changed( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( next_marked_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            rebuild_selected_list();
    void            graphics_models_have_changed();
    int             i, nx, ny, nz;
    void            get_volume_size();

    if( get_current_volume(graphics,&volume) )
    {
        get_volume_size( volume, &nx, &ny, &nz );

        i = volume->current_slice;

        do
        {
            ++i;
            if( i >= nz )
                i = 0;
        } while( i != volume->current_slice && !volume->slice_marked_flags[i] );

        if( i != volume->current_slice )
        {
            volume->slice_visibilities[volume->current_slice] = OFF;

            volume->current_slice = i;
            volume->slice_visibilities[volume->current_slice] = ON;
        }

        rebuild_selected_list( graphics, menu_window );
    }

    graphics_models_have_changed( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(next_marked_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( prev_marked_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            rebuild_selected_list();
    void            graphics_models_have_changed();
    int             i, nx, ny, nz;
    void            get_volume_size();

    if( get_current_volume(graphics,&volume) )
    {
        get_volume_size( volume, &nx, &ny, &nz );

        i = volume->current_slice;

        do
        {
            --i;
            if( i < 0 )
                i = nz - 1;
        } while( i != volume->current_slice && !volume->slice_marked_flags[i] );

        if( i != volume->current_slice )
        {
            volume->slice_visibilities[volume->current_slice] = OFF;

            volume->current_slice = i;
            volume->slice_visibilities[volume->current_slice] = ON;
        }

        rebuild_selected_list( graphics, menu_window );
    }

    graphics_models_have_changed( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(prev_marked_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_marked_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            graphics_models_have_changed();

    if( get_current_volume(graphics,&volume) )
    {
        if( volume->slice_marked_flags[volume->current_slice] )
        {
            volume->slice_visibilities[volume->current_slice] = OFF;
            volume->slice_marked_flags[volume->current_slice] = OFF;
        }
        else
        {
            volume->slice_visibilities[volume->current_slice] = ON;
            volume->slice_marked_flags[volume->current_slice] = ON;
        }
    }

    graphics_models_have_changed( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_marked_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(open_slice_window )   /* ARGSUSED */
{
    Status           status;
    Status           create_graphics_window();
    volume_struct    *volume;
    graphics_struct  *slice_window;
    Status           set_slice_window_volume();
    void             rebuild_slice_models();
    void             set_update_required();

    status = OK;

    if( get_current_volume( graphics, &volume ) &&
        graphics->associated[SLICE_WINDOW] == (graphics_struct *) 0 )
    {
        status = create_graphics_window( SLICE_WINDOW, &slice_window,
                                         "Slice Window", 0, 0 );

        if( status == OK )
        {
            slice_window->associated[THREE_D_WINDOW] = graphics;
            slice_window->associated[MENU_WINDOW] = menu_window;
            slice_window->associated[SLICE_WINDOW] = slice_window;
            graphics->associated[SLICE_WINDOW] = slice_window;
            menu_window->associated[SLICE_WINDOW] = slice_window;

            status = set_slice_window_volume( slice_window, volume );

            rebuild_slice_models( slice_window );

            set_update_required( slice_window, NORMAL_PLANES );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(open_slice_window )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(double_slice_voxels)   /* ARGSUSED */
{
    Status           status;
    volume_struct    *volume;
    graphics_struct  *slice_window;
    Point            *mouse;
    int              x, y, view_index;
    void             get_mouse_in_pixels();
    Boolean          find_slice_view_mouse_is_in();
    void             set_slice_window_update();
    void             set_update_required();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        mouse = &slice_window->mouse_position;

        get_mouse_in_pixels( slice_window, mouse, &x, &y );

        if( find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
        {
            slice_window->slice.slice_views[view_index].x_scale *= 2.0;
            slice_window->slice.slice_views[view_index].y_scale *= 2.0;
            set_slice_window_update( slice_window, view_index );
            set_update_required( slice_window, NORMAL_PLANES );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(double_slice_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(halve_slice_voxels)   /* ARGSUSED */
{
    Status           status;
    volume_struct    *volume;
    graphics_struct  *slice_window;
    Point            *mouse;
    int              x, y, view_index;
    void             get_mouse_in_pixels();
    Boolean          find_slice_view_mouse_is_in();
    void             set_slice_window_update();
    void             set_update_required();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        mouse = &slice_window->mouse_position;

        get_mouse_in_pixels( slice_window, mouse, &x, &y );

        if( find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
        {
            slice_window->slice.slice_views[view_index].x_scale *= 0.5;
            slice_window->slice.slice_views[view_index].y_scale *= 0.5;
            set_slice_window_update( slice_window, view_index );
            set_update_required( slice_window, NORMAL_PLANES );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(halve_slice_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(save_labeled_voxels)   /* ARGSUSED */
{
    FILE             *file;
    Status           status;
    Status           open_file();
    Status           close_file();
    Status           io_bitlist_3d();
    volume_struct    *volume;
    String           filename;

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        (void) printf( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_file( filename, WRITE_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_bitlist_3d( file, WRITE_FILE, &volume->label_flags );

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
    Status           open_file();
    Status           io_bitlist_3d();
    Status           close_file();
    volume_struct    *volume;
    String           filename;
    void             rebuild_slice_models();
    void             set_update_required();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        (void) printf( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_file( filename, READ_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_bitlist_3d( file, READ_FILE, &volume->label_flags );

        if( status == OK )
            status = close_file( file );

        if( status == OK )
        {
            rebuild_slice_models( graphics->associated[SLICE_WINDOW] );

            set_update_required( graphics->associated[SLICE_WINDOW],
                                 NORMAL_PLANES );
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
    Status           open_file();
    Status           close_file();
    Status           io_bitlist_3d();
    volume_struct    *volume;
    String           filename;

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        (void) printf( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_file( filename, WRITE_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_bitlist_3d( file, WRITE_FILE, &volume->active_flags );

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
    Status           open_file();
    Status           io_bitlist_3d();
    Status           close_file();
    volume_struct    *volume;
    String           filename;
    void             rebuild_slice_models();
    void             set_update_required();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        (void) printf( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_file( filename, READ_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_bitlist_3d( file, READ_FILE, &volume->active_flags );

        if( status == OK )
            status = close_file( file );

        if( status == OK )
        {
            rebuild_slice_models( graphics->associated[SLICE_WINDOW] );

            set_update_required( graphics->associated[SLICE_WINDOW],
                                 NORMAL_PLANES );
        }

        PRINT( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_active_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_colour_limits )   /* ARGSUSED */
{
    volume_struct    *volume;
    Real             min_value, max_value;
    graphics_struct  *slice_window;
    void             rebuild_slice_models();
    void             change_colour_coding_range();
    void             set_update_required();

    if( get_current_volume(graphics,&volume) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        PRINT( "Current limits:\t%g\t%g\n",
               slice_window->slice.colour_coding.min_value,
               slice_window->slice.colour_coding.max_value );

        PRINT( "Enter new values: " );

        if( scanf( "%f %f", &min_value, &max_value ) == 2 &&
            min_value <= max_value )
        {
            change_colour_coding_range( slice_window, min_value, max_value );

            PRINT( "    New limits:\t%g\t%g\n",
                   slice_window->slice.colour_coding.min_value,
                   slice_window->slice.colour_coding.max_value );

            rebuild_slice_models( slice_window );

            set_update_required( slice_window, NORMAL_PLANES );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_colour_limits )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_activities)   /* ARGSUSED */
{
    volume_struct    *volume;
    void             set_all_voxel_activity_flags();
    void             rebuild_slice_models();
    graphics_struct  *slice_window;
    void             set_update_required();

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        set_all_voxel_activity_flags( volume, TRUE );

        rebuild_slice_models( slice_window );

        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_activities )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(toggle_lock_slice)   /* ARGSUSED */
{
    Status           status;
    volume_struct    *volume;
    graphics_struct  *slice_window;
    Point            *mouse;
    int              x, y, axis_index, view_index;
    void             get_mouse_in_pixels();
    Boolean          find_slice_view_mouse_is_in();
    void             set_slice_window_update();
    void             set_update_required();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        mouse = &slice_window->mouse_position;

        get_mouse_in_pixels( slice_window, mouse, &x, &y );

        if( find_slice_view_mouse_is_in( slice_window, x, y, &view_index ) )
        {
            axis_index = 
               slice_window->slice.slice_views[view_index].axis_map[Z_AXIS];
            slice_window->slice.slice_locked[axis_index] =
                !slice_window->slice.slice_locked[axis_index];

            set_slice_window_update( slice_window, view_index );
            set_update_required( slice_window, NORMAL_PLANES );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(toggle_lock_slice)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_rotating_slice_mode )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            start_rotating_slice();

    if( get_current_volume(graphics,&volume) )
    {
        start_rotating_slice( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_rotating_slice_mode )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_translating_slice_mode )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            start_translating_slice();

    if( get_current_volume(graphics,&volume) )
    {
        start_translating_slice( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_translating_slice_mode )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_slice_transform )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            make_identity_transform();
    void            graphics_models_have_changed();

    if( get_current_volume(graphics,&volume) )
    {
        make_identity_transform( &volume->slice_transforms[
                                     volume->current_slice] );
    }

    graphics_models_have_changed( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(reset_slice_transform )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(output_slice_transforms )   /* ARGSUSED */
{
    volume_struct   *volume;
    int             z, nx, ny, nz;
    FILE            *file;
    Status          status;
    Status          open_file();
    Status          io_real();
    Status          io_newline();
    Status          close_file();
    String          filename;
    Real            degrees, x_trans, y_trans;
    void            get_volume_size();
    void            convert_2d_transform_to_rotation_translation();

    status = OK;

    if( get_current_volume(graphics,&volume) )
    {
        PRINT( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_file( filename, WRITE_FILE, ASCII_FORMAT, &file );

        if( status == OK )
        {
            get_volume_size( volume, &nx, &ny, &nz );

            for_less( z, 0, nz )
            {
                convert_2d_transform_to_rotation_translation(
                        &volume->slice_transforms[z], &degrees,
                        &x_trans, &y_trans );

                status = io_real( file, WRITE_FILE, ASCII_FORMAT, &degrees );

                if( status == OK )
                    status = io_real( file, WRITE_FILE, ASCII_FORMAT, &x_trans);

                if( status == OK )
                    status = io_real( file, WRITE_FILE, ASCII_FORMAT, &y_trans);

                if( status == OK )
                    status = io_newline( file, WRITE_FILE, ASCII_FORMAT );
            }
        }

        if( status == OK )
            status = close_file( file );

        PRINT( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(output_slice_transforms )   /* ARGSUSED */
{
    return( OK );
}
