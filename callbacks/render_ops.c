
#include  <def_display.h>

private  object_struct  *get_model_object(
    display_struct    *display )
{
    object_struct    *current_object;

    if( !get_current_object( display, &current_object ) ||
        current_object->object_type != MODEL )
    {
        current_object = get_current_model_object( display );
    }

    return( current_object );
}

public  DEF_MENU_FUNCTION( toggle_render_mode )  /* ARGSUSED */
{
    object_struct            *model_object;
    Boolean                  shaded_mode;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    shaded_mode = !get_model_info(get_model_ptr(model_object))->
                   render.shaded_mode;

    initialize_object_traverse( &object_traverse, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.shaded_mode =
                                                  shaded_mode;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_render_mode )  /* ARGSUSED */
{
    object_struct   *model_object;
    String          text;

    model_object = get_model_object( display );

    set_text_boolean( label, text,
               get_model_info(get_model_ptr(model_object))->render.shaded_mode,
               "Wireframe", "Shaded" );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}


public  DEF_MENU_FUNCTION( toggle_shading )  /* ARGSUSED */
{
    object_struct            *model_object;
    Shading_types            new_shading_type;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    if( get_model_info(get_model_ptr(model_object))->render.shading_type ==
        FLAT_SHADING )
    {
        new_shading_type = GOURAUD_SHADING;
    }
    else
    {
        new_shading_type = FLAT_SHADING;
    }

    initialize_object_traverse( &object_traverse, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.shading_type =
                                              new_shading_type;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_shading )  /* ARGSUSED */
{
    object_struct   *model_object;
    String          text;

    model_object = get_model_object( display );

    switch( get_model_info(get_model_ptr(model_object))->render.shading_type )
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
    object_struct            *model_object;
    Boolean                  new_light_switch;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_light_switch = !get_model_info(get_model_ptr(model_object))->render.
                       master_light_switch;

    initialize_object_traverse( &object_traverse, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.
                                      master_light_switch = new_light_switch;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_lights )  /* ARGSUSED */
{
    object_struct   *model_object;
    String          text;

    model_object = get_model_object( display );

    set_text_on_off( label, text,
                     get_model_info(get_model_ptr(model_object))->render.
                     master_light_switch );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_two_sided )  /* ARGSUSED */
{
    object_struct            *model_object;
    Boolean                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               two_sided_surface_flag;

    initialize_object_traverse( &object_traverse, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.
                                    two_sided_surface_flag = new_flag;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_two_sided )  /* ARGSUSED */
{
    object_struct   *model_object;
    String          text;

    model_object = get_model_object( display );

    set_text_on_off( label, text,
                     get_model_info(get_model_ptr(model_object))->render.
                     two_sided_surface_flag );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_backfacing )  /* ARGSUSED */
{
    object_struct            *model_object;
    Boolean                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               backface_flag;

    initialize_object_traverse( &object_traverse, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.
                                     backface_flag = new_flag;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_backfacing )  /* ARGSUSED */
{
    object_struct   *model_object;
    String          text;

    model_object = get_model_object( display );

    set_text_on_off( label, text,
                     get_model_info(get_model_ptr(model_object))->render.
                     backface_flag );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_line_curve_flag )  /* ARGSUSED */
{
    object_struct            *model_object;
    Boolean                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               render_lines_as_curves;

    initialize_object_traverse( &object_traverse, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(object))->render.
                                  render_lines_as_curves = new_flag;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_line_curve_flag )  /* ARGSUSED */
{
    object_struct   *model_object;
    String          text;

    model_object = get_model_object( display );

    set_text_on_off( label, text,
                     get_model_info(get_model_ptr(model_object))->render.
                     render_lines_as_curves );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_marker_label_flag )  /* ARGSUSED */
{
    object_struct            *model_object;
    Boolean                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               show_marker_labels;

    initialize_object_traverse( &object_traverse, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
            get_model_info(get_model_ptr(model_object))->render.
                                   show_marker_labels = new_flag;
    }

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_marker_label_flag )  /* ARGSUSED */
{
    object_struct   *model_object;
    String          text;

    model_object = get_model_object( display );

    set_text_on_off( label, text,
                     get_model_info(get_model_ptr(model_object))->render.
                     show_marker_labels );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_n_curve_segments )  /* ARGSUSED */
{
    int                      n_segments;
    object_struct            *model_object;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    print( "Current number of curve segments is %d.\n",
            get_model_info(get_model_ptr(model_object))->
                                   render.n_curve_segments );
    print( "Enter number of curve segments:" );

    if( input_int( stdin, &n_segments ) == OK && n_segments > 0 )
    {
        initialize_object_traverse( &object_traverse, 1,
                                             &model_object );

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == MODEL )
            {
                get_model_info(get_model_ptr(model_object))->render.
                                       n_curve_segments = n_segments;
            }
        }

        set_update_required( display, NORMAL_PLANES );

        print( "New number of curve segments: %d\n", n_segments );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_n_curve_segments )  /* ARGSUSED */
{
    object_struct   *model_object;
    String          text;

    model_object = get_model_object( display );

    set_text_on_off( label, text,
                     get_model_info(get_model_ptr(model_object))->render.
                     render_lines_as_curves );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_double_buffer_threed )  /* ARGSUSED */
{
    Boolean   double_buffer;

    double_buffer = !G_get_double_buffer_state( display->window );

    G_set_double_buffer_state( display->window, double_buffer );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_double_buffer_threed )  /* ARGSUSED */
{
    String          text;

    set_text_on_off( label, text, G_get_double_buffer_state(display->window) );
    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_double_buffer_slice )  /* ARGSUSED */
{
    Boolean           double_buffer;
    display_struct    *slice_window;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window != (display_struct  *) 0 )
    {
        double_buffer = !G_get_double_buffer_state( slice_window->window );

        G_set_double_buffer_state( slice_window->window, double_buffer );

        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_double_buffer_slice )  /* ARGSUSED */
{
    display_struct  *slice_window;
    Boolean         state;
    String          text;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window != (display_struct  *) 0 )
        state = G_get_double_buffer_state( slice_window->window );
    else
        state = TRUE;

    set_text_on_off( label, text, state );
    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}
