
#include  <def_graphics.h>
#include  <def_globals.h>

public  DEF_MENU_FUNCTION( start_surface_line )   /* ARGSUSED */
{
    void  start_surface_curve();

    start_surface_curve( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(start_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( end_surface_line )   /* ARGSUSED */
{
    void  end_surface_curve();

    end_surface_curve( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(end_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_surface_line )   /* ARGSUSED */
{
    void  reset_surface_curve();

    reset_surface_curve( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(reset_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_surface_line_permanent )   /* ARGSUSED */
{
    void  make_surface_curve_permanent();

    make_surface_curve_permanent( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(make_surface_line_permanent)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_invisible )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             graphics_models_have_changed();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                                             N_painting_polygons, FALSE );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_invisible)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_visible )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             graphics_models_have_changed();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                                             N_painting_polygons, TRUE );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_visible)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( remove_invisible_parts_of_polygon )   /* ARGSUSED */
{
    Status           status;
    Status           remove_invisible_polygons();
    polygons_struct  *polygons;
    void             graphics_models_have_changed();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = remove_invisible_polygons( polygons );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(remove_invisible_parts_of_polygon)   /* ARGSUSED */
{
    return( OK );
}
