
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
    void     extract_more_triangles();
    void     display_triangles();

    if( graphics->three_d.surface_extraction.extraction_in_progress &&
        voxels_remaining( &graphics->three_d.surface_extraction.voxels_to_do ) )
    {
        extract_more_triangles( graphics );

        graphics->update_required = TRUE;
    }

    return( OK );
}
