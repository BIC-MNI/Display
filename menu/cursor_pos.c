/**
 * \file menu/cursor_pos.c
 * \brief Maintain the cursor position text in the menu window.
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

static  void  create_cursor_pos_text(
    menu_window_struct  *menu,
    model_struct        *model )
{
    object_struct  *object;
    VIO_Point          origin;
    text_struct    *text;

    /*--- make the label */

    object = create_object( TEXT );
    text = get_text_ptr( object );

    fill_Point( origin,
                menu->cursor_pos_x_origin,
                menu->cursor_pos_y_origin, 0.0 );

    initialize_text( text, &origin, Cursor_pos_colour,
                     (Font_types) Menu_window_font, menu->font_size );

    replace_string( &text->string, create_string(Cursor_pos_title) );

    add_object_to_model( model, object );

    /*--- make the numbers */

    object = create_object( TEXT );
    text = get_text_ptr( object );

    fill_Point( origin,
                menu->cursor_pos_x_origin,
                menu->cursor_pos_y_origin - menu->character_height, 0.0 );

    initialize_text( text, &origin, Cursor_pos_colour,
                     (Font_types) Menu_window_font, menu->font_size );

    replace_string( &text->string, create_string(NULL) );

    add_object_to_model( model, object );
}

/**
 * Generates the text associated with the cursor position display
 * in the menu window. This shows the current position in world 
 * coordinates.
 */
  void  rebuild_cursor_position_model(
    display_struct    *display )
{
    text_struct     *text;
    model_struct    *cursor_pos_model;
    display_struct  *menu_window;
    char            buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    display_struct  *slice_window;
    int             volume_index;
    VIO_Point       cursor_origin;

    get_cursor_origin(display, &cursor_origin);

    if (!get_slice_window(display, &slice_window))
        return;

    menu_window = get_display_by_type( MENU_WINDOW );
    if( menu_window == NULL )
        return;

    cursor_pos_model = get_graphics_model( menu_window, CURSOR_POS_MODEL );

    if( cursor_pos_model->n_objects == 0 )
        create_cursor_pos_text( &menu_window->menu, cursor_pos_model );

    text = get_text_ptr( cursor_pos_model->objects[0] );
    text->size = menu_window->menu.font_size;
    fill_Point( text->origin,
                menu_window->menu.cursor_pos_x_origin,
                menu_window->menu.cursor_pos_y_origin, 0.0 );

    text = get_text_ptr( cursor_pos_model->objects[1] );
    text->size = menu_window->menu.font_size;
    fill_Point( text->origin,
                menu_window->menu.cursor_pos_x_origin,
                menu_window->menu.cursor_pos_y_origin -
                menu_window->menu.character_height, 0.0 );
    
    (void) snprintf( buffer, VIO_EXTREMELY_LARGE_STRING_SIZE,
                     Cursor_pos_format,
                     Point_x(cursor_origin),
                     Point_y(cursor_origin),
                     Point_z(cursor_origin) );

    /*
     * See if we need to display the time position. This is a more
     * work than the position, as we will have to transform the 
     * time correctly using starts and steps.
     */
    volume_index = get_current_volume_index(slice_window);
    if (volume_index >= 0)
    {
        VIO_Volume volume = get_nth_volume(slice_window, volume_index);
        if (get_volume_n_dimensions(volume) > 3)
        {
            VIO_Real voxel[VIO_MAX_DIMENSIONS];
            char temp[64];
            VIO_Real world_time;

            get_current_voxel(slice_window, volume_index, voxel);
            world_time = nonspatial_voxel_to_world(volume, VIO_T, voxel[VIO_T]);
            snprintf(temp, sizeof(temp), Cursor_time_format, world_time);
            strncat(buffer, temp, VIO_EXTREMELY_LARGE_STRING_SIZE);
        }
    }
    

    replace_string( &text->string, create_string(buffer) );

    set_update_required( menu_window, NORMAL_PLANES );
}
