
#include  <def_graphics.h>
#include  <def_math.h>
#include  <def_stdio.h>

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
    Status         add_object_to_model();
    Status         create_object();
    volume_struct  *volume;
    model_struct   *get_current_model();
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

            status = add_object_to_model( get_current_model(graphics), object );
        }

        ALLOC1( status, graphics->three_d.surface_extraction.polygons->colours,
                1, Colour );
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
