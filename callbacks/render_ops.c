
#include  <def_graphics.h>

private  model_struct  *get_relevant_model( graphics )
    graphics_struct   *graphics;
{
    object_struct   *current_object;
    object_struct   *get_current_object();
    model_struct    *get_current_model();
    model_struct    *model;

    current_object = get_current_object( graphics );

    if( current_object != (object_struct *) 0 &&
        current_object->object_type == MODEL )
    {
        model = current_object->ptr.model;
    }
    else
    {
        model = get_current_model( graphics );
    }

    return( model );
}

public  DEF_MENU_FUNCTION( toggle_render_mode )  /* ARGSUSED */
{
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    if( model->render.render_mode == WIREFRAME_MODE )
    {
        model->render.render_mode = SHADED_MODE;
    }
    else
    {
        model->render.render_mode = WIREFRAME_MODE;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_render_mode )  /* ARGSUSED */
{
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    switch( model->render.render_mode )
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
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    if( model->render.shading_type == FLAT_SHADING )
    {
        model->render.shading_type = GOURAUD_SHADING;
    }
    else
    {
        model->render.shading_type = FLAT_SHADING;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_shading )  /* ARGSUSED */
{
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    switch( model->render.shading_type )
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
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    model->render.master_light_switch = !model->render.master_light_switch;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_lights )  /* ARGSUSED */
{
    void           set_text_on_off();
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    set_text_on_off( format, text, model->render.master_light_switch );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_two_sided )  /* ARGSUSED */
{
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    model->render.two_sided_surface_flag = 
                  !model->render.two_sided_surface_flag;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_two_sided )  /* ARGSUSED */
{
    void           set_text_on_off();
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    set_text_on_off( format, text, model->render.two_sided_surface_flag );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_backfacing )  /* ARGSUSED */
{
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    model->render.backface_flag = !model->render.backface_flag;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_backfacing )  /* ARGSUSED */
{
    void           set_text_on_off();
    model_struct   *get_relevant_model();
    model_struct   *model;

    model = get_relevant_model( graphics );

    set_text_on_off( format, text, model->render.backface_flag );

    menu_window->update_required = TRUE;

    return( OK );
}
