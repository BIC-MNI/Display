
#include  <def_graphics.h>

private  Boolean  get_current_volume( graphics, volume )
    graphics_struct   *graphics;
    volume_struct     **volume;
{
    Status          status;
    Boolean         found;
    object_struct   *current_object;
    Boolean         get_current_object();

    found = FALSE;

    if( get_current_object( graphics, &current_object ) )
    {
        BEGIN_TRAVERSE_OBJECT( status, current_object )

            if( !found && OBJECT->object_type == VOLUME )
            {
                found = TRUE;
                *volume = OBJECT->ptr.volume;
            }

        END_TRAVERSE_OBJECT
    }

    return( found );
}

public  DEF_MENU_FUNCTION( advance_slice )   /* ARGSUSED */
{
    volume_struct   *volume;

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        ++volume->current_slice;
        if( volume->current_slice >= volume->nz )
        {
            volume->current_slice = 0;
        }
        volume->slice_visibilities[volume->current_slice] = ON;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(advance_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_slice )   /* ARGSUSED */
{
    volume_struct   *volume;

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        --volume->current_slice;
        if( volume->current_slice < 0 )
        {
            volume->current_slice = volume->nz-1;
        }
        volume->slice_visibilities[volume->current_slice] = ON;
    }

    graphics->update_required = TRUE;

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_slice )   /* ARGSUSED */
{
    return( OK );
}
