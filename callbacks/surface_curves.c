
#include  <def_graphics.h>
#include  <def_globals.h>

public  DEF_MENU_FUNCTION( start_surface_line )   /* ARGSUSED */
{
    void  start_surface_curve();

    start_surface_curve( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(start_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( end_surface_line )   /* ARGSUSED */
{
    void  end_surface_curve();

    end_surface_curve( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(end_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_surface_line )   /* ARGSUSED */
{
    void  reset_surface_curve();

    reset_surface_curve( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(reset_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_surface_line_permanent )   /* ARGSUSED */
{
    void  make_surface_curve_permanent();

    make_surface_curve_permanent( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(make_surface_line_permanent)   /* ARGSUSED */
{
    return( OK );
}
