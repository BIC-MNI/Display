
#include  <def_graphics.h>
#include  <string.h>

private  DEF_MENU_FUNCTION( null_function )   /* ARGSUSED */
{
    return( OK );
}

private  void  set_menu_key_entry( graphics, ch, menu_entry )
    graphics_struct     *graphics;
    int                 ch;
    menu_entry_struct   *menu_entry;
{
    if( ch < 0 )
    {
        ch += 256;
    }

    graphics->menu_window->menu.key_menus[ch] = menu_entry;
}

private  menu_entry_struct  *get_menu_key_entry( graphics, ch )
    graphics_struct     *graphics;
    int                 ch;
{
    if( ch < 0 )
    {
        ch += 256;
    }

    return( graphics->menu_window->menu.key_menus[ch] );
}

public  void  initialize_menu( graphics )
    graphics_struct  *graphics;
{
    int                 ch;
    DECL_EVENT_FUNCTION( handle_character );
    void                 add_action_table_function();
    menu_function_type   toggle_perspective;

    graphics->menu.n_entries = 1;

    graphics->menu.entries[0].visible = TRUE;
    graphics->menu.entries[0].active = TRUE;
    graphics->menu.entries[0].key = 'a';
    graphics->menu.entries[0].x_pos = 0;
    graphics->menu.entries[0].y_pos = 0;
    (void) strcpy( graphics->menu.entries[0].label, "test" );
    graphics->menu.entries[0].n_children = 0;
    graphics->menu.entries[0].action = toggle_perspective;

    graphics->menu.depth = 1;
    graphics->menu.stack[0] = &graphics->menu.entries[0];

    add_action_table_function( &graphics->action_table, KEYBOARD_EVENT,
                               handle_character );
    add_action_table_function( &graphics->graphics_window->action_table,
                               KEYBOARD_EVENT, handle_character );

    for_less( ch, 0, N_CHARACTERS )
    {
        set_menu_key_entry( graphics, ch, (menu_entry_struct *) 0 );
    }

    set_menu_key_entry( graphics, 'a', &graphics->menu.entries[0] );
}

private  DEF_EVENT_FUNCTION( handle_character )
{
    Status             status;
    char               key_pressed;
    int                i;
    menu_entry_struct  *menu_entry;
    Status             process_menu();

    status = OK;

    graphics = graphics->menu_window;

    key_pressed = event->event_data.key_pressed;

    menu_entry = get_menu_key_entry( graphics, key_pressed );

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

private  DEF_MENU_FUNCTION( toggle_perspective )
{
    void  update_view();

    graphics->view.perspective_flag = !graphics->view.perspective_flag;

    update_view( graphics );

    graphics->update_required = TRUE;

    return( OK );
}
