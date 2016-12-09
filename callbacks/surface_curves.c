/**
 * \file callbacks/surface_curves.c
 * \brief Menu commands for drawing curves on 3D surfaces.
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

/* ARGSUSED */

DEF_MENU_FUNCTION( start_surface_line )
{
    start_surface_curve( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(start_surface_line)
{
    return( !display->three_d.surface_curve.picking_points );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( end_surface_line )
{
    end_surface_curve( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(end_surface_line)
{
    return( display->three_d.surface_curve.picking_points );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( close_surface_line )
{
    close_surface_curve( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(close_surface_line)
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( reset_surface_line )
{
    reset_surface_curve( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(reset_surface_line)
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( make_surface_line_permanent )
{
    make_surface_curve_permanent( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(make_surface_line_permanent)
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_line_curvature_weight )
{
    VIO_Real    weight;
    char        prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    snprintf( prompt, sizeof( prompt ),
              "The current line curvature weight is: %g\n"
              "Enter the new value: ",
              display->three_d.surface_curve.line_curvature_weight);

    if( get_user_input( prompt, "r", &weight ) == VIO_OK )
    {
        display->three_d.surface_curve.line_curvature_weight = weight;
    }
    return( VIO_OK );
}


/* ARGSUSED */

DEF_MENU_UPDATE(set_line_curvature_weight )
{
    set_menu_text_real( menu_window, menu_entry,
                        display->three_d.surface_curve.line_curvature_weight );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_surface_curve_curvature )
{
    VIO_Real    min_curve, max_curve;
    char        prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    snprintf( prompt, sizeof( prompt ),
              "The current surface curve curvature limits are: %g %g\n"
              "Enter the new value: ",
              display->three_d.surface_curve.min_curvature,
              display->three_d.surface_curve.min_curvature );

    if (get_user_input(prompt, "rr", &min_curve, &max_curve ) == VIO_OK )
    {
        display->three_d.surface_curve.min_curvature = min_curve;
        display->three_d.surface_curve.max_curvature = max_curve;
    }
    return( VIO_OK );
}


/* ARGSUSED */

DEF_MENU_UPDATE(set_surface_curve_curvature )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( pick_surface_point_on_line )
{
    pick_surface_point_near_a_line( display );

    return( VIO_OK );
}


/* ARGSUSED */

DEF_MENU_UPDATE(pick_surface_point_on_line )
{
    return( TRUE );
}
