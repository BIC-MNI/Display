
#include  <def_graphics.h>
#include  <def_string.h>
#include  <def_globals.h>

#ifdef  NOT_NEEDED
private  DEF_MENU_FUNCTION( null_function )   /* ARGSUSED */
{
    return( OK );
}
#endif

private  void  set_menu_key_entry( menu, ch, menu_entry )
    menu_window_struct     *menu;
    int                    ch;
    menu_entry_struct      *menu_entry;
{
    if( ch < 0 )
    {
        ch += 256;
    }

    menu->key_menus[ch] = menu_entry;
}

private  menu_entry_struct  *get_menu_key_entry( menu, ch )
    menu_window_struct     *menu;
    int                    ch;
{
    if( ch < 0 )
    {
        ch += 256;
    }

    return( menu->key_menus[ch] );
}

private  void  turn_on_menu_entry( menu, menu_entry )
    menu_window_struct     *menu;
    menu_entry_struct      *menu_entry;
{
    int                 i;
    menu_entry_struct   *previous;
    void                turn_off_menu_entry();

    previous = get_menu_key_entry( menu, (int) menu_entry->key );
    if( previous != (menu_entry_struct *) 0 )
    {
        turn_off_menu_entry( menu, previous );
    }

    for_less( i, 0, menu->n_lines_in_entry )
    {
        menu_entry->text_list[i]->visibility = TRUE;
    }

    menu_entry->current_depth = menu->depth;

    set_menu_key_entry( menu, (int) menu_entry->key, menu_entry );
}

private  void  turn_off_menu_entry( menu, menu_entry )
    menu_window_struct  *menu;
    menu_entry_struct   *menu_entry;
{
    int                 i;

    for_less( i, 0, menu->n_lines_in_entry )
    {
        menu_entry->text_list[i]->visibility = FALSE;
    }

    set_menu_key_entry( menu, (int) menu_entry->key,
                        (menu_entry_struct *) 0 );
}

private  void  add_menu_actions( menu, menu_entry )
    menu_window_struct  *menu;
    menu_entry_struct   *menu_entry;
{
    int   i;

    for_less( i, 0, menu_entry->n_children )
    {
        turn_on_menu_entry( menu, menu_entry->children[i] );
    }
}

private  void  remove_menu_actions( menu, menu_entry )
    menu_window_struct  *menu;
    menu_entry_struct   *menu_entry;
{
    int   i;

    for_less( i, 0, menu_entry->n_children )
    {
        if( !menu_entry->children[i]->permanent_flag )
        {
            turn_off_menu_entry( menu, menu_entry->children[i] );
        }
    }
}

public  Status  initialize_menu( graphics, runtime_directory )
    graphics_struct   *graphics;
    char              runtime_directory[];
{
    Status               status;
    menu_window_struct   *menu;
    Status               read_menu();
    Status               build_menu();
    int                  ch;
    void                 set_update_required();
    void                 get_absolute_filename();
    String               filename;
    char                 *menu_filename = "menu.dat";
    char                 *getenv();
    Status               open_input_file();
    Status               close_file();
    FILE                 *file;

    menu = &graphics->menu;

    for_less( ch, 0, N_CHARACTERS )
    {
        set_menu_key_entry( menu, ch, (menu_entry_struct *) 0 );
    }

    menu->x_dx = X_menu_dx;
    menu->x_dy = X_menu_dy;
    menu->y_dx = Y_menu_dx;
    menu->y_dy = Y_menu_dy;

    menu->n_chars_across_entry = Menu_n_chars_per_entry;
    menu->n_lines_in_entry = Menu_n_lines_per_entry;
    menu->character_width = Menu_character_width;
    menu->character_height = Menu_character_height;

    if( getenv( "MENU_FILE" ) != (char *) 0 )
    {
        (void) strcpy( menu_filename, getenv( "MENU_FILE" ) );
    }

    if( file_exists( menu_filename ) )
        status = open_input_file( menu_filename, &file );
    else
    {
        get_absolute_filename( menu_filename, runtime_directory, filename );

        status = open_input_file( filename, &file );
    }

    if( status == OK )
    {
        status = read_menu( menu, file );

        menu->depth = 0;
        menu->stack[0] = &menu->entries[0];
        menu->entries[0].current_depth = 0;
    }

    if( status == OK )
    {
        status = build_menu( graphics );

        add_menu_actions( menu, &menu->entries[0] );

        set_update_required( graphics, NORMAL_PLANES );
    }

    if( status == OK )
    {
        status = close_file( file );
    }

    return( status );
}

public  void  initialize_menu_actions( graphics )
    graphics_struct   *graphics;
{
    DECL_EVENT_FUNCTION( handle_character );
    void                 add_action_table_function();

    add_action_table_function( &graphics->action_table, KEYBOARD_EVENT,
                               handle_character );
}

private  DEF_EVENT_FUNCTION( handle_character )
{
    Status             status;
    char               key_pressed;
    menu_entry_struct  *menu_entry;
    Status             process_menu();
    graphics_struct    *menu_window;

    status = OK;

    menu_window = graphics->associated[MENU_WINDOW];

    key_pressed = event->event_data.key_pressed;

    menu_entry = get_menu_key_entry( &menu_window->menu, (int) key_pressed );

    if( menu_entry != (menu_entry_struct *) 0 )
    {
        status = process_menu( menu_window, menu_entry );
    }

    return( status );
}

private  Status  process_menu( graphics, menu_entry )
    graphics_struct     *graphics;
    menu_entry_struct   *menu_entry;
{
    Status                  status;
    menu_function_pointer   function;
    Status                  update_menu_text();

    function = menu_entry->action;

    status = (*function)( graphics->associated[THREE_D_WINDOW],
                          graphics->associated[MENU_WINDOW],
                          menu_entry );

    if( status == OK )
    {
        status = update_menu_text( graphics, menu_entry );
    }

    return( status );
}

public  Status  update_menu_text( graphics, menu_entry )
    graphics_struct     *graphics;
    menu_entry_struct   *menu_entry;
{
    Status                  status;
    menu_update_pointer     update_function;

    update_function = menu_entry->update_action;

    status = (*update_function)( graphics->associated[THREE_D_WINDOW],
                                 graphics->associated[MENU_WINDOW],
                                 menu_entry,
                                 menu_entry->label );

    return( status );
}

void  set_menu_text( menu_window, menu_entry, text )
    graphics_struct     *menu_window;
    menu_entry_struct   *menu_entry;
    char                text[];
{
    int                 line, i, n_chars, len;
    char                *text_ptr;
    menu_window_struct  *menu;
    void                set_update_required();

    menu = &menu_window->menu;

    len = strlen( text );
    n_chars = 0;

    for_less( line, 0, menu->n_lines_in_entry )
    {
        i = 0;
        text_ptr = menu_entry->text_list[line]->ptr.text->text;
        while( n_chars < len && i < menu->n_chars_across_entry )
        {
            if( text[n_chars] == '\n' ||
                (text[n_chars] == ' ' &&
                 (len - n_chars-1) <=
                 (menu->n_lines_in_entry-line-1) * menu->n_chars_across_entry) )
            {
                ++n_chars;
                break;
            }

            text_ptr[i] = text[n_chars];
            ++i;
            ++n_chars;
        }
        text_ptr[i] = (char) 0;
    }

    set_update_required( menu_window, NORMAL_PLANES );
}

public  DEF_MENU_FUNCTION( push_menu )      /* ARGSUSED */
{
    Status   status;
    void     set_update_required();

    status = OK;

    while( menu_window->menu.depth > menu_entry->current_depth )
    {
        remove_menu_actions( &menu_window->menu,
                             menu_window->menu.stack[menu_window->menu.depth] );

        --menu_window->menu.depth;
    }

    if( menu_window->menu.depth >= MAX_MENU_DEPTH )
    {
        PRINT_ERROR( "Max menu depth\n" );
        status = ERROR;
    }
    else
    {
        ++menu_window->menu.depth;
        menu_window->menu.stack[menu_window->menu.depth] = menu_entry;

        add_menu_actions( &menu_window->menu, menu_entry );

        set_update_required( menu_window, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(push_menu )      /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( pop_menu )      /* ARGSUSED */
{
    void    set_update_required();

    if( menu_window->menu.depth > 0 )
    {
        remove_menu_actions( &menu_window->menu,
                             menu_window->menu.stack[menu_window->menu.depth] );

        --menu_window->menu.depth;

        add_menu_actions( &menu_window->menu,
                          menu_window->menu.stack[menu_window->menu.depth] );

        set_update_required( menu_window, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(pop_menu )      /* ARGSUSED */
{
    return( OK );
}
