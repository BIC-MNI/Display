
#include  <display.h>

public  BOOLEAN  get_slice_view_index_under_mouse(
    display_struct   *display,
    int              *view_index )
{
    BOOLEAN          found;
    Volume           volume;
    display_struct   *slice_window;
    int              x, y;

    found = FALSE;

    if( get_slice_window_volume( display, &volume ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        if( G_get_mouse_position( slice_window->window, &x, &y ) &&
            find_slice_view_mouse_is_in( slice_window, x, y, view_index ) )
        {
            found = TRUE;
        }
    }

    return( found );
}

private  void  change_current_slice_by_one(
    display_struct   *display,
    int              delta )
{
    display_struct   *slice_window;
    Volume           volume;
    Real             voxel[N_DIMENSIONS];
    int              sizes[N_DIMENSIONS], axis_index;

    if( get_axis_index_under_mouse( display, &axis_index ) &&
        get_slice_window_volume( display, &volume ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        get_volume_sizes( volume, sizes );

        get_current_voxel( slice_window, voxel );

        voxel[axis_index] = (Real) ROUND( voxel[axis_index] + (Real) delta );

        if( voxel_is_within_volume( volume, voxel ) )
        {
            if( set_current_voxel( slice_window, voxel ))
            {
                if( update_cursor_from_voxel( slice_window ) )
                {
                    set_update_required( slice_window->
                                         associated[THREE_D_WINDOW],
                                         get_cursor_bitplanes() );
                }
            }
        }
    }
}

public  DEF_MENU_FUNCTION(move_slice_plus)   /* ARGSUSED */
{
    change_current_slice_by_one( display, 1 );

    return( OK );
}

public  DEF_MENU_UPDATE(move_slice_plus )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(move_slice_minus)   /* ARGSUSED */
{
    change_current_slice_by_one( display, -1 );

    return( OK );
}

public  DEF_MENU_UPDATE(move_slice_minus )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(toggle_slice_visibility)   /* ARGSUSED */
{
    int              view_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        set_slice_visibility( slice_window, view_index,
                              !get_slice_visibility(slice_window,view_index) );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_slice_visibility )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(toggle_cross_section_visibility)   /* ARGSUSED */
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        set_volume_cross_section_visibility( display,
                                !get_volume_cross_section_visibility(display) );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_cross_section_visibility )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_current_slice_view)   /* ARGSUSED */
{
    int              view_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        reset_slice_view( slice_window, view_index );
        set_slice_window_update( slice_window, view_index );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_current_slice_view )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(colour_code_objects )   /* ARGSUSED */
{
    object_struct           *object, *current_object;
    Volume                  volume;
    object_traverse_struct  object_traverse;

    if( get_current_object(display,&current_object) &&
        get_slice_window_volume( display, &volume ) )
    {
        initialize_object_traverse( &object_traverse, 1, &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            colour_code_an_object( display, object );
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(colour_code_objects )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(create_3d_slice)   /* ARGSUSED */
{
    display_struct   *slice_window;
    int              x_index, y_index, axis_index, view_index;
    Real             current_voxel[N_DIMENSIONS];
    object_struct    *object;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) &&
        slice_has_ortho_axes( slice_window, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        get_current_voxel( slice_window, current_voxel );
        object = create_3d_slice_quadmesh( get_volume(display),
                                           axis_index,
                                           current_voxel[axis_index] );

        colour_code_an_object( display, object );

        add_object_to_current_model( display, object );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(create_3d_slice)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(resample_slice_window_volume)   /* ARGSUSED */
{
    int              sizes[N_DIMENSIONS];
    int              new_nx, new_ny, new_nz;
    display_struct   *slice_window;
    Volume           volume, resampled_volume;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        get_volume_sizes( slice_window->slice.original_volume, sizes );
        print( "The original volume is %d by %d by %d.\n",
               sizes[X], sizes[Y], sizes[Z] );


        if( volume != slice_window->slice.original_volume )
        {
            get_volume_sizes( volume, sizes );
            print( "Currently resampled to %d by %d by %d.\n",
                   sizes[X], sizes[Y], sizes[Z] );
        }

        print( "Enter desired resampled size: " );

        if( input_int( stdin, &new_nx ) == OK &&
            input_int( stdin, &new_ny ) == OK &&
            input_int( stdin, &new_nz ) == OK )
        {
            if( new_nx > 0 || new_ny > 0 || new_nz > 0 )
            {
                resampled_volume = smooth_resample_volume(
                                        slice_window->slice.original_volume,
                                        new_nx, new_ny, new_nz );
            }
            else
                resampled_volume = slice_window->slice.original_volume;

            set_slice_window_volume( slice_window, resampled_volume );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(resample_slice_window_volume)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(box_filter_slice_window_volume)   /* ARGSUSED */
{
    char             ch;
    Real             x_width, y_width, z_width;
    Real             separations[N_DIMENSIONS];
    display_struct   *slice_window;
    Volume           volume, resampled_volume;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        get_volume_separations( slice_window->slice.original_volume,
                                separations );

        print( "Enter box filter  x_width, y_width, z_width, v/w: " );

        if( input_real( stdin, &x_width ) == OK &&
            input_real( stdin, &y_width ) == OK &&
            input_real( stdin, &z_width ) == OK &&
            input_nonwhite_character( stdin, &ch ) == OK )
        {
            if( ch == 'w' )
            {
                x_width /= ABS( separations[X] );
                y_width /= ABS( separations[Y] );
                z_width /= ABS( separations[Z] );
            }

            if( x_width > 1.0 || y_width > 1.0 || z_width > 1.0 )
            {
                resampled_volume = create_box_filtered_volume(
                                        slice_window->slice.original_volume,
                                        NC_BYTE, FALSE, 0.0, 0.0,
                                        x_width, y_width, z_width );
            }
            else
                resampled_volume = slice_window->slice.original_volume;

            set_slice_window_volume( slice_window, resampled_volume );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(box_filter_slice_window_volume)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(pick_slice_angle_point)   /* ARGSUSED */
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        start_picking_slice_angle( slice_window );

    return( OK );
}

public  DEF_MENU_UPDATE(pick_slice_angle_point)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( rotate_slice_axes )      /* ARGSUSED */
{
    initialize_rotating_slice( display );

    return( OK );
}

public  DEF_MENU_UPDATE(rotate_slice_axes )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(redo_histogram)      /* ARGSUSED */
{
    int              axis_index;
    Real             voxel[MAX_DIMENSIONS], slice;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        if( get_voxel_in_slice_window( slice_window, voxel, &axis_index ) )
            slice = voxel[axis_index];
        else
        {
            axis_index = -1;
            slice = 0.0;
        }

        compute_histogram( slice_window, axis_index, ROUND(slice), FALSE );

        set_update_required( slice_window, NORMAL_PLANES );
    }

}

public  DEF_MENU_UPDATE(redo_histogram)      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(redo_histogram_labeled)      /* ARGSUSED */
{
    int              axis_index;
    Real             voxel[MAX_DIMENSIONS], slice;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        if( get_voxel_in_slice_window( slice_window, voxel, &axis_index ) )
            slice = voxel[axis_index];
        else
        {
            axis_index = -1;
            slice = 0.0;
        }

        compute_histogram( slice_window, axis_index, ROUND(slice), TRUE );

        set_update_required( slice_window, NORMAL_PLANES );
    }

}

public  DEF_MENU_UPDATE(redo_histogram_labeled)      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(print_voxel_origin)      /* ARGSUSED */
{
    Real             voxel[MAX_DIMENSIONS], xw, yw, zw;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        get_current_voxel( slice_window, voxel );

        convert_voxel_to_world( get_volume(slice_window), voxel,
                                &xw, &yw, &zw );

        print( "Current world origin: %g %g %g\n", xw, yw, zw );
    }
}

public  DEF_MENU_UPDATE(print_voxel_origin)      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(print_slice_plane)      /* ARGSUSED */
{
    display_struct   *slice_window;
    Vector           normal;
    int              view_index;
    Real             perp_axis[MAX_DIMENSIONS], xw, yw, zw;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        get_slice_perp_axis( slice_window, view_index, perp_axis );
        convert_voxel_vector_to_world( get_volume(slice_window),
                                       perp_axis, &xw, &yw, &zw );

        fill_Vector( normal, xw, yw, zw );
        NORMALIZE_VECTOR( normal, normal );

        print( "View: %d    Perpendicular: %g %g %g\n",
               view_index,
               Vector_x(normal), Vector_y(normal), Vector_z(normal) );
    }
}

public  DEF_MENU_UPDATE(print_slice_plane)      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(type_in_voxel_origin)      /* ARGSUSED */
{
    Real             voxel[MAX_DIMENSIONS], xw, yw, zw;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter x y z world coordinate: " );

        if( input_real( stdin, &xw ) == OK &&
            input_real( stdin, &yw ) == OK &&
            input_real( stdin, &zw ) == OK )
        {
            convert_world_to_voxel( get_volume(slice_window), xw, yw, zw,
                                    voxel );

            if( set_current_voxel( slice_window, voxel ) )
            {
                set_slice_window_all_update( slice_window );
                set_update_required( display, NORMAL_PLANES );
            }
        }

        (void) input_newline( stdin );
    }
}

public  DEF_MENU_UPDATE(type_in_voxel_origin)      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(type_in_slice_plane)      /* ARGSUSED */
{
    int              view_index;
    Real             perp_axis[MAX_DIMENSIONS], xw, yw, zw;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        print( "View %d:  enter x y z plane normal in world coordinate: ",
               view_index );

        if( input_real( stdin, &xw ) == OK &&
            input_real( stdin, &yw ) == OK &&
            input_real( stdin, &zw ) == OK )
        {
            convert_world_vector_to_voxel( get_volume(slice_window), xw, yw, zw,
                                    perp_axis );

            set_slice_plane_perp_axis( slice_window, view_index, perp_axis );
            reset_slice_view( slice_window, view_index );

            set_slice_window_update( slice_window, view_index );
        }

        (void) input_newline( stdin );
    }
}

public  DEF_MENU_UPDATE(type_in_slice_plane)      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(toggle_slice_cross_section_visibility)  /* ARGSUSED */
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.cross_section_visibility =
                             !slice_window->slice.cross_section_visibility;
        rebuild_slice_cross_sections( slice_window );
        set_slice_window_all_update( slice_window );
    }
}

public  DEF_MENU_UPDATE(toggle_slice_cross_section_visibility)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_current_arbitrary_view)  /* ARGSUSED */
{
    int              view_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        print( "Current arbitrary view is now: %d\n", view_index );

        slice_window->slice.cross_section_index = view_index;
        rebuild_slice_cross_sections( slice_window );
        rebuild_volume_cross_section( slice_window );
        set_slice_window_all_update( slice_window );
        set_update_required( display, NORMAL_PLANES );
    }
}

public  DEF_MENU_UPDATE(set_current_arbitrary_view)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(toggle_slice_anchor)  /* ARGSUSED */
{
    int              c, view_index;
    Vector           axis;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        if( slice_window->slice.cross_section_vector_present ||
            !get_slice_view_index_under_mouse( slice_window, &view_index ) ||
            view_index == get_arbitrary_view_index( slice_window ) )
        {
            slice_window->slice.cross_section_vector_present = FALSE;
            print( "Slice anchor turned off\n" );
        }
        else
        {
            slice_window->slice.cross_section_vector_present = TRUE;
            print( "Setting slice anchor to view index: %d\n", view_index );

            get_slice_cross_section_direction( slice_window, view_index,
                           get_arbitrary_view_index( slice_window ),
                           &axis );

            for_less( c, 0, N_DIMENSIONS )
                slice_window->slice.cross_section_vector[c] =
                                                     Vector_coord(axis,c);
        }
    }
}

public  DEF_MENU_UPDATE(toggle_slice_anchor)    /* ARGSUSED */
{
    return( OK );
}
