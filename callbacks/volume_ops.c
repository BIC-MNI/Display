
#include  <display.h>

private  void  colour_code_object(
    display_struct   *display,
    object_struct    *object );

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

        voxel[axis_index] += (Real) delta;

        if( voxel_is_within_volume( volume, voxel ) )
        {
            if( set_current_voxel( slice_window, voxel ))
            {
                rebuild_probe( slice_window );
                rebuild_cursors( slice_window );

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

private  void  scale_slice_voxels(
    display_struct   *display,
    int              view_index,
    Real             scale_factor )
{
    display_struct   *slice_window;

    slice_window = display->associated[SLICE_WINDOW];

    scale_slice_view( slice_window, view_index, scale_factor );

    set_slice_window_update( slice_window, view_index );
    set_update_required( slice_window, NORMAL_PLANES );
}

public  DEF_MENU_FUNCTION(double_slice_voxels)   /* ARGSUSED */
{
    int      view_index;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        scale_slice_voxels( display, view_index, Slice_magnification_step );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(double_slice_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(halve_slice_voxels)   /* ARGSUSED */
{
    int      view_index;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        scale_slice_voxels( display, view_index,
                            1.0 / Slice_magnification_step );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(halve_slice_voxels )   /* ARGSUSED */
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
            colour_code_object( display, object );
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(colour_code_objects )   /* ARGSUSED */
{
    return( OK );
}

private  void  colour_code_points(
    colour_coding_struct  *colour_coding,
    Volume                volume,
    Colour_flags          *colour_flag,
    Colour                *colours[],
    int                   n_points,
    Point                 points[] )
{
    int      i;
    Real     val;

    if( *colour_flag != PER_VERTEX_COLOURS )
    {
        REALLOC( *colours, n_points );
        *colour_flag = PER_VERTEX_COLOURS;
    }

    for_less( i, 0, n_points )
    {
        evaluate_3D_volume_in_world( volume,
                                     Point_x(points[i]),
                                     Point_y(points[i]),
                                     Point_z(points[i]), Volume_continuity,
                                     &val, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL );

        (*colours)[i] = get_colour_code( colour_coding, val );
    }
}

private  void  colour_code_object(
    display_struct   *display,
    object_struct    *object )
{
    polygons_struct         *polygons;
    quadmesh_struct         *quadmesh;
    lines_struct            *lines;
    Volume                  volume;
    colour_coding_struct    *colour_coding;

    if( get_slice_window_volume( display, &volume ) )
    {
        colour_coding = &display->associated[SLICE_WINDOW]->slice.colour_coding;

        switch( object->object_type )
        {
        case POLYGONS:
            polygons = get_polygons_ptr( object );
            colour_code_points( colour_coding, volume,
                                &polygons->colour_flag, &polygons->colours,
                                polygons->n_points, polygons->points );
            break;

        case QUADMESH:
            quadmesh = get_quadmesh_ptr( object );
            colour_code_points( colour_coding, volume,
                                &quadmesh->colour_flag, &quadmesh->colours,
                                quadmesh->m * quadmesh->n,
                                quadmesh->points );
            break;

        case LINES:
            lines = get_lines_ptr( object );
            colour_code_points( colour_coding, volume,
                                &lines->colour_flag, &lines->colours,
                                lines->n_points, lines->points );
            break;
        }
    }
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

        colour_code_object( display, object );

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
    int              sizes[N_DIMENSIONS];
    char             ch;
    Real             x_width, y_width, z_width;
    Real             separations[N_DIMENSIONS];
    display_struct   *slice_window;
    Volume           volume, resampled_volume;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        get_volume_sizes( slice_window->slice.original_volume, sizes );
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
                x_width /= separations[X];
                y_width /= separations[Y];
                z_width /= separations[Z];
            }

            if( x_width > 1.0 || y_width > 1.0 || z_width > 1.0 )
            {
                resampled_volume = create_box_filtered_volume(
                                        slice_window->slice.original_volume,
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

