
#include  <def_graphics.h>
#include  <def_files.h>
#include  <def_globals.h>
#include  <def_string.h>

private  Boolean  get_current_marker( graphics, marker )
    graphics_struct   *graphics;
    marker_struct     **marker;
{
    Status                  status;
    Boolean                 found;
    object_struct           *current_object, *object;
    Boolean                 get_current_object();
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();

    found = FALSE;

    if( get_current_object( graphics, &current_object ) )
    {
        status = initialize_object_traverse( &object_traverse, 1,
                                             &current_object );

        if( status == OK )
        {
            while( get_next_object_traverse(&object_traverse,&object) )
            {
                if( !found && object->object_type == MARKER )
                {
                    found = TRUE;
                    *marker = object->ptr.marker;
                }
            }
        }
    }

    return( found );
}

private  void  get_position_pointed_to( graphics, pos )
    graphics_struct  *graphics;
    Point            *pos;
{
    int             x, y, z, axis_index;
    volume_struct   *volume;
    void            convert_voxel_to_point();

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) &&
        get_slice_window_volume( graphics, &volume ) )
    {
        convert_voxel_to_point( volume, (Real) x, (Real) y, (Real) z, pos );
    }
    else
    {
        *pos = graphics->three_d.cursor.origin;
    }
}

public  DEF_MENU_FUNCTION( create_marker_at_cursor )   /* ARGSUSED */
{
    Status          status;
    Status          create_object();
    Status          add_object_to_current_model();
    object_struct   *object;
    Boolean         get_voxel_corresponding_to_point();
    void            get_position_pointed_to();
    void            regenerate_voxel_marker_labels();

    status = create_object( &object, MARKER );

    if( status == OK )
    {
        get_position_pointed_to( graphics, &object->ptr.marker->position );

        (void) strcpy( object->ptr.marker->label,
                       graphics->three_d.default_marker_label );
        object->ptr.marker->type = graphics->three_d.default_marker_type;
        object->ptr.marker->colour = graphics->three_d.default_marker_colour;
        object->ptr.marker->size = graphics->three_d.default_marker_size;
        object->ptr.marker->structure_id =
                            graphics->three_d.default_marker_structure_id;
        object->ptr.marker->patient_id =
                            graphics->three_d.default_marker_patient_id;

        status = add_object_to_current_model( graphics, object );

        regenerate_voxel_marker_labels( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(create_marker_at_cursor )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_cursor_to_marker )   /* ARGSUSED */
{
    object_struct   *object;
    void            set_update_required();
    void            update_cursor();
    graphics_struct *slice_window;

    if( get_current_object( graphics, &object ) &&
        object->object_type == MARKER )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        graphics->three_d.cursor.origin = object->ptr.marker->position;

        update_cursor( graphics );
        set_update_required( graphics, OVERLAY_PLANES );

        if( slice_window != (graphics_struct *) 0 )
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
    object_struct           *get_current_model_object();
    volume_struct           *volume;
    void                    update_cursor();
    String                  filename;
    FILE                    *file;
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();
    Status                  io_tag_point();

    object = get_current_model_object( graphics );

    PRINT( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    (void) input_newline( stdin );

    if( !get_slice_window_volume( graphics, &volume ) )
        volume = (volume_struct *) 0;

    if( status == OK )
        status = open_file_with_default_suffix( filename, "lmk", WRITE_FILE,
                                                ASCII_FORMAT, &file );

    if( status == OK )
    {
        status = initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( current_object->object_type == MARKER &&
                current_object->visibility )
            {
                status = io_tag_point( file, WRITE_FILE, volume,
                                       current_object->ptr.marker );
            }
        }
    }

    if( status == OK )
        status = close_file( file );

    PRINT( "Done.\n" );

    return( status );
}

public  DEF_MENU_UPDATE(save_markers )   /* ARGSUSED */
{
    return( OK );
}

public  void  markers_have_changed( graphics )
    graphics_struct  *graphics;
{
    void    regenerate_voxel_marker_labels();
    void    graphics_models_have_changed();

    regenerate_voxel_marker_labels( graphics );
    graphics_models_have_changed( graphics );
}

public  DEF_MENU_FUNCTION( set_default_marker_structure_id )   /* ARGSUSED */
{
    int             id;

    PRINT( "The current default marker id is: %d\n",
           graphics->three_d.default_marker_structure_id );

    PRINT( "Enter the new value: " );

    if( input_int( stdin, &id ) == OK )
    {
        graphics->three_d.default_marker_structure_id = id;
        PRINT( "The new default marker id is: %d\n",
               graphics->three_d.default_marker_structure_id );

    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_structure_id )   /* ARGSUSED */
{
    String  text;
    void    set_menu_text();

    (void) sprintf( text, label, graphics->three_d.default_marker_structure_id);

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_patient_id )   /* ARGSUSED */
{
    int             id;

    PRINT( "The current default marker id is: %d\n",
           graphics->three_d.default_marker_patient_id );

    PRINT( "Enter the new value: " );

    if( input_int( stdin, &id ) == OK )
    {
        graphics->three_d.default_marker_patient_id = id;
        PRINT( "The new default marker id is: %d\n",
               graphics->three_d.default_marker_patient_id );

    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_patient_id )   /* ARGSUSED */
{
    String  text;
    void    set_menu_text();

    (void) sprintf( text, label, graphics->three_d.default_marker_patient_id );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_size )   /* ARGSUSED */
{
    Real        size;

    PRINT( "The current default marker size is: %g\n",
           graphics->three_d.default_marker_size );

    PRINT( "Enter the new value: " );

    if( input_real( stdin, &size ) == OK )
    {
        graphics->three_d.default_marker_size = size;
        PRINT( "The new default marker size is: %g\n",
               graphics->three_d.default_marker_size );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_size )   /* ARGSUSED */
{
    String  text;
    void    set_menu_text();

    (void) sprintf( text, label, graphics->three_d.default_marker_size );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_colour )   /* ARGSUSED */
{
    Status      status;
    String      string;
    Colour      colour;
    Status      convert_string_to_colour();
    void        convert_colour_to_string();

    convert_colour_to_string( &graphics->three_d.default_marker_colour,
                              string );

    PRINT( "The current default marker colour is: %s\n", string );

    PRINT( "Enter the new colour name or r g b: " );

    status = input_line( stdin, string, MAX_STRING_LENGTH );

    if( status == OK )
        status = convert_string_to_colour( string, &colour );

    if( status == OK )
    {
        graphics->three_d.default_marker_colour = colour;

        convert_colour_to_string( &graphics->three_d.default_marker_colour,
                                  string );

        PRINT( "The new default marker colour is: %s\n", string );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_default_marker_colour )   /* ARGSUSED */
{
    void    set_menu_text_with_colour();

    set_menu_text_with_colour( menu_window, menu_entry, label,
                     &graphics->three_d.default_marker_colour );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_default_marker_type )   /* ARGSUSED */
{
    int       type;

    PRINT( "The current default marker type is: %d\n",
              (int) graphics->three_d.default_marker_type );

    PRINT( "Enter the new type [0-%d]:", N_MARKER_TYPES-1 );

    if( input_int( stdin, &type ) == OK && type >= 0 && type < N_MARKER_TYPES )
    {
        graphics->three_d.default_marker_type = (Marker_types) type;
        PRINT( "The new default marker type is: %d\n",
               (int) graphics->three_d.default_marker_type );

    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_type )   /* ARGSUSED */
{
    String  text;
    char    *name;
    void    set_menu_text();

    switch( graphics->three_d.default_marker_type )
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
    String       label;

    PRINT( "The current default marker label is: %s\n",
                 graphics->three_d.default_marker_label );

    PRINT( "Enter the new default label: " );

    status = input_string( stdin, label, MAX_STRING_LENGTH, ' ' );

    if( status == OK )
    {
        (void) strcpy( graphics->three_d.default_marker_label, label );
        PRINT( "The new default marker label is: %s\n",
               graphics->three_d.default_marker_label );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_default_marker_label )   /* ARGSUSED */
{
    String  text;
    void    set_menu_text();

    (void) sprintf( text, label, graphics->three_d.default_marker_label );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_structure_id )   /* ARGSUSED */
{
    int             id;
    marker_struct   *marker;
    void            rebuild_selected_list();

    if( get_current_marker(graphics,&marker) )
    {
        PRINT( "The current value of this marker id is: %d\n",
               marker->structure_id );

        PRINT( "Enter the new value: " );

        if( input_int( stdin, &id ) == OK )
        {
            marker->structure_id = id;
            PRINT( "The new value of this marker id is: %d\n",
               marker->structure_id );
            rebuild_selected_list( graphics, menu_window );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_structure_id )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_patient_id )   /* ARGSUSED */
{
    int             id;
    marker_struct   *marker;
    void            rebuild_selected_list();

    if( get_current_marker(graphics,&marker) )
    {
        PRINT( "The current value of this marker id is: %d\n",
               marker->patient_id );

        PRINT( "Enter the new value: " );

        if( input_int( stdin, &id ) == OK )
        {
            marker->patient_id = id;
            PRINT( "The new value of this marker id is: %d\n",
               marker->patient_id );
            rebuild_selected_list( graphics, menu_window );
        }

        (void) input_newline( stdin );
    }

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
    void            graphics_models_have_changed();

    if( get_current_marker(graphics,&marker) )
    {
        PRINT( "The current marker type is: %d\n", (int) marker->type );

        PRINT( "Enter the new type [0-%d]: ", N_MARKER_TYPES-1 );

        if( input_int( stdin, &type ) == OK &&
            type >= 0 && type < N_MARKER_TYPES )
        {
            marker->type = (Marker_types) type;
            PRINT( "The new value of this marker type is: %d\n",
               (int) marker->type );
            graphics_models_have_changed( graphics );
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
    void            markers_have_changed();

    if( get_current_marker(graphics,&marker) )
    {
        PRINT( "The current size of this marker is: %g\n", marker->size );

        PRINT( "Enter the new value: " );

        if( input_real( stdin, &size ) == OK && size > 0.0 )
        {
            marker->size = size;
            PRINT( "The new size of this marker is: %g\n", marker->size );
            markers_have_changed( graphics );
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
    void            markers_have_changed();

    if( get_current_marker(graphics,&marker) )
    {
        get_position_pointed_to( graphics, &marker->position );

        PRINT( "Marker position changed to: %g %g %g\n",
               Point_x(marker->position),
               Point_y(marker->position),
               Point_z(marker->position) );

        markers_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_position )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( change_marker_label )   /* ARGSUSED */
{
    String          label;
    marker_struct   *marker;
    void            graphics_models_have_changed();

    if( get_current_marker(graphics,&marker) )
    {
        PRINT( "The current marker label is: %s\n", marker->label );

        PRINT( "Enter the new label: " );

        if( input_string( stdin, label, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            (void) strcpy( marker->label, label );
            PRINT( "The new marker label is: %s\n", marker->label );
            graphics_models_have_changed( graphics );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_marker_label )   /* ARGSUSED */
{
    return( OK );
}

public  void  regenerate_voxel_marker_labels( graphics )
    graphics_struct   *graphics;
{
    Status                  status;
    object_struct           *object;
    volume_struct           *volume;
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();
    void                    render_marker_to_volume();
    void                    set_all_voxel_label_flags();
    void                    set_slice_window_update();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        set_all_voxel_label_flags( volume, FALSE );

        object = graphics->models[THREED_MODEL];

        status = initialize_object_traverse( &object_traverse, 1, &object );

        if( status == OK )
        {
            while( get_next_object_traverse(&object_traverse,&object) )
            {
                if( object->object_type == MARKER )
                    render_marker_to_volume( volume, object->ptr.marker );
            }
        }

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }
}

private  void  render_marker_to_volume( volume, marker )
    volume_struct   *volume;
    marker_struct   *marker;
{
    Real    xl, xh, yl, yh, zl, zh;
    int     xvl, xvh, yvl, yvh, zvl, zvh, x_voxel, y_voxel, z_voxel;
    void    convert_point_to_voxel();
    void    set_voxel_label_flag();

    convert_point_to_voxel( volume,
                            Point_x(marker->position) - marker->size,
                            Point_y(marker->position) - marker->size,
                            Point_z(marker->position) - marker->size,
                            &xl, &yl, &zl );

    convert_point_to_voxel( volume,
                            Point_x(marker->position) + marker->size,
                            Point_y(marker->position) + marker->size,
                            Point_z(marker->position) + marker->size,
                            &xh, &yh, &zh );

    xvl = CEILING( xl );
    xvh = (int) xh;
    yvl = CEILING( yl );
    yvh = (int) yh;
    zvl = CEILING( zl );
    zvh = (int) zh;

    for_inclusive( x_voxel, xvl, xvh )
    {
        for_inclusive( y_voxel, yvl, yvh )
        {
            for_inclusive( z_voxel, zvl, zvh )
            {
                if( voxel_is_within_volume( volume,
                            (Real) x_voxel, (Real) y_voxel, (Real) z_voxel) )
                    set_voxel_label_flag( volume, x_voxel, y_voxel, z_voxel,
                                          TRUE );
            }
        }
    }
}
