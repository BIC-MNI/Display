/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/call_globals.c,v 1.14 1995-10-19 15:50:17 david Exp $";
#endif


#include  <display.h>
#include  <malloc.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION( menu_set_global_variable )
{
    Status   status;
    STRING   input_str, variable_name, new_value;

    print( "variable_name [= value]: " );

    status = input_string( stdin, &input_str, '\n' );

    if( status == OK )
    {
        status = change_global_variable( input_str,
                                         &variable_name, &new_value );

        if( status == OK )
            print( "%s = %s\n", variable_name, new_value );

        delete_string( variable_name );
        delete_string( new_value );
        delete_string( input_str );
    }

    if( status != OK )
        print( "Error in setting global variable\n" );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(menu_set_global_variable )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( show_memory )
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

/* ARGSUSED */

public  DEF_MENU_UPDATE(show_memory )
{
    return( TRUE );
}
