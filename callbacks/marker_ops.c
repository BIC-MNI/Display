
#include  <def_graphics.h>
#include  <def_stdio.h>
#include  <def_globals.h>

public  DEF_MENU_FUNCTION( create_marker_at_cursor )   /* ARGSUSED */
{
    Status          status;
    Status          create_object();
    Status          input_string();
    Status          add_object_to_model();
    Point           origin;
    object_struct   *object;
    void            set_update_required();
    model_struct    *get_current_model();
    void            rebuild_selected_list();
    Boolean         convert_point_to_voxel();
    graphics_struct *slice_window;

    status = create_object( &object, MARKER );

    if( status == OK )
    {
        PRINT( "Enter label for marker: " );

        status = input_string( stdin, object->ptr.marker->label, '\n' );
    }

    if( status == OK )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

#ifdef NO
        if( slice_window != (graphics_struct *) 0 &&
            convert_point_to_voxel( graphics, &graphics->three_d.cursor.origin,
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
            set_update_required( graphics, NORMAL_PLANES );

            rebuild_selected_list( graphics, menu_window );
            set_update_required( menu_window, NORMAL_PLANES );
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
        convert_voxel_to_point( slice_window, 
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
    object_struct   *object;
    object_struct   *get_current_model_object();
    void            set_update_required();
    void            update_cursor();
    Status          input_string();
    String          filename;
    FILE            *file;
    Status          open_output_file();
    Status          output_marker();
    Status          close_file();

    object = get_current_model_object( graphics );

    PRINT( "Enter filename: " );

    status = input_string( stdin, filename, ' ' );

    if( status == OK )
    {
        status = open_output_file( filename, &file );
    }

    if( status == OK )
    {
        BEGIN_TRAVERSE_OBJECT( status, object )

            if( OBJECT->object_type == MARKER && OBJECT->visibility )
                status = output_marker( file, OBJECT->ptr.marker );

        END_TRAVERSE_OBJECT
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
        status = io_point( file, OUTPUTTING, ASCII_FORMAT, &marker->position );

    if( status == OK )
        status = io_int( file, OUTPUTTING, ASCII_FORMAT, &patient_id );

    if( status == OK )
        status = io_int( file, OUTPUTTING, ASCII_FORMAT, &feature_id );

    if( status == OK )
        status = io_newline( file, OUTPUTTING, ASCII_FORMAT );

    return( status );
}
