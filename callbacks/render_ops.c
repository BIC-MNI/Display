
#include  <def_graphics.h>

private  object_struct  *get_model_object( graphics )
    graphics_struct   *graphics;
{
    object_struct    *current_object;
    Boolean          get_current_object();
    object_struct    *get_current_model_object();

    if( !get_current_object( graphics, &current_object ) ||
        current_object->object_type != MODEL )
    {
        current_object = get_current_model_object( graphics );
    }

    return( current_object );
}

public  DEF_MENU_FUNCTION( toggle_render_mode )  /* ARGSUSED */
{
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    render_modes    new_render_mode;

    model_object = get_model_object( graphics );

    if( model_object->ptr.model->render.render_mode == WIREFRAME_MODE )
    {
        new_render_mode = SHADED_MODE;
    }
    else
    {
        new_render_mode = WIREFRAME_MODE;
    }

    BEGIN_TRAVERSE_OBJECT( status, model_object )

        if( OBJECT->object_type == MODEL )
        {
            OBJECT->ptr.model->render.render_mode = new_render_mode;
        }

    END_TRAVERSE_OBJECT

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_render_mode )  /* ARGSUSED */
{
    object_struct   *get_model_object();
    object_struct   *model_object;

    model_object = get_model_object( graphics );

    switch( model_object->ptr.model->render.render_mode )
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
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    shading_types   new_shading_type;

    model_object = get_model_object( graphics );

    if( model_object->ptr.model->render.shading_type == FLAT_SHADING )
    {
        new_shading_type = GOURAUD_SHADING;
    }
    else
    {
        new_shading_type = FLAT_SHADING;
    }

    BEGIN_TRAVERSE_OBJECT( status, model_object )

        if( OBJECT->object_type == MODEL )
        {
            OBJECT->ptr.model->render.shading_type = new_shading_type;
        }

    END_TRAVERSE_OBJECT

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_shading )  /* ARGSUSED */
{
    object_struct   *get_model_object();
    object_struct   *model_object;

    model_object = get_model_object( graphics );

    switch( model_object->ptr.model->render.shading_type )
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
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    Boolean         new_light_switch;

    model_object = get_model_object( graphics );

    new_light_switch = !model_object->ptr.model->render.master_light_switch;

    BEGIN_TRAVERSE_OBJECT( status, model_object )

        if( OBJECT->object_type == MODEL )
        {
            OBJECT->ptr.model->render.master_light_switch = new_light_switch;
        }

    END_TRAVERSE_OBJECT

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_lights )  /* ARGSUSED */
{
    void            set_text_on_off();
    object_struct   *get_model_object();
    object_struct   *model_object;

    model_object = get_model_object( graphics );

    set_text_on_off( format, text,
                     model_object->ptr.model->render.master_light_switch );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_two_sided )  /* ARGSUSED */
{
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    Boolean         new_flag;

    model_object = get_model_object( graphics );

    new_flag = !model_object->ptr.model->render.two_sided_surface_flag;

    BEGIN_TRAVERSE_OBJECT( status, model_object )

        if( OBJECT->object_type == MODEL )
        {
            OBJECT->ptr.model->render.two_sided_surface_flag = new_flag;
        }

    END_TRAVERSE_OBJECT

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_two_sided )  /* ARGSUSED */
{
    void            set_text_on_off();
    object_struct   *get_model_object();
    object_struct   *model_object;

    model_object = get_model_object( graphics );

    set_text_on_off( format, text,
                     model_object->ptr.model->render.two_sided_surface_flag );

    menu_window->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_backfacing )  /* ARGSUSED */
{
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    Boolean         new_flag;

    model_object = get_model_object( graphics );

    new_flag = !model_object->ptr.model->render.backface_flag;

    BEGIN_TRAVERSE_OBJECT( status, model_object )

        if( OBJECT->object_type == MODEL )
        {
            OBJECT->ptr.model->render.backface_flag = new_flag;
        }

    END_TRAVERSE_OBJECT

    graphics->update_required = TRUE;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_backfacing )  /* ARGSUSED */
{
    void           set_text_on_off();
    object_struct   *get_model_object();
    object_struct   *model_object;

    model_object = get_model_object( graphics );

    set_text_on_off( format, text,
                     model_object->ptr.model->render.backface_flag );

    menu_window->update_required = TRUE;

    return( OK );
}
