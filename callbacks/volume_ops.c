
#include  <def_display.h>

private  void  colour_code_object(
    display_struct   *display,
    object_struct    *object );

public  Boolean  get_slice_view_index_under_mouse(
    display_struct   *display,
    int              *view_index )
{
    Boolean          found;
    volume_struct    *volume;
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

public  Boolean  get_axis_index_under_mouse(
    display_struct   *display,
    int              *axis_index )
{
    Boolean          found;
    int              view_index;
    display_struct   *slice_window;

    found = get_slice_view_index_under_mouse( display, &view_index );

    if( found )
    {
        slice_window = display->associated[SLICE_WINDOW];

        *axis_index = 
             slice_window->slice.slice_views[view_index].axis_map[Z];
    }

    return( found );
}

private  void  change_current_slice_by_one(
    display_struct   *display,
    int              delta )
{
    display_struct   *slice_window;
    volume_struct    *volume;
    int              index[3], sizes[3], axis_index;

    if( get_axis_index_under_mouse( display, &axis_index ) &&
        get_slice_window_volume( display, &volume ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        get_volume_size( volume, &sizes[X], &sizes[Y],
                         &sizes[Z] );

        get_current_voxel( slice_window,
                           &index[X], &index[Y], &index[Z] );

        index[axis_index] += delta;

        if( index[axis_index] >= 0 && index[axis_index] < sizes[axis_index] )
        {
            if( set_current_voxel( slice_window, index[X], index[Y],
                                   index[Z] ) )
            {
                rebuild_probe( slice_window );
                rebuild_cursor( slice_window, 0 );
                rebuild_cursor( slice_window, 1 );
                rebuild_cursor( slice_window, 2 );
                set_slice_window_update( slice_window, axis_index );

                if( update_cursor_from_voxel( slice_window ) )
                {
                    set_update_required( slice_window->
                                         associated[THREE_D_WINDOW],
                                         OVERLAY_PLANES );
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

public  DEF_MENU_FUNCTION(double_slice_voxels)   /* ARGSUSED */
{
    display_struct   *slice_window;
    int              view_index;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        slice_window->slice.slice_views[view_index].x_scale *= 2.0;
        slice_window->slice.slice_views[view_index].y_scale *= 2.0;
        set_slice_window_update( slice_window, view_index );
        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(double_slice_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(halve_slice_voxels)   /* ARGSUSED */
{
    display_struct   *slice_window;
    int              view_index;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        slice_window->slice.slice_views[view_index].x_scale *= 0.5;
        slice_window->slice.slice_views[view_index].y_scale *= 0.5;
        set_slice_window_update( slice_window, view_index );
        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(halve_slice_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(toggle_lock_slice)   /* ARGSUSED */
{
    display_struct   *slice_window;
    int              axis_index, view_index;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        axis_index = 
           slice_window->slice.slice_views[view_index].axis_map[Z];
        slice_window->slice.slice_locked[axis_index] =
            !slice_window->slice.slice_locked[axis_index];

        set_slice_window_update( slice_window, view_index );
        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_lock_slice)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(colour_code_objects )   /* ARGSUSED */
{
    object_struct           *object, *current_object;
    volume_struct           *volume;
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
    volume_struct         *volume,
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
        (void) evaluate_volume_in_world( volume,
                                         Point_x(points[i]),
                                         Point_y(points[i]),
                                         Point_z(points[i]), FALSE,
                                         &val, (Real *) 0,
                                         (Real *) 0, (Real *) 0 );

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
    volume_struct           *volume;
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
    int              axis_index, view_index;
    object_struct    *object;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        axis_index = slice_window->slice.slice_views[view_index].axis_map[Z];

        object = create_3d_slice_quadmesh( get_volume(slice_window),
                      axis_index,
                      (Real) slice_window->slice.slice_index[axis_index] );

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
    int              nx, ny, nz;
    int              new_nx, new_ny, new_nz;
    display_struct   *slice_window;
    volume_struct    *volume, *resampled_volume;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        print( "The original volume is %d by %d by %d.\n",
               slice_window->slice.original_volume.sizes[X],
               slice_window->slice.original_volume.sizes[Y],
               slice_window->slice.original_volume.sizes[Z] );

        get_volume_size( volume, &nx, &ny, &nz );

        if( volume != &slice_window->slice.original_volume )
            print( "Currently resampled to %d by %d by %d.\n",
                   nx, ny, nz );

        print( "Enter desired resampled size: " );

        if( input_int( stdin, &new_nx ) == OK &&
            input_int( stdin, &new_ny ) == OK &&
            input_int( stdin, &new_nz ) == OK )
        {
            if( new_nx > 0 || new_ny > 0 || new_nz > 0 )
            {
                ALLOC( resampled_volume, 1 );
                smooth_resample_volume( &slice_window->slice.original_volume,
                                        new_nx, new_ny, new_nz,
                                        resampled_volume );
            }
            else
                resampled_volume = &slice_window->slice.original_volume;

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

