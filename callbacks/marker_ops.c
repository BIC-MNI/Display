/**
 * \file marker_ops.c
 * \brief Menu commands to manipulate markers.
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
 * Retrieves a pointer to the currently selected marker, if any.
 *
 * \param display A pointer to a top-level display_struct.
 * \param marker Location for the marker_struct pointer retrieved.
 */
static  VIO_BOOL  get_current_marker(
    display_struct    *display,
    marker_struct     **marker )
{
    VIO_BOOL                found;
    object_struct           *current_object, *object;
    object_traverse_struct  obj_traverse;

    found = FALSE;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &obj_traverse, FALSE, 1, &current_object );

        while( get_next_object_traverse( &obj_traverse, &object ) )
        {
            if( !found && object->object_type == MARKER )
            {
                found = TRUE;
                *marker = get_marker_ptr( object );
            }
        }
    }

    return( found );
}

/**
 * Initialize a marker object to the current default values.
 *
 * \param display A pointer to a top-level display_struct.
 * \param marker A pointer to the marker_struct to update.
 */
static void  set_marker_to_defaults(
    display_struct  *display,
    marker_struct   *marker )
{
    marker->type = display->three_d.default_marker_type;
    marker->colour = display->three_d.default_marker_colour;
    marker->size = display->three_d.default_marker_size;
    replace_string( &marker->label,
                    create_string( display->three_d.default_marker_label ) );
    marker->structure_id = display->three_d.default_marker_structure_id;
    marker->patient_id = display->three_d.default_marker_patient_id;
}

/**
 * Get the position in world coordinates associated with the mouse, if 
 * possible. Otherwise returns the position of the cursor.
 *
 * \param display A pointer to a top-level display_struct.
 * \param pos A VIO_Point to be filled in with the appropriate position.
 */
static  void  get_position_pointed_to(
    display_struct   *display,
    VIO_Point        *pos )
{
    int             axis_index, volume_index;
    VIO_Real        voxel[VIO_MAX_DIMENSIONS];
    VIO_Real        x_w, y_w, z_w;

    if( get_voxel_under_mouse( display, &volume_index, &axis_index, voxel ) )
    {
        convert_voxel_to_world( get_nth_volume(display, volume_index),
                                voxel, &x_w, &y_w, &z_w );
        fill_Point( *pos, x_w, y_w, z_w );
    }
    else
    {
        get_cursor_origin( display, pos );
    }
}

/**
 * Public function to create a marker at a particular point in world space.
 *
 * \param display A pointer to a top-level display_struct.
 * \param position The desired 3D point where the marker should be placed.
 * \param label A text label to associate with the marker.
 */
void create_marker_at_position(
    display_struct    *display,
    VIO_Point         *position,
    VIO_STR           label )
{
    object_struct   *object;
    marker_struct   *marker;

    object = create_object( MARKER );
    marker = get_marker_ptr( object );

    initialize_marker( marker, BOX_MARKER, WHITE );

    marker->position = *position;

    set_marker_to_defaults( display, marker );

    if( label != NULL )
        replace_string( &marker->label, create_string( label ) );

    add_object_to_current_model( display, object, FALSE );
}

/**
 * Command to create a new marker at the current cursor position,
 * using the current marker default properties.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( create_marker_at_cursor )
{
    VIO_Point           position;

    get_position_pointed_to( display, &position );

    create_marker_at_position( display, &position, NULL );

    return( VIO_OK );
}

DEF_MENU_UPDATE( create_marker_at_cursor )
{
    return( TRUE );
}

/** 
 * Command to move the crosshair cursor to the position of the currently
 * selected marker.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( set_cursor_to_marker )
{
    marker_struct   *marker;

    if( get_current_marker( display, &marker ) )
    {
        set_cursor_origin( display, &marker->position );
        update_voxel_from_cursor( display );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( set_cursor_to_marker )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to save all markers to a .tag file.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( save_markers )
{
    VIO_Status              status;
    object_struct           *object, *current_object;
    VIO_Volume              volume;
    VIO_STR                 filename;
    FILE                    *file;
    marker_struct           *marker;
    object_traverse_struct  obj_traverse;
    int                     n_tags;
    VIO_Real                **tags, *weights;
    int                     *structure_ids, *patient_ids;
    VIO_STR                 *labels;

    object = get_current_model_object( display );

    status = get_user_file( "Enter filename: ", TRUE,
                            get_default_tag_file_suffix(), &filename );
    if (status != VIO_OK)
    {
        return VIO_ERROR;
    }

    if( !get_slice_window_volume( display, &volume ) )
        volume = (VIO_Volume) NULL;

    n_tags = 0;
    tags = NULL;
    weights = NULL;
    structure_ids = NULL;
    patient_ids = NULL;
    labels = NULL;
    if( status == VIO_OK )
    {
        initialize_object_traverse( &obj_traverse, TRUE, 1, &object );

        while( get_next_object_traverse(&obj_traverse, &current_object) )
        {
            if( current_object->object_type == MARKER )
            {
                marker = get_marker_ptr(current_object);
                SET_ARRAY_SIZE( tags, n_tags, n_tags+1, DEFAULT_CHUNK_SIZE);
                ALLOC( tags[n_tags], VIO_N_DIMENSIONS );
                SET_ARRAY_SIZE( weights, n_tags, n_tags+1, DEFAULT_CHUNK_SIZE);
                SET_ARRAY_SIZE( structure_ids, n_tags, n_tags+1,
                                DEFAULT_CHUNK_SIZE);
                SET_ARRAY_SIZE( patient_ids, n_tags, n_tags+1,
                                DEFAULT_CHUNK_SIZE);
                SET_ARRAY_SIZE( labels, n_tags, n_tags+1, DEFAULT_CHUNK_SIZE);
                labels[n_tags] = create_string( marker->label );

                tags[n_tags][VIO_X] = (VIO_Real) Point_x(marker->position);
                tags[n_tags][VIO_Y] = (VIO_Real) Point_y(marker->position);
                tags[n_tags][VIO_Z] = (VIO_Real) Point_z(marker->position);
                weights[n_tags] = (VIO_Real) marker->size;
                structure_ids[n_tags] = marker->structure_id;
                patient_ids[n_tags] = marker->patient_id;
                ++n_tags;
            }
        }
    }

    if( status == VIO_OK )
        status = open_file_with_default_suffix( filename,
                              get_default_tag_file_suffix(), WRITE_FILE,
                              ASCII_FORMAT, &file );

    if( status == VIO_OK )
        status = output_tag_points( file, (char *) NULL, 1, n_tags,
                                    tags, (VIO_Real **) NULL, weights,
                                    structure_ids, patient_ids, labels );

    if( n_tags > 0 )
    {
        free_tag_points( 1, n_tags, tags, (VIO_Real **) NULL,
                         weights, structure_ids, patient_ids, labels );
    }

    if( status == VIO_OK )
        status = close_file( file );

    delete_string( filename );

    print( "Done.\n" );

    return( status );
}

DEF_MENU_UPDATE( save_markers )
{
    return( TRUE );
}

/**
 * Command to set the current default marker structure ID.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( set_default_marker_structure_id )
{
    int             id;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    snprintf( prompt, sizeof( prompt ),
              "The current default marker id is: %d\nEnter the new value: ",
              display->three_d.default_marker_structure_id );

    if( get_user_input( prompt, "d", &id ) == VIO_OK )
    {
        display->three_d.default_marker_structure_id = id;
        print( "The new default marker id is: %d\n",
               display->three_d.default_marker_structure_id );

    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( set_default_marker_structure_id )
{
    set_menu_text_int( menu_window, menu_entry,
                       display->three_d.default_marker_structure_id );

    return( TRUE );
}

/**
 * Command to set the current default marker patient ID.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( set_default_marker_patient_id )
{
    int             id;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    snprintf( prompt, sizeof( prompt ),
              "The current default marker id is: %d\nEnter the new value: ",
              display->three_d.default_marker_patient_id );

    if( get_user_input( prompt, "d", &id ) == VIO_OK )
    {
        display->three_d.default_marker_patient_id = id;
        print( "The new default marker id is: %d\n",
               display->three_d.default_marker_patient_id );

    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( set_default_marker_patient_id )
{
    set_menu_text_int( menu_window, menu_entry,
                       display->three_d.default_marker_patient_id );

    return( TRUE );
}

/**
 * Command to change the size used for new markers.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( set_default_marker_size )
{
    VIO_Real        size;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    snprintf( prompt, sizeof( prompt ),
              "The current default marker size is: %g\nEnter the new value: ",
              display->three_d.default_marker_size );

    if( get_user_input( prompt, "r", &size ) == VIO_OK )
    {
        display->three_d.default_marker_size = size;
        print( "The new default marker size is: %g\n",
               display->three_d.default_marker_size );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( set_default_marker_size )
{
    set_menu_text_real( menu_window, menu_entry,
                        display->three_d.default_marker_size );

    return( TRUE );
}

/**
 * Command to change the colour used for new markers.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( set_default_marker_colour )
{
    VIO_Status  status;
    VIO_STR     string;
    VIO_Colour  colour;
    char        prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    string = convert_colour_to_string( display->three_d.default_marker_colour );

    snprintf( prompt, sizeof( prompt ),
              "The current default marker colour is: %s\n"
              "Enter the new colour name or 3 or 4 colour components: ",
              string );

    delete_string( string );

    status = get_user_input( prompt, "s", &string );

    if( status == VIO_OK )
    {
        status = string_to_colour( string, &colour );
        delete_string( string );
        if (status == VIO_OK )
        {
            display->three_d.default_marker_colour = colour;
            string = convert_colour_to_string( colour );
            print( "The new default marker colour is: %s\n", string );
            delete_string( string );
        }
    }
    return( status );
}

DEF_MENU_UPDATE( set_default_marker_colour )
{
    set_menu_text_with_colour( menu_window, menu_entry,
                               display->three_d.default_marker_colour );

    return( TRUE );
}

/**
 * Command to change the type used for new markers.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( set_default_marker_type )
{
    int type = (int) display->three_d.default_marker_type + 1;
    if (type >= N_MARKER_TYPES)
    {
        type = 0;
    }
    display->three_d.default_marker_type = (Marker_types) type;
    print( "The new default marker type is: %d\n",
           (int) display->three_d.default_marker_type );
    return( VIO_OK );
}

DEF_MENU_UPDATE( set_default_marker_type )
{
    VIO_STR    name;

    switch( display->three_d.default_marker_type )
    {
    case BOX_MARKER:
        name = "Cube";
        break;

    case SPHERE_MARKER:
        name = "Sphere";
        break;

    default:
        name = "Undefined";
        break;
    }
    set_menu_text_string( menu_window, menu_entry, name );
    return( TRUE );
}

/**
 * Command to change the structure ID of the currently selected marker.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( set_default_marker_label )
{
    VIO_Status   status;
    VIO_STR      label;
    char         prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    snprintf( prompt, sizeof( prompt ),
              "The current default marker label is: %s\n"
              "Enter the new default label: ",
              display->three_d.default_marker_label );

    status = get_user_input( prompt, "s", &label );

    if( status == VIO_OK )
    {
        replace_string( &display->three_d.default_marker_label, label );
        print( "The new default marker label is: %s\n",
               display->three_d.default_marker_label );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( set_default_marker_label )
{
    set_menu_text_string( menu_window, menu_entry,
                          display->three_d.default_marker_label );

    return( TRUE );
}

/**
 * Command to change the structure ID of all markers in the currently
 * selected model.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( change_marker_structure_id )
{
    int                     id;
    marker_struct           *marker;
    object_struct           *current_object, *object;
    object_traverse_struct  obj_traverse;

    if (get_user_input( "Enter the new structure id: ", "d", &id) == VIO_OK &&
        get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &obj_traverse, FALSE, 1, &current_object);

        while( get_next_object_traverse( &obj_traverse, &object ) )
        {
            if( object->object_type == MARKER )
            {
                marker = get_marker_ptr( object );
                marker->structure_id = id;
            }
        }

        rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( change_marker_structure_id )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to change the patient ID of all markers in the current model.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( change_marker_patient_id )
{
    int                     id;
    marker_struct           *marker;
    object_struct           *current_object, *object;
    object_traverse_struct  obj_traverse;

    if (get_user_input( "Enter the new patient id: ", "d", &id ) == VIO_OK &&
        get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &obj_traverse, FALSE, 1, &current_object);

        while( get_next_object_traverse( &obj_traverse, &object) )
        {
            if( object->object_type == MARKER )
            {
                marker = get_marker_ptr( object );
                marker->patient_id = id;
            }
        }

        rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( change_marker_patient_id )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to change the type of the currently selected marker.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( change_marker_type )
{
    int             type;
    marker_struct   *marker;

    if( get_current_marker(display, &marker) )
    {
        type = (int) marker->type + 1;
        if (type >= N_MARKER_TYPES)
        {
            type = 0;
        }
        marker->type = (Marker_types) type;
        print( "The new value of this marker type is: %d\n",
               (int) marker->type );
        graphics_models_have_changed( display );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( change_marker_type )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to change the colour of the currently selected marker.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( change_marker_colour )
{
    marker_struct   *marker;

    if( get_current_marker(display, &marker) )
    {
        VIO_STR    string = convert_colour_to_string( marker->colour );
        char       prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];
        VIO_Colour colour;
        VIO_Status status;

        snprintf( prompt, sizeof( prompt ),
                  "The current marker colour is: %s\n"
                  "Enter the new colour name or 3 or 4 colour components: ",
                  string );

        delete_string( string );

        status = get_user_input( prompt, "s", &string );
        if( status == VIO_OK )
        {
            status = string_to_colour( string, &colour );
            delete_string( string );
            if ( status == VIO_OK )
            {
                marker->colour = colour;
                string = convert_colour_to_string( colour );
                print( "The new colour of this marker is: %s\n", string );
                delete_string( string );
                graphics_models_have_changed( display );
            }
        }
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( change_marker_colour )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to change the size of the currently selected marker.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( change_marker_size )
{
    VIO_Real        size;
    marker_struct   *marker;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    if( get_current_marker(display, &marker) )
    {
        snprintf( prompt, sizeof( prompt ),
                  "The current size of this marker is: %g\n"
                  "Enter the new value: ",
                  marker->size );

        if( get_user_input( prompt, "r", &size ) == VIO_OK && size >= 0.0 )
        {
            marker->size = size;
            print( "The new size of this marker is: %g\n", marker->size );
            graphics_models_have_changed( display );
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE( change_marker_size )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to change the position of the currently selected marker, by
 * moving the marker to the current mouse (or cursor) position.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( change_marker_position )
{
    marker_struct   *marker;

    if( get_current_marker( display, &marker ) )
    {
        get_position_pointed_to( display, &marker->position );

        print( "Marker position changed to: %g %g %g\n",
               Point_x(marker->position),
               Point_y(marker->position),
               Point_z(marker->position) );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE( change_marker_position )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to change the label of the currently selected marker.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( change_marker_label )
{
    VIO_STR         label;
    marker_struct   *marker;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    if( get_current_marker(display,&marker) )
    {
        snprintf( prompt, sizeof( prompt ),
                  "The current marker label is: %s\nEnter the new label: ",
                  marker->label );

        if( get_user_input( prompt, "s", &label ) == VIO_OK )
        {
            replace_string( &marker->label, label );
            print( "The new marker label is: %s\n", marker->label );
            graphics_models_have_changed( display );
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE( change_marker_label )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to copy the current default settings to the currently selected
 * marker.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( copy_defaults_to_marker )
{
    marker_struct   *marker;

    if( get_current_marker(display,&marker) )
    {
        set_marker_to_defaults( display, marker );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE( copy_defaults_to_marker )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/**
 * Command to copy the current default settings to all markers whose
 * patient and structure ids match that of the current marker.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( copy_defaults_to_markers )
{
    int                     patient_id, structure_id;
    marker_struct           *marker;
    object_struct           *object, *current_object;
    object_traverse_struct  obj_traverse;

    if( get_current_marker( display, &marker) )
    {
        patient_id = marker->patient_id;
        structure_id = marker->structure_id;

        object = display->models[THREED_MODEL];
        initialize_object_traverse( &obj_traverse, TRUE, 1, &object );

        while( get_next_object_traverse( &obj_traverse, &current_object ) )
        {
            if( current_object->object_type == MARKER )
            {
                marker = get_marker_ptr( current_object );

                if( marker->patient_id == patient_id &&
                    marker->structure_id == structure_id )
                {
                    set_marker_to_defaults( display, marker );
                }
            }
        }
        graphics_models_have_changed( display );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE( copy_defaults_to_markers )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/** 
 * Command to move the cursor to the home (0,0,0) position in world
 * coordinates.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param menu_window A pointer to the display_struct of the menu window.
 * \param menu_entry  A pointer to the menu_entry for this command.
 * \returns VIO_OK if successful.
 */
DEF_MENU_FUNCTION( move_cursor_to_home )
{
    set_cursor_origin( display, &Cursor_home );
    update_voxel_from_cursor( display );
    return( VIO_OK );
}

DEF_MENU_UPDATE( move_cursor_to_home )
{
    return( TRUE );
}
