
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( toggle_render_mode )  /* ARGSUSED */
{
    if( graphics->model.render.render_mode == WIREFRAME_MODE )
    {
        graphics->model.render.render_mode = SHADED_MODE;
    }
    else
    {
        graphics->model.render.render_mode = WIREFRAME_MODE;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_render_mode )  /* ARGSUSED */
{
    switch( graphics->model.render.render_mode )
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
    if( graphics->model.render.shading_type == FLAT_SHADING )
    {
        graphics->model.render.shading_type = GOURAUD_SHADING;
    }
    else
    {
        graphics->model.render.shading_type = FLAT_SHADING;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_shading )  /* ARGSUSED */
{
    switch( graphics->model.render.shading_type )
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
    graphics->model.render.master_light_switch = 
                  !graphics->model.render.master_light_switch;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_lights )  /* ARGSUSED */
{
    void  set_text_on_off();

    set_text_on_off( format, text, graphics->model.render.master_light_switch );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_two_sided )  /* ARGSUSED */
{
    graphics->model.render.two_sided_surface_flag = 
                  !graphics->model.render.two_sided_surface_flag;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_two_sided )  /* ARGSUSED */
{
    void  set_text_on_off();

    set_text_on_off( format, text,
                     graphics->model.render.two_sided_surface_flag );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_backfacing )  /* ARGSUSED */
{
    graphics->model.render.backface_flag = 
                  !graphics->model.render.backface_flag;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_backfacing )  /* ARGSUSED */
{
    void  set_text_on_off();

    set_text_on_off( format, text,
                     graphics->model.render.backface_flag );

    menu_window->update_required = TRUE;

    return( OK );
}
