
#include  <display.h>

private    DEF_EVENT_FUNCTION( start_translation );
private    DEF_EVENT_FUNCTION( turn_off_translation );
private    DEF_EVENT_FUNCTION( handle_update );
private    DEF_EVENT_FUNCTION( terminate_translation );
private  BOOLEAN  perform_translation(
    display_struct   *display );

public  void  initialize_translation(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_translation );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_translation );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( turn_off_translation )
{
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_translation );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_translation );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_translation )
{
    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_translation );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_translation );

    record_mouse_position( display );

    return( OK );
}

private  void  update_translation(
    display_struct  *display )
{
    if( perform_translation( display ) )
    {
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_translation )
{
    update_translation( display );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_translation );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_translation );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_update )
{
    update_translation( display );

    return( OK );
}

private  BOOLEAN  perform_translation(
    display_struct   *display )
{
    BOOLEAN        moved;
    Real           x, y, x_prev, y_prev;
    Vector         delta, hor, vert;
    Transform      transform;

    moved = FALSE;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) )
    {
        fill_Vector( delta, x - x_prev, y - y_prev, 0.0 );

        get_screen_axes( &display->three_d.view, &hor, &vert );

        SCALE_VECTOR( hor, hor, Point_x(delta) );
        SCALE_VECTOR( vert, vert, Point_y(delta) );

        ADD_VECTORS( delta, hor, vert );

        make_translation_transform( Vector_x(delta), Vector_y(delta),
                                    Vector_z(delta), &transform );

        transform_model( display, &transform );

        moved = TRUE;
    }

    return( moved );
}
