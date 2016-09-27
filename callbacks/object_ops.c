/**
 * \file object_ops.c
 * \brief Menu commands for manipulating geometric objects and models.
 *
 * \copyright
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
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

/**
 * \brief Reverses the direction of the normal vectors of the currently
 * selected polygon(s).
 *
 * Default menu path: Polygons (Z) -> Reverse Normals (G)
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( reverse_normals )
{
    object_struct   *current_object;

    if( get_current_object( display, &current_object ) )
    {
        reverse_object_normals( current_object );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(reverse_normals )
{
    return( current_object_exists(display) );
}

static void
set_object_global_visibility( object_struct *object_ptr, VIO_BOOL is_visible )
{
  object_traverse_struct object_traverse;
  object_struct *sub_object_ptr;

  initialize_object_traverse( &object_traverse, FALSE, 1, &object_ptr);

  while (get_next_object_traverse(&object_traverse, &sub_object_ptr))
    set_object_visibility(sub_object_ptr, is_visible);
}

/**
 * Change the opacity of the currently selected object.
 *
 * \param display The display_struct of a the 3D view window.
 * \param delta The amount by which to change the opacity. This
 * should be a number between zero and one.
 */
VIO_Status
change_current_object_opacity(display_struct *display, VIO_Real delta)
{
    object_struct   *current_object;

    if( get_current_object( display, &current_object ) )
    {
      VIO_Real opacity;
      VIO_Surfprop *spr_ptr = get_object_surfprop( current_object );
      if (spr_ptr == NULL)
      {
          return VIO_OK;
      }
      opacity = Surfprop_t(*spr_ptr);
      opacity += delta;
      /* Force the opacity to stay within the interval [0, 1]. */
      if (opacity > 1.0)
        opacity = 1.0;
      if (opacity < 0.0)
        opacity = 0.0;
      Surfprop_t(*spr_ptr) = opacity;
      set_object_surfprop( current_object, spr_ptr );
      set_update_required( display, NORMAL_PLANES );
      /* Is this really necessary? */
      rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
    }
    return( VIO_OK );
}

/** 
 * \brief Make the current object on the marker list invisible, and 
 * advance to the next object, making it visible.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( advance_visible )
{
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) )
    {
        set_object_global_visibility( current_object, FALSE );

        advance_current_object( display );

        if( get_current_object( display, &current_object ) )
          set_object_global_visibility( current_object, TRUE );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(advance_visible )
{
    return( current_object_exists(display) );
}

/** 
 * \brief Make the current object on the marker list invisible, and 
 * move to the previous object, making it visible.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( retreat_visible )
{
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) )
    {
        set_object_global_visibility( current_object, FALSE );

        retreat_current_object( display );

        if( get_current_object( display, &current_object ) )
            set_object_global_visibility( current_object, TRUE );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(retreat_visible )
{
    return( current_object_exists(display) );
}

/**
 * \brief Hide all of the objects contained within the currently selected 
 * model.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( make_all_invisible )
{
    object_struct           *current_object;

    if( get_current_object( display, &current_object ) )
    {
        set_object_global_visibility( current_object, FALSE );
        graphics_models_have_changed( display );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(make_all_invisible )
{
    return( current_object_exists(display) );
}

/**
 * \brief Show all of the objects contained within the currently selected 
 * model.
 */

/* ARGSUSED */

DEF_MENU_FUNCTION( make_all_visible )
{
    object_struct           *current_object;

    if( get_current_object( display, &current_object ) )
    {
        set_object_global_visibility( current_object, TRUE );
        graphics_models_have_changed( display );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(make_all_visible )
{
    return( current_object_exists(display) );
}

/**
 * \brief Select the object after the current object in the marker list.
 *
 * \param display The top-level display structure.
 *
 * Default menu path: Down (Right arrow)
 */

/* ARGSUSED */

DEF_MENU_FUNCTION( advance_selected )
{
    advance_current_object( display );

    rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(advance_selected )
{
    return( current_object_exists(display) );
}

/**
 * \brief Select the object before the current object in the marker list.
 *
 * \param display The top-level display structure.
 */

/* ARGSUSED */

DEF_MENU_FUNCTION( retreat_selected )
{
    retreat_current_object( display );

    rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(retreat_selected )
{
    return( current_object_exists(display) );
}

/**
 * \brief Descend into the currently selected model on the marker list.
 *
 * If the object is a model containing other objects, the marker list
 * will be updated to show the substructure, if any, associated  with the
 * currently selected model.
 *
 * \param display The top-level display structure.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( descend_selected )
{
    rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(descend_selected )
{
    return( current_object_is_this_type(display,MODEL) );
}

/**
 * \brief Pop the marker list display.
 *
 * If the marker list is currently displaying the substructure of a model,
 * the marker list will be "popped" back to the next higher level.
 *
 * \param display The top-level display structure.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( ascend_selected )
{
    rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(ascend_selected )
{
    return( display->three_d.current_object > 1 );
}

/* ARGSUSED */
/** 
 * \brief Toggle the visibility state of the currently selected object.
 */

DEF_MENU_FUNCTION( toggle_object_visibility )
{
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) )
    {
        set_object_global_visibility( current_object,
                                      !current_object->visibility );
        graphics_models_have_changed( display );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_object_visibility )
{
    return( current_object_exists(display) );
}

/**
 * \brief Create a new, empty model and place it in the marker list. 
 * 
 * Models are container objects that can be used to hold other objects,
 * including lines, polygons, and markers.
 */

/* ARGSUSED */

DEF_MENU_FUNCTION( create_model )
{
    create_model_after_current( display );

    graphics_models_have_changed( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(create_model )
{
    return( TRUE );
}

/* ARGSUSED */

/**
 * \brief Change the name of the model that is currently selected in the
 * marker list.
 */
DEF_MENU_FUNCTION( change_model_name )
{
    object_struct    *current_object;
    VIO_STR          name;

    if( get_current_object( display, &current_object ) &&
        current_object->object_type == MODEL )
    {
        if (get_user_input( "Enter the new model name: ", "s", &name) == VIO_OK)
        {
            replace_string( &get_model_ptr(current_object)->filename, name );
        }
        rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(change_model_name )
{
    return( current_object_is_this_type(display,MODEL) );
}

/**
 * \brief Delete the currently selected object.
 *
 * Has the side effect of moving the cursor to the location of the currently
 * selected marker before deleting it.
 *
 * Default menu path: Objects (R) -> Delete Object (Q) -> Really Delete (B)
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( delete_current_object )
{
    object_struct    *object;

    /* If we are about to delete a marker, immediately move 
     * the cursor to the position of the marker we are about
     * to delete.
     */
    set_cursor_to_marker(display, menu_window, menu_entry);

    if( get_current_object( display, &object ) &&
        get_object_type( object ) == POLYGONS &&
        get_polygons_ptr(object) == display->three_d.surface_extraction.polygons )
    {
        print( "Cannot delete the polygons used for surface extraction.\n" );
        return( VIO_OK );
    }

    if( remove_current_object_from_hierarchy( display, &object ) )
    {
        graphics_models_have_changed( display );

        clear_label_connected_3d(display, menu_window, menu_entry);
        delete_object( object );
        pop_menu_one_level( get_display_by_type( MENU_WINDOW ) );
    }
    rebuild_selected_list( get_display_by_type( THREE_D_WINDOW ), 
                           get_display_by_type( MARKER_WINDOW ) );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(delete_current_object )
{
    return( current_object_exists(display) );
}

/**
 * \brief Prompt the user for a new colour for the currently selected object.
 *
 * If the user fails to enter a valid colour, will just set the color to 
 * black.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( set_current_object_colour )
{
    object_struct   *current_object;
    VIO_Colour      col;
    VIO_STR         line;

    if( get_current_object( display, &current_object ) &&
        get_object_type(current_object) != MODEL )
    {
        if( get_user_input( "Enter colour name or 3 or 4 colour components: ",
                            "s", &line ) == VIO_OK )
        {
            if (string_to_colour( line, &col ) == VIO_OK )
            {
                set_object_colour( current_object, col );
                graphics_models_have_changed( display );
            }
            delete_string( line );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_current_object_colour )
{
    return( current_object_exists(display) &&
            !current_object_is_this_type(display,MODEL) );
}

/**
 * \brief Set the surface properties for the currently selected object.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( set_current_object_surfprop )
{
    object_struct   *current_object;
    VIO_Surfprop    spr;

    if( get_current_object( display, &current_object ) )
    {
      if (get_user_input( "Enter ambient, diffuse, specular, shininess, opacity: ",
                          "fffff",
                          &Surfprop_a(spr),
                          &Surfprop_d(spr),
                          &Surfprop_s(spr),
                          &Surfprop_se(spr),
                          &Surfprop_t(spr) ) == VIO_OK )
        {
            set_object_surfprop( current_object, &spr );

            set_update_required( display, NORMAL_PLANES );
            rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_current_object_surfprop )
{
    return( current_object_exists(display) );
}

/**
 * \brief Cut the currently selected object from the marker list, and add 
 * it to the cut buffer.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( cut_object )
{
    object_struct   *object;
    model_struct    *cut_model;

    if( remove_current_object_from_hierarchy( display, &object ) )
    {
        graphics_models_have_changed( display );

        cut_model = get_graphics_model( display, CUT_BUFFER_MODEL );

        add_object_to_model( cut_model, object );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(cut_object )
{
    return( current_object_exists(display) );
}

/**
 * \brief Move objects from the cut buffer to the currently selected model.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( paste_object )
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
        rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(paste_object )
{
    return( get_graphics_model( display, CUT_BUFFER_MODEL )->n_objects > 0 );
}

/**
 * \brief Create a marker for each vertex in the current object.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( mark_vertices )
{
    object_struct  *object;
    int            i, n_points;
    VIO_Point      *points;
    char           label[VIO_EXTREMELY_LARGE_STRING_SIZE];

    if( get_current_object( display, &object ) )
    {
        n_points = get_object_points( object, &points );

        snprintf( label, VIO_EXTREMELY_LARGE_STRING_SIZE,
                  "This command will create %d markers and cannot be undone. "
                  "Are you sure you want to do this",
                  n_points );
        if ( get_user_yes_or_no( label ) == VIO_OK )
        {
            for_less( i, 0, n_points )
            {
                (void) sprintf( label, "%d", i );
                create_marker_at_position( display, &points[i], label );
            }
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(mark_vertices )
{
    return( current_object_exists(display) );
}

/**
 * \brief Completely invert a geometric object.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( flip_object )
{
    object_struct  *object;
    int            i, n_points, n_normals;
    VIO_Point      *points;
    VIO_Vector     *normals;

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

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(flip_object )
{
    return( current_object_exists(display) );
}

/**
 * \brief Create an approximation of an object or surface in the current label volume.
 */
/* ARGSUSED */

DEF_MENU_FUNCTION( scan_current_object_to_volume )
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

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(scan_current_object_to_volume )
{
    return( get_n_volumes(display) > 0 && current_object_exists(display) );
}
