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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/surface_curves.c,v 1.17 1995-07-31 19:53:53 david Exp $";
#endif


#include  <display.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION( start_surface_line )
{
    start_surface_curve( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(start_surface_line)
{
    return( !display->three_d.surface_curve.picking_points );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( end_surface_line )
{
    end_surface_curve( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(end_surface_line)
{
    return( display->three_d.surface_curve.picking_points );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( close_surface_line )
{
    close_surface_curve( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(close_surface_line)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( reset_surface_line )
{
    reset_surface_curve( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(reset_surface_line)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( make_surface_line_permanent )
{
    make_surface_curve_permanent( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(make_surface_line_permanent)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_line_curvature_weight )
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


/* ARGSUSED */

public  DEF_MENU_UPDATE(set_line_curvature_weight )
{
    set_menu_text_real( menu_window, menu_entry,
                        display->three_d.surface_curve.line_curvature_weight );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_surface_curve_curvature )
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


/* ARGSUSED */

public  DEF_MENU_UPDATE(set_surface_curve_curvature )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( pick_surface_point_on_line )
{
    pick_surface_point_near_a_line( display );

    return( OK );
}


/* ARGSUSED */

public  DEF_MENU_UPDATE(pick_surface_point_on_line )
{
    return( TRUE );
}
