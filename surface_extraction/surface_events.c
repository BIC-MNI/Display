/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_extraction/surface_events.c,v 1.19 1996/05/17 19:38:21 david Exp $";
#endif


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
        some_voxels_remaining_to_do( &display->three_d.surface_extraction ) )
    {
        if( extract_more_surface( display ) )
            graphics_models_have_changed( display );
    }

    return( OK );
}
