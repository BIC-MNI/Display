
#include  <display.h>

public  void  set_menu_text_real(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    Real               value )
{
    STRING           text;

    (void) sprintf( text, menu_entry->label, value );

    set_menu_text( menu_window, menu_entry, text );
}

public  void  set_menu_text_int(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    int                value )
{
    STRING           text;

    (void) sprintf( text, menu_entry->label, value );

    set_menu_text( menu_window, menu_entry, text );
}

public  void  set_menu_text_boolean(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    BOOLEAN            value,
    char               off_str[],
    char               on_str[] )
{
    STRING           text;

    if( value )
        (void) sprintf( text, menu_entry->label, on_str );
    else
        (void) sprintf( text, menu_entry->label, off_str );

    set_menu_text( menu_window, menu_entry, text );
}

public  void  set_menu_text_on_off(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    BOOLEAN            value )
{
    set_menu_text_boolean( menu_window, menu_entry, value, "Off", "On" );
}

public  void  set_menu_text_with_colour(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    Colour             colour )
{
    STRING           text, name;

    convert_colour_to_string( colour, name );

    (void) sprintf( text, menu_entry->label, name );

    set_menu_text( menu_window, menu_entry, text );
}

public  void  set_menu_text_string(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    char               str[] )
{
    STRING           text;

    (void) sprintf( text, menu_entry->label, str );

    set_menu_text( menu_window, menu_entry, text );
}
