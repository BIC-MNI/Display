
#include  <display.h>

public  DEF_MENU_FUNCTION(start_surface )   /* ARGSUSED */
{
    Real           voxel[MAX_DIMENSIONS];
    int            int_voxel[MAX_DIMENSIONS];
    Volume         volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        if( get_voxel_corresponding_to_point( display,
                                              &display->three_d.cursor.origin,
                                              voxel ) )
        {
            convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
            start_surface_extraction_at_point( display, int_voxel[X],
                                               int_voxel[Y], int_voxel[Z] );
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
    Volume                  volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        if( display->three_d.surface_extraction.extraction_in_progress )
            stop_surface_extraction( display );
        else
            start_surface_extraction( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_surface_extraction )   /* ARGSUSED */
{
    STRING    text;

    set_text_on_off( label, text,
              display->three_d.surface_extraction.extraction_in_progress );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION(reset_surface)   /* ARGSUSED */
{
    Volume                  volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        reset_surface_extraction( display );

        set_all_volume_label_data( get_label_volume(display), 0 );

        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_surface )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(make_surface_permanent)   /* ARGSUSED */
{
    Volume         volume;
    object_struct  *object;

    if( get_slice_window_volume( display, &volume ) &&
        !display->three_d.surface_extraction.extraction_in_progress &&
        display->three_d.surface_extraction.polygons->n_items > 0 )
    {
        object = create_object( POLYGONS );

        *(get_polygons_ptr(object)) =
                  *(display->three_d.surface_extraction.polygons);

        add_object_to_current_model( display, object );

        ALLOC( display->three_d.surface_extraction.polygons->colours, 1 );
        display->three_d.surface_extraction.polygons->n_items = 0;
        display->three_d.surface_extraction.polygons->n_points = 0;

        reset_surface_extraction( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(make_surface_permanent )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_isovalue )   /* ARGSUSED */
{
    set_isosurface_value( &display->three_d.surface_extraction );

    return( OK );
}

public  DEF_MENU_UPDATE(set_isovalue )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(get_labeled_boundary)   /* ARGSUSED */
{
    object_struct    *object;
    Volume           volume;
    int              label;

    if( get_slice_window_volume( display, &volume ) )
    {
        print( "Enter label value to get boundary of: " );

        if( input_int( stdin, &label ) == OK &&
            label >= 0 && label < NUM_LABELS )
        {
            object = create_object( POLYGONS );

            extract_boundary_of_labeled_voxels( volume,
                      get_label_volume(display), label,
                      get_polygons_ptr(object) );

            add_object_to_model( get_current_model(display), object );

            graphics_models_have_changed( display );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(get_labeled_boundary )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_surface_extract_x_max_distance )   /* ARGSUSED */
{
    int             dist;

    print( "Enter X max distance: " );

    if( input_int( stdin, &dist ) == OK )
        display->three_d.surface_extraction.x_voxel_max_distance = dist;

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_surface_extract_x_max_distance )   /* ARGSUSED */
{
    STRING  text;

    (void) sprintf( text, label,
                    display->three_d.surface_extraction.x_voxel_max_distance);

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_surface_extract_y_max_distance )   /* ARGSUSED */
{
    int             dist;

    print( "Enter Y max distance: " );

    if( input_int( stdin, &dist ) == OK )
        display->three_d.surface_extraction.y_voxel_max_distance = dist;

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_surface_extract_y_max_distance )   /* ARGSUSED */
{
    STRING  text;

    (void) sprintf( text, label,
                    display->three_d.surface_extraction.y_voxel_max_distance);

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_surface_extract_z_max_distance )   /* ARGSUSED */
{
    int             dist;

    print( "Enter Z max distance: " );

    if( input_int( stdin, &dist ) == OK )
        display->three_d.surface_extraction.z_voxel_max_distance = dist;

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_surface_extract_z_max_distance )   /* ARGSUSED */
{
    STRING  text;

    (void) sprintf( text, label,
                    display->three_d.surface_extraction.z_voxel_max_distance);

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}
