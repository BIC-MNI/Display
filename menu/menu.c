
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

private  void  turn_on_menu_entry( graphics, menu_entry )
    graphics_struct     *graphics;
    menu_entry_struct   *menu_entry;
{
    set_menu_key_entry( graphics, (int) menu_entry->key, menu_entry );
}

private  void  turn_off_menu_entry( graphics, menu_entry )
    graphics_struct     *graphics;
    menu_entry_struct   *menu_entry;
{
    set_menu_key_entry( graphics, (int) menu_entry->key,
                        (menu_entry_struct *) 0 );
}

private  void  add_menu_actions( menu_window, menu_entry )
    graphics_struct     *menu_window;
    menu_entry_struct   *menu_entry;
{
    int   i;

    for_less( i, 0, menu_entry->n_children )
    {
        turn_on_menu_entry( menu_window, menu_entry->children[i] );
    }
}

private  void  remove_menu_actions( menu_window, menu_entry )
    graphics_struct     *menu_window;
    menu_entry_struct   *menu_entry;
{
    int   i;

    for_less( i, 0, menu_entry->n_children )
    {
        turn_off_menu_entry( menu_window, menu_entry->children[i] );
    }
}

public  Status  initialize_menu( graphics )
    graphics_struct  *graphics;
{
    Status               status;
    Status               read_menu();
    int                  ch;
    DECL_EVENT_FUNCTION( handle_character );
    void                 add_action_table_function();
    menu_function_type   toggle_perspective;
    menu_function_type   push_menu;
    menu_function_type   pop_menu;
    menu_function_type   reverse_normals;

    graphics->menu.n_entries = 1;

    graphics->menu.entries[0].visible = TRUE;
    graphics->menu.entries[0].active = TRUE;
    graphics->menu.entries[0].key = '1';
    graphics->menu.entries[0].x_pos = 0;
    graphics->menu.entries[0].y_pos = 0;
    (void) strcpy( graphics->menu.entries[0].label, "test" );
    graphics->menu.entries[0].n_children = 1;
    graphics->menu.entries[0].children[0] = &graphics->menu.entries[1];
    graphics->menu.entries[0].action = push_menu;

    graphics->menu.entries[1].visible = TRUE;
    graphics->menu.entries[1].active = TRUE;
    graphics->menu.entries[1].key = 'f';
    graphics->menu.entries[1].x_pos = 0;
    graphics->menu.entries[1].y_pos = 0;
    (void) strcpy( graphics->menu.entries[1].label, "test" );
    graphics->menu.entries[1].n_children = 2;
    graphics->menu.entries[1].children[0] = &graphics->menu.entries[2];
    graphics->menu.entries[1].children[1] = &graphics->menu.entries[3];
    graphics->menu.entries[1].action = push_menu;

    graphics->menu.entries[2].visible = TRUE;
    graphics->menu.entries[2].active = TRUE;
    graphics->menu.entries[2].key = 'a';
    graphics->menu.entries[2].x_pos = 0;
    graphics->menu.entries[2].y_pos = 0;
    (void) strcpy( graphics->menu.entries[2].label, "test" );
    graphics->menu.entries[2].n_children = 0;
    graphics->menu.entries[2].action = toggle_perspective;

    graphics->menu.entries[3].visible = TRUE;
    graphics->menu.entries[3].active = TRUE;
    graphics->menu.entries[3].key = 'r';
    graphics->menu.entries[3].x_pos = 0;
    graphics->menu.entries[3].y_pos = 0;
    (void) strcpy( graphics->menu.entries[3].label, "test" );
    graphics->menu.entries[3].n_children = 0;
    graphics->menu.entries[3].action = reverse_normals;

    graphics->menu.entries[4].visible = TRUE;
    graphics->menu.entries[4].active = TRUE;
    graphics->menu.entries[4].key = ' ';
    graphics->menu.entries[4].x_pos = 0;
    graphics->menu.entries[4].y_pos = 0;
    (void) strcpy( graphics->menu.entries[4].label, "test" );
    graphics->menu.entries[4].n_children = 0;
    graphics->menu.entries[4].action = pop_menu;

    graphics->menu.depth = 0;
    graphics->menu.stack[0] = &graphics->menu.entries[0];

    add_action_table_function( &graphics->action_table, KEYBOARD_EVENT,
                               handle_character );
    add_action_table_function( &graphics->graphics_window->action_table,
                               KEYBOARD_EVENT, handle_character );

    for_less( ch, 0, N_CHARACTERS )
    {
        set_menu_key_entry( graphics, ch, (menu_entry_struct *) 0 );
    }

    add_menu_actions( graphics, &graphics->menu.entries[0] );
    turn_on_menu_entry( graphics, &graphics->menu.entries[4] );

    status = read_menu( &graphics->menu, "menu.dat" );

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

    menu_entry = get_menu_key_entry( graphics, (int) key_pressed );

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
        remove_menu_actions( menu_window,
                             menu_window->menu.stack[menu_window->menu.depth] );

        ++menu_window->menu.depth;
        menu_window->menu.stack[menu_window->menu.depth] = menu_entry;

        add_menu_actions( menu_window, menu_entry );
    }

    return( status );
}

public  DEF_MENU_FUNCTION( pop_menu )
{
    if( menu_window->menu.depth > 0 )
    {
        remove_menu_actions( menu_window,
                             menu_window->menu.stack[menu_window->menu.depth] );

        --menu_window->menu.depth;

        add_menu_actions( menu_window,
                          menu_window->menu.stack[menu_window->menu.depth] );
    }

    return( OK );
}

private  DEF_MENU_FUNCTION( reverse_normals )
{
    void  reverse_object_normals();

    reverse_object_normals( graphics->objects );

    graphics->update_required = TRUE;

    return( OK );
}

