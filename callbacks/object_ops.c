
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( reverse_normals )   /* ARGSUSED */
{
    object_struct   *current_object;
    object_struct   *get_current_object();
    Status          status;
    Status          reverse_object_normals();

    current_object = get_current_object( graphics );

    status = reverse_object_normals( current_object );

    graphics->update_required = TRUE;

    return( status );
}

public  DEF_MENU_UPDATE(reverse_normals )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( advance_visible )     /* ARGSUSED */
{
    object_struct    *current_object;
    object_struct    *get_current_object();
    void             advance_current_object();

    current_object = get_current_object( graphics );

    current_object->visibility = OFF;

    advance_current_object( graphics );

    current_object = get_current_object( graphics );

    current_object->visibility = ON;

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(advance_visible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_visible )     /* ARGSUSED */
{
    object_struct    *current_object;
    object_struct    *get_current_object();
    void             retreat_current_object();

    current_object = get_current_object( graphics );

    current_object->visibility = OFF;

    retreat_current_object( graphics );

    current_object = get_current_object( graphics );

    current_object->visibility = ON;

    graphics->update_required = TRUE;

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
    object_struct    *get_current_object();

    current_object = get_current_object( graphics );

    BEGIN_TRAVERSE_OBJECT( status, current_object )

        OBJECT->visibility = FALSE;

    END_TRAVERSE_OBJECT

    graphics->update_required = TRUE;

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
    object_struct    *get_current_object();

    current_object = get_current_object( graphics );

    BEGIN_TRAVERSE_OBJECT( status, current_object )

        OBJECT->visibility = TRUE;

    END_TRAVERSE_OBJECT

    graphics->update_required = TRUE;

    return( status );
}

public  DEF_MENU_UPDATE(make_all_visible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( advance_selected )     /* ARGSUSED */
{
    void    advance_current_object();

    advance_current_object( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(advance_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_selected )     /* ARGSUSED */
{
    void    retreat_current_object();

    retreat_current_object( graphics );

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

    status = push_current_object( graphics );

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

    status = pop_current_object( graphics );

    return( status );
}

public  DEF_MENU_UPDATE(ascend_selected )     /* ARGSUSED */
{
    return( OK );
}
