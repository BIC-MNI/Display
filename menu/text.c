/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

  void  set_menu_text_real(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_Real               value )
{
    char  text[VIO_EXTREMELY_LARGE_STRING_SIZE];

    (void) sprintf( text, menu_entry->label, value );

    set_menu_text( menu_window, menu_entry, text );
}

  void  set_menu_text_int(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    int                value )
{
    char  text[VIO_EXTREMELY_LARGE_STRING_SIZE];

    (void) sprintf( text, menu_entry->label, value );

    set_menu_text( menu_window, menu_entry, text );
}

  void  set_menu_text_boolean(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_BOOL            value,
    VIO_STR             off_str,
    VIO_STR             on_str )
{
    char  text[VIO_EXTREMELY_LARGE_STRING_SIZE];

    if( value )
        (void) sprintf( text, menu_entry->label, on_str );
    else
        (void) sprintf( text, menu_entry->label, off_str );

    set_menu_text( menu_window, menu_entry, text );
}

  void  set_menu_text_on_off(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_BOOL            value )
{
    set_menu_text_boolean( menu_window, menu_entry, value, "Off", "On" );
}

  void  set_menu_text_with_colour(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_Colour             colour )
{
    VIO_STR           name;
    char             text[VIO_EXTREMELY_LARGE_STRING_SIZE];

    name = convert_colour_to_string( colour );

    (void) sprintf( text, menu_entry->label, name );

    delete_string( name );

    set_menu_text( menu_window, menu_entry, text );
}

  void  set_menu_text_string(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_STR             str )
{
    char  text[VIO_EXTREMELY_LARGE_STRING_SIZE];

    (void) sprintf( text, menu_entry->label, str );

    set_menu_text( menu_window, menu_entry, text );
}
