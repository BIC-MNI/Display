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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/object_ops.c,v 1.57 2001/05/27 00:19:38 stever Exp $";
#endif


#include  <display.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION( reverse_normals )
{
    object_struct   *current_object;

    if( get_current_object( display, &current_object ) )
    {
        reverse_object_normals( current_object );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(reverse_normals )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( advance_visible )
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

/* ARGSUSED */

public  DEF_MENU_UPDATE(advance_visible )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( retreat_visible )
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

/* ARGSUSED */

public  DEF_MENU_UPDATE(retreat_visible )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( make_all_invisible )
{
    object_struct           *object, *current_object;
    object_traverse_struct  object_traverse;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1,&current_object);

        while( get_next_object_traverse(&object_traverse,&object) )
               set_object_visibility( object, OFF );

        graphics_models_have_changed( display );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(make_all_invisible )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( make_all_visible )
{
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1,&current_object);

        while( get_next_object_traverse(&object_traverse,&object) )
            set_object_visibility( object, ON );

        graphics_models_have_changed( display );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(make_all_visible )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( advance_selected )
{
    advance_current_object( display );

    rebuild_selected_list( display, menu_window );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(advance_selected )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( retreat_selected )
{
    retreat_current_object( display );

    rebuild_selected_list( display, menu_window );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(retreat_selected )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( descend_selected )
{
    push_current_object( display );

    rebuild_selected_list( display, menu_window );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(descend_selected )
{
    return( current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( ascend_selected )
{
    pop_current_object( display );

    rebuild_selected_list( display, menu_window );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(ascend_selected )
{
    return( display->three_d.current_object.current_level > 1 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_object_visibility )
{
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) )
    {
        current_object->visibility = !current_object->visibility;

        graphics_models_have_changed( display );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_object_visibility )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( create_model )
{
    create_model_after_current( display );

    graphics_models_have_changed( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(create_model )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( change_model_name )
{
    object_struct    *current_object;
    STRING           name;

    if( get_current_object( display, &current_object ) &&
        current_object->object_type == MODEL )
    {
        print( "Enter the new model name: " );

        if( input_string( stdin, &name, ' ' ) == OK )
        {
            replace_string( &get_model_ptr(current_object)->filename, name );
        }

        (void) input_newline( stdin );

        rebuild_selected_list( display, menu_window );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(change_model_name )
{
    return( current_object_is_this_type(display,MODEL) );
}

public  BOOLEAN  remove_current_object_from_hierarchy(
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

/* ARGSUSED */

public  DEF_MENU_FUNCTION( delete_current_object )
{
    object_struct    *object;
    display_struct   *slice_window;
    Real              voxel[MAX_DIMENSIONS];
    BOOLEAN           changed;
    int               volume_index;

    slice_window = display->associated[SLICE_WINDOW];
    set_cursor_to_marker(display, menu_window, menu_entry);
    if( slice_window != (display_struct  *) 0 )
    {
        (void) update_voxel_from_cursor( slice_window );
    }

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
        clear_label_connected_3d(display, menu_window, menu_entry);
        delete_object( object );
        pop_menu_one_level( display->associated[MENU_WINDOW] );
    }
    rebuild_selected_list( display->associated[THREE_D_WINDOW], display->associated[MARKER_WINDOW]);

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(delete_current_object )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_current_object_colour )
{
    object_struct   *current_object;
    Colour          col;
    STRING          line;

    if( get_current_object( display, &current_object ) &&
        get_object_type(current_object) != MODEL )
    {
        print( "Enter colour name or 3 or 4 colour components: " );


        if( input_line( stdin, &line ) == OK )
        {
            col = convert_string_to_colour( line );

            set_object_colour( current_object, col );

            set_update_required( display, NORMAL_PLANES );
            rebuild_selected_list( display, menu_window );
        }

        delete_string( line );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_current_object_colour )
{
    return( current_object_exists(display) &&
            !current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_current_object_surfprop )
{
    object_struct   *current_object;
    Surfprop        spr;

    if( get_current_object( display, &current_object ) )
    {
        print( "Enter ambient, diffuse, specular, shininess, opacity: " );

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

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_current_object_surfprop )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( cut_object )
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

/* ARGSUSED */

public  DEF_MENU_UPDATE(cut_object )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( paste_object )
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

/* ARGSUSED */

public  DEF_MENU_UPDATE(paste_object )
{
    return( get_graphics_model( display, CUT_BUFFER_MODEL )->n_objects > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( mark_vertices )
{
    object_struct  *object;
    int            i, n_points;
    Point          *points;
    char           label[EXTREMELY_LARGE_STRING_SIZE];

    if( get_current_object( display, &object ) )
    {
        n_points = get_object_points( object, &points );

        for_less( i, 0, n_points )
        {
            (void) sprintf( label, "%d", i );
            create_marker_at_position( display, &points[i], label );
        }
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(mark_vertices )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( flip_object )
{
    object_struct  *object;
    int            i, n_points, n_normals;
    Point          *points;
    Vector         *normals;

    if( get_current_object( display, &object ) )
    {
        n_points = get_object_points( object, &points );
        n_normals = get_object_normals( object, &normals );

        for_less( i, 0, n_points )
            Point_x(points[i]) *= -1.0f;

        for_less( i, 0, n_normals )
            Vector_x(normals[i]) *= -1.0f;

        if( object->object_type == POLYGONS )
            reverse_polygons_vertices( get_polygons_ptr(object) );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(flip_object )
{
    return( current_object_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( scan_current_object_to_volume )
{
    object_struct     *current_object;
    display_struct    *slice_window;

    if( get_current_object( display, &current_object ) &&
        get_slice_window( display, &slice_window ) &&
        get_n_volumes( slice_window ) > 0 )
    {
        if( Clear_before_polygon_scan )
            clear_labels( slice_window, get_current_volume_index(slice_window));

        scan_object_to_current_volume( slice_window, current_object );

        print( " done.\n" );

        set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(scan_current_object_to_volume )
{
    return( get_n_volumes(display) > 0 && current_object_exists(display) );
}
