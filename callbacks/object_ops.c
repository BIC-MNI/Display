
#include  <display.h>

public  DEF_MENU_FUNCTION( reverse_normals )   /* ARGSUSED */
{
    object_struct   *current_object;

    if( get_current_object( display, &current_object ) )
    {
        reverse_object_normals( current_object );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reverse_normals )   /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( advance_visible )     /* ARGSUSED */
{
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) )
    {
        current_object->visibility = OFF;

        advance_current_object( display );

        if( get_current_object( display, &current_object ) )
            current_object->visibility = ON;

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(advance_visible )     /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( retreat_visible )     /* ARGSUSED */
{
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) )
    {
        current_object->visibility = OFF;

        retreat_current_object( display );

        if( get_current_object( display, &current_object ) )
            current_object->visibility = ON;

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_visible )     /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( make_all_invisible )     /* ARGSUSED */
{
    object_struct           *object, *current_object;
    object_traverse_struct  object_traverse;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, 1,
                                             &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
               object->visibility = FALSE;

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(make_all_invisible )     /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( make_all_visible )     /* ARGSUSED */
{
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, 1, &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
            object->visibility = TRUE;

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(make_all_visible )     /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( advance_selected )     /* ARGSUSED */
{
    advance_current_object( display );

    rebuild_selected_list( display, menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(advance_selected )     /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( retreat_selected )     /* ARGSUSED */
{
    retreat_current_object( display );

    rebuild_selected_list( display, menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_selected )     /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( descend_selected )     /* ARGSUSED */
{
    push_current_object( display );

    rebuild_selected_list( display, menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(descend_selected )     /* ARGSUSED */
{
    return( current_object_is_this_type(display,MODEL) );
}

public  DEF_MENU_FUNCTION( ascend_selected )     /* ARGSUSED */
{
    pop_current_object( display );

    rebuild_selected_list( display, menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(ascend_selected )     /* ARGSUSED */
{
    return( display->three_d.current_object.current_level > 1 );
}

public  DEF_MENU_FUNCTION( toggle_object_visibility )     /* ARGSUSED */
{
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) )
    {
        current_object->visibility = !current_object->visibility;

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_object_visibility )     /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( create_model )     /* ARGSUSED */
{
    create_model_after_current( display );

    graphics_models_have_changed( display );

    return( OK );
}

public  DEF_MENU_UPDATE(create_model )     /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( change_model_name )     /* ARGSUSED */
{
    object_struct    *current_object;
    STRING           name;

    if( get_current_object( display, &current_object ) &&
        current_object->object_type == MODEL )
    {
        print( "Enter the new model name: " );

        if( input_string( stdin, name, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            (void) strcpy( get_model_ptr(current_object)->filename, name );
        }

        (void) input_newline( stdin );

        rebuild_selected_list( display, menu_window );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_model_name )     /* ARGSUSED */
{
    return( current_object_is_this_type(display,MODEL) );
}

private  BOOLEAN  remove_current_object_from_hierarchy(
    display_struct   *display,
    object_struct    **object )
{
    BOOLEAN          removed;
    int              obj_index;
    model_struct     *current_model;

    if( !current_object_is_top_level( display ) &&
        get_current_object( display, object ) )
    {
        obj_index = get_current_object_index( display );

        current_model = get_current_model( display );

        remove_ith_object_from_model( current_model, obj_index );

        if( current_model->n_objects == 0 )
            obj_index = 0;
        else if( obj_index >= current_model->n_objects )
            obj_index = current_model->n_objects - 1;

        set_current_object_index( display, obj_index );

        graphics_models_have_changed( display );

        removed = TRUE;
    }
    else
        removed = FALSE;

    return( removed );
}

public  DEF_MENU_FUNCTION( delete_current_object )     /* ARGSUSED */
{
    object_struct    *object;

    if( get_current_object( display, &object ) &&
        get_object_type( object ) == POLYGONS &&
        get_polygons_ptr(object) ==
                   display->three_d.surface_extraction.polygons )
    {
        print( "Cannot delete the polygons used for surface extraction.\n" );
        return( OK );
    }

    if( remove_current_object_from_hierarchy( display, &object ) )
    {
        delete_object( object );
        pop_menu_one_level( display->associated[MENU_WINDOW] );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(delete_current_object )     /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( set_current_object_colour )   /* ARGSUSED */
{
    Status          status;
    object_struct   *current_object;
    Colour          col;
    STRING          line;

    if( get_current_object( display, &current_object ) &&
        get_object_type(current_object) != MODEL )
    {
        print( "Enter colour name or r g b:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
        {
            col = convert_string_to_colour( line );

            set_object_colour( current_object, col );

            set_update_required( display, NORMAL_PLANES );
            rebuild_selected_list( display, menu_window );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_current_object_colour )   /* ARGSUSED */
{
    return( current_object_exists(display) &&
            !current_object_is_this_type(display,MODEL) );
}

public  DEF_MENU_FUNCTION( set_current_object_surfprop )   /* ARGSUSED */
{
    object_struct   *current_object;
    Surfprop        spr;

    if( get_current_object( display, &current_object ) )
    {
        print( "Enter ambient, diffuse, specular, shininess, opacity:" );

        if( input_float( stdin, &Surfprop_a(spr) ) == OK &&
            input_float( stdin, &Surfprop_d(spr) ) == OK &&
            input_float( stdin, &Surfprop_s(spr) ) == OK &&
            input_float( stdin, &Surfprop_se(spr) ) == OK &&
            input_float( stdin, &Surfprop_t(spr) ) == OK )
        {
            set_object_surfprop( current_object, &spr );

            set_update_required( display, NORMAL_PLANES );
            rebuild_selected_list( display, menu_window );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_current_object_surfprop )   /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( cut_object )   /* ARGSUSED */
{
    object_struct   *object;
    model_struct    *cut_model;

    if( remove_current_object_from_hierarchy( display, &object ) )
    {
        cut_model = get_graphics_model( display, CUT_BUFFER_MODEL );

        add_object_to_model( cut_model, object );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(cut_object )   /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( paste_object )   /* ARGSUSED */
{
    int             n_objects;
    object_struct   *object;
    model_struct    *cut_model, *current_model;

    cut_model = get_graphics_model( display, CUT_BUFFER_MODEL );
    current_model = get_current_model( display );
    n_objects = cut_model->n_objects;

    while( cut_model->n_objects > 0 )
    {
        object = cut_model->objects[0];

        add_object_to_model( current_model, object );

        remove_ith_object_from_model( cut_model, 0 );
    }

    if( n_objects > 0 )
    {
        graphics_models_have_changed( display );
        rebuild_selected_list( display, menu_window );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(paste_object )   /* ARGSUSED */
{
    return( get_graphics_model( display, CUT_BUFFER_MODEL )->n_objects > 0 );
}

public  DEF_MENU_FUNCTION( mark_vertices )   /* ARGSUSED */
{
    object_struct  *object;
    int            i, n_points;
    Point          *points;
    STRING         label;

    if( get_current_object( display, &object ) &&
        object->object_type == LINES )
    {
        n_points = get_lines_ptr(object)->n_points;
        points = get_lines_ptr(object)->points;

        for_less( i, 0, n_points )
        {
            (void) sprintf( label, "%d", i );
            create_marker_at_position( display, &points[i], label );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(mark_vertices )   /* ARGSUSED */
{
    return( current_object_is_this_type(display,LINES) );
}

public  DEF_MENU_FUNCTION( flip_object )   /* ARGSUSED */
{
    object_struct  *object;
    int            i, n_points;
    Point          *points;
    Vector         *normals;

    if( get_current_object( display, &object ) )
    {
        n_points = get_object_points( object, &points );
        (void) get_object_normals( object, &normals );

        for_less( i, 0, n_points )
        {
            Point_x(points[i]) *= -1.0;
            Vector_x(normals[i]) *= -1.0;
        }

        if( object->object_type == POLYGONS )
            reverse_polygons_vertices( get_polygons_ptr(object) );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(flip_object )   /* ARGSUSED */
{
    return( current_object_exists(display) );
}

public  DEF_MENU_FUNCTION( scan_current_object_to_volume )   /* ARGSUSED */
{
    object_struct     *current_object;
    display_struct    *slice_window;

    if( get_current_object( display, &current_object ) &&
        get_slice_window( display, &slice_window ) &&
        get_n_volumes( slice_window ) > 0 )
    {
        if( Clear_before_polygon_scan )
            set_all_volume_label_data( get_label_volume(slice_window), 0 );

        scan_object_to_current_volume( slice_window, current_object );

        print( " done.\n" );

        set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(scan_current_object_to_volume )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 && current_object_exists(display) );
}
