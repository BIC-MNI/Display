
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_voxel_selection( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_picking_voxel );
    void                 add_action_table_function();

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               start_picking_voxel );
}

private  DEF_EVENT_FUNCTION( start_picking_voxel )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION( handle_update_voxel );
    DECL_EVENT_FUNCTION( end_picking_voxel );
    void                 add_action_table_function();
    void                 update_voxel_cursor();
    void                 push_action_table();

    push_action_table( &graphics->action_table, LEFT_MOUSE_UP_EVENT );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               handle_update_voxel );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_UP_EVENT,
                               end_picking_voxel );

    graphics->prev_mouse_position = graphics->mouse_position;

    update_voxel_cursor( graphics );

    return( OK );
}

private  DEF_EVENT_FUNCTION( end_picking_voxel )
    /* ARGSUSED */
{
    void    pop_action_table();
    void    remove_action_table_function();
    void    update_voxel_cursor();

    remove_action_table_function( &graphics->action_table, NO_EVENT );

    pop_action_table( &graphics->action_table, LEFT_MOUSE_UP_EVENT );

    update_voxel_cursor( graphics );
}

private  DEF_EVENT_FUNCTION( handle_update_voxel )
    /* ARGSUSED */
{
    Boolean  mouse_moved();
    void     update_voxel_cursor();

    if( mouse_moved(graphics) || graphics->update_required )
    {
        update_voxel_cursor( graphics );
    }

    return( OK );
}

private  void  update_voxel_cursor( slice_window )
    graphics_struct   *slice_window;
{
    int               x, y, z, axis_index;
    Boolean           get_voxel_in_slice_window();
    Boolean           set_current_voxel();
    Boolean           update_cursor_from_voxel();

    if( get_voxel_in_slice_window( slice_window, &x, &y, &z, &axis_index ) )
    {
        if( set_current_voxel( slice_window, x, y, z ) )
        {
            slice_window->update_required = TRUE;
        }

        if( update_cursor_from_voxel( slice_window ) )
        {
            slice_window->associated[THREE_D_WINDOW]->update_required = TRUE;
        }
    }
}
