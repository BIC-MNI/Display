
#include  <def_graphics.h>
#include  <def_globals.h>

public  DEF_MENU_FUNCTION( make_view_fit )      /* ARGSUSED */
{
    void          update_view();
    void          fit_view_to_domain();
    Point         min_limit, max_limit;
    Boolean       get_range_of_object();
    void          set_update_required();

    if( get_range_of_object( graphics->models[THREED_MODEL],
                              TRUE, &min_limit, &max_limit ) )
    {
        fit_view_to_domain( &graphics->three_d.view, &min_limit, &max_limit );

        update_view( graphics );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(make_view_fit )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_view )      /* ARGSUSED */
{
    void  update_view();
    void  reset_view_parameters();
    void  set_update_required();

    reset_view_parameters( graphics,
                           &Default_line_of_sight, &Default_horizontal );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

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
    void             update_view();
    void             reset_view_parameters();
    void             set_update_required();

    reset_view_parameters( graphics, &line_of_sight, &horizontal );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

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
    void             update_view();
    void             reset_view_parameters();
    void             set_update_required();

    reset_view_parameters( graphics, &line_of_sight, &horizontal );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

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
    void             update_view();
    void             reset_view_parameters();
    void             set_update_required();

    reset_view_parameters( graphics, &line_of_sight, &horizontal );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

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
    void             update_view();
    void             reset_view_parameters();
    void             set_update_required();

    reset_view_parameters( graphics, &line_of_sight, &horizontal );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

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
    void             update_view();
    void             reset_view_parameters();
    void             set_update_required();

    reset_view_parameters( graphics, &line_of_sight, &horizontal );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

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
    void             update_view();
    void             reset_view_parameters();
    void             set_update_required();

    reset_view_parameters( graphics, &line_of_sight, &horizontal );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(right_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_perspective )      /* ARGSUSED */
{
    void  update_view();
    void  set_update_required();

    graphics->three_d.view.perspective_flag =
         !graphics->three_d.view.perspective_flag;

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_perspective )      /* ARGSUSED */
{
    void      set_text_boolean();
    String    text;
    void      set_menu_text();

    set_text_boolean( label, text, graphics->three_d.view.perspective_flag,
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
    void  initialize_magnification();

    initialize_magnification( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(magnify_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( translate_view )      /* ARGSUSED */
{
    void  initialize_translation();

    initialize_translation( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(translate_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( rotate_view )      /* ARGSUSED */
{
    void  initialize_virtual_spaceball();

    initialize_virtual_spaceball( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(rotate_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( front_clipping )      /* ARGSUSED */
{
    void  initialize_front_clipping();

    initialize_front_clipping( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(front_clipping )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( back_clipping )      /* ARGSUSED */
{
    void  initialize_back_clipping();

    initialize_back_clipping( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(back_clipping )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( pick_view_rectangle )      /* ARGSUSED */
{
    void  start_picking_viewport();

    start_picking_viewport( graphics );

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
    String  base_filename;
    Status  start_film_loop();

    PRINT( "Enter base_filename, axis_index, and n_steps: " );
    if( scanf( "%s %d %d", base_filename, &axis_index, &n_steps ) == 3 &&
        axis_index >= 0 && axis_index < N_DIMENSIONS &&
        n_steps > 1 )
    {
        status = start_film_loop( graphics, base_filename, axis_index,
                                  n_steps );
    }
    else
    {
        PRINT_ERROR( "Invalid values.\n" );
        status = ERROR;
    }

    return( status );
}

public  DEF_MENU_UPDATE(create_film_loop )      /* ARGSUSED */
{
    return( OK );
}
