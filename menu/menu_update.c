
#include  <stdio.h>
#include  <def_graphics.h>

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
    { 
        (void) sprintf( text, format, on_str );
    }
    else
    {
        (void) sprintf( text, format, off_str );
    }
}
