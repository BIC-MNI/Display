
#include  <display.h>

public  DEF_MENU_FUNCTION( start_surface_line )   /* ARGSUSED */
{
    start_surface_curve( display );

    return( OK );
}

public  DEF_MENU_UPDATE(start_surface_line)   /* ARGSUSED */
{
    return( !display->three_d.surface_curve.picking_points );
}

public  DEF_MENU_FUNCTION( end_surface_line )   /* ARGSUSED */
{
    end_surface_curve( display );

    return( OK );
}

public  DEF_MENU_UPDATE(end_surface_line)   /* ARGSUSED */
{
    return( display->three_d.surface_curve.picking_points );
}

public  DEF_MENU_FUNCTION( close_surface_line )   /* ARGSUSED */
{
    close_surface_curve( display );

    return( OK );
}

public  DEF_MENU_UPDATE(close_surface_line)   /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( reset_surface_line )   /* ARGSUSED */
{
    reset_surface_curve( display );

    return( OK );
}

public  DEF_MENU_UPDATE(reset_surface_line)   /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( make_surface_line_permanent )   /* ARGSUSED */
{
    make_surface_curve_permanent( display );

    return( OK );
}

public  DEF_MENU_UPDATE(make_surface_line_permanent)   /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( set_line_curvature_weight )   /* ARGSUSED */
{
    Real        weight;

    print( "The current line curvature weight is: %g\n",
           display->three_d.surface_curve.line_curvature_weight );

    print( "Enter the new value: " );

    if( input_real( stdin, &weight ) == OK )
    {
        display->three_d.surface_curve.line_curvature_weight = weight;
    }

    (void) input_newline( stdin );

    return( OK );
}


public  DEF_MENU_UPDATE(set_line_curvature_weight )   /* ARGSUSED */
{
    set_menu_text_real( menu_window, menu_entry,
                        display->three_d.surface_curve.line_curvature_weight );

    return( TRUE );
}

public  DEF_MENU_FUNCTION( set_surface_curve_curvature )   /* ARGSUSED */
{
    Real        min_curve, max_curve;

    print( "The current surface curve curvature limits are: %g %g\n",
           display->three_d.surface_curve.min_curvature,
           display->three_d.surface_curve.min_curvature );

    print( "Enter the new value: " );

    if( input_real( stdin, &min_curve ) == OK &&
        input_real( stdin, &max_curve ) == OK )
    {
        display->three_d.surface_curve.min_curvature = min_curve;
        display->three_d.surface_curve.max_curvature = max_curve;
    }

    (void) input_newline( stdin );

    return( OK );
}


public  DEF_MENU_UPDATE(set_surface_curve_curvature )   /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( pick_surface_point_on_line )   /* ARGSUSED */
{
    pick_surface_point_near_a_line( display );

    return( OK );
}


public  DEF_MENU_UPDATE(pick_surface_point_on_line )   /* ARGSUSED */
{
    return( TRUE );
}
