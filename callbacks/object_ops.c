
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( reverse_normals )   /* ARGSUSED */
{
    object_struct   *current_object;
    Boolean         get_current_object();
    Status          status;
    Status          reverse_object_normals();
    void            set_update_required();

    if( get_current_object( graphics, &current_object ) )
    {
        status = reverse_object_normals( current_object );

        set_update_required( graphics, NORMAL_PLANES );
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
    void             set_update_required();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = OFF;

        advance_current_object( graphics );

        if( get_current_object( graphics, &current_object ) )
        {
            current_object->visibility = ON;
        }

        rebuild_selected_list( graphics, menu_window );

        set_update_required( graphics, NORMAL_PLANES );
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
    void             set_update_required();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = OFF;

        retreat_current_object( graphics );

        if( get_current_object( graphics, &current_object ) )
        {
            current_object->visibility = ON;
        }

        rebuild_selected_list( graphics, menu_window );

        set_update_required( graphics, NORMAL_PLANES );
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
    void             set_update_required();

    if( get_current_object( graphics, &current_object ) )
    {
        BEGIN_TRAVERSE_OBJECT( status, current_object )
            OBJECT->visibility = FALSE;
        END_TRAVERSE_OBJECT

        rebuild_selected_list( graphics, menu_window );

        set_update_required( graphics, NORMAL_PLANES );
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
    void             set_update_required();

    if( get_current_object( graphics, &current_object ) )
    {
        BEGIN_TRAVERSE_OBJECT( status, current_object )
            OBJECT->visibility = TRUE;
        END_TRAVERSE_OBJECT

        rebuild_selected_list( graphics, menu_window );

        set_update_required( graphics, NORMAL_PLANES );
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
    void             set_update_required();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = !current_object->visibility;

        rebuild_selected_list( graphics, menu_window );

        set_update_required( graphics, NORMAL_PLANES );
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
    void             set_update_required();

    status = OK;

    if( get_current_object( graphics, &current_object ) )
    {
        status = create_model_after_current( graphics );

        rebuild_selected_list( graphics, menu_window );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(create_model )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( delete_current_object )     /* ARGSUSED */
{
    int              obj_index;
    object_struct    *object;
    model_struct     *current_model;
    model_struct     *get_current_model();
    Boolean          get_current_object();
    void             rebuild_selected_list();
    Status           status;
    void             set_update_required();
    void             set_current_object_index();
    Status           remove_object_from_model();
    Status           delete_object();

    status = OK;

    if( !current_object_is_top_level( graphics ) &&
        get_current_object( graphics, &object ) )
    {
        obj_index = get_current_object_index( graphics );

        current_model = get_current_model( graphics );

        status = remove_object_from_model( current_model, obj_index );

        if( status == OK )
        {
            status = delete_object( object );
        }

        if( status == OK )
        {
            if( current_model->n_objects == 0 )
                obj_index = 0;
            else if( obj_index >= current_model->n_objects )
                obj_index = current_model->n_objects - 1;

            set_current_object_index( graphics, obj_index );

            rebuild_selected_list( graphics, menu_window );
            set_update_required( menu_window, NORMAL_PLANES );

            set_update_required( graphics, NORMAL_PLANES );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(delete_current_object )     /* ARGSUSED */
{
    return( OK );
}
