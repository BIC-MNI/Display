
#include  <def_stdio.h>
#include  <def_globals.h>
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( load_file )   /* ARGSUSED */
{
    Status   load_graphics_file();
    String   filename;

    (void) printf( "Enter filename: " );
    (void) scanf( "%s", filename );

    (void) load_graphics_file( graphics, filename );

    return( OK );
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
        (void) printf( "Enter filename: " );
        (void) scanf( "%s", filename );

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
