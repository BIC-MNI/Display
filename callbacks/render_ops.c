
#include  <def_graphics.h>
#include  <def_files.h>

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
    void            set_update_required();
    object_struct   *object;
    object_traverse_struct   object_traverse;
    Status                   initialize_object_traverse();

    model_object = get_model_object( graphics );

    if( model_object->ptr.model->render.render_mode == WIREFRAME_MODE )
    {
        new_render_mode = SHADED_MODE;
    }
    else
    {
        new_render_mode = WIREFRAME_MODE;
    }

    status = initialize_object_traverse( &object_traverse, 1, &model_object );

    while( status == OK && get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            object->ptr.model->render.render_mode = new_render_mode;
        }
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_render_mode )  /* ARGSUSED */
{
    object_struct   *get_model_object();
    object_struct   *model_object;
    String          text;
    void            set_menu_text();

    model_object = get_model_object( graphics );

    switch( model_object->ptr.model->render.render_mode )
    {
    case WIREFRAME_MODE:
        (void) sprintf( text, label, "Wireframe" );
        break;

    case SHADED_MODE:
        (void) sprintf( text, label, "Shaded" );
        break;
    }

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}


public  DEF_MENU_FUNCTION( toggle_shading )  /* ARGSUSED */
{
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    shading_types   new_shading_type;
    void            set_update_required();
    object_struct   *object;
    object_traverse_struct   object_traverse;
    Status                   initialize_object_traverse();

    model_object = get_model_object( graphics );

    if( model_object->ptr.model->render.shading_type == FLAT_SHADING )
    {
        new_shading_type = GOURAUD_SHADING;
    }
    else
    {
        new_shading_type = FLAT_SHADING;
    }

    status = initialize_object_traverse( &object_traverse, 1, &model_object );

    while( status == OK && get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            object->ptr.model->render.shading_type = new_shading_type;
        }
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_shading )  /* ARGSUSED */
{
    object_struct   *get_model_object();
    object_struct   *model_object;
    String          text;
    void            set_menu_text();

    model_object = get_model_object( graphics );

    switch( model_object->ptr.model->render.shading_type )
    {
    case FLAT_SHADING:
        (void) sprintf( text, label, "Flat" );
        break;

    case GOURAUD_SHADING:
        (void) sprintf( text, label, "Gouraud" );
        break;
    }

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_lights )  /* ARGSUSED */
{
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    Boolean         new_light_switch;
    void            set_update_required();
    object_struct   *object;
    object_traverse_struct   object_traverse;
    Status                   initialize_object_traverse();

    model_object = get_model_object( graphics );

    new_light_switch = !model_object->ptr.model->render.master_light_switch;

    status = initialize_object_traverse( &object_traverse, 1, &model_object );

    while( status == OK && get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            object->ptr.model->render.master_light_switch = new_light_switch;
        }
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_lights )  /* ARGSUSED */
{
    void            set_text_on_off();
    object_struct   *get_model_object();
    object_struct   *model_object;
    String          text;
    void            set_menu_text();

    model_object = get_model_object( graphics );

    set_text_on_off( label, text,
                     model_object->ptr.model->render.master_light_switch );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_two_sided )  /* ARGSUSED */
{
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    Boolean         new_flag;
    void            set_update_required();
    object_struct   *object;
    object_traverse_struct   object_traverse;
    Status                   initialize_object_traverse();

    model_object = get_model_object( graphics );

    new_flag = !model_object->ptr.model->render.two_sided_surface_flag;

    status = initialize_object_traverse( &object_traverse, 1, &model_object );

    while( status == OK && get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            object->ptr.model->render.two_sided_surface_flag = new_flag;
        }
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_two_sided )  /* ARGSUSED */
{
    void            set_text_on_off();
    object_struct   *get_model_object();
    object_struct   *model_object;
    String          text;
    void            set_menu_text();

    model_object = get_model_object( graphics );

    set_text_on_off( label, text,
                     model_object->ptr.model->render.two_sided_surface_flag );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_backfacing )  /* ARGSUSED */
{
    Status          status;
    object_struct   *get_model_object();
    object_struct   *model_object;
    Boolean         new_flag;
    void            set_update_required();
    object_struct   *object;
    object_traverse_struct   object_traverse;
    Status                   initialize_object_traverse();

    model_object = get_model_object( graphics );

    new_flag = !model_object->ptr.model->render.backface_flag;

    status = initialize_object_traverse( &object_traverse, 1, &model_object );

    while( status == OK && get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            object->ptr.model->render.backface_flag = new_flag;
        }
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_backfacing )  /* ARGSUSED */
{
    void            set_text_on_off();
    object_struct   *get_model_object();
    object_struct   *model_object;
    String          text;
    void            set_menu_text();

    model_object = get_model_object( graphics );

    set_text_on_off( label, text,
                     model_object->ptr.model->render.backface_flag );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_line_curve_flag )  /* ARGSUSED */
{
    Status                   status;
    object_struct            *get_model_object();
    object_struct            *model_object;
    Boolean                  new_flag;
    void                     set_update_required();
    object_struct            *object;
    object_traverse_struct   object_traverse;
    Status                   initialize_object_traverse();

    model_object = get_model_object( graphics );

    new_flag = !model_object->ptr.model->render.render_lines_as_curves;

    status = initialize_object_traverse( &object_traverse, 1, &model_object );

    while( status == OK && get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            object->ptr.model->render.render_lines_as_curves = new_flag;
        }
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( status );
}

public  DEF_MENU_UPDATE(toggle_line_curve_flag )  /* ARGSUSED */
{
    void            set_text_on_off();
    object_struct   *get_model_object();
    object_struct   *model_object;
    String          text;
    void            set_menu_text();

    model_object = get_model_object( graphics );

    set_text_on_off( label, text,
                     model_object->ptr.model->render.render_lines_as_curves );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_n_curve_segments )  /* ARGSUSED */
{
    Status                   status;
    int                      n_segments;
    object_struct            *get_model_object();
    object_struct            *model_object;
    void                     set_update_required();
    object_struct            *object;
    object_traverse_struct   object_traverse;
    Status                   initialize_object_traverse();

    status = OK;

    model_object = get_model_object( graphics );

    PRINT( "Current number of curve segments is %d.\n",
            model_object->ptr.model->render.n_curve_segments );
    PRINT( "Enter number of curve segments:" );

    if( input_int( stdin, &n_segments ) == OK && n_segments > 0 &&
        input_newline( stdin ) == OK )
    {
        status = initialize_object_traverse( &object_traverse, 1,
                                             &model_object );

        while( status == OK &&
               get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == MODEL )
            {
                object->ptr.model->render.n_curve_segments = n_segments;
            }
        }

        set_update_required( graphics, NORMAL_PLANES );

        PRINT( "New number of curve segments: %d\n", n_segments );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_n_curve_segments )  /* ARGSUSED */
{
    void            set_text_on_off();
    object_struct   *get_model_object();
    object_struct   *model_object;
    String          text;
    void            set_menu_text();

    model_object = get_model_object( graphics );

    set_text_on_off( label, text,
                     model_object->ptr.model->render.render_lines_as_curves );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_double_buffer_threed )  /* ARGSUSED */
{
    void   G_set_double_buffer_state();
    void   set_update_required();

    graphics->window.double_buffer_flag =
             !graphics->window.double_buffer_flag;

    G_set_double_buffer_state( &graphics->window,
                               graphics->window.double_buffer_flag );

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_double_buffer_threed )  /* ARGSUSED */
{
    String          text;
    void            set_menu_text();
    void            set_text_on_off();

    set_text_on_off( label, text, graphics->window.double_buffer_flag );
    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_double_buffer_slice )  /* ARGSUSED */
{
    graphics_struct   *slice_window;
    void              G_set_double_buffer_state();
    void              set_update_required();

    slice_window = graphics->associated[SLICE_WINDOW];

    if( slice_window != (graphics_struct *) 0 )
    {
        slice_window->window.double_buffer_flag =
                 !slice_window->window.double_buffer_flag;

        G_set_double_buffer_state( &slice_window->window,
                                   slice_window->window.double_buffer_flag );

        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_double_buffer_slice )  /* ARGSUSED */
{
    graphics_struct *slice_window;
    Boolean         state;
    String          text;
    void            set_menu_text();
    void            set_text_on_off();

    slice_window = graphics->associated[SLICE_WINDOW];

    if( slice_window != (graphics_struct *) 0 )
        state = slice_window->window.double_buffer_flag;
    else
        state = TRUE;

    set_text_on_off( label, text, state );
    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}
