
#include  <def_graphics.h>

private  Boolean  get_current_volume( graphics, volume )
    graphics_struct   *graphics;
    volume_struct     **volume;
{
    Boolean         current_is_volume;
    object_struct   *current_object;
    object_struct   *get_current_object();

    current_object = get_current_object( graphics );

    if( current_object == (object_struct *) 0 ||
        current_object->object_type != VOLUME )
    {
        current_is_volume = FALSE;
    }
    else
    {
        *volume = current_object->ptr.volume;
        current_is_volume = TRUE;
    }

    return( current_is_volume );
}

public  DEF_MENU_FUNCTION( advance_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    Status          status;
    Status          reverse_object_normals();

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        ++volume->current_slice;
        if( volume->current_slice >= volume->nz )
        {
            volume->current_slice = 0;
        }
    }

    graphics->update_required = TRUE;

    return( status );
}

public  DEF_MENU_UPDATE(advance_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    Status          status;
    Status          reverse_object_normals();

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        --volume->current_slice;
        if( volume->current_slice < 0 )
        {
            volume->current_slice = volume->nz-1;
        }
    }

    graphics->update_required = TRUE;

    return( status );
}

public  DEF_MENU_UPDATE(retreat_slice )   /* ARGSUSED */
{
    return( OK );
}
