
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
