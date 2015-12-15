/**
 * \file build_menu.c
 * \brief Functions to create the menu command structures.
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

typedef  struct
{
    int       key;              /**< BICGL code for this key. */
    VIO_STR   key_name;         /**< Text name for this key. */
    VIO_Real  x_pos;            /**< X position of key box. */
    VIO_Real  y_pos;            /**< Y position of key box. */
    VIO_Real  length;           /**< Length of key box, zero if invisible. */
    VIO_BOOL  in_slanted_part_of_keyboard; /**< True if rows are offset. */
} position_struct;

static   position_struct   positions[] = {
                           {(int) LEFT_ARROW_KEY, "<", 7.0, 0.0, 0.6, FALSE},
                           {(int) DOWN_ARROW_KEY, "v", 7.6, 0.0, 0.6, FALSE},
                           {(int) RIGHT_ARROW_KEY, ">", 8.2, 0.0, 0.6, FALSE},
                           {(int) UP_ARROW_KEY, "^", 7.6, 1.0, 0.6, FALSE},

                           {' ', "space", 1.0, 0.0, 3.0, FALSE },

                           {'z', "Z", 0.0, 1.0, 1.0, TRUE },
                           {'x', "X", 1.0, 1.0, 1.0, TRUE },
                           {'c', "C", 2.0, 1.0, 1.0, TRUE },
                           {'v', "V", 3.0, 1.0, 1.0, TRUE },
                           {'b', "B", 4.0, 1.0, 1.0, TRUE },
                           {'n', "N", 5.0, 1.0, 1.0, TRUE },
                           {'m', "M", 6.0, 1.0, 1.0, TRUE },

                           {'a', "A", 0.0, 2.0, 1.0, TRUE },
                           {'s', "S", 1.0, 2.0, 1.0, TRUE },
                           {'d', "D", 2.0, 2.0, 1.0, TRUE },
                           {'f', "F", 3.0, 2.0, 1.0, TRUE },
                           {'g', "G", 4.0, 2.0, 1.0, TRUE },
                           {'h', "H", 5.0, 2.0, 1.0, TRUE },
                           {'j', "J", 6.0, 2.0, 1.0, TRUE },

                           {'q', "Q", 0.0, 3.0, 1.0, TRUE },
                           {'w', "W", 1.0, 3.0, 1.0, TRUE },
                           {'e', "E", 2.0, 3.0, 1.0, TRUE },
                           {'r', "R", 3.0, 3.0, 1.0, TRUE },
                           {'t', "T", 4.0, 3.0, 1.0, TRUE },
                           {'y', "Y", 5.0, 3.0, 1.0, TRUE },
                           {'u', "U", 6.0, 3.0, 1.0, TRUE },

                           {'1', "1", 0.0, 4.0, 1.0, TRUE },
                           {'2', "2", 1.0, 4.0, 1.0, TRUE },
                           {'3', "3", 2.0, 4.0, 1.0, TRUE },
                           {'4', "4", 3.0, 4.0, 1.0, TRUE },
                           {'5', "5", 4.0, 4.0, 1.0, TRUE },
                           {'6', "6", 5.0, 4.0, 1.0, TRUE },
                           {'7', "7", 6.0, 4.0, 1.0, TRUE },

                           {'+', "+", 7.6, 2.0, 0.6, TRUE },
                           {'=', "=", 0.0, 0.0, 0.0, TRUE }, /* Invisible? */
                           {'>', ">", 0.0, 0.0, 0.0, TRUE }, /* Invisible? */
                           {'<', "<", 0.0, 0.0, 0.0, TRUE }, /* Invisible? */
                           {'-', "-", 8.3, 2.0, 0.6, TRUE },

                           {BICGL_F1_KEY, "F1", 0, 0, 0, FALSE },
                           {BICGL_F2_KEY, "F2", 0, 0, 0, FALSE },
                           {BICGL_F3_KEY, "F3", 0, 0, 0, FALSE },
                           {BICGL_F4_KEY, "F4", 0, 0, 0, FALSE },
                           {BICGL_F5_KEY, "F5", 0, 0, 0, FALSE },
                           {BICGL_F6_KEY, "F6", 0, 0, 0, FALSE },
                           {BICGL_F7_KEY, "F7", 0, 0, 0, FALSE },
                           {BICGL_F8_KEY, "F8", 0, 0, 0, FALSE },
                           {BICGL_F9_KEY, "F9", 0, 0, 0, FALSE },
                           {BICGL_F10_KEY, "F10", 0, 0, 0, FALSE },
                           {BICGL_F11_KEY, "F11", 0, 0, 0, FALSE },
                           {BICGL_F12_KEY, "F12", 0, 0, 0, FALSE },
                           {BICGL_PGUP_KEY, "Page Up", 0, 0, 0, FALSE },
                           {BICGL_PGDN_KEY, "Page Down", 0, 0, 0, FALSE },
                           {26, "Ctrl-Z", 0, 0, 0, FALSE },
                           {15, "Ctrl-O", 0, 0, 0, FALSE },
                           {19, "Ctrl-S", 0, 0, 0, FALSE },
                           { 2, "Ctrl-B", 0, 0, 0, FALSE },
                           {22, "Ctrl-V", 0, 0, 0, FALSE },
                           {27, "Esc", 0, 0, 0, FALSE }
                         };

static  void   create_menu_text(
    display_struct    *menu_window,
    menu_entry_struct *menu_entry );
static  VIO_BOOL  lookup_key(
    int              key,
    position_struct  **desc );
static  int  compute_key_position(
    menu_window_struct   *menu,
    int                  key,
    VIO_Real                 *x1,
    VIO_Real                 *y1,
    VIO_Real                 *x2,
    VIO_Real                 *y2,
    VIO_Real                 *length );
static  VIO_STR  get_key_string(
    int     key );
static  object_struct   *create_menu_box(
    display_struct    *menu_window,
    int               key );
static  object_struct   *create_menu_character(
    display_struct    *menu_window,
    int               key );
static  VIO_BOOL   point_within_menu_key_entry(
    display_struct   *menu_window,
    int              key,
    VIO_Real             x,
    VIO_Real             y );
static  void   set_menu_text_position(
    display_struct    *menu_window,
    menu_entry_struct *menu_entry );
static  void   set_menu_box_position(
    display_struct    *menu_window,
    int               key,
    object_struct     *box_object );
static  void   set_menu_character_position(
    display_struct    *menu_window,
    int               key,
    object_struct     *text_object );

/* -------------------------------------------------------------------- */

/** Given a newly created display_struct, create the graphical objects
 * corresponding to the text labels and boxes that make up the menu
 * window.
 * \param menu_window The menu window to build.
 */
void
build_menu(display_struct *menu_window)
{
    int      i;

    for_less( i, 0, VIO_SIZEOF_STATIC_ARRAY(positions) )
    {
        if( positions[i].length > 0.0 )
        {
            int key = positions[i].key;
            if (key < 0 || key >= N_CHARACTERS)
            {
                HANDLE_INTERNAL_ERROR("Key value outside legal range");
            }
            menu_window->menu.box_objects[key] =
                                   create_menu_box( menu_window, key );
            menu_window->menu.text_objects[key] =
                                   create_menu_character( menu_window, key );
        }
    }

    for_less( i, 1, menu_window->menu.n_entries )
        create_menu_text( menu_window, &menu_window->menu.entries[i] );

    rebuild_menu( menu_window );
}

/** Given an already filled-in display_struct, recalculate the positions
 * for the menu key boxes, their key labels, and their text objects.
 * \param menu_window The menu window to rebuild.
 */
void rebuild_menu(display_struct *menu_window)
{
    int      i;

    for_less( i, 0, VIO_SIZEOF_STATIC_ARRAY(positions) )
    {
        if( positions[i].length > 0.0 )
        {
            int key = positions[i].key;
            set_menu_box_position( menu_window, key,
                                   menu_window->menu.box_objects[key] );
            set_menu_character_position( menu_window, key,
                                     menu_window->menu.text_objects[key] );
        }
    }

    for_less( i, 1, menu_window->menu.n_entries )
    {
        menu_entry_struct *menu_entry = &menu_window->menu.entries[i];
        set_menu_text_position( menu_window, menu_entry );
        set_menu_text( menu_window, menu_entry, menu_entry->label );
        update_menu_text( menu_window, menu_entry );
    }
}


static  void   create_menu_text(
    display_struct    *menu_window,
    menu_entry_struct *menu_entry )
{
    int             i;
    VIO_Point           origin;
    text_struct     *text;
    model_struct    *model;

    ALLOC( menu_entry->text_list, menu_window->menu.n_lines_in_entry );

    for_less( i, 0, menu_window->menu.n_lines_in_entry )
    {
        menu_entry->text_list[i] = create_object( TEXT );

        menu_entry->text_list[i]->visibility = FALSE;

        model = get_graphics_model( menu_window, MENU_BUTTONS_MODEL );

        add_object_to_model( model, menu_entry->text_list[i] );

        text = get_text_ptr( menu_entry->text_list[i] );

        fill_Point( origin, 0.0, 0.0, 0.0 );

        initialize_text( text, &origin, Menu_character_colour,
                         (Font_types) Menu_window_font,
                         menu_window->menu.font_size );
    }

    set_menu_text_position( menu_window, menu_entry );

    set_menu_text( menu_window, menu_entry, menu_entry->label );

    update_menu_text( menu_window, menu_entry );
}

static  void   set_menu_text_position(
    display_struct    *menu_window,
    menu_entry_struct *menu_entry )
{
    VIO_Real            x, y, length;
    int             i;
    text_struct     *text;
    VIO_STR          key_string;

    for_less( i, 0, menu_window->menu.n_lines_in_entry )
    {
        text = get_text_ptr( menu_entry->text_list[i] );

        if (!compute_key_position( &menu_window->menu, menu_entry->key, &x, &y,
                                   NULL, NULL, &length )) {
            continue;
        }

        x += menu_window->menu.x_menu_text_offset;
        y += (VIO_Real) (menu_window->menu.n_lines_in_entry - i) *
             menu_window->menu.character_height -
             menu_window->menu.y_menu_text_offset;

        if( i == 0 )
        {
            key_string = get_key_string( menu_entry->key );
            x += G_get_text_length( key_string, (Font_types) Menu_window_font,
                                    menu_window->menu.font_size );
            delete_string( key_string );
        }

        fill_Point( text->origin, x, y, 0.0 );
        text->size = menu_window->menu.font_size;
    }

    menu_entry->key_text_width = (length * menu_window->menu.basic_key_width) - 
      (1.5 * menu_window->menu.x_menu_text_offset);
}

static  VIO_BOOL  lookup_key(
    int              key,
    position_struct  **desc )
{
    int      i;
    VIO_BOOL  found;

    found = FALSE;

    for_less( i, 0, VIO_SIZEOF_STATIC_ARRAY(positions) )
    {
        if( positions[i].key == key )
        {
            *desc = &positions[i];
            found = TRUE;
            break;
        }
    }

    return( found );
}

static  int   compute_key_position(
    menu_window_struct   *menu,
    int                  key,
    VIO_Real                 *x1,
    VIO_Real                 *y1,
    VIO_Real                 *x2,
    VIO_Real                 *y2,
    VIO_Real                 *length )
{
    VIO_BOOL          found;
    position_struct  *desc;
    VIO_Real             x_dx, y_dy;

    found = lookup_key( key, &desc );

    if( !found )
    {
        print( "Warning: Character %c (\\0%o) is not in the menu.\n", 
               key, key );
    }
    else if( desc->length <= 0.0 )
    {
        *x1 = 0.0;
        if( x2 != NULL )
            *x2 = 0.0;
        *y1 = 0.0;
        if( y2 != NULL )
            *y2 = 0.0;
        *length = 0.0;
    }
    else
    {
        x_dx = menu->x_dx + menu->basic_key_width;
                                                       
        y_dy = menu->y_dy + (VIO_Real) menu->n_lines_in_entry *
               menu->character_height;

        *x1 = menu->x_menu_origin + (VIO_Real) desc->x_pos * x_dx;
        *y1 = menu->y_menu_origin + (VIO_Real) desc->y_pos * y_dy;

        if( desc->in_slanted_part_of_keyboard )
        {
            *x1 += (VIO_Real) desc->y_pos * menu->y_dx;
            *y1 += (VIO_Real) desc->x_pos * menu->x_dy;
        }

        *length = desc->length;

        if( x2 != NULL )
        {
            *x2 = *x1 + *length * menu->basic_key_width;
        }

        if( y2 != NULL )
        {
            *y2 = *y1 + (VIO_Real) menu->n_lines_in_entry * menu->character_height;
        }
    }
    return found;
}

static  VIO_STR  get_key_string(
    int      key )
{
    char              buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_STR            string;
    position_struct   *desc;

    if( lookup_key( key, &desc ) )
    {
        (void) sprintf( buffer, "%s ", desc->key_name );
        string = create_string( buffer );
    }
    else
        string = create_string( NULL );

    return( string );
}

  VIO_Real  get_size_of_menu_text_area(
    display_struct   *menu_window,
    int              key,
    int              line_number )
{
    VIO_Real          size, x1, y1, x2, y2, length;
    text_struct   *text;

    if (!compute_key_position( &menu_window->menu, key, &x1, &y1, &x2, &y2, &length)) {
      return 0;
    }

    text = get_text_ptr(
                menu_window->menu.key_menus[key]->text_list[line_number] );

    size = x2 - (VIO_Real) Point_x(text->origin);

    return( size );
}

static  object_struct   *create_menu_box(
    display_struct    *menu_window,
    int               key )
{
    object_struct   *object;
    lines_struct    *lines;
    model_struct    *model;

    object = create_object( LINES );

    model = get_graphics_model( menu_window, MENU_BUTTONS_MODEL );

    add_object_to_model( model, object );

    lines = get_lines_ptr( object );
    initialize_lines( lines, Menu_box_colour );

    lines->line_thickness = 1.0f;
    lines->n_points = 4;
    lines->n_items = 1;

    ALLOC( lines->points, lines->n_points );
    ALLOC( lines->end_indices, lines->n_items );

    lines->end_indices[0] = 5;

    ALLOC( lines->indices, lines->end_indices[0] );

    lines->indices[0] = 0;
    lines->indices[1] = 1;
    lines->indices[2] = 2;
    lines->indices[3] = 3;
    lines->indices[4] = 0;

    return( object );
}

static  void   set_menu_box_position(
    display_struct    *menu_window,
    int               key,
    object_struct     *box_object )
{
    lines_struct    *lines;
    VIO_Real            x1, y1, x2, y2, length;

    lines = get_lines_ptr( box_object );

    if (!compute_key_position( &menu_window->menu, key, &x1, &y1, &x2, &y2, &length)) {
      return;
    }

    fill_Point( lines->points[0], x1, y1, 0.0 );
    fill_Point( lines->points[1], x2, y1, 0.0 );
    fill_Point( lines->points[2], x2, y2, 0.0 );
    fill_Point( lines->points[3], x1, y2, 0.0 );
}

static  object_struct   *create_menu_character(
    display_struct    *menu_window,
    int               key )
{
    VIO_Real            x, y, length;
    VIO_Point           origin;
    object_struct   *object;
    text_struct     *text;
    model_struct    *model;

    object = create_object( TEXT );

    if (!compute_key_position( &menu_window->menu, key, &x, &y, NULL, NULL, &length)) {
      return NULL;
    }

    fill_Point( origin,
                x + menu_window->menu.x_menu_text_offset,
                y + (VIO_Real) menu_window->menu.n_lines_in_entry *
                    menu_window->menu.character_height -
                    menu_window->menu.y_menu_text_offset,
                0.0 );

    text = get_text_ptr( object );

    initialize_text( text, &origin, Menu_key_colour,
                     (Font_types) Menu_window_font,
                     menu_window->menu.font_size );

    replace_string( &text->string, get_key_string( key ) );

    model = get_graphics_model( menu_window, MENU_BUTTONS_MODEL );

    add_object_to_model( model, object );

    return( object );
}

static  void   set_menu_character_position(
    display_struct    *menu_window,
    int               key,
    object_struct     *text_object )
{
    VIO_Real            x, y, length;
    text_struct     *text;

    text = get_text_ptr( text_object );

    if (!compute_key_position( &menu_window->menu, key, &x, &y, NULL, NULL, &length)) {
      return;
    }

    fill_Point( text->origin,
                x + menu_window->menu.x_menu_text_offset,
                y + (VIO_Real) menu_window->menu.n_lines_in_entry *
                    menu_window->menu.character_height -
                    menu_window->menu.y_menu_text_offset,
                0.0 );

    text->size = menu_window->menu.font_size;
}

static  VIO_BOOL   point_within_menu_key_entry(
    display_struct   *menu_window,
    int              key,
    VIO_Real             x,
    VIO_Real             y )
{
    VIO_Real      x1, y1, x2, y2, length;

    if (!compute_key_position( &menu_window->menu, key, &x1, &y1, &x2, &y2, &length)) {
      return FALSE;
    }

    if( length <= 0.0 )
        return( FALSE );
    else
        return( x >= x1 && x <= x2 && y >= y1 && y <= y2 );
}

  VIO_BOOL   lookup_key_for_mouse_position(
    display_struct   *menu_window,
    VIO_Real             x,
    VIO_Real             y,
    int              *key )
{
    int      i;
    VIO_BOOL  found;

    found = FALSE;

    for_less( i, 0, VIO_SIZEOF_STATIC_ARRAY(positions) )
    {
        if( point_within_menu_key_entry( menu_window, positions[i].key, x, y ) )
        {
            *key = positions[i].key;
            found = TRUE;
            break;
        }
    }

    return( found );
}
