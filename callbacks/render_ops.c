
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( toggle_render_mode )  /* ARGSUSED */
{
    if( graphics->models[THREED_MODEL].render.render_mode == WIREFRAME_MODE )
    {
        graphics->models[THREED_MODEL].render.render_mode = SHADED_MODE;
    }
    else
    {
        graphics->models[THREED_MODEL].render.render_mode = WIREFRAME_MODE;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_render_mode )  /* ARGSUSED */
{
    switch( graphics->models[THREED_MODEL].render.render_mode )
    {
    case WIREFRAME_MODE:
        (void) sprintf( text, format, "Wireframe" );
        break;

    case SHADED_MODE:
        (void) sprintf( text, format, "Shaded" );
        break;
    }

    menu_window->update_required = TRUE;

    return( OK );
}


public  DEF_MENU_FUNCTION( toggle_shading )  /* ARGSUSED */
{
    if( graphics->models[THREED_MODEL].render.shading_type == FLAT_SHADING )
    {
        graphics->models[THREED_MODEL].render.shading_type = GOURAUD_SHADING;
    }
    else
    {
        graphics->models[THREED_MODEL].render.shading_type = FLAT_SHADING;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_shading )  /* ARGSUSED */
{
    switch( graphics->models[THREED_MODEL].render.shading_type )
    {
    case FLAT_SHADING:
        (void) sprintf( text, format, "Flat" );
        break;

    case GOURAUD_SHADING:
        (void) sprintf( text, format, "Gouraud" );
        break;
    }

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_lights )  /* ARGSUSED */
{
    graphics->models[THREED_MODEL].render.master_light_switch = 
                  !graphics->models[THREED_MODEL].render.master_light_switch;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_lights )  /* ARGSUSED */
{
    void  set_text_on_off();

    set_text_on_off( format, text,
                    graphics->models[THREED_MODEL].render.master_light_switch );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_two_sided )  /* ARGSUSED */
{
    graphics->models[THREED_MODEL].render.two_sided_surface_flag = 
                  !graphics->models[THREED_MODEL].render.two_sided_surface_flag;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_two_sided )  /* ARGSUSED */
{
    void  set_text_on_off();

    set_text_on_off( format, text,
                graphics->models[THREED_MODEL].render.two_sided_surface_flag );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_backfacing )  /* ARGSUSED */
{
    graphics->models[THREED_MODEL].render.backface_flag = 
                  !graphics->models[THREED_MODEL].render.backface_flag;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_backfacing )  /* ARGSUSED */
{
    void  set_text_on_off();

    set_text_on_off( format, text,
                     graphics->models[THREED_MODEL].render.backface_flag );

    menu_window->update_required = TRUE;

    return( OK );
}
