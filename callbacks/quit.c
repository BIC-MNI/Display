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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/quit.c,v 1.12 1995-07-31 19:53:51 david Exp $";
#endif


#include  <display.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION( exit_program )
{
    return( QUIT );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(exit_program )
{
    return( TRUE );
}
