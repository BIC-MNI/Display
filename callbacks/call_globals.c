/**
 * \file call_globals.c
 * \brief Menu commands to change or examine global variables.
 *
 * \copyright
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
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

#ifdef HAVE_MALLOC_H
#  include  <malloc.h>
#endif


/* ARGSUSED */

DEF_MENU_FUNCTION( menu_set_global_variable )
{
    VIO_Status   status;
    VIO_STR   input_str, variable_name, new_value;

    status = get_user_input( "variable_name [= value]: ", "s", &input_str);
    if( status == VIO_OK )
    {
        status = change_global_variable( input_str,
                                         &variable_name, &new_value );

        if( status == VIO_OK )
            print( "%s = %s\n", variable_name, new_value );

        delete_string( variable_name );
        delete_string( new_value );
        delete_string( input_str );
    }

    if( status != VIO_OK )
        print( "Error in setting global variable\n" );

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(menu_set_global_variable )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( show_memory )
{
#ifdef HAVE_MALLINFO
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

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(show_memory )
{
    return( TRUE );
}
