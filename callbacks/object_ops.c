
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( reverse_normals )   /* ARGSUSED */
{
    object_struct   *current_object;
    Boolean         get_current_object();
    Status          status;
    Status          reverse_object_normals();

    if( get_current_object( graphics, &current_object ) )
    {
        status = reverse_object_normals( current_object );

        graphics->update_required = TRUE;
    }

    return( status );
}

public  DEF_MENU_UPDATE(reverse_normals )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( advance_visible )     /* ARGSUSED */
{
    object_struct    *current_object;
    Boolean          get_current_object();
    void             advance_current_object();
    void             rebuild_selected_list();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = OFF;

        advance_current_object( graphics );

        if( get_current_object( graphics, &current_object ) )
        {
            current_object->visibility = ON;
        }

        rebuild_selected_list( graphics, menu_window );

        graphics->update_required = TRUE;
    }

    return( OK );
}

public  DEF_MENU_UPDATE(advance_visible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_visible )     /* ARGSUSED */
{
    object_struct    *current_object;
    Boolean          get_current_object();
    void             retreat_current_object();
    void             rebuild_selected_list();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = OFF;

        retreat_current_object( graphics );

        if( get_current_object( graphics, &current_object ) )
        {
            current_object->visibility = ON;
        }

        rebuild_selected_list( graphics, menu_window );

        graphics->update_required = TRUE;
    }

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_visible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_all_invisible )     /* ARGSUSED */
{
    Status           status;
    object_struct    *current_object;
    Boolean          get_current_object();
    void             rebuild_selected_list();

    if( get_current_object( graphics, &current_object ) )
    {
        BEGIN_TRAVERSE_OBJECT( status, current_object )
            OBJECT->visibility = FALSE;
        END_TRAVERSE_OBJECT

        rebuild_selected_list( graphics, menu_window );

        graphics->update_required = TRUE;
    }

    return( status );
}

public  DEF_MENU_UPDATE(make_all_invisible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_all_visible )     /* ARGSUSED */
{
    Status           status;
    object_struct    *current_object;
    Boolean          get_current_object();
    void             rebuild_selected_list();

    if( get_current_object( graphics, &current_object ) )
    {
        BEGIN_TRAVERSE_OBJECT( status, current_object )
            OBJECT->visibility = TRUE;
        END_TRAVERSE_OBJECT

        rebuild_selected_list( graphics, menu_window );

        graphics->update_required = TRUE;
    }

    return( status );
}

public  DEF_MENU_UPDATE(make_all_visible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( advance_selected )     /* ARGSUSED */
{
    void    advance_current_object();
    void    rebuild_selected_list();

    advance_current_object( graphics );

    rebuild_selected_list( graphics, menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(advance_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_selected )     /* ARGSUSED */
{
    void    retreat_current_object();
    void    rebuild_selected_list();

    retreat_current_object( graphics );

    rebuild_selected_list( graphics, menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( descend_selected )     /* ARGSUSED */
{
    Status    status;
    Status    push_current_object();
    void      rebuild_selected_list();

    status = push_current_object( graphics );

    rebuild_selected_list( graphics, menu_window );

    return( status );
}

public  DEF_MENU_UPDATE(descend_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( ascend_selected )     /* ARGSUSED */
{
    Status    status;
    Status    pop_current_object();
    void      rebuild_selected_list();

    status = pop_current_object( graphics );

    rebuild_selected_list( graphics, menu_window );

    return( status );
}

public  DEF_MENU_UPDATE(ascend_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_object_visibility )     /* ARGSUSED */
{
    object_struct    *current_object;
    Boolean          get_current_object();
    void             rebuild_selected_list();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = !current_object->visibility;

        rebuild_selected_list( graphics, menu_window );

        graphics->update_required = TRUE;
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_object_visibility )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( create_model )     /* ARGSUSED */
{
    object_struct    *current_object;
    Boolean          get_current_object();
    void             rebuild_selected_list();
    Status           status;
    Status           create_model_after_current();

    status = OK;

    if( get_current_object( graphics, &current_object ) )
    {
        status = create_model_after_current( graphics );

        rebuild_selected_list( graphics, menu_window );

        graphics->update_required = TRUE;
    }

    return( status );
}

public  DEF_MENU_UPDATE(create_model )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( pick_point )     /* ARGSUSED */
{
    void             start_picking_polygon();

    start_picking_polygon( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(pick_point )     /* ARGSUSED */
{
    return( OK );
}
