
#include  <def_display.h>

public  DEF_MENU_FUNCTION( start_surface_line )   /* ARGSUSED */
{
    start_surface_curve( display );

    return( OK );
}

public  DEF_MENU_UPDATE(start_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( end_surface_line )   /* ARGSUSED */
{
    end_surface_curve( display );

    return( OK );
}

public  DEF_MENU_UPDATE(end_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( close_surface_line )   /* ARGSUSED */
{
    close_surface_curve( display );

    return( OK );
}

public  DEF_MENU_UPDATE(close_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_surface_line )   /* ARGSUSED */
{
    reset_surface_curve( display );

    return( OK );
}

public  DEF_MENU_UPDATE(reset_surface_line)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_surface_line_permanent )   /* ARGSUSED */
{
    make_surface_curve_permanent( display );

    return( OK );
}

public  DEF_MENU_UPDATE(make_surface_line_permanent)   /* ARGSUSED */
{
    return( OK );
}
