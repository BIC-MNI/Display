
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
