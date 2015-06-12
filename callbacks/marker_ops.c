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

static  VIO_BOOL  get_current_marker(
    display_struct    *display,
    marker_struct     **marker )
{
    VIO_BOOL                 found;
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    found = FALSE;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1,&current_object);

        while( get_next_object_traverse(&object_traverse,&object) )
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

void  set_marker_to_defaults(
    display_struct  *display,
    marker_struct   *marker )
{
    marker->type = display->three_d.default_marker_type;
    marker->colour = display->three_d.default_marker_colour;
    marker->size = display->three_d.default_marker_size;
    delete_string( marker->label );
    marker->label = create_string( display->three_d.default_marker_label );
    marker->structure_id = display->three_d.default_marker_structure_id;
    marker->patient_id = display->three_d.default_marker_patient_id;
}

static  void  get_position_pointed_to(
    display_struct   *display,
    VIO_Point            *pos )
{
    int             axis_index, volume_index;
    VIO_Real        voxel[VIO_MAX_DIMENSIONS];
    VIO_Real        x_w, y_w, z_w;

    if( get_voxel_under_mouse( display, &volume_index, &axis_index, voxel ) )
    {
        convert_voxel_to_world( get_nth_volume(display,volume_index),
                                voxel, &x_w, &y_w, &z_w );
        fill_Point( *pos, x_w, y_w, z_w );
    }
    else
    {
        get_cursor_origin(display, pos);
    }
}

void  create_marker_at_position(
    display_struct    *display,
    VIO_Point             *position,
    VIO_STR            label )
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

    add_object_to_current_model( display, object );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( create_marker_at_cursor )
{
    VIO_Point           position;

    get_position_pointed_to( display, &position );

    create_marker_at_position( display, &position, NULL );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(create_marker_at_cursor )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_cursor_to_marker )
{
    object_struct   *object;
    display_struct  *slice_window;

    if( get_current_object( display, &object ) &&
        object->object_type == MARKER )
    {
        set_cursor_origin(display, &get_marker_ptr(object)->position);

        slice_window = display->associated[SLICE_WINDOW];
        if( slice_window != (display_struct  *) 0 )
        {
            (void) update_voxel_from_cursor( slice_window );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_cursor_to_marker )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( save_markers )
{
    VIO_Status              status;
    object_struct           *object, *current_object;
    VIO_Volume              volume;
    VIO_STR                 filename;
    FILE                    *file;
    marker_struct           *marker;
    object_traverse_struct  object_traverse;
    int                     n_tags;
    VIO_Real                **tags, *weights;
    int                     *structure_ids, *patient_ids;
    VIO_STR                 *labels;

    object = get_current_model_object( display );

    status = get_user_file( "Enter filename: ", TRUE, &filename );

    if( status == VIO_OK && !check_clobber_file_default_suffix( filename,
                                             get_default_tag_file_suffix() ) )
        status = VIO_ERROR;

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
        initialize_object_traverse( &object_traverse, TRUE, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
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

/* ARGSUSED */

DEF_MENU_UPDATE(save_markers )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_default_marker_structure_id )
{
    int             id;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    sprintf( prompt, 
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

/* ARGSUSED */

DEF_MENU_UPDATE(set_default_marker_structure_id )
{
    set_menu_text_int( menu_window, menu_entry,
                       display->three_d.default_marker_structure_id );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_default_marker_patient_id )
{
    int             id;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    sprintf( prompt, 
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

/* ARGSUSED */

DEF_MENU_UPDATE(set_default_marker_patient_id )
{
    set_menu_text_int( menu_window, menu_entry,
                       display->three_d.default_marker_patient_id );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_default_marker_size )
{
    VIO_Real        size;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    sprintf( prompt,
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

/* ARGSUSED */

DEF_MENU_UPDATE(set_default_marker_size )
{
    set_menu_text_real( menu_window, menu_entry,
                        display->three_d.default_marker_size );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_default_marker_colour )
{
    VIO_Status  status;
    VIO_STR     string;
    VIO_Colour  colour;
    char        prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    string = convert_colour_to_string( display->three_d.default_marker_colour );

    sprintf( prompt, 
             "The current default marker colour is: %s\n"
             "Enter the new colour name or 3 or 4 colour components: ",
             string );

    delete_string( string );

    status = get_user_input( prompt, "s", &string );

    if( status == VIO_OK )
        colour = convert_string_to_colour( string );

    delete_string( string );

    if( status == VIO_OK )
    {
        display->three_d.default_marker_colour = colour;

        string = convert_colour_to_string(
                     display->three_d.default_marker_colour );

        print( "The new default marker colour is: %s\n", string );

        delete_string( string );
    }

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_default_marker_colour )
{
    set_menu_text_with_colour( menu_window, menu_entry,
                               display->three_d.default_marker_colour );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_default_marker_type )
{
    int       type;
    char      prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    sprintf( prompt, 
             "The current default marker type is: %d\nEnter the new type [0-%d]:",
             (int) display->three_d.default_marker_type,
             N_MARKER_TYPES-1);

    if( get_user_input( prompt, "d", &type ) == VIO_OK &&
        type >= 0 && type < N_MARKER_TYPES )
    {
        display->three_d.default_marker_type = (Marker_types) type;
        print( "The new default marker type is: %d\n",
               (int) display->three_d.default_marker_type );

    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_default_marker_type )
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

/* ARGSUSED */

DEF_MENU_FUNCTION( set_default_marker_label )
{
    VIO_Status   status;
    VIO_STR      label;
    char         prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    sprintf( prompt, 
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

/* ARGSUSED */

DEF_MENU_UPDATE(set_default_marker_label )
{
    set_menu_text_string( menu_window, menu_entry,
                          display->three_d.default_marker_label );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( change_marker_structure_id )
{
    int                     id;
    marker_struct           *marker;
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    if (get_user_input( "Enter the new structure id: ", "d", &id) == VIO_OK &&
        get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1,&current_object);

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == MARKER )
            {
                marker = get_marker_ptr( object );
                marker->structure_id = id;
            }
        }

        rebuild_selected_list( display, display->associated[MARKER_WINDOW]  );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(change_marker_structure_id )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( change_marker_patient_id )
{
    int                     id;
    marker_struct           *marker;
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    if (get_user_input( "Enter the new patient id: ", "d", &id ) == VIO_OK &&
        get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1,&current_object);

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == MARKER )
            {
                marker = get_marker_ptr( object );
                marker->patient_id = id;
            }
        }

        rebuild_selected_list( display, display->associated[MARKER_WINDOW]  );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(change_marker_patient_id )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( change_marker_type )
{
    int             type;
    marker_struct   *marker;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    if( get_current_marker(display,&marker) )
    {
         sprintf( prompt,
                 "The current marker type is: %d\n"
                 "Enter the new type [0-%d]: ",
                  (int) marker->type, N_MARKER_TYPES-1 );

       if( get_user_input( prompt, "d", &type ) == VIO_OK &&
            type >= 0 && type < N_MARKER_TYPES )
        {
            marker->type = (Marker_types) type;
            print( "The new value of this marker type is: %d\n",
               (int) marker->type );
            graphics_models_have_changed( display );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(change_marker_type )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( change_marker_size )
{
    VIO_Real        size;
    marker_struct   *marker;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    if( get_current_marker(display,&marker) )
    {
        sprintf( prompt, "The current size of this marker is: %g\n"
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

/* ARGSUSED */

DEF_MENU_UPDATE(change_marker_size )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( change_marker_position )
{
    marker_struct   *marker;

    if( get_current_marker(display,&marker) )
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

/* ARGSUSED */

DEF_MENU_UPDATE(change_marker_position )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( change_marker_label )
{
    VIO_STR         label;
    marker_struct   *marker;
    char            prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    if( get_current_marker(display,&marker) )
    {
        sprintf( prompt,
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

/* ARGSUSED */

DEF_MENU_UPDATE(change_marker_label )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

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

/* ARGSUSED */

DEF_MENU_UPDATE(copy_defaults_to_marker )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( copy_defaults_to_markers )
{
    int                     patient_id, structure_id;
    marker_struct           *marker;
    object_struct           *object, *current_object;
    object_traverse_struct  object_traverse;

    if( get_current_marker(display,&marker) )
    {
        patient_id = marker->patient_id;
        structure_id = marker->structure_id;

        object = display->models[THREED_MODEL];
        initialize_object_traverse( &object_traverse, TRUE, 1, &object );

        while( get_next_object_traverse(&object_traverse, &current_object) )
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

/* ARGSUSED */

DEF_MENU_UPDATE(copy_defaults_to_markers )
{
    return( current_object_is_this_type( display, MARKER ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( move_cursor_to_home )
{
    set_cursor_origin(display, &Cursor_home);
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(move_cursor_to_home )
{
    return( TRUE );
}
