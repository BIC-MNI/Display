
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( reverse_normals )   /* ARGSUSED */
{
    void  reverse_object_normals();
    reverse_object_normals( graphics->models[THREED_MODEL].objects );
    graphics->update_required = TRUE;
    return( OK );
}

public  DEF_MENU_UPDATE(reverse_normals )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( advance_visible )     /* ARGSUSED */
{
    object_struct  *objects;

    objects = graphics->models[THREED_MODEL].objects;
    if( objects != (object_struct *) 0 )
    {
        while( objects != (object_struct *) 0 && objects->visibility == OFF )
        {
            objects = objects->next;
        }

        if( objects == (object_struct *) 0 )
        {
            graphics->models[THREED_MODEL].objects->visibility = ON;
        }
        else
        {
            objects->visibility = OFF;
            if( objects->next == (object_struct *) 0 )
            {
                graphics->models[THREED_MODEL].objects->visibility = ON;
            }
            else
            {
                objects->next->visibility = ON;
            }
        }

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
    object_struct  *objects;

    objects = graphics->models[THREED_MODEL].objects;
    if( objects != (object_struct *) 0 )
    {
        while( objects->next != (object_struct *) 0 &&
            objects->next->visibility == OFF )
        {
            objects = objects->next;
        }

        if( objects->next == (object_struct *) 0 )
        {
            graphics->models[THREED_MODEL].objects->visibility = OFF;
            objects->visibility = ON;
        }
        else
        {
            objects->next->visibility = OFF;
            objects->visibility = ON;
        }

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
    object_struct  *objects;

    objects = graphics->models[THREED_MODEL].objects;
    
    while( objects != (object_struct *) 0 )
    {
        objects->visibility = OFF;
        objects = objects->next;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(make_all_invisible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_all_visible )     /* ARGSUSED */
{
    object_struct  *objects;

    objects = graphics->models[THREED_MODEL].objects;
    
    while( objects != (object_struct *) 0 )
    {
        objects->visibility = ON;
        objects = objects->next;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(make_all_visible )     /* ARGSUSED */
{
    return( OK );
}
