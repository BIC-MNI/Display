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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/menu/menu.c,v 1.45 2001/08/16 23:04:07 stever Exp $";
#endif


#include  <display.h>

static    DEF_EVENT_FUNCTION( handle_character_down );
static    DEF_EVENT_FUNCTION( handle_character_up );
static    DEF_EVENT_FUNCTION( handle_leaving_window );
static    DEF_EVENT_FUNCTION( left_mouse_press );
static    DEF_EVENT_FUNCTION( middle_mouse_press );
private  void  turn_off_menu_entry(
    menu_window_struct  *menu,
    menu_entry_struct   *menu_entry );
private  Status  handle_menu_for_key(
    display_struct      *menu_window,
    int                 key );
private  Status  process_menu(
    display_struct      *display,
    menu_entry_struct   *menu_entry );
private  Status  handle_mouse_press_in_menu(
    display_struct      *menu_window,
    Real                x,
    Real                y );
private  void  update_menu_name_text(
    display_struct   *menu_window );

private  void  set_menu_key_entry(
    menu_window_struct     *menu,
    int                    ch,
    menu_entry_struct      *menu_entry )
{
    menu->key_menus[ch] = menu_entry;
}

private  menu_entry_struct  *get_menu_key_entry(
    menu_window_struct     *menu,
    int                    ch )
{
    return( menu->key_menus[ch] );
}

private  void  turn_on_menu_entry(
    menu_window_struct     *menu,
    menu_entry_struct      *menu_entry )
{
    int                 i;
    menu_entry_struct   *previous;

    previous = get_menu_key_entry( menu, menu_entry->key );
    if( previous != (menu_entry_struct *) 0 )
    {
        turn_off_menu_entry( menu, previous );
    }

    for_less( i, 0, menu->n_lines_in_entry )
        menu_entry->text_list[i]->visibility = TRUE;

    menu_entry->current_depth = menu->depth;

    set_menu_key_entry( menu, menu_entry->key, menu_entry );
}

private  void  turn_off_menu_entry(
    menu_window_struct  *menu,
    menu_entry_struct   *menu_entry )
{
    int                 i;

    for_less( i, 0, menu->n_lines_in_entry )
        menu_entry->text_list[i]->visibility = FALSE;

    set_menu_key_entry( menu, menu_entry->key, (menu_entry_struct *) 0 );
}

private  void  add_menu_actions(
    menu_window_struct  *menu,
    menu_entry_struct   *menu_entry )
{
    int   i;

    for_less( i, 0, menu_entry->n_children )
        turn_on_menu_entry( menu, menu_entry->children[i] );
}

private  void  remove_menu_actions(
    menu_window_struct  *menu,
    menu_entry_struct   *menu_entry )
{
    int   i;

    for_less( i, 0, menu_entry->n_children )
    {
        if( !menu_entry->children[i]->permanent_flag )
            turn_off_menu_entry( menu, menu_entry->children[i] );
    }
}

private  void  initialize_menu_parameters(
    display_struct    *menu_window )
{
    int                 x_size, y_size;
    Real                x_scale, y_scale, scale;
    menu_window_struct  *menu;

    menu = &menu_window->menu;

    G_get_window_size( menu_window->window, &x_size, &y_size );

    x_scale = (Real) x_size / (Real) menu->default_x_size;
    y_scale = (Real) y_size / (Real) menu->default_y_size;

    scale = MIN( x_scale, y_scale );

    menu->x_dx = scale * X_menu_dx;
    menu->x_dy = scale * X_menu_dy;
    menu->y_dx = scale * Y_menu_dx;
    menu->y_dy = scale * Y_menu_dy;
    menu->n_chars_per_unit_across = Menu_n_chars_per_entry;
    menu->n_lines_in_entry = Menu_n_lines_per_entry;
    menu->character_width = scale * Menu_character_width;
    menu->character_height = scale * Menu_character_height;
    menu->character_offset = scale * Menu_key_character_offset;
    menu->x_menu_text_offset = scale * X_menu_text_offset;
    menu->y_menu_text_offset = scale * Y_menu_text_offset;
    menu->x_menu_origin = scale * X_menu_origin;
    menu->y_menu_origin = scale * Y_menu_origin;
    menu->cursor_pos_x_origin = scale * Cursor_pos_x_origin;
    menu->cursor_pos_y_origin = scale * Cursor_pos_y_origin;
    menu->selected_x_origin = scale * Selected_x_origin;
    menu->selected_y_origin = scale * Selected_y_origin;
    menu->selected_x_offset = scale * Selected_box_x_offset;
    menu->selected_y_offset = scale * Selected_box_y_offset;
    menu->selected_box_height = scale * Character_height_in_pixels;
    menu->x_menu_name = scale * Menu_name_x;
    menu->y_menu_name = scale * Menu_name_y;
    menu->font_size = scale * Menu_window_font_size;
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_menu_resize )
{
    display_struct  *menu_window, *three_d;

    three_d = display->associated[THREE_D_WINDOW];
    menu_window = three_d->associated[MENU_WINDOW];

    initialize_menu_parameters( menu_window );
    rebuild_menu( menu_window );
    rebuild_cursor_position_model( three_d );
    update_menu_name_text( menu_window );

    return( OK );
}



/* Allow builder to disable compiled-in menu.  In this case, Display.menu needs
 * to be installed.  The current mac OSX compiler fails here.  (July 2001).
 */
static STRING default_menu_string = 
#if DISPLAY_DISABLE_MENU_FALLBACK
    NULL
#else
#   include "Display.menu.include"
#endif
;

public  Status  initialize_menu(
    display_struct    *menu_window,
    STRING            default_directory1,
    STRING            default_directory2,
    STRING            default_directory3,
    STRING            default_directory4,
    STRING            menu_filename )
{
    Status               status;
    STRING               filename;
    menu_window_struct   *menu;
    Point                position;
    model_struct         *model;
    int                  ch, i, dir, len;
    FILE                 *file;
    BOOLEAN              found;
    STRING               directories[5];

    menu = &menu_window->menu;

    G_set_transparency_state( menu_window->window, OFF );

    initialize_resize_events( menu_window );

    add_action_table_function( &menu_window->action_table, WINDOW_RESIZE_EVENT,
                               handle_menu_resize );

    menu->default_x_size = Canonical_menu_window_width;
    menu->default_y_size = Canonical_menu_window_height;

    initialize_menu_parameters( menu_window );

    for_less( ch, 0, N_CHARACTERS )
        set_menu_key_entry( menu, ch, NULL );

    directories[0] = "";
    directories[1] = default_directory1;
    directories[2] = default_directory2;
    directories[3] = default_directory3;
    directories[4] = default_directory4;



    for_less( dir, 0, 5 )
    {
        filename = get_absolute_filename( menu_filename, directories[dir] );

        if( getenv("DISABLE_MENU") == NULL && file_exists( filename ) )
            break;

        delete_string( filename );
    }

    found = dir < 5;

    if( !found )
    {
	if( default_menu_string == NULL ) 
	{
            print_error(
                "Cannot find menu file %s and no compiled-in fallback\n",
                menu_filename );
            return( ERROR );
        }

        filename = get_temporary_filename();

        if( open_file( filename, WRITE_FILE, ASCII_FORMAT, &file ) != OK )
        {
            print_error(
                "Cannot open temporary Display.menu file for write: %s\n",
                filename );
            return( ERROR );
        }

        len = string_length( default_menu_string );
        for_less( i, 0, len )
            (void) output_character( file, default_menu_string[i] );

        (void) close_file( file );
    }

    status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( !found )                    /*--- if this temporary file */
        remove_file( filename );

    delete_string( filename );

    if( status == OK )
    {
        status = read_menu( menu, file );

        menu->depth = 0;
        menu->stack[0] = &menu->entries[0];
        menu->entries[0].current_depth = 0;
    }

    if( status == OK )
    {
        build_menu( menu_window );

        add_menu_actions( menu, &menu->entries[0] );

        set_update_required( menu_window, NORMAL_PLANES );

        status = close_file( file );
    }

    model = get_graphics_model( menu_window, UTILITY_MODEL );
    menu->menu_name_text = create_object( TEXT );
    fill_Point( position, menu->x_menu_name, menu->y_menu_name, 0.0 );
    initialize_text( get_text_ptr(menu->menu_name_text), &position,
                     Menu_name_colour, (Font_types) Menu_name_font,
                     menu->font_size );

    add_object_to_model( model, menu->menu_name_text );

    update_menu_name_text( menu_window );

    rebuild_cursor_position_model( menu_window );

    return( status );
}

public  void  initialize_menu_actions(
    display_struct    *menu_window )
{
    add_action_table_function( &menu_window->action_table, KEY_DOWN_EVENT,
                               handle_character_down );
    add_action_table_function( &menu_window->action_table, KEY_UP_EVENT,
                               handle_character_up );
    add_action_table_function( &menu_window->action_table, WINDOW_LEAVE_EVENT,
                               handle_leaving_window );
}

public  void  initialize_menu_window(
    display_struct    *menu_window )
{
    add_action_table_function( &menu_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT, left_mouse_press );
    add_action_table_function( &menu_window->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT, middle_mouse_press );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_character_down )
{
    Status             status;
    display_struct     *menu_window;

    status = OK;

    menu_window = display->associated[MENU_WINDOW];

    status = handle_menu_for_key( menu_window, key_pressed );

    return( status );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_leaving_window )
{
    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_character_up )
{
    return( OK );
}

private  BOOLEAN  is_menu_entry_active(
    menu_entry_struct  *menu_entry )
{
    return( menu_entry->is_active );
}

private  Status  handle_menu_for_key(
    display_struct      *menu_window,
    int                 key )
{
    Status             status;
    menu_entry_struct  *menu_entry;

    status = OK;

    menu_entry = get_menu_key_entry( &menu_window->menu, key );

    if( menu_entry != (menu_entry_struct *) 0 &&
        is_menu_entry_active( menu_entry ) )
        status = process_menu( menu_window, menu_entry );

    return( status );
}

private  Status  process_menu(
    display_struct      *display,
    menu_entry_struct   *menu_entry )
{
    Status                  status;
    menu_function_pointer   function;

    function = menu_entry->action;

    status = (*function)( display->associated[THREE_D_WINDOW],
                          display->associated[MENU_WINDOW],
                          menu_entry );

    if( status == OK )
        update_all_menu_text( display );

    return( status );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( left_mouse_press )
{
    Status  status;
    int     x, y;

    status = OK;

    if( G_get_mouse_position( display->window, &x, &y ) )
    {
        status = handle_mouse_press_in_menu( display, (Real) x, (Real) y );
    }

    return( status );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( middle_mouse_press )
{
    pop_menu_one_level( display );

    return( OK );
}

private  Status  handle_mouse_press_in_menu(
    display_struct      *menu_window,
    Real                x,
    Real                y )
{
    display_struct      *three_d;
    Status              status;
    int                 key;
    object_struct       *object, *current;

    status = OK;

    three_d = get_three_d_window( menu_window );

    if( lookup_key_for_mouse_position( menu_window, x, y, &key ) )
    {
        status = handle_menu_for_key( menu_window, key );
    }
    else if( mouse_is_on_object_name( three_d, ROUND(x), ROUND(y), &object ) )
    {
        if( get_current_object( three_d, &current ) &&
            current == object && get_object_type(object) == MODEL )
        {
            push_current_object( three_d );
        }
        else
            set_current_object( three_d, object );

        rebuild_selected_list( three_d, menu_window );
        update_all_menu_text( menu_window );
    }


    return( status );
}

public  void  update_menu_text(
    display_struct      *display,
    menu_entry_struct   *menu_entry )
{
    Colour                  colour;
    int                     i;
    BOOLEAN                 active;
    menu_update_pointer     update_function;
    display_struct          *menu_window;

    menu_window = display->associated[MENU_WINDOW];

    update_function = menu_entry->update_action;

    active = (*update_function)( display->associated[THREE_D_WINDOW],
                                 menu_window, menu_entry );

    menu_entry->is_active = active;

    if( active )
        colour = Menu_character_colour;
    else
        colour = Menu_character_inactive_colour;

    for_less( i, 0, menu_window->menu.n_lines_in_entry )
        get_text_ptr(menu_entry->text_list[i])->colour = colour;
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( push_menu )
{
    Status   status;

    status = OK;

    while( menu_window->menu.depth > menu_entry->current_depth )
    {
        remove_menu_actions( &menu_window->menu,
                             menu_window->menu.stack[menu_window->menu.depth] );

        --menu_window->menu.depth;
    }

    remove_menu_actions( &menu_window->menu,
                         menu_window->menu.stack[menu_window->menu.depth] );

    if( menu_window->menu.depth >= MAX_MENU_DEPTH )
    {
        print( "Max menu depth\n" );
        status = ERROR;
    }
    else
    {
        ++menu_window->menu.depth;
        menu_window->menu.stack[menu_window->menu.depth] = menu_entry;

        add_menu_actions( &menu_window->menu, menu_entry );

        update_menu_name_text( menu_window );

        set_update_required( menu_window, NORMAL_PLANES );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(push_menu )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( pop_menu )
{
    pop_menu_one_level( menu_window );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(pop_menu )
{
    return( menu_window->menu.depth > 0 );
}

public  void  pop_menu_one_level(
    display_struct   *menu_window )
{
    if( menu_window->menu.depth > 0 )
    {
        remove_menu_actions( &menu_window->menu,
                             menu_window->menu.stack[menu_window->menu.depth] );

        --menu_window->menu.depth;

        add_menu_actions( &menu_window->menu,
                          menu_window->menu.stack[menu_window->menu.depth] );

        update_menu_name_text( menu_window );

        set_update_required( menu_window, NORMAL_PLANES );
    }
}

public  void   set_menu_text(
    display_struct      *menu_window,
    menu_entry_struct   *menu_entry,
    STRING              text )
{
    int                 i, line, n_chars, len, n_chars_across;
    STRING              *text_ptr;
    menu_window_struct  *menu;

    menu = &menu_window->menu;

    len = string_length( text );
    n_chars = 0;

    for_less( line, 0, menu->n_lines_in_entry )
    {
        text_ptr = &get_text_ptr(menu_entry->text_list[line])->string;
        delete_string( *text_ptr );
        *text_ptr = create_string( NULL );

        n_chars_across = menu_entry->n_chars_across;

        if( line == 0 )
            n_chars_across = ROUND( (Real) n_chars_across -
                                    menu->character_offset );

        i = 0;

        while( n_chars < len && i < n_chars_across )
        {
            if( text[n_chars] == '\n' ||
                (text[n_chars] == ' ' &&
                 (len - n_chars-1) <=
                 (menu->n_lines_in_entry-line-1) * menu_entry->n_chars_across
                 - ROUND(menu->character_offset)) )
            {
                ++n_chars;
                break;
            }

            concat_char_to_string( text_ptr, text[n_chars] );
            ++n_chars;
            ++i;
        }
    }

    set_update_required( menu_window, NORMAL_PLANES );
}

public  void  update_all_menu_text(
    display_struct   *display )
{
    int                 key;
    display_struct      *menu_window, *marker_window;
    menu_entry_struct   *menu_entry;

    menu_window = display->associated[MENU_WINDOW];
    marker_window = display->associated[MARKER_WINDOW];

    for_less( key, 0, N_CHARACTERS )
    {
        menu_entry = get_menu_key_entry( &menu_window->menu, key );

        if( menu_entry != (menu_entry_struct *) 0 )
            update_menu_text( display, menu_entry );
    }

    set_update_required( menu_window, NORMAL_PLANES );
    set_update_required( marker_window, NORMAL_PLANES );
}

private  void  update_menu_name_text(
    display_struct   *menu_window )
{
    STRING       new_value;
    text_struct  *text;

    text = get_text_ptr( menu_window->menu.menu_name_text );

    fill_Point( text->origin, menu_window->menu.x_menu_name,
                menu_window->menu.y_menu_name, 0.0 );
    text->size = menu_window->menu.font_size;

    new_value = menu_window->menu.stack[menu_window->menu.depth]->label;

    if( !equal_strings( text->string, new_value ) )
        replace_string( &text->string, create_string(new_value) );
}
