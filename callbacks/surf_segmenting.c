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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

private  void  crop_surface(
    display_struct   *display,
    VIO_BOOL          above_flag );
private  Status  io_polygons_visibilities(
    polygons_struct  *polygons,
    IO_types         io_flag );

/* ARGSUSED */

public  DEF_MENU_FUNCTION( reset_polygon_visibility )
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        set_polygons_visibilities( polygons, TRUE );

        if( polygons->colour_flag == PER_ITEM_COLOURS )
        {
            for_less( i, 0, polygons->n_items )
                polygons->colours[i] =
                              display->three_d.surface_edit.visible_colour;
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(reset_polygon_visibility )
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( remove_invisible_parts_of_polygon )
{
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        remove_invisible_polygons( polygons, polygons->visibilities );

        create_polygons_visibilities( polygons );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(remove_invisible_parts_of_polygon)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_n_paint_polygons )
{
    int   n;

    print( "Enter # paint polygons: " );

    if( input_int( stdin, &n ) == OK && n >= 0 )
        display->three_d.surface_edit.n_paint_polygons = n;

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_n_paint_polygons)
{
    set_menu_text_int( menu_window, menu_entry,
                       display->three_d.surface_edit.n_paint_polygons );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_vis_paint_colour )
{
    Status      status;
    STRING      string;
    Colour      colour;

    string = convert_colour_to_string(
                 display->three_d.surface_edit.visible_colour );

    print( "The current visible paint colour is: %s\n", string );

    delete_string( string );

    print( "Enter the new colour name or 3 or 4 colour components: " );

    status = input_line( stdin, &string );

    if( status == OK )
    {
        colour = convert_string_to_colour( string );

        delete_string( string );

        display->three_d.surface_edit.visible_colour = colour;

        string = convert_colour_to_string(
                    display->three_d.surface_edit.visible_colour );

        print( "The new visible paint colour is: %s\n", string );
    }

    delete_string( string );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_vis_paint_colour)
{
    set_menu_text_with_colour( menu_window, menu_entry,
                               display->three_d.surface_edit.visible_colour );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_invis_paint_colour )
{
    Status      status;
    STRING      string;
    Colour      colour;

    string = convert_colour_to_string(
                   display->three_d.surface_edit.invisible_colour );

    print( "The current invisible paint colour is: %s\n", string );

    delete_string( string );

    print( "Enter the new colour name or 3 or 4 colour components: " );

    status = input_line( stdin, &string );

    if( status == OK )
    {
        colour = convert_string_to_colour( string );

        delete_string( string );

        display->three_d.surface_edit.invisible_colour = colour;

        string = convert_colour_to_string(
                     display->three_d.surface_edit.invisible_colour );

        print( "The new invisible paint colour is: %s\n", string );
    }

    delete_string( string );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_invis_paint_colour)
{
    set_menu_text_with_colour( menu_window, menu_entry,
                               display->three_d.surface_edit.invisible_colour );

    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_connected_invisible )
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                            TRUE, OFF, TRUE,
                            display->three_d.surface_edit.invisible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_connected_invisible )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( paint_invisible )
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                       display->three_d.surface_edit.n_paint_polygons,
                       TRUE, OFF,
                       TRUE, display->three_d.surface_edit.invisible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(paint_invisible)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( paint_visible )
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                         display->three_d.surface_edit.n_paint_polygons,
                         TRUE, TRUE,
                         TRUE, display->three_d.surface_edit.visible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(paint_visible)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_connected_vis_colour )
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                                FALSE, OFF, TRUE,
                                display->three_d.surface_edit.visible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_connected_vis_colour )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_connected_invis_colour )
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                            FALSE, OFF, TRUE,
                            display->three_d.surface_edit.invisible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_connected_invis_colour )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( paint_invis_colour )
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                       display->three_d.surface_edit.n_paint_polygons,
                       FALSE, OFF, TRUE,
                       display->three_d.surface_edit.invisible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(paint_invis_colour)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( paint_vis_colour )
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                         display->three_d.surface_edit.n_paint_polygons,
                         FALSE, OFF, TRUE,
                         display->three_d.surface_edit.visible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(paint_vis_colour)
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_visibility_from_colour )
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        create_polygons_visibilities( polygons );

        for_less( i, 0, polygons->n_items )
        {
            polygons->visibilities[i] =
                (Smallest_int) (polygons->colour_flag != PER_ITEM_COLOURS ||
                 display->three_d.surface_edit.invisible_colour !=
                 polygons->colours[i] );
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_visibility_from_colour)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_invis_colour_to_invis )
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) && 
        polygons->colour_flag == PER_ITEM_COLOURS )
    {
        create_polygons_visibilities( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( display->three_d.surface_edit.invisible_colour ==
                polygons->colours[i] )
            {
                polygons->visibilities[i] = FALSE;
            }
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_invis_colour_to_invis)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_vis_to_invis_colour )
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (Smallest_int *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] =
                         display->three_d.surface_edit.invisible_colour;
            }
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_vis_to_invis_colour)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_vis_to_vis_colour )
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (Smallest_int *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] =
                             display->three_d.surface_edit.visible_colour;
            }
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_vis_to_vis_colour)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( crop_above_plane )
{
    crop_surface( display, TRUE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(crop_above_plane)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( crop_below_plane )
{
    crop_surface( display, FALSE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(crop_below_plane)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

private  void  crop_surface(
    display_struct   *display,
    VIO_BOOL          above_flag )
{
    char             ch;
    int              axis_index, volume_index;
    Real             pos;
    Real             voxel[MAX_DIMENSIONS], world[MAX_DIMENSIONS];
    polygons_struct  *polygons;
    display_struct   *slice_window;

    if( get_current_polygons(display,&polygons) )
    {
        if( get_axis_index_under_mouse( display, &volume_index, &axis_index ) &&
            get_slice_window( display, &slice_window ) )
        {
            get_current_voxel( slice_window, volume_index, voxel );

            convert_voxel_to_world( get_nth_volume(slice_window,volume_index),
                                    voxel, &world[X], &world[Y], &world[Z] );
            pos = world[axis_index];
        }
        else
        {
            print( "Specify an axis: " );
            while( input_nonwhite_character( stdin, &ch ) == OK &&
                   ch != '\n' && (ch < 'x' || ch > 'z') )
            {}

            if( ch != '\n' )
                (void) input_newline( stdin );

            axis_index = ch - 'x';
            if( axis_index < 0 || axis_index > 2 )
                return;

            pos = (Real) Point_coord(display->three_d.cursor.origin,axis_index);
        }

        crop_polygons_visibilities( polygons, axis_index, pos, above_flag );
        set_update_required( display, NORMAL_PLANES );
    }
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( save_polygons_visibilities )
{
    Status           status;
    polygons_struct  *polygons;

    status = OK;

    if( get_current_polygons(display,&polygons) )
    {
        status = io_polygons_visibilities( polygons, WRITE_FILE );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(save_polygons_visibilities)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( load_polygons_visibilities )
{
    Status           status;
    polygons_struct  *polygons;

    status = OK;

    if( get_current_polygons(display,&polygons) )
    {
        status = io_polygons_visibilities( polygons, READ_FILE );

        set_update_required( display, NORMAL_PLANES );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(load_polygons_visibilities)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

private  Status  io_polygons_visibilities(
    polygons_struct  *polygons,
    IO_types         io_flag )
{
    Status           status;
    STRING           filename;
    FILE             *file;

    create_polygons_visibilities( polygons );

    print( "Enter filename: " );

    status = input_string( stdin, &filename, ' ' );

    (void) input_newline( stdin );

    if( status == OK )
        status = open_file_with_default_suffix( filename, "vis", io_flag,
                                                BINARY_FORMAT, &file );

    if( status == OK )
        status = io_binary_data( file, io_flag, (void *) polygons->visibilities,
                                 sizeof(polygons->visibilities[0]),
                                 polygons->n_items );

    if( status == OK )
        status = close_file( file );

    delete_string( filename );

    return( status );
}
