
#include  <def_graphics.h>
#include  <def_files.h>

public  DEF_MENU_FUNCTION( menu_set_global_variable )  /* ARGSUSED */
{
    Status   status;
    Status   input_string();
    String   input_str, variable_name, new_value;
    Status   set_or_get_global_variable();

    (void) printf( "variable_name [= value]: " );

    status = input_string( stdin, input_str, MAX_STRING_LENGTH, '\n' );

    if( status == OK )
    {
        status = set_or_get_global_variable( input_str, variable_name,
                                             new_value );
    }

    if( status == OK )
        PRINT( "%s = %s\n", variable_name, new_value );

    if( status != OK )
        PRINT_ERROR( "Error in setting global variable\n" );

    return( status );
}

public  DEF_MENU_UPDATE(menu_set_global_variable )  /* ARGSUSED */
{
    return( OK );
}
