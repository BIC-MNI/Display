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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/view_ops.c,v 1.42 2001/05/27 00:19:39 stever Exp $";
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

    print( "Current eye separation / perspective distance ratio: %g\n",
           display->three_d.view.eye_separation_ratio );

    print( "Enter new eye separation_ratio: " );

    if( input_real( stdin, &eye_separation ) == OK )
    {
        display->three_d.view.eye_separation_ratio = eye_separation;

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
                        display->three_d.view.eye_separation_ratio );

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
    print( "-eye  %g %g %g\n", x, y, z );

    transform_vector( &inverse_model,
                      (Real) Vector_x(display->three_d.view.line_of_sight),
                      (Real) Vector_y(display->three_d.view.line_of_sight),
                      (Real) Vector_z(display->three_d.view.line_of_sight),
                      &x, &y, &z );
    print( "-view %g %g %g  ", x, y, z );

    transform_vector( &inverse_model,
                      (Real) Vector_x(display->three_d.view.y_axis),
                      (Real) Vector_y(display->three_d.view.y_axis),
                      (Real) Vector_z(display->three_d.view.y_axis),
                      &x, &y, &z );
    print( "   %g %g %g\n", x, y, z );

    print( "-window_width %g\n",
           display->three_d.view.window_width );

    if( display->three_d.view.perspective_flag );
    {
        print( "-perspective_distance %g\n",
               display->three_d.view.perspective_distance );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(print_view )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(type_in_3D_origin)
{
    Real             xw, yw, zw;

    print( "Enter x y z world coordinate: " );

    if( input_real( stdin, &xw ) == OK &&
        input_real( stdin, &yw ) == OK &&
        input_real( stdin, &zw ) == OK )
    {
        fill_Point( display->three_d.cursor.origin, xw, yw, zw );
        update_cursor( display );
        set_update_required( display, get_cursor_bitplanes() );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(type_in_3D_origin)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(type_in_view_origin)
{
    Real             x, y, z;

    print( "Enter x y z eye coordinate: " );

    if( input_real( stdin, &x ) == OK &&
        input_real( stdin, &y ) == OK &&
        input_real( stdin, &z ) == OK )
    {
        transform_point( &display->three_d.view.modeling_transform,
                         x, y, z, &x, &y, &z );
        fill_Point( display->three_d.view.origin, x, y, z );
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(type_in_view_origin)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(type_in_view_line_of_sight)
{
    Real      x, y, z;
    Vector    line_of_sight, hor;

    print( "Enter x y z line_of_sight coordinate: " );

    if( input_real( stdin, &x ) == OK &&
        input_real( stdin, &y ) == OK &&
        input_real( stdin, &z ) == OK )
    {
        transform_vector( &display->three_d.view.modeling_transform,
                          x, y, z, &x, &y, &z );
        fill_Vector( line_of_sight, x, y, z );
        NORMALIZE_VECTOR( line_of_sight, line_of_sight );
        hor = display->three_d.view.x_axis;
        assign_view_direction( &display->three_d.view, &line_of_sight, &hor );
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(type_in_view_line_of_sight)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(type_in_view_up_dir)
{
    Real      x, y, z;
    Vector    line_of_sight, up, hor;

    print( "Enter x y z up coordinate: " );

    if( input_real( stdin, &x ) == OK &&
        input_real( stdin, &y ) == OK &&
        input_real( stdin, &z ) == OK )
    {
        transform_vector( &display->three_d.view.modeling_transform,
                          x, y, z, &x, &y, &z );
        fill_Vector( up, x, y, z );
        line_of_sight = display->three_d.view.line_of_sight;
        CROSS_VECTORS( hor, line_of_sight, up );
        assign_view_direction( &display->three_d.view, &line_of_sight, &hor );
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(type_in_view_up_dir)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(type_in_view_window_width)
{
    Real      width, scale;

    print( "Current window width: %g\n", display->three_d.view.window_width );
    print( "Enter window width: " );

    if( input_real( stdin, &width ) == OK &&
        width > 0.0 )
    {
        scale = width / display->three_d.view.window_width;
        display->three_d.view.window_width = width;
        display->three_d.view.window_height *= scale;
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(type_in_view_window_width)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(type_in_view_perspective_distance)
{
    Real      dist;

    print( "Current perspective distance: %g\n",
           display->three_d.view.perspective_distance );
    print( "Enter perspective distance: " );

    if( input_real( stdin, &dist ) == OK && dist > 0.0 )
    {
        display->three_d.view.perspective_distance = dist;
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(type_in_view_perspective_distance)
{
    return( TRUE );
}
