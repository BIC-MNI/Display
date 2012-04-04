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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/surface_extract.c,v 1.32 1996/05/24 18:43:10 david Exp $";
#endif


#include  <display.h>

private  void  start_surface(
    display_struct   *display,
    BOOLEAN          use_label_flag,
    BOOLEAN          binary_flag,
    BOOLEAN          voxelate_flag )
{
    display_struct  *slice_window;
    BOOLEAN         input_okay;
    Real            min_value, max_value;
    Real            voxel[MAX_DIMENSIONS];
    int             int_voxel[MAX_DIMENSIONS];
    Volume          volume, label_volume;

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
            print( "Enter min and max inside value: " );
            if( input_real( stdin, &min_value ) != OK ||
                input_real( stdin, &max_value ) != OK )
                input_okay = FALSE;
        }
        else
        {
            print( "Enter isovalue: " );
            if( input_real( stdin, &min_value ) != OK )
                input_okay = FALSE;
            max_value = min_value;
        }

        (void) input_newline( stdin );

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
        convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
        start_surface_extraction_at_point( display, volume, label_volume,
                                           binary_flag, voxelate_flag,
                                           min_value,
                                           max_value,
                                           int_voxel[X],
                                           int_voxel[Y],
                                           int_voxel[Z] );
    }
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(start_volume_isosurface )
{
    start_surface( display, FALSE, FALSE, FALSE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(start_volume_isosurface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(start_volume_binary_isosurface )
{
    start_surface( display, FALSE, TRUE, FALSE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(start_volume_binary_isosurface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(start_label_binary_isosurface )
{
    start_surface( display, TRUE, TRUE, FALSE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(start_label_binary_isosurface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(toggle_surface_extraction)
{
    Volume                  volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        if( display->three_d.surface_extraction.extraction_in_progress )
            stop_surface_extraction( display );
        else
            start_surface_extraction( display );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_surface_extraction )
{
    set_menu_text_on_off( menu_window, menu_entry,
                  display->three_d.surface_extraction.extraction_in_progress );

    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(reset_surface)
{
    if( get_n_volumes(display) > 0 )
    {
        reset_surface_extraction( display );

        graphics_models_have_changed( display );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(reset_surface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(make_surface_permanent)
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

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(make_surface_permanent )
{
    return( get_n_volumes(display) > 0 &&
            display->three_d.surface_extraction.polygons->n_items > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(get_voxelated_label_surface)
{
    start_surface( display, TRUE, FALSE, TRUE );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(get_voxelated_label_surface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(get_voxelated_surface)
{
    start_surface( display, FALSE, FALSE, TRUE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(get_voxelated_surface )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_surface_invalid_label_range )
{
    int      min_label, max_label;

    print( "Enter min label and max label corresponding to invalid voxels: " );

    if( input_int( stdin, &min_label ) == OK &&
        input_int( stdin, &max_label ) == OK )
    {
        set_invalid_label_range_for_surface_extraction( display,
                                                        min_label, max_label );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_surface_invalid_label_range )
{
    return( TRUE );
}
