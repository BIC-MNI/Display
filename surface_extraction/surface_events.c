
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_marching_cubes.h>
#include  <def_splines.h>
#include  <def_bitlist.h>

public  void  install_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    void                    add_action_table_function();
    DECL_EVENT_FUNCTION(    add_to_surface );

    add_action_table_function( &graphics->action_table, NO_EVENT,
                               add_to_surface );
}

public  void  uninstall_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    void   remove_action_table_function();

    remove_action_table_function( &graphics->action_table, NO_EVENT );
}

private  DEF_EVENT_FUNCTION( add_to_surface )    /* ARGSUSED */
{
    Status     status;
    Status     extract_more_surface();
    void       set_update_required();
    void       rebuild_slice_models();

    status = OK;

    if( graphics->three_d.surface_extraction.extraction_in_progress &&
        voxels_remaining( &graphics->three_d.surface_extraction.voxels_to_do ) )
    {
        status = extract_more_surface( graphics );

        set_update_required( graphics, NORMAL_PLANES );

        if( Display_surface_in_slices )
        {
            rebuild_slice_models( graphics->associated[SLICE_WINDOW] );

            set_update_required( graphics->associated[SLICE_WINDOW],
                                 NORMAL_PLANES );
        }
    }

    return( status );
}
