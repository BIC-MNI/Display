
#include  <def_graphics.h>
#include  <def_string.h>
#include  <def_files.h>

public  void  set_text_on_off( format, text, flag )
    char     format[];
    char     text[];
    Boolean  flag;
{
    void  set_text_boolean();

    set_text_boolean( format, text, flag, "Off", "On" );
}

public  void  set_text_boolean( format, text, flag, off_str, on_str )
    char     format[];
    char     text[];
    Boolean  flag;
    char     off_str[];
    char     on_str[];
{
    if( flag )
        (void) sprintf( text, format, on_str );
    else
        (void) sprintf( text, format, off_str );
}

public  void  set_menu_text_with_colour( menu_window, menu_entry, format,
                                         colour )
    graphics_struct    *menu_window;
    menu_entry_struct  *menu_entry;
    char               format[];
    Colour             *colour;
{
    String           text, name;
    void             set_menu_text();
    void             convert_colour_to_string();

    convert_colour_to_string( colour, name );

    (void) sprintf( text, format, name );

    set_menu_text( menu_window, menu_entry, text );
}
