
#include  <def_graphics.h>
#include  <def_math.h>
#include  <def_files.h>

public  DEF_MENU_FUNCTION(start_surface )   /* ARGSUSED */
{
    Real           x, y, z;
    Boolean        get_current_volume();
    void           start_surface_extraction_at_point();
    volume_struct  *volume;

    if( get_current_volume( graphics, &volume ) )
    {
        if( get_voxel_corresponding_to_point( graphics,
                                              &graphics->three_d.cursor.origin,
                                              &x, &y, &z ) )
        {
            start_surface_extraction_at_point( graphics, ROUND(x), ROUND(y),
                                               ROUND(z) );
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
    void      set_text_on_off();
    String    text;
    void      set_menu_text();

    set_text_on_off( label, text,
              graphics->three_d.surface_extraction.extraction_in_progress );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION(reset_surface)   /* ARGSUSED */
{
    Status         status;
    Status         reset_surface_extraction();
    volume_struct  *volume;
    void           graphics_models_have_changed();
    void           set_all_voxel_label_flags();
    void           set_slice_window_update();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        status = reset_surface_extraction( graphics );

        set_all_voxel_label_flags( volume, FALSE );

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(reset_surface )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(make_surface_permanent)   /* ARGSUSED */
{
    Status         status;
    Status         reset_surface_extraction();
    Status         add_object_to_current_model();
    Status         create_object();
    volume_struct  *volume;
    object_struct  *object;

    status = OK;

    if( get_current_volume( graphics, &volume ) &&
        !graphics->three_d.surface_extraction.extraction_in_progress &&
        graphics->three_d.surface_extraction.polygons->n_items > 0 )
    {
        status = create_object( &object, POLYGONS );

        if( status == OK )
        {
            *(object->ptr.polygons) =
                  *(graphics->three_d.surface_extraction.polygons);

            status = add_object_to_current_model( graphics, object );
        }

        ALLOC( status, graphics->three_d.surface_extraction.polygons->colours,
               1 );
        graphics->three_d.surface_extraction.polygons->n_items = 0;
        graphics->three_d.surface_extraction.polygons->n_points = 0;

        status = reset_surface_extraction( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(make_surface_permanent )   /* ARGSUSED */
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

public  DEF_MENU_FUNCTION(get_labeled_boundary)   /* ARGSUSED */
{
    Status           status;
    Status           add_object_to_model();
    Status           extract_boundary_of_labeled_voxels();
    Status           create_object();
    object_struct    *object;
    volume_struct    *volume;
    model_struct     *get_current_model();
    void             graphics_models_have_changed();

    status = OK;

    if( get_current_volume( graphics, &volume ) )
    {
        status = create_object( &object, POLYGONS );

        if( status == OK )
            status = extract_boundary_of_labeled_voxels( volume,
                                     object->ptr.polygons );

        if( status == OK )
            status = add_object_to_model( get_current_model(graphics), object );

        if( status == OK )
            graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(get_labeled_boundary )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_surface_extract_x_max_distance )   /* ARGSUSED */
{
    int             dist;

    PRINT( "Enter X max distance: " );

    if( input_int( stdin, &dist ) == OK )
        graphics->three_d.surface_extraction.x_voxel_max_distance = dist;

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_surface_extract_x_max_distance )   /* ARGSUSED */
{
    String  text;
    void    set_menu_text();

    (void) sprintf( text, label,
                    graphics->three_d.surface_extraction.x_voxel_max_distance);

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_surface_extract_y_max_distance )   /* ARGSUSED */
{
    int             dist;

    PRINT( "Enter Y max distance: " );

    if( input_int( stdin, &dist ) == OK )
        graphics->three_d.surface_extraction.y_voxel_max_distance = dist;

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_surface_extract_y_max_distance )   /* ARGSUSED */
{
    String  text;
    void    set_menu_text();

    (void) sprintf( text, label,
                    graphics->three_d.surface_extraction.y_voxel_max_distance);

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_surface_extract_z_max_distance )   /* ARGSUSED */
{
    int             dist;

    PRINT( "Enter Z max distance: " );

    if( input_int( stdin, &dist ) == OK )
        graphics->three_d.surface_extraction.z_voxel_max_distance = dist;

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_surface_extract_z_max_distance )   /* ARGSUSED */
{
    String  text;
    void    set_menu_text();

    (void) sprintf( text, label,
                    graphics->three_d.surface_extraction.z_voxel_max_distance);

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}
