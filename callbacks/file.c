
#include  <def_files.h>
#include  <def_globals.h>
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( load_file )   /* ARGSUSED */
{
    Status   status;
    Status   load_graphics_file();
    String   filename;
    void     graphics_models_have_changed();

    PRINT( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    if( status == OK )
        status = input_newline( stdin );

    if( status == OK )
        status = load_graphics_file( graphics, filename );

    if( status == OK )
        graphics_models_have_changed( graphics );

    return( status );
}

public  DEF_MENU_UPDATE(load_file )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( save_file )   /* ARGSUSED */
{
    int            n_objects;
    object_struct  **object_list;
    object_struct  *current_object;
    Status         status;
    Status         output_graphics_file();
    String         filename;

    status = OK;

    if( get_current_object( graphics, &current_object ) )
    {
        PRINT( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        if( status == OK )
            status = input_newline( stdin );

        if( current_object->object_type == MODEL )
        {
            n_objects = current_object->ptr.model->n_objects;
            object_list = current_object->ptr.model->object_list;
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

    PRINT( "Done saving.\n" );

    return( status );
}

public  DEF_MENU_UPDATE(save_file )   /* ARGSUSED */
{
    return( OK );
}
