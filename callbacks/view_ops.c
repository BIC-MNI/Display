
#include  <def_graphics.h>
#include  <def_globals.h>

public  DEF_MENU_FUNCTION( make_view_fit )      /* ARGSUSED */
{
    void     update_view();
    void     fit_view_to_domain();
    Point    min_limit, max_limit;
    Boolean  get_range_of_objects();

    if( get_range_of_objects( graphics->model.objects, TRUE,
                              &min_limit, &max_limit ) )
    {
        fit_view_to_domain( &graphics->view, &min_limit, &max_limit );

        update_view( graphics );

        graphics->update_required = TRUE;
    }

    return( OK );
}

public  DEF_MENU_FUNCTION( reset_view )      /* ARGSUSED */
{
    void  update_view();
    void  reset_view_parameters();

    reset_view_parameters( graphics,
                           Default_line_of_sight_x,
                           Default_line_of_sight_y,
                           Default_line_of_sight_z );

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( top_view )      /* ARGSUSED */
{
    void  update_view();
    void  reset_view_parameters();

    reset_view_parameters( graphics, 0.0, 0.0, -1.0 );

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( bottom_view )      /* ARGSUSED */
{
    void  update_view();
    void  reset_view_parameters();

    reset_view_parameters( graphics, 0.0, 0.0, 1.0 );

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( front_view )      /* ARGSUSED */
{
    void  update_view();
    void  reset_view_parameters();

    reset_view_parameters( graphics, 0.0, -1.0, 0.0 );

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( back_view )      /* ARGSUSED */
{
    void  update_view();
    void  reset_view_parameters();

    reset_view_parameters( graphics, 0.0, 1.0, 0.0 );

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( left_view )      /* ARGSUSED */
{
    void  update_view();
    void  reset_view_parameters();

    reset_view_parameters( graphics, 1.0, 0.0, 0.0 );

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( right_view )      /* ARGSUSED */
{
    void  update_view();
    void  reset_view_parameters();

    reset_view_parameters( graphics, -1.0, 0.0, 0.0 );

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_perspective )      /* ARGSUSED */
{
    void  update_view();

    graphics->view.perspective_flag = !graphics->view.perspective_flag;

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_FUNCTION( centre_view )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( magnify_view )      /* ARGSUSED */
{
    void  initialize_magnification();

    initialize_magnification( &graphics->action_table );

    return( OK );
}

public  DEF_MENU_FUNCTION( translate_view )      /* ARGSUSED */
{
    void  initialize_translation();

    initialize_translation( &graphics->action_table );

    return( OK );
}

public  DEF_MENU_FUNCTION( rotate_view )      /* ARGSUSED */
{
    void  initialize_virtual_spaceball();

    initialize_virtual_spaceball( &graphics->action_table );

    return( OK );
}

public  DEF_MENU_FUNCTION( front_clipping )      /* ARGSUSED */
{
    void  initialize_front_clipping();

    initialize_front_clipping( &graphics->action_table );

    return( OK );
}

public  DEF_MENU_FUNCTION( back_clipping )      /* ARGSUSED */
{
    void  initialize_back_clipping();

    initialize_back_clipping( &graphics->action_table );

    return( OK );
}
