
#include  <def_graphics.h>
#include  <def_string.h>

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
    menu_entry_struct   *previous;
    void                turn_off_menu_entry();

    previous = get_menu_key_entry( menu, (int) menu_entry->key );
    if( previous != (menu_entry_struct *) 0 )
    {
        turn_off_menu_entry( menu, previous );
    }

    menu_entry->text->visibility = TRUE;
    menu_entry->current_depth = menu->depth;

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
    void                 set_update_required();

    menu = &graphics->menu;

    for_less( ch, 0, N_CHARACTERS )
    {
        set_menu_key_entry( menu, ch, (menu_entry_struct *) 0 );
    }

    status = read_menu( menu, "menu.dat" );

    menu->depth = 0;
    menu->stack[0] = &menu->entries[0];
    menu->entries[0].current_depth = 0;

    if( status == OK )
    {
        status = build_menu( graphics );

        add_menu_actions( menu, &menu->entries[0] );

        set_update_required( graphics, NORMAL_PLANES );
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
                                 menu_entry->label,
                                 menu_entry->text->ptr.text->text );

    return( status );
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
