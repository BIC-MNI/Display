
#include  <def_graphics.h>
#include  <def_math.h>
#include  <def_stdio.h>

private  Boolean  get_current_volume( graphics, volume )
    graphics_struct   *graphics;
    volume_struct     **volume;
{
    Status          status;
    Boolean         found;
    object_struct   *current_object;
    Boolean         get_current_object();

    found = get_slice_window_volume( graphics, volume );

    if( !found )
    {
        if( get_current_object( graphics, &current_object ) )
        {
            BEGIN_TRAVERSE_OBJECT( status, current_object )

                if( !found && OBJECT->object_type == VOLUME )
                {
                    found = TRUE;
                    *volume = OBJECT->ptr.volume;
                }

            END_TRAVERSE_OBJECT
        }
    }

    return( found );
}

public  DEF_MENU_FUNCTION( advance_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            rebuild_selected_list();

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        ++volume->current_slice;
        if( volume->current_slice >= volume->size[Z_AXIS] )
        {
            volume->current_slice = 0;
        }
        volume->slice_visibilities[volume->current_slice] = ON;

        rebuild_selected_list( graphics, menu_window );
    }

    graphics->update_required = TRUE;

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

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        --volume->current_slice;
        if( volume->current_slice < 0 )
        {
            volume->current_slice = volume->size[Z_AXIS]-1;
        }
        volume->slice_visibilities[volume->current_slice] = ON;

        rebuild_selected_list( graphics, menu_window );
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_slice_transform )   /* ARGSUSED */
{
    volume_struct   *volume;
    Real            degrees, x_offset, y_offset;
    void            create_2d_transform();

    if( get_current_volume(graphics,&volume) )
    {
        PRINT( "Enter degrees, x_off, y_off: " );
        if( scanf( "%f %f %f", &degrees, &x_offset, &y_offset ) == 3 )
        {
            create_2d_transform( &volume->slice_transforms[
                                 volume->current_slice],
                                 degrees, x_offset, y_offset );
        }
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(set_slice_transform )   /* ARGSUSED */
{
    return( OK );
}

private  void  create_2d_transform( transform, degrees, x_offset, y_offset )
    Transform   *transform;
    Real        degrees;
    Real        x_offset;
    Real        y_offset;
{
    Real   c, s;
    void   make_identity_transform();

    make_identity_transform( transform );

    c = cos( (double) degrees * DEG_TO_RAD );
    s = sin( (double) degrees * DEG_TO_RAD );

    Transform_elem(*transform,0,0) = c;
    Transform_elem(*transform,0,1) = s;
    Transform_elem(*transform,1,0) = -s;
    Transform_elem(*transform,1,1) = c;

    Transform_elem(*transform,0,3) = x_offset;
    Transform_elem(*transform,1,3) = y_offset;
}

public  DEF_MENU_FUNCTION(open_slice_window )   /* ARGSUSED */
{
    Status           status;
    Status           create_graphics_window();
    volume_struct    *volume;
    graphics_struct  *slice_window;
    Status           set_slice_window_volume();
    void             rebuild_slice_models();

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

            slice_window->update_required = TRUE;
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(open_slice_window )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(start_surface )   /* ARGSUSED */
{
    int            x, y, z;
    Boolean        get_current_volume();
    void           start_surface_extraction_at_point();
    volume_struct  *volume;

    if( get_current_volume( graphics, &volume ) )
    {
        if( convert_point_to_voxel( graphics, &graphics->three_d.cursor.origin,
                                    &x, &y, &z ) )
        {
            start_surface_extraction_at_point( graphics, x, y, z );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(start_surface )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(toggle_surface_extraction)   /* ARGSUSED */
{
    void           start_surface_extraction();
    void           stop_surface_extraction();
    volume_struct  *volume;

    if( get_current_volume( graphics, &volume ) )
    {
        if( graphics->three_d.surface_extraction.extraction_in_progress )
        {
            stop_surface_extraction( graphics );
        }
        else
        {
            start_surface_extraction( graphics );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_surface_extraction )   /* ARGSUSED */
{
    void  set_text_on_off();

    set_text_on_off( format, text,
              graphics->three_d.surface_extraction.extraction_in_progress );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION(reset_surface)   /* ARGSUSED */
{
    Status         status;
    Status         reset_surface_extraction();
    volume_struct  *volume;

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        status = reset_surface_extraction( graphics );

        graphics->update_required = TRUE;
    }

    return( status );
}

public  DEF_MENU_UPDATE(reset_surface )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(double_slice_voxels)   /* ARGSUSED */
{
    Status           status;
    volume_struct    *volume;
    graphics_struct  *slice_window;
    Point            *mouse;
    int              x, y, axis_index;
    void             get_mouse_in_pixels();
    Boolean          find_slice_view_mouse_is_in();
    void             rebuild_slice_pixels();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        mouse = &slice_window->mouse_position;

        get_mouse_in_pixels( slice_window, mouse, &x, &y );

        if( find_slice_view_mouse_is_in( slice_window, x, y, &axis_index ) )
        {
            slice_window->slice.slice_views[axis_index].x_scale *= 2.0;
            slice_window->slice.slice_views[axis_index].y_scale *= 2.0;
            rebuild_slice_pixels( slice_window, axis_index );
            slice_window->update_required = TRUE;
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
    int              x, y, axis_index;
    void             get_mouse_in_pixels();
    Boolean          find_slice_view_mouse_is_in();
    void             rebuild_slice_pixels();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        mouse = &slice_window->mouse_position;

        get_mouse_in_pixels( slice_window, mouse, &x, &y );

        if( find_slice_view_mouse_is_in( slice_window, x, y, &axis_index ) )
        {
            slice_window->slice.slice_views[axis_index].x_scale *= 0.5;
            slice_window->slice.slice_views[axis_index].y_scale *= 0.5;
            rebuild_slice_pixels( slice_window, axis_index );
            slice_window->update_required = TRUE;
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(halve_slice_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(turn_voxel_on)   /* ARGSUSED */
{
    Status           status;
    graphics_struct  *slice_window;
    int              x, y, z, axis_index;
    void             set_voxel_inactivity();
    void             rebuild_slice_pixels();

    status = OK;

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        set_voxel_inactivity( slice_window->slice.volume, x, y, z, FALSE );

        rebuild_slice_pixels( slice_window, axis_index );

        slice_window->update_required = TRUE;
    }

    return( status );
}

public  DEF_MENU_UPDATE(turn_voxel_on )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(turn_voxel_off)   /* ARGSUSED */
{
    Status           status;
    int              x, y, z, axis_index;
    void             set_voxel_inactivity();
    void             rebuild_slice_pixels();
    graphics_struct  *slice_window;

    status = OK;

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        set_voxel_inactivity( slice_window->slice.volume, x, y, z, TRUE );

        rebuild_slice_pixels( slice_window, axis_index );

        slice_window->update_required = TRUE;
    }

    return( status );
}

public  DEF_MENU_UPDATE(turn_voxel_off )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(save_inactive_voxels)   /* ARGSUSED */
{
    FILE             *file;
    Status           status;
    Status           open_output_file();
    Status           close_file();
    Status           io_int();
    Status           io_newline();
    volume_struct    *volume;
    int              x, y, z;
    Boolean          get_voxel_activity();
    String           filename;

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        (void) printf( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_output_file( filename, &file );

        if( status == OK )
        {
            for_less( x, 0, volume->size[X_AXIS] )
            {
                for_less( y, 0, volume->size[Y_AXIS] )
                {
                    for_less( z, 0, volume->size[Z_AXIS] )
                    {
                        if( get_voxel_inactivity_flag( volume, x, y, z ) )
                        {
                            if( status == OK )
                            {
                                status = io_int( file, OUTPUTTING, ASCII_FORMAT,
                                                 &x );
                            }

                            if( status == OK )
                            {
                                status = io_int( file, OUTPUTTING, ASCII_FORMAT,
                                                 &y );
                            }

                            if( status == OK )
                            {
                                status = io_int( file, OUTPUTTING, ASCII_FORMAT,
                                                 &z );
                            }

                            if( status == OK )
                            {
                                status = io_newline( file, OUTPUTTING,
                                                     ASCII_FORMAT );
                            }
                        }
                    }
                }
            }
        }

        if( status == OK )
        {
            status = close_file( file );
        }

        PRINT( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(save_inactive_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(load_inactive_voxels)   /* ARGSUSED */
{
    FILE             *file;
    Status           status;
    Status           open_input_file();
    Status           close_file();
    Status           io_int();
    volume_struct    *volume;
    int              x, y, z;
    Boolean          get_voxel_activity();
    String           filename;
    void             rebuild_slice_models();
    void             set_voxel_inactivity();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        (void) printf( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_input_file( filename, &file );

        while( status == OK &&
               io_int( file, INPUTTING, ASCII_FORMAT, &x ) == OK )
        {
            status = io_int( file, INPUTTING, ASCII_FORMAT, &y );

            if( status == OK )
            {
                status = io_int( file, INPUTTING, ASCII_FORMAT, &z );
            }

            if( status == OK )
            {
                set_voxel_inactivity( volume, x, y, z, TRUE );
            }
        }

        status = close_file( file );

        if( status == OK )
        {
            rebuild_slice_models( graphics->associated[SLICE_WINDOW] );

            graphics->associated[SLICE_WINDOW]->update_required = TRUE;
        }

        PRINT( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_inactive_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_colour_limits )   /* ARGSUSED */
{
    volume_struct    *volume;
    Real             min_value, max_value;
    graphics_struct  *slice_window;
    void             rebuild_slice_models();
    void             set_colour_coding_range();

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
            set_colour_coding_range( &slice_window->slice.colour_coding,
                                     min_value, max_value );

            PRINT( "    New limits:\t%g\t%g\n",
                   slice_window->slice.colour_coding.min_value,
                   slice_window->slice.colour_coding.max_value );

            rebuild_slice_models( slice_window );

            slice_window->update_required = TRUE;
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_colour_limits )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_inactivities)   /* ARGSUSED */
{
    volume_struct    *volume;
    void             set_all_voxel_inactivities();
    void             rebuild_slice_models();
    graphics_struct  *slice_window;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        set_all_voxel_inactivities( volume, FALSE );

        rebuild_slice_models( slice_window );

        slice_window->update_required = TRUE;
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_inactivities )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_activities)   /* ARGSUSED */
{
    volume_struct    *volume;
    void             set_all_voxel_activities();
    void             rebuild_slice_models();
    graphics_struct  *slice_window;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        set_all_voxel_activities( volume, TRUE );

        rebuild_slice_models( slice_window );

        slice_window->update_required = TRUE;
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_activities )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(generate_activities )   /* ARGSUSED */
{
    int              x, y, z;
    Boolean          get_current_volume();
    void             generate_activity_from_point();
    volume_struct    *volume;
    graphics_struct  *slice_window;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        if( convert_point_to_voxel( slice_window,
                                    &graphics->three_d.cursor.origin,
                                    &x, &y, &z ) )
        {
            generate_activity_from_point( graphics, x, y, z );

            rebuild_slice_models( slice_window );

            slice_window->update_required = TRUE;
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(generate_activities )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_isovalue )   /* ARGSUSED */
{
    void             set_isosurface_value();

    set_isosurface_value( &graphics->three_d.surface_extraction );

    return( OK );
}

public  DEF_MENU_UPDATE(set_isovalue )   /* ARGSUSED */
{
    return( OK );
}
