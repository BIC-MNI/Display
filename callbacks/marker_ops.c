
#include  <def_graphics.h>
#include  <def_stdio.h>
#include  <def_globals.h>

static    Status          output_marker();

public  DEF_MENU_FUNCTION( create_marker_at_cursor )   /* ARGSUSED */
{
    Status          status;
    Status          create_object();
    Status          input_string();
    Status          add_object_to_model();
    object_struct   *object;
    void            graphics_models_have_changed();
    model_struct    *get_current_model();
    Boolean         get_voxel_corresponding_to_point();

    status = create_object( &object, MARKER );

    if( status == OK )
    {
        PRINT( "Enter label for marker: " );

        status = input_string( stdin, object->ptr.marker->label, '\n' );
    }

    if( status == OK )
    {
#ifdef NO
        graphics_struct  *slice_window;
        Point            origin;

        slice_window = graphics->associated[SLICE_WINDOW];
        if( slice_window != (graphics_struct *) 0 &&
            get_voxel_corresponding_to_point( graphics,
                            &graphics->three_d.cursor.origin,
                            &Point_x(origin), &Point_y(origin),
                            &Point_z(origin) ) )
        {
            object->ptr.marker->position = origin;
        }
        else
#endif
        {
            object->ptr.marker->position = graphics->three_d.cursor.origin;
        }


        object->ptr.marker->colour = Marker_colour;
        object->ptr.marker->size = Marker_size;

        status = add_object_to_model( get_current_model(graphics), object );

        if( status == OK )
        {
            graphics_models_have_changed( graphics );
        }
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
    void            convert_voxel_to_point();
    graphics_struct *slice_window;

    if( get_current_object( graphics, &object ) &&
        object->object_type == MARKER )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

#ifdef NO
        convert_voxel_to_point( slice_window->slice.volume, 
                                Point_x(object->ptr.marker->position),
                                Point_y(object->ptr.marker->position),
                                Point_z(object->ptr.marker->position),
                                &graphics->three_d.cursor.origin );
#else
        graphics->three_d.cursor.origin = object->ptr.marker->position;
#endif

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
    Status          status;
    object_struct   *object, *current_object;
    object_struct   *get_current_model_object();
    void            update_cursor();
    Status          input_string();
    String          filename;
    FILE            *file;
    Status          open_file();
    Status          close_file();
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();

    object = get_current_model_object( graphics );

    PRINT( "Enter filename: " );

    status = input_string( stdin, filename, ' ' );

    if( status == OK )
    {
        status = open_file( filename, WRITE_FILE, ASCII_FORMAT, &file );
    }

    if( status == OK )
    {
        status = initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( current_object->object_type == MARKER &&
                current_object->visibility )
            {
                status = output_marker( file, current_object->ptr.marker );
            }
        }
    }

    if( status == OK )
    {
        status = close_file( file );
    }

    PRINT( "Done.\n" );

    return( status );
}

public  DEF_MENU_UPDATE(save_markers )   /* ARGSUSED */
{
    return( OK );
}

private  Status  output_marker( file, marker )
    FILE            *file;
    marker_struct   *marker;
{
    Status   status;
    Status   io_point();
    Status   io_newline();
    Status   io_int();
    Status   io_real();
    int      feature_id = 0;
    int      patient_id = 0;

    status = OK;

    if( status == OK )
        status = io_point( file, WRITE_FILE, ASCII_FORMAT, &marker->position );

    if( status == OK )
        status = io_int( file, WRITE_FILE, ASCII_FORMAT, &patient_id );

    if( status == OK )
        status = io_int( file, WRITE_FILE, ASCII_FORMAT, &feature_id );

    if( status == OK )
        status = io_newline( file, WRITE_FILE, ASCII_FORMAT );

    return( status );
}
