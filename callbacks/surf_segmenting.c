
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_stdio.h>

public  DEF_MENU_FUNCTION( reset_polygon_visibility )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    void             graphics_models_have_changed();
    void             set_polygons_visibilities();

    if( get_current_polygons(graphics,&polygons) )
    {
        set_polygons_visibilities( polygons, TRUE );

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_polygon_visibility )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_invisible )   /* ARGSUSED */
{
    void   turn_off_connected_polygons();

    turn_off_connected_polygons( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(set_connected_invisible )   /* ARGSUSED */
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
