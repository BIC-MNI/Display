
#include  <def_display.h>

public  void  set_text_on_off(
    char     format[],
    char     text[],
    Boolean  flag )
{
    set_text_boolean( format, text, flag, "Off", "On" );
}

public  void  set_text_boolean(
    char     format[],
    char     text[],
    Boolean  flag,
    char     off_str[],
    char     on_str[] )
{
    if( flag )
        (void) sprintf( text, format, on_str );
    else
        (void) sprintf( text, format, off_str );
}

public  void  set_menu_text_with_colour(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    char               format[],
    Colour             colour )
{
    String           text, name;

    convert_colour_to_string( colour, name );

    (void) sprintf( text, format, name );

    set_menu_text( menu_window, menu_entry, text );
}
