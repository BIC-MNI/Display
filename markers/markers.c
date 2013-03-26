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

  VIO_BOOL  update_current_marker(
    display_struct   *display,
    int              volume_index,
    VIO_Real             voxel[] )
{
    object_traverse_struct  object_traverse;
    VIO_BOOL                 found;
    object_struct           *object, *closest_marker;
    VIO_Volume                  volume;
    VIO_Point                   voxel_pos;
    VIO_Real                    x_w, y_w, z_w;
    VIO_Real                    dist, closest_dist;

    initialize_object_traverse( &object_traverse, FALSE, 1,
                                &display->models[THREED_MODEL] );

    volume = get_nth_volume( display, volume_index );

    convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
    fill_Point( voxel_pos, x_w, y_w, z_w );

    found = FALSE;
    closest_dist = 0.0;

    while( get_next_object_traverse( &object_traverse, &object ) )
    {
        if( object->object_type == MARKER &&
            points_within_distance( &voxel_pos,
                                    &get_marker_ptr(object)->position,
                                    Marker_pick_size ) )
        {
            dist = distance_between_points( &voxel_pos,
                                            &get_marker_ptr(object)->position );

            if( !found || dist < closest_dist )
            {
                found = TRUE;
                closest_dist = dist;
                closest_marker = object;
            }
        }
    }

    if( found && (!get_current_object(display,&object) ||
                  object != closest_marker) )
    {
        set_current_object( display, closest_marker );
    }

    return( found );
}

static  void  initialize_marker_parameters(
    display_struct    *marker_window )
{
    int                 x_size, y_size;
    VIO_Real                x_scale, y_scale, scale;
    menu_window_struct  *marker;

    marker = &marker_window->marker;

    G_get_window_size( marker_window->window, &x_size, &y_size );

    x_scale = (VIO_Real) x_size / (VIO_Real) marker->default_x_size;
    y_scale = (VIO_Real) y_size / (VIO_Real) marker->default_y_size;

    scale = MIN( x_scale, y_scale );

    marker->x_dx = scale * X_menu_dx;
    marker->x_dy = scale * X_menu_dy;
    marker->y_dx = scale * Y_menu_dx;
    marker->y_dy = scale * Y_menu_dy;
    marker->n_chars_per_unit_across = Menu_n_chars_per_entry;
    marker->n_lines_in_entry = Menu_n_lines_per_entry;
    marker->character_width = scale * Menu_character_width;
    marker->character_height = scale * Menu_character_height;
    marker->character_offset = scale * Menu_key_character_offset;
    marker->x_menu_text_offset = scale * X_menu_text_offset;
    marker->y_menu_text_offset = scale * Y_menu_text_offset;
    marker->x_menu_origin = scale * X_menu_origin;
    marker->y_menu_origin = scale * Y_menu_origin;
    marker->cursor_pos_x_origin = scale * Cursor_pos_x_origin;
    marker->cursor_pos_y_origin = scale * Cursor_pos_y_origin;
    marker->selected_x_origin = scale * Selected_x_origin;
    marker->selected_y_origin = scale * Selected_y_origin;
    marker->selected_x_offset = scale * Selected_box_x_offset;
    marker->selected_y_offset = scale * Selected_box_y_offset;
    marker->selected_box_height = scale * Character_height_in_pixels;
    marker->x_menu_name = scale * Menu_name_x;
    marker->y_menu_name = scale * Menu_name_y;
    marker->font_size = scale * Menu_window_font_size;
}

static  DEF_EVENT_FUNCTION( handle_marker_resize )
{
    display_struct  *marker_window, *three_d;

    three_d = display->associated[THREE_D_WINDOW];
    marker_window = three_d->associated[MARKER_WINDOW];

    initialize_marker_parameters( marker_window );
    rebuild_cursor_position_model( three_d );
    rebuild_selected_list( three_d, marker_window );

    return( OK );
}


  VIO_Status  initialize_marker_window(
    display_struct    *marker_window)
{
    VIO_Status               status;
    menu_window_struct   *marker;
    VIO_Point                position;
    model_struct         *model;
    int                  ch, i, dir, len;
    VIO_BOOL              found;

    status = OK;
    marker = &marker_window->marker;

    G_set_transparency_state( marker_window->window, FALSE );

    initialize_resize_events( marker_window );
    add_action_table_function( &marker_window->action_table, WINDOW_RESIZE_EVENT,
                               handle_marker_resize );

    marker->default_x_size = Canonical_marker_window_width;
    marker->default_y_size = Canonical_marker_window_height;

    initialize_marker_parameters( marker_window );

//    for_less( ch, 0, N_CHARACTERS )
//        set_menu_key_entry( menu, ch, NULL );

//    directories[0] = "";
//    directories[1] = default_directory1;
//    directories[2] = default_directory2;
//    directories[3] = default_directory3;
//    directories[4] = default_directory4;


//    if( status == OK )
//    {
//        status = read_menu( menu, file );
//
//        menu->depth = 0;
//        menu->stack[0] = &menu->entries[0];
//        menu->entries[0].current_depth = 0;
//    }

//    if( status == OK )
//    {
//        build_menu( menu_window );
//
//        add_menu_actions( menu, &menu->entries[0] );
//
//        set_update_required( menu_window, NORMAL_PLANES );
//
//        status = close_file( file );
//    }
//
//    model = get_graphics_model( menu_window, UTILITY_MODEL );
//    menu->menu_name_text = create_object( TEXT );
//    fill_Point( position, menu->x_menu_name, menu->y_menu_name, 0.0 );
//    initialize_text( get_text_ptr(menu->menu_name_text), &position,
//                     Menu_name_colour, (Font_types) Menu_name_font,
//                     menu->font_size );
//
//    add_object_to_model( model, menu->menu_name_text );
//
//    update_menu_name_text( menu_window );
//
//    rebuild_cursor_position_model( menu_window );

    return( status );
}
