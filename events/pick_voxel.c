
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_voxel_selection( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( start_picking_voxel );
    void                 install_action_table_function();

    install_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_DOWN_EVENT,
                                   start_picking_voxel );
}

private  DEF_EVENT_FUNCTION( start_picking_voxel )
    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION( handle_update_voxel );
    DECL_EVENT_FUNCTION( end_picking_voxel );
    void                 add_action_table_function();
    void                 install_action_table_function();
    void                 update_voxel_cursor();
    void                 push_action_table();

    push_action_table( &graphics->action_table, LEFT_MOUSE_UP_EVENT );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               handle_update_voxel );

    install_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   end_picking_voxel );

    graphics->prev_mouse_position = graphics->mouse_position;

    update_voxel_cursor( graphics );
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

    if( mouse_moved(graphics) )
    {
        update_voxel_cursor( graphics );
    }

    return( OK );
}

private  void  update_voxel_cursor( slice_window )
    graphics_struct   *slice_window;
{
    int               x, y;
    int               axis, indices[N_DIMENSIONS];
    Point             new_origin;
    void              get_mouse_in_pixels();
    Boolean           convert_pixel_to_voxel();
    graphics_struct   *graphics;
    void              update_cursor();
    void              rebuild_slice_pixels();

    graphics = slice_window->associated[THREE_D_WINDOW];

    get_mouse_in_pixels( slice_window, &slice_window->mouse_position, &x, &y );

    if( convert_pixel_to_voxel( slice_window, x, y,
                  &indices[X_AXIS], &indices[Y_AXIS], &indices[Z_AXIS] ) )
    {
        fill_Point( new_origin, (Real) indices[X_AXIS],
                    (Real) indices[Y_AXIS], (Real) indices[Z_AXIS] );

        if( !EQUAL_POINTS( new_origin, graphics->three_d.cursor.origin ) )
        {
            graphics->three_d.cursor.origin = new_origin;

            update_cursor( graphics );

            graphics->update_required = TRUE;
        }

        for_less( axis, 0, N_DIMENSIONS )
        {
            if( indices[axis] !=
                   slice_window->slice.slice_views[axis].slice_index )
            {
                slice_window->slice.slice_views[axis].slice_index =
                                                             indices[axis];

                rebuild_slice_pixels( slice_window, axis );

                slice_window->update_required = TRUE;
            }
        }
    }
}
