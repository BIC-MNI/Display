
#include  <def_display.h>

public  DEF_MENU_FUNCTION( menu_set_global_variable )  /* ARGSUSED */
{
    Status   status;
    String   input_str, variable_name, new_value;

    print( "variable_name [= value]: " );

    status = input_string( stdin, input_str, MAX_STRING_LENGTH, '\n' );

    if( status == OK )
    {
        status = change_global_variable( input_str, variable_name, new_value );
    }

    if( status == OK )
        print( "%s = %s\n", variable_name, new_value );

    if( status != OK )
        print( "Error in setting global variable\n" );

    return( status );
}

public  DEF_MENU_UPDATE(menu_set_global_variable )  /* ARGSUSED */
{
    return( OK );
}
