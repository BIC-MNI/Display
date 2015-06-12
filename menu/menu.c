/** \file menu.c
 * \brief Event handling for the menu window.
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

static    DEF_EVENT_FUNCTION( handle_character_down );
static    DEF_EVENT_FUNCTION( handle_character_up );
static    DEF_EVENT_FUNCTION( handle_leaving_window );

static    DEF_EVENT_FUNCTION( left_mouse_press );
static    DEF_EVENT_FUNCTION( middle_mouse_press );
static    DEF_EVENT_FUNCTION( handle_mouse_position );

static  void  turn_off_menu_entry(
    menu_window_struct  *menu,
    menu_entry_struct   *menu_entry );
static  VIO_Status  handle_menu_for_key(
    display_struct      *menu_window,
    int                 key );
static  VIO_Status  process_menu(
    display_struct      *display,
    menu_entry_struct   *menu_entry );
static  VIO_Status  handle_mouse_press_in_menu(
    display_struct      *menu_window,
    VIO_Real                x,
    VIO_Real                y );
static  void  update_menu_name_text(
    display_struct   *menu_window );

static VIO_BOOL update_menu_help_text(
    display_struct   *menu_window, VIO_STR help_text );

static  void  set_menu_key_entry(
    menu_window_struct     *menu,
    int                    ch,
    menu_entry_struct      *menu_entry )
{
    menu->key_menus[ch] = menu_entry;
}

static  menu_entry_struct  *get_menu_key_entry(
    menu_window_struct     *menu,
    int                    ch )
{
    return( menu->key_menus[ch] );
}

static  void  turn_on_menu_entry(
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

static  void  turn_off_menu_entry(
    menu_window_struct  *menu,
    menu_entry_struct   *menu_entry )
{
    int                 i;

    for_less( i, 0, menu->n_lines_in_entry )
        menu_entry->text_list[i]->visibility = FALSE;

    set_menu_key_entry( menu, menu_entry->key, (menu_entry_struct *) 0 );
}

/**
 * Show all of the child entries associated with a menu_entry_struct.
 *
 * \param menu The menu_window_struct defining the state of the menu window.
 * \param menu_entry The menu_entry_struct whose commands should be shown.
 */
static  void  add_menu_actions(
    menu_window_struct  *menu,
    menu_entry_struct   *menu_entry )
{
    int   i;

    for_less( i, 0, menu_entry->n_children )
        turn_on_menu_entry( menu, menu_entry->children[i] );
}

/**
 * Hide all of the child entries associated with a  menu_entry_struct. 
 * Does not remove entries if the permanent_flag is TRUE. Used when popping
 * up the menu hierarchy.
 *
 * \param menu The menu_window_struct defining the state of the menu window.
 * \param menu_entry The menu_entry_struct whose commands should be hidden.
 */
static  void  remove_menu_actions(
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

/**
 * Calculates the basic width of the keys. This was determined
 * empirically to give decent results. I could not find a string
 * which gave consistent results when fed to G_get_text_length().
 */
static VIO_Real 
get_basic_menu_key_width(menu_window_struct *menu)
{
  return 13.0 * (3.0 * menu->font_size / 5.0);
}

/**
 * Calculates the character offset that accounts for the width of the
 * fixed key labels.
 */
static int 
get_menu_character_offset(menu_window_struct *menu)
{
  return G_get_text_length("X ", Menu_window_font, menu->font_size);
}

/**
 * Recompute the menu's layout parameters after a resize.
 */
static void
initialize_menu_parameters(display_struct *menu_window)
{
    int                 x_size, y_size;
    VIO_Real            x_scale, y_scale, scale;
    menu_window_struct  *menu;

    menu = &menu_window->menu;

    G_get_window_size( menu_window->window, &x_size, &y_size );

    x_scale = (VIO_Real) x_size / (VIO_Real) menu->default_x_size;
    y_scale = (VIO_Real) y_size / (VIO_Real) menu->default_y_size;

    scale = MIN( x_scale, y_scale );

    menu->x_dx = scale * X_menu_dx;
    menu->x_dy = scale * X_menu_dy;
    menu->y_dx = scale * Y_menu_dx;
    menu->y_dy = scale * Y_menu_dy;
    menu->n_lines_in_entry = Menu_n_lines_per_entry;
    menu->x_menu_text_offset = scale * X_menu_text_offset;
    menu->y_menu_text_offset = scale * Y_menu_text_offset;
    menu->x_menu_origin = scale * X_menu_origin;
    menu->y_menu_origin = scale * Y_menu_origin;
    menu->cursor_pos_x_origin = scale * Cursor_pos_x_origin;
    menu->cursor_pos_y_origin = scale * Cursor_pos_y_origin;
    menu->help_x_origin = scale * 640;
    menu->help_y_origin = scale * 220;
    menu->x_menu_name = scale * Menu_name_x;
    menu->y_menu_name = scale * Menu_name_y;
    menu->font_size = scale * Menu_window_font_size;

    /* Do this after setting the font size above. 
     */
    menu->basic_key_width = get_basic_menu_key_width(menu);
    menu->character_height = G_get_text_height(Menu_window_font,
                                               menu->font_size) * 2.0;
    menu->character_offset = get_menu_character_offset(menu);
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_menu_resize )
{
    display_struct  *menu_window, *three_d;

    three_d = get_three_d_window(display);
    menu_window = three_d->associated[MENU_WINDOW];

    initialize_menu_parameters( menu_window );
    rebuild_menu( menu_window );
    rebuild_cursor_position_model( three_d );
    update_menu_name_text( menu_window );

    return( VIO_OK );
}



/* Allow builder to disable compiled-in menu.  In this case, Display.menu needs
 * to be installed.  The current mac OSX compiler fails here.  (July 2001).
 */
static VIO_STR default_menu_string = 
#if DISPLAY_DISABLE_MENU_FALLBACK
    NULL
#else
#   include "Display.menu.include"
#endif
;

  VIO_Status  initialize_menu(
    display_struct    *menu_window,
    VIO_STR            default_directory1,
    VIO_STR            default_directory2,
    VIO_STR            default_directory3,
    VIO_STR            default_directory4,
    VIO_STR            menu_filename )
{
    VIO_Status               status;
    VIO_STR               filename;
    menu_window_struct   *menu;
    VIO_Point                position;
    model_struct         *model;
    int                  ch, i, dir, len;
    FILE                 *file;
    VIO_BOOL              found;
    VIO_STR               directories[5];

    menu = &menu_window->menu;

    G_set_transparency_state( menu_window->window, FALSE );

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
            return( VIO_ERROR );
        }

        filename = get_temporary_filename();

        if( open_file( filename, WRITE_FILE, ASCII_FORMAT, &file ) != VIO_OK )
        {
            print_error(
                "Cannot open temporary Display.menu file for write: %s\n",
                filename );
            return( VIO_ERROR );
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

    if( status == VIO_OK )
    {
        status = read_menu( menu, file );

        menu->depth = 0;
        menu->stack[0] = &menu->entries[0];
        menu->entries[0].current_depth = 0;
    }

    if( status == VIO_OK )
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

    for (i = 0; i < N_HELP_LINES; i++)
    {
      menu->menu_help_text[i] = create_object( TEXT );
      fill_Point( position, menu->help_x_origin, menu->help_y_origin - (i * menu->font_size), 0.0);
      initialize_text( get_text_ptr(menu->menu_help_text[i]), &position,
                       WHITE, (Font_types) Menu_name_font,
                       menu->font_size );
      add_object_to_model( model, menu->menu_help_text[i] );
    }

    rebuild_cursor_position_model( menu_window );

    return( status );
}

/**
 * This function is used to install handlers for actions in EVERY window, 
 * so that we can intercept keys in each application window. As a result,
 * the event functions have to assume they can be called in the context
 * of any application window.
 */
void  initialize_menu_actions(
    display_struct    *menu_window )
{
    add_action_table_function( &menu_window->action_table, KEY_DOWN_EVENT,
                               handle_character_down );
    add_action_table_function( &menu_window->action_table, KEY_UP_EVENT,
                               handle_character_up );
    add_action_table_function( &menu_window->action_table, WINDOW_LEAVE_EVENT,
                               handle_leaving_window );
}

/**
 * This function is used to install handlers for actions in the menu window
 * only. This handles mouse events, for example, within the menu.
 */
void  initialize_menu_window(
    display_struct    *menu_window )
{

    add_action_table_function( &menu_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT, left_mouse_press );
    add_action_table_function( &menu_window->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT, middle_mouse_press );
    add_action_table_function( &menu_window->action_table, NO_EVENT,
                               handle_mouse_position );
}

static  DEF_EVENT_FUNCTION( handle_mouse_position )
{
    int x, y;

    if( G_get_mouse_position( display->window, &x, &y ) )
    {
        int key;
        menu_entry_struct *menu_entry;
        if( lookup_key_for_mouse_position( display, x, y, &key ) &&
            (menu_entry = get_menu_key_entry( &display->menu, key )) != NULL)
        {
          if (update_menu_help_text(display, menu_entry->help_text))
          {
            set_update_required( display, NORMAL_PLANES );
          }
        }
        else
        {
          if (update_menu_help_text(display, "Hover over a virtual key for help.")) 
          {
            set_update_required( display, NORMAL_PLANES );
          }
        }
    }
    return VIO_OK;
}

/* ARGSUSED */

/** 
 * This function is somewhat unusual in that it can be called from a 
 * variety of contexts (windows), so we can't count on the callback
 * window being the menu window. So we do need to get the menu window
 * as the first thing.
 */
static  DEF_EVENT_FUNCTION( handle_character_down )
{
    display_struct  *menu_window = display->associated[MENU_WINDOW];

    return handle_menu_for_key( menu_window, key_pressed );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_leaving_window )
{
    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_character_up )
{
    return( VIO_OK );
}

static  VIO_BOOL  is_menu_entry_active(
    menu_entry_struct  *menu_entry )
{
    return( menu_entry->is_active );
}

static  VIO_Status  handle_menu_for_key(
    display_struct      *menu_window,
    int                 key )
{
    VIO_Status             status;
    menu_entry_struct  *menu_entry;

    status = VIO_OK;

    menu_entry = get_menu_key_entry( &menu_window->menu, key );

    if( menu_entry != (menu_entry_struct *) 0 &&
        is_menu_entry_active( menu_entry ) )
        status = process_menu( menu_window, menu_entry );

    return( status );
}

static  VIO_Status  process_menu(
    display_struct      *display,
    menu_entry_struct   *menu_entry )
{
    VIO_Status                  status;
    menu_function_pointer   function;

    function = menu_entry->action;

    status = (*function)( get_three_d_window(display),
                          display->associated[MENU_WINDOW],
                          menu_entry );

    if( status == VIO_OK )
        update_all_menu_text( display );

    return( status );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( left_mouse_press )
{
    VIO_Status  status;
    int     x, y;

    status = VIO_OK;

    if( G_get_mouse_position( display->window, &x, &y ) )
    {
        status = handle_mouse_press_in_menu( display, (VIO_Real) x, (VIO_Real) y );
    }

    return( status );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( middle_mouse_press )
{
    pop_menu_one_level( display );

    return( VIO_OK );
}

static  VIO_Status  handle_mouse_press_in_menu(
    display_struct      *menu_window,
    VIO_Real            x,
    VIO_Real            y )
{
    VIO_Status          status;
    int                 key;

    status = VIO_OK;

    if( lookup_key_for_mouse_position( menu_window, x, y, &key ) )
    {
        status = handle_menu_for_key( menu_window, key );
    }

    return( status );
}

/** 
 * Given a pointer to the menu window and menu entry, this function
 * will call the update function for this menu entry. This will update
 * the appearance and active status of the menu item.
 */
void  update_menu_text(
    display_struct      *menu_window,
    menu_entry_struct   *menu_entry )
{
    VIO_Colour              colour;
    int                     i;
    VIO_BOOL                active;
    menu_update_pointer     update_function;

    update_function = menu_entry->update_action;

    active = (*update_function)( get_three_d_window(menu_window),
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

DEF_MENU_FUNCTION( push_menu )
{
    VIO_Status   status;

    status = VIO_OK;

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
        status = VIO_ERROR;
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

DEF_MENU_UPDATE(push_menu )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( pop_menu )
{
    pop_menu_one_level( menu_window );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(pop_menu )
{
    return( menu_window->menu.depth > 0 );
}

/**
 * Pops the menu to the next level up in the hierarchy.
 * If the current menu.depth field is zero or less, does nothing.
 * Otherwise, it removes all of the menu actions associated with the
 * current 
 */
void  pop_menu_one_level(
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

/**
 * Set the text for a menu entry, breaking up the text into 
 * separate lines semi-intelligently.
 * \param menu_window The menu window structure
 * \param menu_entry The menu entry information.
 * \param full_text The full text which should be displayed for this entry.
 */
void   set_menu_text(
    display_struct      *menu_window,
    menu_entry_struct   *menu_entry,
    VIO_STR              full_text )
{
    menu_window_struct  *menu;
    int                 line;
    int                 full_text_len;
    int                 full_text_pos;
    int                 part_text_len;
    int                 part_text_width;
    char                part_text_buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];

    menu = &menu_window->menu;

    full_text_len = string_length( full_text );
    full_text_pos = 0;

    for_less( line, 0, menu->n_lines_in_entry )
    {
        part_text_len = 0;
        part_text_buffer[0] = 0;

        while( full_text_pos < full_text_len )
        {
            char ch = full_text[full_text_pos++];
            if( ch == '\n')
                break;

            part_text_buffer[part_text_len++] = ch;
            part_text_buffer[part_text_len] = 0;

            part_text_width = ((line == 0) ? menu->character_offset : 0) + 
              G_get_text_length(part_text_buffer,
                                Menu_window_font,
                                menu->font_size);

            if (part_text_width >= menu_entry->key_text_width)
            {
                char *space_ptr = strrchr(part_text_buffer, ' ');
                if (space_ptr == NULL || line != 0)
                {
                  /* No space in the string so far. */
                  full_text_pos--;
                  part_text_buffer[--part_text_len] = 0;
                }
                else
                {
                  /* Try to break on the space. */
                  *space_ptr = 0;
                  full_text_pos = (space_ptr - part_text_buffer) + 1;
                }
                break;
            }

        }

        replace_string(&get_text_ptr(menu_entry->text_list[line])->string,
                       create_string(part_text_buffer));
    }

    set_update_required( menu_window, NORMAL_PLANES );
}

/**
 * This function will update the menu text for each of the 
 * menu items.
 *
 * \param display Any of the top-level display_struct objects.
 */
void  update_all_menu_text(
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
            update_menu_text( menu_window, menu_entry );
    }

    set_update_required( menu_window, NORMAL_PLANES );
    set_update_required( marker_window, NORMAL_PLANES );
}

static  void  update_menu_name_text(
    display_struct   *menu_window )
{
    VIO_STR       new_value;
    text_struct  *text;

    text = get_text_ptr( menu_window->menu.menu_name_text );

    fill_Point( text->origin, menu_window->menu.x_menu_name,
                menu_window->menu.y_menu_name, 0.0 );
    text->size = menu_window->menu.font_size;

    new_value = menu_window->menu.stack[menu_window->menu.depth]->label;

    if( !equal_strings( text->string, new_value ) )
        replace_string( &text->string, create_string(new_value) );
}

static int truncate_to_at_most(char *str_ptr, int max_length)
{
  int length = strlen(str_ptr);
  if (length < max_length)
  {
    return length;
  }
  else
  {
    while (--max_length >= 0)
    {
      if (str_ptr[max_length] == ' ') {
        break;
      }
    }
  }
  return max_length;
}

#define MAX_LINE_LENGTH 40      /* This is determined by trial and error. */

static VIO_BOOL update_menu_help_text(
    display_struct *menu_window, VIO_STR new_value)
{
    text_struct *text;
    char buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    int i;
    VIO_BOOL changed = FALSE;
    int remaining_length = string_length(new_value);
    char *buf_ptr = &buffer[0];
    menu_window_struct  *menu = &menu_window->menu;

    strcpy(buffer, new_value);

    for (i = 0; i < N_HELP_LINES; i++) 
    {
      int k;

      if (remaining_length > MAX_LINE_LENGTH)
      {
        k = truncate_to_at_most(buf_ptr, MAX_LINE_LENGTH);
        buf_ptr[k++] = '\0';
      }
      else
      {
        k = remaining_length;
      } 

      text = get_text_ptr( menu->menu_help_text[i] );

      fill_Point( text->origin, 
                  menu->help_x_origin, /* X */
                  menu->help_y_origin - i * menu->font_size, /* Y */
                  0.0 );

      text->size = menu->font_size;

      if( !equal_strings( text->string, buf_ptr ) )
      {
        replace_string( &text->string, create_string(buf_ptr) );
        changed = TRUE;
      }

      buf_ptr += k;
      remaining_length -= k;
    }
    return changed;
}
