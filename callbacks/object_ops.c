
#include  <def_graphics.h>

public  DEF_MENU_FUNCTION( reverse_normals )   /* ARGSUSED */
{
    void  reverse_object_normals();
    reverse_object_normals( graphics->models[THREED_MODEL].n_objects,
                            graphics->models[THREED_MODEL].object_list );
    graphics->update_required = TRUE;
    return( OK );
}

public  DEF_MENU_UPDATE(reverse_normals )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( advance_visible )     /* ARGSUSED */
{
    int              i, n;
    object_struct    *list;

    n = graphics->models[THREED_MODEL].n_objects;

    if( n > 0 )
    {
        list = graphics->models[THREED_MODEL].object_list;

        i = 0;
        while( i < n && list[i].visibility == OFF )
        {
            ++i;
        }

        if( i >= n )
        {
            list[0].visibility = ON;
        }
        else
        {
            list[i].visibility = OFF;
            if( i != n-1 )
            {
                list[i+1].visibility = ON;
            }
            else
            {
                list[0].visibility = ON;
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
    int              i, n;
    object_struct    *list;

    n = graphics->models[THREED_MODEL].n_objects;

    if( n > 0 )
    {
        list = graphics->models[THREED_MODEL].object_list;

        i = 0;
        while( i < n && list[i].visibility == OFF )
        {
            ++i;
        }

        if( i >= n )
        {
            list[n-1].visibility = ON;
        }
        else
        {
            list[i].visibility = OFF;
            if( i != 0 )
            {
                list[i-1].visibility = ON;
            }
            else
            {
                list[n-1].visibility = ON;
            }
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
    int              i, n;
    object_struct    *list;

    n = graphics->models[THREED_MODEL].n_objects;
    list = graphics->models[THREED_MODEL].object_list;

    for_less( i, 0, n )
    {
        list[i].visibility = OFF;
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
    int              i, n;
    object_struct    *list;

    n = graphics->models[THREED_MODEL].n_objects;
    list = graphics->models[THREED_MODEL].object_list;

    for_less( i, 0, n )
    {
        list[i].visibility = ON;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(make_all_visible )     /* ARGSUSED */
{
    return( OK );
}
