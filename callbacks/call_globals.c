
#include  <display.h>
#include  <malloc.h>

public  DEF_MENU_FUNCTION( menu_set_global_variable )  /* ARGSUSED */
{
    Status   status;
    STRING   input_str, variable_name, new_value;

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
    return( TRUE );
}

public  DEF_MENU_FUNCTION( show_memory )  /* ARGSUSED */
{
#ifndef linux
    struct  mallinfo   m;

    m = mallinfo();

    print( "\n" );
    print( "total space in arena           : %d\n", m.arena );
    print( "number of ordinary blocks      : %d\n", m.ordblks );
    print( "number of small blocks         : %d\n", m.smblks );
    print( "space in holding block headers : %d\n", m.hblkhd );
    print( "number of holding blocks       : %d\n", m.hblks );
    print( "space in small blocks in use   : %d\n", m.usmblks );
    print( "space in free small blocks     : %d\n", m.fsmblks );
    print( "space in ordinary blocks in use: %d\n", m.uordblks );
    print( "space in free ordinary blocks  : %d\n", m.fordblks );
    print( "space penalty if keep option   : %d\n", m.keepcost );
    print( "\n" );
#endif

    return( OK );
}

public  DEF_MENU_UPDATE(show_memory )  /* ARGSUSED */
{
    return( TRUE );
}
