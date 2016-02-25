/**
 * \file surface_extract.c
 * \brief Menu commands for generating surfaces from labeled volumes.
 *
 * \copyright
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
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

/**
 * Start a surface extraction process in response to a user command.
 */
static  void  start_surface(
    display_struct   *display,
    VIO_BOOL          use_label_flag,
    VIO_BOOL          binary_flag,
    VIO_BOOL          voxelate_flag )
{
    display_struct  *slice_window;
    VIO_BOOL        input_okay;
    VIO_Real        min_value, max_value;
    VIO_Real        voxel[VIO_MAX_DIMENSIONS];
    int             int_voxel[VIO_MAX_DIMENSIONS];
    VIO_Volume          volume, label_volume;

    if( get_n_volumes(display) == 0 ||
        !get_slice_window(display,&slice_window) )
        return;

    if( display->three_d.surface_extraction.volume != NULL )
    {
        print( "Extraction already started.\n" );
        return;
    }

    if( use_label_flag )
        volume = get_label_volume( slice_window );
    else
        volume = get_volume( slice_window );

    label_volume = get_label_volume( slice_window );

    if( volume == NULL )
        return;

    if( display->three_d.surface_extraction.polygons->n_points == 0 )
    {
        input_okay = TRUE;

        if( binary_flag || voxelate_flag )
        {
            if (get_user_input("Enter min and max inside value: " ,
                               "rr", &min_value, &max_value) != VIO_OK)
                input_okay = FALSE;
        }
        else
        {
            if (get_user_input( "Enter isovalue: ", "r", &min_value) != VIO_OK)
                input_okay = FALSE;
            max_value = min_value;
        }

        if( !input_okay )
            return;
    }
    else
    {
        if( binary_flag || voxelate_flag )
        {
            min_value = display->three_d.surface_extraction.min_value;
            max_value = display->three_d.surface_extraction.max_value;
        }
        else
        {
            min_value = display->three_d.surface_extraction.min_value;
            max_value = min_value;
        }
    }

    if( get_voxel_corresponding_to_point( display,
                                          &display->three_d.cursor.origin,
                                          voxel ) )
    {
        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );
        start_surface_extraction_at_point( display, volume, label_volume,
                                           binary_flag, voxelate_flag,
                                           min_value,
                                           max_value,
                                           int_voxel[VIO_X],
                                           int_voxel[VIO_Y],
                                           int_voxel[VIO_Z] );
    }

    /* Make sure the 3D view window is visible! */
    show_three_d_window( display, display->associated[MARKER_WINDOW] );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(start_volume_isosurface )
{
    start_surface( display, FALSE, FALSE, FALSE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(start_volume_isosurface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(start_volume_binary_isosurface )
{
    start_surface( display, FALSE, TRUE, FALSE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(start_volume_binary_isosurface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(start_label_binary_isosurface )
{
    start_surface( display, TRUE, TRUE, FALSE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(start_label_binary_isosurface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(toggle_surface_extraction)
{
    VIO_Volume                  volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        if( display->three_d.surface_extraction.extraction_in_progress )
            stop_surface_extraction( display );
        else
            start_surface_extraction( display );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_surface_extraction )
{
    set_menu_text_on_off( menu_window, menu_entry,
                  display->three_d.surface_extraction.extraction_in_progress );

    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(reset_surface)
{
    if( get_n_volumes(display) > 0 )
    {
        reset_surface_extraction( display );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(reset_surface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(make_surface_permanent)
{
    object_struct  *object;

    if( get_n_volumes(display) > 0 &&
        display->three_d.surface_extraction.polygons->n_items > 0 )
    {
        stop_surface_extraction( display );

        object = create_object( POLYGONS );

        *(get_polygons_ptr(object)) =
                  *(display->three_d.surface_extraction.polygons);

        remove_empty_polygons( get_polygons_ptr(object) );

        ALLOC( display->three_d.surface_extraction.polygons->colours, 1 );
        display->three_d.surface_extraction.polygons->n_items = 0;
        display->three_d.surface_extraction.polygons->n_points = 0;
        reset_surface_extraction( display );

        add_object_to_current_model( display, object );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(make_surface_permanent )
{
    return( get_n_volumes(display) > 0 &&
            display->three_d.surface_extraction.polygons->n_items > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(get_voxelated_label_surface)
{
    start_surface( display, TRUE, FALSE, TRUE );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(get_voxelated_label_surface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(get_voxelated_surface)
{
    start_surface( display, FALSE, FALSE, TRUE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(get_voxelated_surface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_surface_invalid_label_range )
{
    int      min_label, max_label;

    if (get_user_input( "Enter min label and max label corresponding to invalid voxels: ",
                        "dd", &min_label, &max_label ) == VIO_OK )
    {
        set_invalid_label_range_for_surface_extraction( display,
                                                        min_label, max_label );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_surface_invalid_label_range )
{
    return( TRUE );
}
