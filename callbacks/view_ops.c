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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/view_ops.c,v 1.37 1996-04-19 13:24:58 david Exp $";
#endif


#include  <display.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION( make_view_fit )
{
    fit_view_to_visible_models( display );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(make_view_fit )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( reset_view )
{
    reset_view_parameters( display,
                           &Default_line_of_sight, &Default_horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(reset_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( right_tilted_view )
{
    static  Vector   line_of_sight = { -1.0f, 0.0f, 0.0f };
    static  Vector   up = { 0.0f, 0.5f, 0.8666f };
    Vector           horizontal;

    CROSS_VECTORS( horizontal, line_of_sight, up );
    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(right_tilted_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( left_tilted_view )
{
    static  Vector   line_of_sight = { 1.0f, 0.0f, 0.0f };
    static  Vector   up = { 0.0f, 0.5f, 0.8666f };
    Vector           horizontal;

    CROSS_VECTORS( horizontal, line_of_sight, up );
    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(left_tilted_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( top_view )
{
    static  Vector   line_of_sight = { 0.0f, 0.0f, -1.0f };
    static  Vector   horizontal = { 1.0f, 0.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(top_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( bottom_view )
{
    static  Vector   line_of_sight = { 0.0f, 0.0f, 1.0f };
    static  Vector   horizontal = { -1.0f, 0.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(bottom_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( front_view )
{
    static  Vector   line_of_sight = { 0.0f, -1.0f, 0.0f };
    static  Vector   horizontal = { -1.0f, 0.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(front_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( back_view )
{
    static  Vector   line_of_sight = { 0.0f, 1.0f, 0.0f };
    static  Vector   horizontal = { 1.0f, 0.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(back_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( left_view )
{
    static  Vector   line_of_sight = { 1.0f, 0.0f, 0.0f };
    static  Vector   horizontal = { 0.0f, -1.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(left_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( right_view )
{
    static  Vector   line_of_sight = { -1.0f, 0.0f, 0.0f };
    static  Vector   horizontal = { 0.0f, 1.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(right_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_perspective )
{
    display->three_d.view.perspective_flag =
         !display->three_d.view.perspective_flag;

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_perspective )
{
    set_menu_text_boolean( menu_window, menu_entry,
                           display->three_d.view.perspective_flag,
                           "Parallel", "Perspective" );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( magnify_view )
{
    initialize_magnification( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(magnify_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( translate_view )
{
    initialize_translation( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(translate_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( rotate_view )
{
    initialize_virtual_spaceball( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(rotate_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( front_clipping )
{
    initialize_front_clipping( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(front_clipping )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( back_clipping )
{
    initialize_back_clipping( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(back_clipping )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( pick_view_rectangle )
{
    start_picking_viewport( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(pick_view_rectangle )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( create_film_loop )
{
    Status  status;
    int     axis_index, n_steps;
    STRING  base_filename;

    print( "Enter base_filename, axis_index, and n_steps: " );

    base_filename = NULL;

    if( input_string( stdin, &base_filename, ' ' ) == OK &&
        input_int( stdin, &axis_index ) == OK &&
        input_int( stdin, &n_steps ) == OK &&
        axis_index >= 0 && axis_index < N_DIMENSIONS && n_steps > 1 )
    {
        status = start_film_loop( display, base_filename, axis_index,
                                  n_steps );
    }
    else
    {
        print( "Invalid values.\n" );
        status = ERROR;
    }

    (void) input_newline( stdin );

    delete_string( base_filename );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(create_film_loop )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( save_image )
{
    Status  status;
    STRING  filename;

    status = OK;

    print( "Enter filename: " );

    if( input_string( stdin, &filename, ' ' ) == OK )
    {
        status = save_window_to_file( display, filename, 0, -1, 0, -1 );

        if( status == OK )
            print( "Done saving image to %s.\n", filename );
        else
            print( "Could not save image to %s.\n", filename );
    }

    (void) input_newline( stdin );

    delete_string( filename );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(save_image )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( toggle_stereo_mode )
{
    display->three_d.view.stereo_flag = !display->three_d.view.stereo_flag;

    if( display->three_d.view.stereo_flag )
        G_set_background_colour( display->window, WHITE );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_stereo_mode )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_eye_separation )
{
    Real   eye_separation;

    print( "Current eye separation: %g\n",
           display->three_d.view.eye_separation );

    print( "Enter new eye separation: " );

    if( input_real( stdin, &eye_separation ) == OK )
    {
        display->three_d.view.eye_separation = eye_separation;

        update_view( display );

        set_update_required( display, NORMAL_PLANES );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_eye_separation )
{
    set_menu_text_real( menu_window, menu_entry,
                        display->three_d.view.eye_separation );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( print_view )
{
    Real        x, y, z;
    Transform   inverse_model;

    get_inverse_model_transform( display, &inverse_model );

    transform_point( &inverse_model,
                     (Real) Point_x(display->three_d.view.origin),
                     (Real) Point_y(display->three_d.view.origin),
                     (Real) Point_z(display->three_d.view.origin),
                      &x, &y, &z );
    print( "Origin       : %g %g %g\n", x, y, z );

    transform_vector( &inverse_model,
                      (Real) Vector_x(display->three_d.view.line_of_sight),
                      (Real) Vector_y(display->three_d.view.line_of_sight),
                      (Real) Vector_z(display->three_d.view.line_of_sight),
                      &x, &y, &z );
    print( "Line of sight: %g %g %g\n", x, y, z );

    transform_vector( &inverse_model,
                      (Real) Vector_x(display->three_d.view.y_axis),
                      (Real) Vector_y(display->three_d.view.y_axis),
                      (Real) Vector_z(display->three_d.view.y_axis),
                      &x, &y, &z );
    print( "Up Direction : %g %g %g\n", x, y, z );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(print_view )
{
    return( TRUE );
}
