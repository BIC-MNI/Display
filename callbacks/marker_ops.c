
#include  <display.h>

private  BOOLEAN  get_current_marker( display, marker )
    display_struct    *display;
    marker_struct     **marker;
{
    BOOLEAN                 found;
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    found = FALSE;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, 1, &current_object );

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

public  void  set_marker_to_defaults(
    display_struct  *display,
    marker_struct   *marker )
{
    marker->type = display->three_d.default_marker_type;
    marker->colour = display->three_d.default_marker_colour;
    marker->size = display->three_d.default_marker_size;
    (void) strcpy( marker->label, display->three_d.default_marker_label );
    marker->structure_id = display->three_d.default_marker_structure_id;
    marker->patient_id = display->three_d.default_marker_patient_id;
}

private  void  get_position_pointed_to(
    display_struct   *display,
    Point            *pos )
{
    int             axis_index;
    Real            voxel[MAX_DIMENSIONS];
    Real            x_w, y_w, z_w;
    Volume          volume;

    if( get_voxel_under_mouse( display, voxel, &axis_index ) &&
        get_slice_window_volume( display, &volume ) )
    {
        convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
        fill_Point( *pos, x_w, y_w, z_w );
    }
    else
    {
        *pos = display->three_d.cursor.origin;
    }
}

public  void  create_marker_at_position(
    display_struct    *display,
    Point             *position,
    char              label[] )
{
    object_struct   *object;
    marker_struct   *marker;

    object = create_object( MARKER );
    marker = get_marker_ptr( object );

    marker->position = *position;

    set_marker_to_defaults( display, marker );

    if( label != (char *) NULL )
        (void) strcpy( marker->label, label );

    add_object_to_current_model( display, object );

    render_marker_to_volume( display, marker );

    set_slice_window_all_update( display->associated[SLICE_WINDOW] );
}

public  DEF_MENU_FUNCTION( create_marker_at_cursor )   /* ARGSUSED */
{
    Point           position;

    get_position_pointed_to( display, &position );

    create_marker_at_position( display, &position, (char *) NULL );

    return( OK );
}

public  DEF_MENU_UPDATE(create_marker_at_cursor )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_cursor_to_marker )   /* ARGSUSED */
{
    object_struct   *object;
    display_struct  *slice_window;

    if( get_current_object( display, &object ) &&
        object->object_type == MARKER )
    {
        slice_window = display->associated[SLICE_WINDOW];

        display->three_d.cursor.origin = get_marker_ptr(object)->position;

        update_cursor( display );
        set_update_required( display, get_cursor_bitplanes() );

        if( slice_window != (display_struct  *) 0 )
        {
            if( update_voxel_from_cursor( slice_window ) )
                set_update_required( slice_window, NORMAL_PLANES );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_cursor_to_marker )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( save_markers )   /* ARGSUSED */
{
    Status                  status;
    object_struct           *object, *current_object;
    Volume                  volume;
    STRING                  filename;
    FILE                    *file;
    marker_struct           *marker;
    object_traverse_struct  object_traverse;
    int                     n_tags;
    Real                    **tags, *weights;
    int                     *structure_ids, *patient_ids;
    char                    **labels;

    object = get_current_model_object( display );

    print( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    (void) input_newline( stdin );

    if( !get_slice_window_volume( display, &volume ) )
        volume = (Volume) NULL;

    n_tags = 0;
    if( status == OK )
    {
        initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( current_object->object_type == MARKER &&
                current_object->visibility )
            {
                marker = get_marker_ptr(current_object);
                SET_ARRAY_SIZE( tags, n_tags, n_tags+1, DEFAULT_CHUNK_SIZE);
                ALLOC( tags[n_tags], N_DIMENSIONS );
                SET_ARRAY_SIZE( weights, n_tags, n_tags+1, DEFAULT_CHUNK_SIZE);
                SET_ARRAY_SIZE( structure_ids, n_tags, n_tags+1,
                                DEFAULT_CHUNK_SIZE);
                SET_ARRAY_SIZE( patient_ids, n_tags, n_tags+1,
                                DEFAULT_CHUNK_SIZE);
                SET_ARRAY_SIZE( labels, n_tags, n_tags+1, DEFAULT_CHUNK_SIZE);
                ALLOC( labels[n_tags], strlen(marker->label)+1 );

                tags[n_tags][X] = Point_x(marker->position);
                tags[n_tags][Y] = Point_y(marker->position);
                tags[n_tags][Z] = Point_z(marker->position);
                weights[n_tags] = marker->size;
                structure_ids[n_tags] = marker->structure_id;
                patient_ids[n_tags] = marker->patient_id;
                (void) strcpy( labels[n_tags], marker->label );
                ++n_tags;
            }
        }
    }

    if( status == OK )
        status = open_file_with_default_suffix( filename,
                              get_default_tag_file_suffix(), WRITE_FILE,
                              ASCII_FORMAT, &file );

    if( status == OK )
        status = output_tag_points( file, (char *) NULL, 1, n_tags,
                                    tags, (Real **) NULL, weights,
                                    structure_ids, patient_ids, labels );

    if( n_tags > 0 )
    {
        free_tag_points( 1, n_tags, tags, (Real **) NULL,
                         weights, structure_ids, patient_ids, labels );
    }

    if( status == OK )
        status = close_file( file );

    print( "Done.\n" );

    return( status );
}

public  DEF_MENU_UPDATE(save_markers )   /* ARGSUSED */
{
    return( OK );
}

public  void  markers_have_changed(
    display_struct   *display )
{
    regenerate_voxel_marker_labels( display );
    graphics_models_have_changed( display );
}

public  DEF_MENU_FUNCTION( set_default_marker_structure_id )   /* ARGSUSED */
{
    int             id;

    print( "The current default marker id is: %d\n",
           display->three_d.default_marker_structure_id );

    print( "Enter the new value: " );

    if( input_int( stdin, &id ) == OK )
    {
        display->three_d.default_marker_structure_id = id;
        print( "The new default marker id is: %d\n",
               display->three_d.default_marker_structure_id );

    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_structure_id )   /* ARGSUSED */
{
    STRING  text;

    (void) sprintf( text, label, display->three_d.default_marker_structure_id);

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_patient_id )   /* ARGSUSED */
{
    int             id;

    print( "The current default marker id is: %d\n",
           display->three_d.default_marker_patient_id );

    print( "Enter the new value: " );

    if( input_int( stdin, &id ) == OK )
    {
        display->three_d.default_marker_patient_id = id;
        print( "The new default marker id is: %d\n",
               display->three_d.default_marker_patient_id );

    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_patient_id )   /* ARGSUSED */
{
    STRING  text;

    (void) sprintf( text, label, display->three_d.default_marker_patient_id );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_size )   /* ARGSUSED */
{
    Real        size;

    print( "The current default marker size is: %g\n",
           display->three_d.default_marker_size );

    print( "Enter the new value: " );

    if( input_real( stdin, &size ) == OK )
    {
        display->three_d.default_marker_size = size;
        print( "The new default marker size is: %g\n",
               display->three_d.default_marker_size );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_size )   /* ARGSUSED */
{
    STRING  text;

    (void) sprintf( text, label, display->three_d.default_marker_size );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_colour )   /* ARGSUSED */
{
    Status      status;
    STRING      string;
    Colour      colour;

    convert_colour_to_string( display->three_d.default_marker_colour,
                              string );

    print( "The current default marker colour is: %s\n", string );

    print( "Enter the new colour name or r g b: " );

    status = input_line( stdin, string, MAX_STRING_LENGTH );

    if( status == OK )
        colour = convert_string_to_colour( string );

    if( status == OK )
    {
        display->three_d.default_marker_colour = colour;

        convert_colour_to_string( display->three_d.default_marker_colour,
                                  string );

        print( "The new default marker colour is: %s\n", string );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_default_marker_colour )   /* ARGSUSED */
{
    set_menu_text_with_colour( menu_window, menu_entry, label,
                               display->three_d.default_marker_colour );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_type )   /* ARGSUSED */
{
    int       type;

    print( "The current default marker type is: %d\n",
              (int) display->three_d.default_marker_type );

    print( "Enter the new type [0-%d]:", N_MARKER_TYPES-1 );

    if( input_int( stdin, &type ) == OK && type >= 0 && type < N_MARKER_TYPES )
    {
        display->three_d.default_marker_type = (Marker_types) type;
        print( "The new default marker type is: %d\n",
               (int) display->three_d.default_marker_type );

    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_type )   /* ARGSUSED */
{
    STRING  text;
    char    *name;

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

    (void) sprintf( text, label, name );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_label )   /* ARGSUSED */
{
    Status       status;
    STRING       label;

    print( "The current default marker label is: %s\n",
                 display->three_d.default_marker_label );

    print( "Enter the new default label: " );

    status = input_string( stdin, label, MAX_STRING_LENGTH, ' ' );

    if( status == OK )
    {
        (void) strcpy( display->three_d.default_marker_label, label );
        print( "The new default marker label is: %s\n",
               display->three_d.default_marker_label );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_label )   /* ARGSUSED */
{
    STRING  text;

    (void) sprintf( text, label, display->three_d.default_marker_label );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_structure_id )   /* ARGSUSED */
{
    int                     id;
    marker_struct           *marker;
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    print( "Enter the new structure id: " );

    if( input_int( stdin, &id ) == OK &&
        get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, 1, &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == MARKER )
            {
                marker = get_marker_ptr( object );
                marker->structure_id = id;
            }
        }

        rebuild_selected_list( display, menu_window );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_structure_id )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_patient_id )   /* ARGSUSED */
{
    int                     id;
    marker_struct           *marker;
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    print( "Enter the new patient id: " );

    if( input_int( stdin, &id ) == OK &&
        get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, 1, &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == MARKER )
            {
                marker = get_marker_ptr( object );
                marker->patient_id = id;
            }
        }

        rebuild_selected_list( display, menu_window );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_patient_id )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_type )   /* ARGSUSED */
{
    int             type;
    marker_struct   *marker;

    if( get_current_marker(display,&marker) )
    {
        print( "The current marker type is: %d\n", (int) marker->type );

        print( "Enter the new type [0-%d]: ", N_MARKER_TYPES-1 );

        if( input_int( stdin, &type ) == OK &&
            type >= 0 && type < N_MARKER_TYPES )
        {
            marker->type = (Marker_types) type;
            print( "The new value of this marker type is: %d\n",
               (int) marker->type );
            graphics_models_have_changed( display );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_type )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_size )   /* ARGSUSED */
{
    Real            size;
    marker_struct   *marker;

    if( get_current_marker(display,&marker) )
    {
        print( "The current size of this marker is: %g\n", marker->size );

        print( "Enter the new value: " );

        if( input_real( stdin, &size ) == OK && size > 0.0 )
        {
            marker->size = size;
            print( "The new size of this marker is: %g\n", marker->size );
            markers_have_changed( display );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_size )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_position )   /* ARGSUSED */
{
    marker_struct   *marker;

    if( get_current_marker(display,&marker) )
    {
        get_position_pointed_to( display, &marker->position );

        print( "Marker position changed to: %g %g %g\n",
               Point_x(marker->position),
               Point_y(marker->position),
               Point_z(marker->position) );

        markers_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_position )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_label )   /* ARGSUSED */
{
    STRING          label;
    marker_struct   *marker;

    if( get_current_marker(display,&marker) )
    {
        print( "The current marker label is: %s\n", marker->label );

        print( "Enter the new label: " );

        if( input_string( stdin, label, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            (void) strcpy( marker->label, label );
            print( "The new marker label is: %s\n", marker->label );
            graphics_models_have_changed( display );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_label )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( copy_defaults_to_marker )   /* ARGSUSED */
{
    marker_struct   *marker;

    if( get_current_marker(display,&marker) )
    {
        set_marker_to_defaults( display, marker );

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(copy_defaults_to_marker )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( copy_defaults_to_markers )   /* ARGSUSED */
{
    int                     patient_id, structure_id;
    marker_struct           *marker;
    object_struct           *object, *current_object;
    object_traverse_struct  object_traverse;

    if( get_current_marker(display,&marker) )
    {
        patient_id = marker->patient_id;
        structure_id = marker->structure_id;
        if( structure_id >= Marker_segment_id )
            structure_id -= Marker_segment_id;

        object = display->models[THREED_MODEL];
        initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse, &current_object) )
        {
            if( current_object->object_type == MARKER &&
                current_object->visibility )
            {
                marker = get_marker_ptr( current_object );

                if( marker->patient_id == patient_id &&
                    (marker->structure_id == structure_id ||
                     marker->structure_id == structure_id + Marker_segment_id) )
                {
                    set_marker_to_defaults( display, marker );
                }
            }
        }

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(copy_defaults_to_markers )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( classify_markers )   /* ARGSUSED */
{
    model_struct    *model;

    model = get_current_model( display );

    segment_markers( display, model );

    graphics_models_have_changed( display );

    return( OK );
}

public  DEF_MENU_UPDATE(classify_markers )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_marker_segmentation_threshold )   /* ARGSUSED */
{
    Real        threshold;

    print( "The current marker threshold is: %g\n",
           get_marker_threshold(&display->three_d.marker_segmentation) );

    print( "Enter the new value: " );

    if( input_real( stdin, &threshold ) == OK )
    {
        set_marker_threshold(&display->three_d.marker_segmentation, threshold);
        
        print( "The new default marker size is: %g\n",
               get_marker_threshold(&display->three_d.marker_segmentation) );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_marker_segmentation_threshold )   /* ARGSUSED */
{
    STRING  text;

    (void) sprintf( text, label,
                get_marker_threshold(&display->three_d.marker_segmentation) );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( pick_marker_defaults )      /* ARGSUSED */
{
    start_picking_markers( display );

    return( OK );
}

public  DEF_MENU_UPDATE(pick_marker_defaults )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( move_cursor_to_home )      /* ARGSUSED */
{
    display->three_d.cursor.origin = Cursor_home;
    update_cursor( display );

    set_update_required( display, get_cursor_bitplanes() );

    return( OK );
}

public  DEF_MENU_UPDATE(move_cursor_to_home )      /* ARGSUSED */
{
    return( OK );
}
