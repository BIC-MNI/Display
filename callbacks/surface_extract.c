
#include  <display.h>

private  void  start_surface(
    display_struct   *display,
    BOOLEAN          use_label_flag,
    BOOLEAN          binary_flag )
{
    BOOLEAN        input_okay;
    Real           min_value, max_value;
    Real           voxel[MAX_DIMENSIONS];
    int            int_voxel[MAX_DIMENSIONS];
    Volume         volume, label_volume;

    if( use_label_flag )
    {
        volume = get_label_volume( display );
        label_volume = NULL;
    }
    else
    {
        volume = get_volume( display );
        label_volume = get_label_volume( display );
    }

    if( volume == NULL )
        return;

    input_okay = TRUE;

    if( binary_flag )
    {
        print( "Enter min and max inside value: " );
        if( input_real( stdin, &min_value ) != OK ||
            input_real( stdin, &max_value ) != OK )
            input_okay = FALSE;
    }
    else
    {
        print( "Enter isovalue: " );
        if( input_real( stdin, &min_value ) != OK )
            input_okay = FALSE;
        max_value = min_value;
    }

    (void) input_newline( stdin );

    if( !input_okay )
        return;

    if( get_voxel_corresponding_to_point( display,
                                          &display->three_d.cursor.origin,
                                          voxel ) )
    {
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        start_surface_extraction_at_point( display, volume, label_volume,
                                           binary_flag,
                                           min_value,
                                           max_value,
                                           int_voxel[X],
                                           int_voxel[Y],
                                           int_voxel[Z] );
    }
}

public  DEF_MENU_FUNCTION(start_volume_isosurface )   /* ARGSUSED */
{
    start_surface( display, FALSE, FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(start_volume_isosurface )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(start_volume_binary_isosurface )   /* ARGSUSED */
{
    start_surface( display, FALSE, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(start_volume_binary_isosurface )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(start_label_binary_isosurface )   /* ARGSUSED */
{
    start_surface( display, TRUE, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(start_label_binary_isosurface )   /* ARGSUSED */
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

        ALLOC( display->three_d.surface_extraction.polygons->colours, 1 );
        display->three_d.surface_extraction.polygons->n_items = 0;
        display->three_d.surface_extraction.polygons->n_points = 0;
        reset_surface_extraction( display );

        add_object_to_current_model( display, object );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(make_surface_permanent )   /* ARGSUSED */
{
    return( OK );
}

private  void   voxelate_surface(
    display_struct   *display,
    BOOLEAN          use_label_volume )
{
    object_struct    *object;
    Volume           volume;
    STRING           line;
    Real             min_value, max_value;

    if( use_label_volume )
        volume = get_label_volume( display );
    else
        volume = get_volume( display );

    if( volume == NULL )
        return;

    print( "Enter value or range to get boundary of: " );

    if( input_line( stdin, line, MAX_STRING_LENGTH ) != OK )
        return;

    if( sscanf( line, "%lf %lf\n", &min_value, &max_value ) != 2 )
    {
        if( sscanf( line, "%lf\n", &min_value ) != 1 )
            return;
        max_value = min_value;
    }

    object = create_object( POLYGONS );

    create_voxelated_surface( volume,
               min_value, max_value, get_polygons_ptr(object) );

    add_object_to_model( get_current_model(display), object );

    graphics_models_have_changed( display );
}

public  DEF_MENU_FUNCTION(get_voxelated_label_surface)   /* ARGSUSED */
{
    voxelate_surface( display, TRUE );
    return( OK );
}

public  DEF_MENU_UPDATE(get_voxelated_label_surface )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(get_voxelated_surface)   /* ARGSUSED */
{
    voxelate_surface( display, FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(get_voxelated_surface )   /* ARGSUSED */
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
