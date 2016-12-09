/**
 * \file surf_segmenting.c
 * \brief Commands for manipulating surfaces.
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

static  void  crop_surface(
    display_struct   *display,
    VIO_BOOL          above_flag );
static  VIO_Status  io_polygons_visibilities(
    polygons_struct  *polygons,
    VIO_IO_types     io_flag );

/* ARGSUSED */

  DEF_MENU_FUNCTION( reset_polygon_visibility )
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

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(reset_polygon_visibility )
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( remove_invisible_parts_of_polygon )
{
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        remove_invisible_polygons( polygons, polygons->visibilities );

        create_polygons_visibilities( polygons );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(remove_invisible_parts_of_polygon)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_n_paint_polygons )
{
    int   n;

    if (get_user_input( "Enter # paint polygons: ", "d", &n) == VIO_OK &&
        n >= 0)
    {
        display->three_d.surface_edit.n_paint_polygons = n;
    }
    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_n_paint_polygons)
{
    set_menu_text_int( menu_window, menu_entry,
                       display->three_d.surface_edit.n_paint_polygons );

    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_vis_paint_colour )
{
    VIO_Status   status;
    VIO_STR      string;
    VIO_Colour   colour;
    char         prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    string = convert_colour_to_string(
                 display->three_d.surface_edit.visible_colour );

    snprintf( prompt, sizeof( prompt ),
              "The current visible paint colour is: %s\n"
              "Enter the new colour name or 3 or 4 colour components: ",
              string );

    delete_string( string );

    status = get_user_input(prompt, "s", &string );

    if( status == VIO_OK )
    {
        status = string_to_colour( string, &colour );

        delete_string( string );

        if ( status == VIO_OK )
        {
            display->three_d.surface_edit.visible_colour = colour;
        }
        string = convert_colour_to_string(
                     display->three_d.surface_edit.visible_colour );
        print( "The new visible paint colour is: %s\n", string );
    }
    return( status );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_vis_paint_colour)
{
    set_menu_text_with_colour( menu_window, menu_entry,
                               display->three_d.surface_edit.visible_colour );

    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_invis_paint_colour )
{
    VIO_Status   status;
    VIO_STR      string;
    VIO_Colour   colour;
    char         prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    string = convert_colour_to_string(
                   display->three_d.surface_edit.invisible_colour );

    snprintf( prompt, sizeof( prompt ),
              "The current invisible paint colour is: %s\n"
              "Enter the new colour name or 3 or 4 colour components: ",
              string );

    delete_string( string );

    status = get_user_input( prompt, "s", &string );

    if( status == VIO_OK )
    {
        status = string_to_colour( string, &colour );

        delete_string( string );

        if ( status == VIO_OK )
        {
            display->three_d.surface_edit.invisible_colour = colour;
        }
        string = convert_colour_to_string(
                     display->three_d.surface_edit.invisible_colour );

        print( "The new invisible paint colour is: %s\n", string );
    }

    delete_string( string );

    return( status );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_invis_paint_colour)
{
    set_menu_text_with_colour( menu_window, menu_entry,
                               display->three_d.surface_edit.invisible_colour );

    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_connected_invisible )
{
    polygons_struct  *polygons;
    int              poly_index;
    VIO_Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                            TRUE, FALSE, TRUE,
                            display->three_d.surface_edit.invisible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_connected_invisible )
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( paint_invisible )
{
    polygons_struct  *polygons;
    int              poly_index;
    VIO_Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                       display->three_d.surface_edit.n_paint_polygons,
                       TRUE, FALSE,
                       TRUE, display->three_d.surface_edit.invisible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(paint_invisible)
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( paint_visible )
{
    polygons_struct  *polygons;
    int              poly_index;
    VIO_Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                         display->three_d.surface_edit.n_paint_polygons,
                         TRUE, TRUE,
                         TRUE, display->three_d.surface_edit.visible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(paint_visible)
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_connected_vis_colour )
{
    polygons_struct  *polygons;
    int              poly_index;
    VIO_Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                                FALSE, FALSE, TRUE,
                                display->three_d.surface_edit.visible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_connected_vis_colour )
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_connected_invis_colour )
{
    polygons_struct  *polygons;
    int              poly_index;
    VIO_Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                            FALSE, FALSE, TRUE,
                            display->three_d.surface_edit.invisible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_connected_invis_colour )
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( paint_invis_colour )
{
    polygons_struct  *polygons;
    int              poly_index;
    VIO_Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                       display->three_d.surface_edit.n_paint_polygons,
                       FALSE, FALSE, TRUE,
                       display->three_d.surface_edit.invisible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(paint_invis_colour)
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( paint_vis_colour )
{
    polygons_struct  *polygons;
    int              poly_index;
    VIO_Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                         display->three_d.surface_edit.n_paint_polygons,
                         FALSE, FALSE, TRUE,
                         display->three_d.surface_edit.visible_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(paint_vis_colour)
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_visibility_from_colour )
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        create_polygons_visibilities( polygons );

        for_less( i, 0, polygons->n_items )
        {
            polygons->visibilities[i] =
                (VIO_SCHAR) (polygons->colour_flag != PER_ITEM_COLOURS ||
                 display->three_d.surface_edit.invisible_colour !=
                 polygons->colours[i] );
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_visibility_from_colour)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_invis_colour_to_invis )
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

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_invis_colour_to_invis)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_vis_to_invis_colour )
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (VIO_SCHAR *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] =
                         display->three_d.surface_edit.invisible_colour;
            }
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_vis_to_invis_colour)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_vis_to_vis_colour )
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (VIO_SCHAR *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] =
                             display->three_d.surface_edit.visible_colour;
            }
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_vis_to_vis_colour)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( crop_above_plane )
{
    crop_surface( display, TRUE );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(crop_above_plane)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( crop_below_plane )
{
    crop_surface( display, FALSE );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(crop_below_plane)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

static  void  crop_surface(
    display_struct   *display,
    VIO_BOOL          above_flag )
{
    char             ch;
    int              axis_index, volume_index;
    VIO_Real             pos;
    VIO_Real             voxel[VIO_MAX_DIMENSIONS], world[VIO_MAX_DIMENSIONS];
    polygons_struct  *polygons;
    display_struct   *slice_window;

    if( get_current_polygons(display,&polygons) )
    {
        if( get_axis_index_under_mouse( display, &volume_index, &axis_index ) &&
            get_slice_window( display, &slice_window ) )
        {
            get_current_voxel( slice_window, volume_index, voxel );

            convert_voxel_to_world( get_nth_volume(slice_window,volume_index),
                                    voxel, &world[VIO_X], &world[VIO_Y], &world[VIO_Z] );
            pos = world[axis_index];
        }
        else
        {
            if (get_user_input( "Specify an axis: ", "c", &ch) != VIO_OK)
                return;
            switch (ch) {
            case 'x':
            case 'X':
              axis_index = 0;
              break;
            case 'y':
            case 'Y':
              axis_index = 1;
              break;
            case 'z':
            case 'Z':
              axis_index = 2;
              break;
            default:
              return;
            }

            pos = (VIO_Real) Point_coord(display->three_d.cursor.origin,axis_index);
        }

        crop_polygons_visibilities( polygons, axis_index, pos, above_flag );
        set_update_required( display, NORMAL_PLANES );
    }
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( save_polygons_visibilities )
{
    VIO_Status           status;
    polygons_struct  *polygons;

    status = VIO_OK;

    if( get_current_polygons(display,&polygons) )
    {
        status = io_polygons_visibilities( polygons, WRITE_FILE );
    }

    return( status );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(save_polygons_visibilities)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( load_polygons_visibilities )
{
    VIO_Status           status;
    polygons_struct  *polygons;

    status = VIO_OK;

    if( get_current_polygons(display,&polygons) )
    {
        status = io_polygons_visibilities( polygons, READ_FILE );

        set_update_required( display, NORMAL_PLANES );
    }

    return( status );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(load_polygons_visibilities)
{
    return( current_object_is_this_type(display,POLYGONS) );
}

static  VIO_Status  io_polygons_visibilities(
    polygons_struct  *polygons,
    VIO_IO_types     io_flag )
{
    VIO_Status       status;
    VIO_STR          filename;
    FILE             *file;

    create_polygons_visibilities( polygons );

    status = get_user_file("Enter filename: ", io_flag == WRITE_FILE,
                           "vis", &filename);
    if( status == VIO_OK )
    {
        status = open_file_with_default_suffix( filename, "vis", io_flag,
                                                BINARY_FORMAT, &file );
        if (status == VIO_OK)
        {

            status = io_binary_data( file, io_flag, 
                                     (void *) polygons->visibilities,
                                     sizeof(polygons->visibilities[0]),
                                     polygons->n_items );

            close_file( file );
        }

        delete_string( filename );
    }

    return( status );
}
