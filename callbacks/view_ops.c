
#include  <display.h>

public  DEF_MENU_FUNCTION( make_view_fit )      /* ARGSUSED */
{
    fit_view_to_visible_models( display );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(make_view_fit )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( reset_view )      /* ARGSUSED */
{
    reset_view_parameters( display,
                           &Default_line_of_sight, &Default_horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(reset_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( right_tilted_view )      /* ARGSUSED */
{
    static  Vector   line_of_sight = { -1.0, 0.0, 0.0 };
    static  Vector   up = { 0.0, 0.5, 0.8666 };
    Vector           horizontal;

    CROSS_VECTORS( horizontal, line_of_sight, up );
    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(right_tilted_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( left_tilted_view )      /* ARGSUSED */
{
    static  Vector   line_of_sight = { 1.0, 0.0, 0.0 };
    static  Vector   up = { 0.0, 0.5, 0.8666 };
    Vector           horizontal;

    CROSS_VECTORS( horizontal, line_of_sight, up );
    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(left_tilted_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( top_view )      /* ARGSUSED */
{
    static  Vector   line_of_sight = { 0.0, 0.0, -1.0 };
    static  Vector   horizontal = { 1.0, 0.0, 0.0 };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(top_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( bottom_view )      /* ARGSUSED */
{
    static  Vector   line_of_sight = { 0.0, 0.0, 1.0 };
    static  Vector   horizontal = { -1.0, 0.0, 0.0 };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(bottom_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( front_view )      /* ARGSUSED */
{
    static  Vector   line_of_sight = { 0.0, -1.0, 0.0 };
    static  Vector   horizontal = { -1.0, 0.0, 0.0 };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(front_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( back_view )      /* ARGSUSED */
{
    static  Vector   line_of_sight = { 0.0, 1.0, 0.0 };
    static  Vector   horizontal = { 1.0, 0.0, 0.0 };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(back_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( left_view )      /* ARGSUSED */
{
    static  Vector   line_of_sight = { 1.0, 0.0, 0.0 };
    static  Vector   horizontal = { 0.0, -1.0, 0.0 };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(left_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( right_view )      /* ARGSUSED */
{
    static  Vector   line_of_sight = { -1.0, 0.0, 0.0 };
    static  Vector   horizontal = { 0.0, 1.0, 0.0 };

    reset_view_parameters( display, &line_of_sight, &horizontal );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(right_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( toggle_perspective )      /* ARGSUSED */
{
    display->three_d.view.perspective_flag =
         !display->three_d.view.perspective_flag;

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_perspective )      /* ARGSUSED */
{
    set_menu_text_boolean( menu_window, menu_entry,
                           display->three_d.view.perspective_flag,
                           "Parallel", "Perspective" );

    return( TRUE );
}

public  DEF_MENU_FUNCTION( magnify_view )      /* ARGSUSED */
{
    initialize_magnification( display );

    return( OK );
}

public  DEF_MENU_UPDATE(magnify_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( translate_view )      /* ARGSUSED */
{
    initialize_translation( display );

    return( OK );
}

public  DEF_MENU_UPDATE(translate_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( rotate_view )      /* ARGSUSED */
{
    initialize_virtual_spaceball( display );

    return( OK );
}

public  DEF_MENU_UPDATE(rotate_view )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( front_clipping )      /* ARGSUSED */
{
    initialize_front_clipping( display );

    return( OK );
}

public  DEF_MENU_UPDATE(front_clipping )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( back_clipping )      /* ARGSUSED */
{
    initialize_back_clipping( display );

    return( OK );
}

public  DEF_MENU_UPDATE(back_clipping )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( pick_view_rectangle )      /* ARGSUSED */
{
    start_picking_viewport( display );

    return( OK );
}

public  DEF_MENU_UPDATE(pick_view_rectangle )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( create_film_loop )      /* ARGSUSED */
{
    Status  status;
    int     axis_index, n_steps;
    STRING  base_filename;

    print( "Enter base_filename, axis_index, and n_steps: " );

    if( input_string( stdin, base_filename, MAX_STRING_LENGTH, ' ' ) == OK &&
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

    return( status );
}

public  DEF_MENU_UPDATE(create_film_loop )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( save_image )      /* ARGSUSED */
{
    Status  status;
    STRING  filename;

    status = OK;

    print( "Enter filename: " );

    if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
    {
        status = save_window_to_file( display, filename );
        print( "Done saving image to %s.\n", filename );
    }

    (void) input_newline( stdin );

    return( status );
}

public  DEF_MENU_UPDATE(save_image )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( toggle_stereo_mode )      /* ARGSUSED */
{
    display->three_d.view.stereo_flag = !display->three_d.view.stereo_flag;

    if( display->three_d.view.stereo_flag )
        G_set_background_colour( display->window, WHITE );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_stereo_mode )      /* ARGSUSED */
{
    return( TRUE );
}

public  DEF_MENU_FUNCTION( set_eye_separation )      /* ARGSUSED */
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

public  DEF_MENU_UPDATE(set_eye_separation )      /* ARGSUSED */
{
    set_menu_text_real( menu_window, menu_entry,
                        display->three_d.view.eye_separation );

    return( TRUE );
}

public  DEF_MENU_FUNCTION( print_view )      /* ARGSUSED */
{
    Real        x, y, z;
    Transform   inverse_model;

    get_inverse_model_transform( display, &inverse_model );
    transform_vector( &inverse_model,
                      Vector_x(display->three_d.view.line_of_sight),
                      Vector_y(display->three_d.view.line_of_sight),
                      Vector_z(display->three_d.view.line_of_sight),
                      &x, &y, &z );

    print( "Line of sight: %g %g %g\n", x, y, z );

    transform_vector( &inverse_model,
                      Vector_x(display->three_d.view.y_axis),
                      Vector_y(display->three_d.view.y_axis),
                      Vector_z(display->three_d.view.y_axis),
                      &x, &y, &z );
    print( "Up Direction: %g %g %g\n", x, y, z );

    return( OK );
}

public  DEF_MENU_UPDATE(print_view )      /* ARGSUSED */
{
    return( TRUE );
}
