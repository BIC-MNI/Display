
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

public  Status  initialize_menu(
    display_struct    *menu_window,
    char              default_directory1[],
    char              default_directory2[],
    char              default_directory3[],
    char              default_directory4[],
    char              menu_filename[] )
{
    Status               status;
    STRING               filename;
    menu_window_struct   *menu;
    Point                position;
    model_struct         *model;
    int                  ch;
    FILE                 *file;
    BOOLEAN              found;

    G_set_transparency_state( menu_window->window, OFF );

    initialize_resize_events( menu_window );

    menu = &menu_window->menu;

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

    found = FALSE;

    if( file_exists( menu_filename ) )
    {
        (void) strcpy( filename, menu_filename );
        found = TRUE;
    }

    if( !found )
    {
        get_absolute_filename( menu_filename, default_directory1, filename );

        found = file_exists( filename );
    }

    if( !found )
    {
        get_absolute_filename( menu_filename, default_directory2, filename);

        found = file_exists( filename );
    }

    if( !found )
    {
        get_absolute_filename( menu_filename, default_directory3, filename);

        found = file_exists( filename );
    }

    if( !found )
    {
        get_absolute_filename( menu_filename, default_directory4, filename);

        found = file_exists( filename );
    }

    if( !found )
        (void) strcpy( menu_filename, filename );

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
        build_menu( menu_window );

        add_menu_actions( menu, &menu->entries[0] );

        set_update_required( menu_window, NORMAL_PLANES );

        status = close_file( file );
    }

    model = get_graphics_model( menu_window, UTILITY_MODEL );
    menu->menu_name_text = create_object( TEXT );
    fill_Point( position, Menu_name_x, Menu_name_y, 0.0 );
    initialize_text( get_text_ptr(menu->menu_name_text),
                     &position,
                     Menu_name_colour, Menu_name_font, Menu_name_font_size );

    add_object_to_model( model, menu->menu_name_text );

    update_menu_name_text( menu_window );

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

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_character_down )
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

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_leaving_window )
{
    display_struct     *menu_window;

    menu_window = display->associated[MENU_WINDOW];
    menu_window->menu.shift_key_down = FALSE;

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_character_up )
{
    display_struct     *menu_window;

    if( is_shift_key( key_pressed ) )
    {
        menu_window = display->associated[MENU_WINDOW];
        menu_window->menu.shift_key_down = FALSE;
    }

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
    else if( mouse_is_on_object_name( three_d, x, y, &object ) )
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
    int                 key;
    display_struct      *menu_window;
    menu_entry_struct   *menu_entry;

    menu_window = display->associated[MENU_WINDOW];

    for_less( key, 0, N_CHARACTERS )
    {
        menu_entry = get_menu_key_entry( &menu_window->menu, key );

        if( menu_entry != (menu_entry_struct *) 0 )
            update_menu_text( display, menu_entry );
    }

    set_update_required( menu_window, NORMAL_PLANES );
}

private  void  update_menu_name_text(
    display_struct   *menu_window )
{
    text_struct  *text;

    text = get_text_ptr( menu_window->menu.menu_name_text );

    (void) strcpy( text->string,
                   menu_window->menu.stack[menu_window->menu.depth]->label );
}
