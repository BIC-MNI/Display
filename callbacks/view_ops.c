/**
 * \file view_ops.c
 * \brief Commands to manipulate the 3D view angles, save images and film 
 * loops, etc.
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

DEF_MENU_FUNCTION( make_view_fit )
{
    fit_view_to_visible_models( display );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(make_view_fit )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( reset_view )
{
    reset_view_parameters( display,
                           &Default_line_of_sight, &Default_horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(reset_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( right_tilted_view )
{
    static  VIO_Vector   line_of_sight = { -1.0f, 0.0f, 0.0f };
    static  VIO_Vector   up = { 0.0f, 0.5f, 0.8666f };
    VIO_Vector           horizontal;

    CROSS_VECTORS( horizontal, line_of_sight, up );
    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(right_tilted_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( left_tilted_view )
{
    static  VIO_Vector   line_of_sight = { 1.0f, 0.0f, 0.0f };
    static  VIO_Vector   up = { 0.0f, 0.5f, 0.8666f };
    VIO_Vector           horizontal;

    CROSS_VECTORS( horizontal, line_of_sight, up );
    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(left_tilted_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( top_view )
{
    static  VIO_Vector   line_of_sight = { 0.0f, 0.0f, -1.0f };
    static  VIO_Vector   horizontal = { 1.0f, 0.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(top_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( bottom_view )
{
    static  VIO_Vector   line_of_sight = { 0.0f, 0.0f, 1.0f };
    static  VIO_Vector   horizontal = { -1.0f, 0.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(bottom_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( front_view )
{
    static  VIO_Vector   line_of_sight = { 0.0f, -1.0f, 0.0f };
    static  VIO_Vector   horizontal = { -1.0f, 0.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(front_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( back_view )
{
    static  VIO_Vector   line_of_sight = { 0.0f, 1.0f, 0.0f };
    static  VIO_Vector   horizontal = { 1.0f, 0.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(back_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( left_view )
{
    static  VIO_Vector   line_of_sight = { 1.0f, 0.0f, 0.0f };
    static  VIO_Vector   horizontal = { 0.0f, -1.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(left_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( right_view )
{
    static  VIO_Vector   line_of_sight = { -1.0f, 0.0f, 0.0f };
    static  VIO_Vector   horizontal = { 0.0f, 1.0f, 0.0f };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(right_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_perspective )
{
    display->three_d.view.perspective_flag =
         !display->three_d.view.perspective_flag;

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_perspective )
{
    set_menu_text_boolean( menu_window, menu_entry,
                           display->three_d.view.perspective_flag,
                           "Parallel", "Perspective" );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( magnify_view )
{
    initialize_magnification( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(magnify_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( translate_view )
{
    initialize_translation( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(translate_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( rotate_view )
{
    initialize_virtual_spaceball( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(rotate_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( front_clipping )
{
    initialize_front_clipping( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(front_clipping )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( back_clipping )
{
    initialize_back_clipping( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(back_clipping )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( pick_view_rectangle )
{
    start_picking_viewport( display );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(pick_view_rectangle )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( create_film_loop )
{
    VIO_Status  status;
    int     axis_index, n_steps;
    VIO_STR  base_filename;

    base_filename = NULL;

    if (get_user_input( "Enter base_filename, axis_index, and n_steps: ",
                        "sdd", 
                        &base_filename, &axis_index, &n_steps) == VIO_OK &&
        axis_index >= 0 && axis_index < VIO_N_DIMENSIONS && n_steps > 1 )
    {
        status = start_film_loop( display, base_filename, axis_index,
                                  n_steps );
    }
    else
    {
        print( "Invalid values.\n" );
        status = VIO_ERROR;
    }

    delete_string( base_filename );

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(create_film_loop )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( save_image )
{
    VIO_Status  status;
    VIO_STR  filename;

    status = VIO_OK;

    if (get_user_file( "Enter filename: ", TRUE, &filename ) == VIO_OK)
    {
        status = save_window_to_file( display, filename, 0, -1, 0, -1 );

        if( status == VIO_OK )
            print( "Done saving image to %s.\n", filename );
        else
            print( "Could not save image to %s.\n", filename );
    }

    delete_string( filename );

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(save_image )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_stereo_mode )
{
    display->three_d.view.stereo_flag = !display->three_d.view.stereo_flag;

    if( display->three_d.view.stereo_flag )
        G_set_background_colour( display->window, WHITE );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_stereo_mode )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_eye_separation )
{
    VIO_Real   eye_separation;
    char       prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    sprintf(prompt, 
            "Current eye separation / perspective distance ratio: %g\n"
            "Enter new eye separation_ratio: ",
           display->three_d.view.eye_separation_ratio );

    if( get_user_input( prompt, "r", &eye_separation ) == VIO_OK )
    {
        display->three_d.view.eye_separation_ratio = eye_separation;

        update_view( display );

        set_update_required( display, NORMAL_PLANES );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_eye_separation )
{
    set_menu_text_real( menu_window, menu_entry,
                        display->three_d.view.eye_separation_ratio );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( print_view )
{
    VIO_Real        x, y, z;
    VIO_Transform   inverse_model;

    get_inverse_model_transform( display, &inverse_model );

    transform_point( &inverse_model,
                     (VIO_Real) Point_x(display->three_d.view.origin),
                     (VIO_Real) Point_y(display->three_d.view.origin),
                     (VIO_Real) Point_z(display->three_d.view.origin),
                      &x, &y, &z );
    print( "-eye  %g %g %g\n", x, y, z );

    transform_vector( &inverse_model,
                      (VIO_Real) Vector_x(display->three_d.view.line_of_sight),
                      (VIO_Real) Vector_y(display->three_d.view.line_of_sight),
                      (VIO_Real) Vector_z(display->three_d.view.line_of_sight),
                      &x, &y, &z );
    print( "-view %g %g %g  ", x, y, z );

    transform_vector( &inverse_model,
                      (VIO_Real) Vector_x(display->three_d.view.y_axis),
                      (VIO_Real) Vector_y(display->three_d.view.y_axis),
                      (VIO_Real) Vector_z(display->three_d.view.y_axis),
                      &x, &y, &z );
    print( "   %g %g %g\n", x, y, z );

    print( "-window_width %g\n",
           display->three_d.view.window_width );

    if( display->three_d.view.perspective_flag );
    {
        print( "-perspective_distance %g\n",
               display->three_d.view.perspective_distance );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(print_view )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(type_in_3D_origin)
{
    VIO_Real             xw, yw, zw;

    if (get_user_input( "Enter x y z world coordinate: ", "rrr",
                        &xw, &yw, &zw) == VIO_OK)
    {
        fill_Point( display->three_d.cursor.origin, xw, yw, zw );
        update_cursor( display );
        set_update_required( display, get_cursor_bitplanes() );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(type_in_3D_origin)
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(type_in_view_origin)
{
    VIO_Real             x, y, z;

    if (get_user_input( "Enter x y z eye coordinate: ", "rrr", 
                        &x, &y, &z) == VIO_OK)
    {
        transform_point( &display->three_d.view.modeling_transform,
                         x, y, z, &x, &y, &z );
        fill_Point( display->three_d.view.origin, x, y, z );
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(type_in_view_origin)
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(type_in_view_line_of_sight)
{
    VIO_Real      x, y, z;
    VIO_Vector    line_of_sight, hor;

    if (get_user_input( "Enter x y z line_of_sight coordinate: ", "rrr",
                        &x, &y, &z) == VIO_OK)
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
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(type_in_view_line_of_sight)
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(type_in_view_up_dir)
{
    VIO_Real      x, y, z;
    VIO_Vector    line_of_sight, up, hor;

    if (get_user_input( "Enter x y z up coordinate: ", "rrr", 
                        &x, &y, &z) == VIO_OK)
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
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(type_in_view_up_dir)
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(type_in_view_window_width)
{
    VIO_Real  width, scale;
    char      prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    sprintf( prompt, "Current window width: %g\nEnter window width: ",
             display->three_d.view.window_width );

    if( get_user_input(prompt, "r", &width ) == VIO_OK &&
        width > 0.0 )
    {
        scale = width / display->three_d.view.window_width;
        display->three_d.view.window_width = width;
        display->three_d.view.window_height *= scale;
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(type_in_view_window_width)
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(type_in_view_perspective_distance)
{
    VIO_Real  dist;
    char      prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    sprintf( prompt,
             "Current perspective distance: %g\nEnter perspective distance: ",
             display->three_d.view.perspective_distance );

    if( get_user_input( prompt, "r", &dist ) == VIO_OK && dist > 0.0 )
    {
        display->three_d.view.perspective_distance = dist;
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(type_in_view_perspective_distance)
{
    return( TRUE );
}
