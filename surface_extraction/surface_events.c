
#include  <display.h>

static    DEF_EVENT_FUNCTION(    add_to_surface );

public  void  install_surface_extraction(
    display_struct     *display )
{
    add_action_table_function( &display->action_table, NO_EVENT,
                               add_to_surface );
}

public  void  uninstall_surface_extraction(
    display_struct     *display )
{
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  add_to_surface );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( add_to_surface )
{
    if( display->three_d.surface_extraction.extraction_in_progress &&
        voxels_remaining( &display->three_d.surface_extraction.voxels_to_do ) )
    {
        extract_more_surface( display );

        graphics_models_have_changed( display );
    }

    return( OK );
}
