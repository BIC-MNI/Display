
#include  <display.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION( load_file )
{
    Status   status;
    STRING   filename;

    print( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    (void) input_newline( stdin );

    if( status == OK )
        status = load_graphics_file( display, filename, FALSE );

    if( status == OK )
        graphics_models_have_changed( display );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(load_file )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( save_file )
{
    int            n_objects;
    object_struct  **object_list;
    object_struct  *current_object;
    Status         status;
    STRING         filename;

    status = OK;

    if( get_current_object( display, &current_object ) )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( current_object->object_type == MODEL )
        {
            n_objects = get_model_ptr(current_object)->n_objects;
            object_list = get_model_ptr(current_object)->objects;
        }
        else
        {
            n_objects = 1;
            object_list = &current_object;
        }

        if( status == OK )
            status = output_graphics_file( filename, (File_formats) Save_format,
                                           n_objects, object_list );
    }

    print( "Done saving.\n" );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(save_file )
{
    return( TRUE );
}
