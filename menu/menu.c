
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

public  Status  initialize_menu(
    display_struct    *display,
    char              default_directory1[],
    char              default_directory2[],
    char              menu_filename[] )
{
    Status               status;
    STRING               filename;
    menu_window_struct   *menu;
    int                  ch;
    FILE                 *file;

    initialize_resize_events( display );

    menu = &display->menu;

    menu->shift_key_down = FALSE;

    for_less( ch, 0, N_CHARACTERS )
        set_menu_key_entry( menu, ch, (menu_entry_struct *) 0 );

    menu->x_dx = X_menu_dx;
    menu->x_dy = X_menu_dy;
    menu->y_dx = Y_menu_dx;
    menu->y_dy = Y_menu_dy;

    menu->n_chars_per_unit_across = Menu_n_chars_per_entry;
    menu->n_lines_in_entry = Menu_n_lines_per_entry;
    menu->character_width = Menu_character_width;
    menu->character_height = Menu_character_height;

    if( file_exists( menu_filename ) )
    {
        (void) strcpy( filename, menu_filename );
    }
    else
    {
        get_absolute_filename( menu_filename, default_directory1, filename );

        if( !file_exists( filename ) )
        {
            get_absolute_filename( menu_filename, default_directory2, filename);

            if( !file_exists( filename ) )
                (void) strcpy( filename, menu_filename );
        }
    }

    status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        status = read_menu( menu, file );

        menu->depth = 0;
        menu->stack[0] = &menu->entries[0];
        menu->entries[0].current_depth = 0;
    }

    if( status == OK )
    {
        build_menu( display );

        add_menu_actions( menu, &menu->entries[0] );

        set_update_required( display, NORMAL_PLANES );

        status = close_file( file );
    }

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

private  BOOLEAN   is_shift_key(
    int   key )
{
    return( key == LEFT_SHIFT_KEY ||
            key == RIGHT_SHIFT_KEY ||
            key == LEFT_CTRL_KEY ||
            key == RIGHT_CTRL_KEY ||
            key == LEFT_ALT_KEY ||
            key == RIGHT_ALT_KEY );
}

public  BOOLEAN  is_shift_key_pressed(
    display_struct     *display )
{
    display_struct     *menu_window;

    menu_window = display->associated[MENU_WINDOW];

    return( menu_window->menu.shift_key_down );
}

private  DEF_EVENT_FUNCTION( handle_character_down )   /* ARGSUSED */
{
    Status             status;
    display_struct     *menu_window;

    status = OK;

    menu_window = display->associated[MENU_WINDOW];

    if( is_shift_key( key_pressed ) )
        menu_window->menu.shift_key_down = TRUE;
    else
        status = handle_menu_for_key( menu_window, key_pressed );

    return( status );
}

private  DEF_EVENT_FUNCTION( handle_leaving_window )   /* ARGSUSED */
{
    display_struct     *menu_window;

    menu_window = display->associated[MENU_WINDOW];
    menu_window->menu.shift_key_down = FALSE;

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_character_up )   /* ARGSUSED */
{
    display_struct     *menu_window;

    if( is_shift_key( key_pressed ) )
    {
        menu_window = display->associated[MENU_WINDOW];
        menu_window->menu.shift_key_down = FALSE;
    }

    return( OK );
}

private  Status  handle_menu_for_key(
    display_struct      *menu_window,
    int                 key )
{
    Status             status;
    menu_entry_struct  *menu_entry;

    status = OK;

    menu_entry = get_menu_key_entry( &menu_window->menu, key );

    if( menu_entry != (menu_entry_struct *) 0 )
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

private  DEF_EVENT_FUNCTION( left_mouse_press )    /* ARGSUSED */
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

private  DEF_EVENT_FUNCTION( middle_mouse_press )    /* ARGSUSED */
{
    pop_menu_one_level( display );

    return( OK );
}

private  Status  handle_mouse_press_in_menu(
    display_struct      *menu_window,
    Real                x,
    Real                y )
{
    Status              status;
    int                 key;
    object_struct       *object;

    status = OK;

    if( lookup_key_for_mouse_position( menu_window, x, y, &key ) )
    {
        status = handle_menu_for_key( menu_window, key );
    }
    else if( mouse_is_on_object_name( menu_window->associated[THREE_D_WINDOW],
                                      x, y, &object ) )
    {
        set_current_object( menu_window->associated[THREE_D_WINDOW],
                            object );
        rebuild_selected_list( menu_window->associated[THREE_D_WINDOW],
                               menu_window );
    }


    return( status );
}

public  Status  update_menu_text(
    display_struct      *display,
    menu_entry_struct   *menu_entry )
{
    Status                  status;
    menu_update_pointer     update_function;

    update_function = menu_entry->update_action;

    status = (*update_function)( display->associated[THREE_D_WINDOW],
                                 display->associated[MENU_WINDOW],
                                 menu_entry,
                                 menu_entry->label );

    return( status );
}

public  DEF_MENU_FUNCTION( push_menu )      /* ARGSUSED */
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
    pop_menu_one_level( menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(pop_menu )      /* ARGSUSED */
{
    return( OK );
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

        set_update_required( menu_window, NORMAL_PLANES );
    }
}

public  void   set_menu_text(
    display_struct      *menu_window,
    menu_entry_struct   *menu_entry,
    char                text[] )
{
    int                 line, i, n_chars, len, n_chars_across;
    char                *text_ptr;
    menu_window_struct  *menu;

    menu = &menu_window->menu;

    len = strlen( text );
    n_chars = 0;

    for_less( line, 0, menu->n_lines_in_entry )
    {
        i = 0;
        text_ptr = get_text_ptr(menu_entry->text_list[line])->string;

        n_chars_across = menu_entry->n_chars_across;

        if( line == 0 )
            n_chars_across = ROUND( n_chars_across - Menu_key_character_offset);

        while( n_chars < len && i < n_chars_across )
        {
            if( text[n_chars] == '\n' ||
                (text[n_chars] == ' ' &&
                 (len - n_chars-1) <=
                 (menu->n_lines_in_entry-line-1) * menu_entry->n_chars_across
                 - Menu_key_character_offset) )
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

public  void  update_all_menu_text(
    display_struct   *display )
{
    Status              status;
    int                 key;
    display_struct      *menu_window;
    menu_entry_struct   *menu_entry;

    menu_window = display->associated[MENU_WINDOW];

    for_less( key, 0, N_CHARACTERS )
    {
        menu_entry = get_menu_key_entry( &menu_window->menu, key );

        if( menu_entry != (menu_entry_struct *) 0 )
            status = update_menu_text( display, menu_entry );
    }
}
