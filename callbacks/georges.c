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
static char rcsid[] = "$Header: /software/source/visualization/Display/callbacks/georges.c,v 1.2 2001/05/27 00:19:38 stever Exp $";
#endif


#include  <display.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION( georges_menu1 )
{
    int             obj_index;
    object_struct   *current_object;
    lines_struct    *lines;

    if( get_current_object( display, &current_object ) )
    {
        obj_index = get_current_object_index( display );

        print( "The current selected object is index: %d\n", obj_index );

        if( get_object_type( current_object ) == LINES )
        {
            lines = get_lines_ptr( current_object );
            print( "  %d lines made of %d points.\n",
                                       lines->n_items, lines->n_points );
        }
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(georges_menu1 )
{
    return( TRUE );  /*--- always active */
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( georges_menu2 )
{
    print( "Georges menu 2\n" );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(georges_menu2 )
{
    return( TRUE );   /*--- always active */
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( georges_menu3 )
{
    print( "Georges menu 3\n" );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(georges_menu3 )
{
    return( TRUE );   /*--- always active */
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( georges_menu4 )
{
    print( "Georges menu 4\n" );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(georges_menu4 )
{
    return( TRUE );   /*--- always active */
}
