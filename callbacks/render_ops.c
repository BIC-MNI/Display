/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/render_ops.c,v 1.34 1998-03-18 21:17:52 david Exp $";
#endif


#include  <display.h>

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

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_render_mode )
{
    object_struct            *model_object;
    BOOLEAN                  shaded_mode;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    shaded_mode = !get_model_info(get_model_ptr(model_object))->
                   render.shaded_mode;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

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

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_render_mode )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_boolean( menu_window, menu_entry,
               get_model_info(get_model_ptr(model_object))->render.shaded_mode,
               "Wireframe", "Shaded" );

    return( TRUE );
}


/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_shading )
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

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

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

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_shading )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_boolean( menu_window, menu_entry,
     (BOOLEAN) get_model_info(get_model_ptr(model_object))->render.shading_type,
     "Flat", "Gouraud" );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_lights )
{
    object_struct            *model_object;
    BOOLEAN                  new_light_switch;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_light_switch = !get_model_info(get_model_ptr(model_object))->render.
                       master_light_switch;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

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

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_lights )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          master_light_switch );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_two_sided )
{
    object_struct            *model_object;
    BOOLEAN                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               two_sided_surface_flag;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

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

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_two_sided )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          two_sided_surface_flag );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_backfacing )
{
    object_struct            *model_object;
    BOOLEAN                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               backface_flag;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

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

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_backfacing )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          backface_flag );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_line_curve_flag )
{
    object_struct            *model_object;
    BOOLEAN                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               render_lines_as_curves;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

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

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_line_curve_flag )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          render_lines_as_curves );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_marker_label_flag )
{
    object_struct            *model_object;
    BOOLEAN                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               show_marker_labels;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
            get_model_info(get_model_ptr(model_object))->render.
                                   show_marker_labels = new_flag;
    }

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_marker_label_flag )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          show_marker_labels );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_n_curve_segments )
{
    int                      n_segments;
    object_struct            *model_object;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    print( "Current number of curve segments is %d.\n",
            get_model_info(get_model_ptr(model_object))->
                                   render.n_curve_segments );
    print( "Enter number of curve segments: " );

    if( input_int( stdin, &n_segments ) == OK && n_segments > 0 )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

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

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_n_curve_segments )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_int( menu_window, menu_entry,
                       get_model_info(get_model_ptr(model_object))->render.
                       n_curve_segments );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_double_buffer_threed )
{
    BOOLEAN   double_buffer;

    double_buffer = !G_get_double_buffer_state( display->window );

    G_set_double_buffer_state( display->window, double_buffer );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_double_buffer_threed )
{
    set_menu_text_on_off( menu_window, menu_entry,
                          G_get_double_buffer_state(display->window) );

    return( G_can_switch_double_buffering() );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_double_buffer_slice )
{
    BOOLEAN           double_buffer;
    display_struct    *slice_window;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window != (display_struct  *) 0 )
    {
        double_buffer = !G_get_double_buffer_state( slice_window->window );

        G_set_double_buffer_state( slice_window->window, double_buffer );

        set_slice_viewport_update( slice_window, FULL_WINDOW_MODEL );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_double_buffer_slice )
{
    display_struct  *slice_window;
    BOOLEAN         state;

    state = get_slice_window( display, &slice_window );

    set_menu_text_on_off( menu_window, menu_entry, state &&
                          G_get_double_buffer_state( slice_window->window ) );

    return( state && G_can_switch_double_buffering() );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( change_background_colour )
{
    Status            status;
    display_struct    *slice_window;
    Colour            col;
    STRING            line;

    print( "Enter colour name or 3 or 4 colour components: " );

    status = input_line( stdin, &line );

    if( status == OK )
    {
        col = convert_string_to_colour( line );

        G_set_background_colour( display->window, col );
        set_update_required( display, NORMAL_PLANES );

        if( get_slice_window( display, &slice_window ) )
        {
            G_set_background_colour( slice_window->window, col );
            set_slice_viewport_update( slice_window, FULL_WINDOW_MODEL );
        }
    }

    delete_string( line );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(change_background_colour )
{
    return( TRUE );
}
