
#include  <def_graphics.h>
#include  <string.h>

private  DEF_MENU_FUNCTION( null_function )   /* ARGSUSED */
{
    return( OK );
}

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
    menu_entry_struct   *previous;
    void                turn_off_menu_entry();

    previous = get_menu_key_entry( menu, (int) menu_entry->key );
    if( previous != (menu_entry_struct *) 0 )
    {
        turn_off_menu_entry( menu, previous );
    }

    menu_entry->text->visibility = TRUE;

    set_menu_key_entry( menu, (int) menu_entry->key, menu_entry );
}

private  void  turn_off_menu_entry( menu, menu_entry )
    menu_window_struct  *menu;
    menu_entry_struct   *menu_entry;
{
    menu_entry->text->visibility = FALSE;

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

public  Status  initialize_menu( graphics )
    graphics_struct   *graphics;
{
    Status               status;
    menu_window_struct   *menu;
    Status               read_menu();
    Status               build_menu();
    int                  ch;
    DECL_EVENT_FUNCTION( handle_character );
    void                 add_action_table_function();

    menu = &graphics->menu_window->menu;

    add_action_table_function( &graphics->menu_window->action_table,
                               KEYBOARD_EVENT,
                               handle_character );
    add_action_table_function( &graphics->graphics_window->action_table,
                               KEYBOARD_EVENT, handle_character );

    for_less( ch, 0, N_CHARACTERS )
    {
        set_menu_key_entry( menu, ch, (menu_entry_struct *) 0 );
    }

    status = read_menu( menu, "menu.dat" );

    menu->depth = 0;
    menu->stack[0] = &menu->entries[0];

    if( status == OK )
    {
        status = build_menu( graphics->menu_window );

        add_menu_actions( menu, &menu->entries[0] );

        graphics->menu_window->update_required = TRUE;
    }

    return( status );
}

private  DEF_EVENT_FUNCTION( handle_character )
{
    Status             status;
    char               key_pressed;
    menu_entry_struct  *menu_entry;
    Status             process_menu();

    status = OK;

    graphics = graphics->menu_window;

    key_pressed = event->event_data.key_pressed;

    menu_entry = get_menu_key_entry( &graphics->menu_window->menu,
                                     (int) key_pressed );

    if( menu_entry != (menu_entry_struct *) 0 )
    {
        status = process_menu( graphics, menu_entry );
    }

    return( status );
}

private  Status  process_menu( graphics, menu_entry )
    graphics_struct     *graphics;
    menu_entry_struct   *menu_entry;
{
    Status                  status;
    menu_function_pointer   function;

    function = menu_entry->action;

    status = (*function)( graphics->graphics_window, graphics->menu_window,
                          menu_entry );

    return( status );
}

public  DEF_MENU_FUNCTION( push_menu )
{
    Status   status;

    status = OK;

    if( menu_window->menu.depth >= MAX_MENU_DEPTH )
    {
        status = ERROR;
    }
    else
    {
        remove_menu_actions( &menu_window->menu,
                             menu_window->menu.stack[menu_window->menu.depth] );

        ++menu_window->menu.depth;
        menu_window->menu.stack[menu_window->menu.depth] = menu_entry;

        add_menu_actions( &menu_window->menu, menu_entry );

        menu_window->update_required = TRUE;
    }

    return( status );
}

public  DEF_MENU_FUNCTION( pop_menu )
{
    if( menu_window->menu.depth > 0 )
    {
        remove_menu_actions( &menu_window->menu,
                             menu_window->menu.stack[menu_window->menu.depth] );

        --menu_window->menu.depth;

        add_menu_actions( &menu_window->menu,
                          menu_window->menu.stack[menu_window->menu.depth] );

        menu_window->update_required = TRUE;
    }

    return( OK );
}
