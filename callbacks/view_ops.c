
#include  <display.h>

public  DEF_MENU_FUNCTION( make_view_fit )      /* ARGSUSED */
{
    Point         min_limit, max_limit;

    if( get_range_of_object( display->models[THREED_MODEL],
                              TRUE, &min_limit, &max_limit ) )
    {
        fit_view_to_domain( &display->three_d.view, &min_limit, &max_limit );

        update_view( display );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(make_view_fit )      /* ARGSUSED */
{
    return( OK );
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
    return( OK );
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
    return( OK );
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
    return( OK );
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
    return( OK );
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
    return( OK );
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
    return( OK );
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
    return( OK );
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
    STRING    text;

    set_text_boolean( label, text, display->three_d.view.perspective_flag,
                      "Parallel", "Perspective" );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( centre_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_UPDATE(centre_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( magnify_view )      /* ARGSUSED */
{
    initialize_magnification( display );

    return( OK );
}

public  DEF_MENU_UPDATE(magnify_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( translate_view )      /* ARGSUSED */
{
    initialize_translation( display );

    return( OK );
}

public  DEF_MENU_UPDATE(translate_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( rotate_view )      /* ARGSUSED */
{
    initialize_virtual_spaceball( display );

    return( OK );
}

public  DEF_MENU_UPDATE(rotate_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( front_clipping )      /* ARGSUSED */
{
    initialize_front_clipping( display );

    return( OK );
}

public  DEF_MENU_UPDATE(front_clipping )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( back_clipping )      /* ARGSUSED */
{
    initialize_back_clipping( display );

    return( OK );
}

public  DEF_MENU_UPDATE(back_clipping )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( pick_view_rectangle )      /* ARGSUSED */
{
    start_picking_viewport( display );

    return( OK );
}

public  DEF_MENU_UPDATE(pick_view_rectangle )      /* ARGSUSED */
{
    return( OK );
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
    return( OK );
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
    }

    (void) input_newline( stdin );

    return( status );
}

public  DEF_MENU_UPDATE(save_image )      /* ARGSUSED */
{
    return( OK );
}
