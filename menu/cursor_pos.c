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

private  void  create_cursor_pos_text(
    menu_window_struct  *menu,
    model_struct        *model )
{
    object_struct  *object;
    Point          origin;
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

public  void  rebuild_cursor_position_model(
    display_struct    *display )
{
    text_struct     *text;
    model_struct    *cursor_pos_model;
    display_struct  *menu_window, *marker_window;
    char            buffer[EXTREMELY_LARGE_STRING_SIZE];

    menu_window = display->associated[MENU_WINDOW];
    marker_window = display->associated[MARKER_WINDOW];

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

    (void) sprintf( buffer, Cursor_pos_format,
                    Point_x(display->three_d.cursor.origin),
                    Point_y(display->three_d.cursor.origin),
                    Point_z(display->three_d.cursor.origin) );

    replace_string( &text->string, create_string(buffer) );

    set_update_required( menu_window, NORMAL_PLANES );
    set_update_required( marker_window, NORMAL_PLANES );
}
