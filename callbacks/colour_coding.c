
#include  <def_display.h>

public  DEF_MENU_FUNCTION(set_colour_limits )   /* ARGSUSED */
{
    Volume           volume;
    Real             min_value, max_value;
    display_struct   *slice_window;

    if( get_slice_window_volume(display,&volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        print( "Current limits:\t%g\t%g\n",
               slice_window->slice.colour_coding.min_value,
               slice_window->slice.colour_coding.max_value );

        print( "Enter new values: " );

        if( input_real( stdin, &min_value ) == OK &&
            input_real( stdin, &max_value ) == OK &&
            min_value <= max_value )
        {
            change_colour_coding_range( slice_window, min_value, max_value );

            print( "    New limits:\t%g\t%g\n",
                   slice_window->slice.colour_coding.min_value,
                   slice_window->slice.colour_coding.max_value );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_colour_limits )   /* ARGSUSED */
{
    return( OK );
}

private  void  set_the_colour_coding_type(
    display_struct       *display,
    Colour_coding_types  type )
{
    display_struct          *slice_window;
    Volume                  volume;
    colour_coding_struct    *colour_coding;

    if( get_slice_window_volume( display, &volume ) )
    {
        slice_window = display->associated[SLICE_WINDOW];
        colour_coding = &slice_window->slice.colour_coding;

        colour_coding->type = type;

        colour_coding_has_changed( slice_window );
    }
}

public  DEF_MENU_FUNCTION(set_contour_colour_map )   /* ARGSUSED */
{
    set_the_colour_coding_type( display, CONTOUR_COLOUR_MAP );

    return( OK );
}

public  DEF_MENU_UPDATE(set_contour_colour_map )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_hot_metal )   /* ARGSUSED */
{
    set_the_colour_coding_type( display, HOT_METAL );

    return( OK );
}

public  DEF_MENU_UPDATE(set_hot_metal )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_gray_scale )   /* ARGSUSED */
{
    set_the_colour_coding_type( display, GRAY_SCALE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_gray_scale )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_spectral )   /* ARGSUSED */
{
    set_the_colour_coding_type( display, SPECTRAL );

    return( OK );
}

public  DEF_MENU_UPDATE(set_spectral )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_under_colour )   /* ARGSUSED */
{
    Status                  status;
    display_struct          *slice_window;
    Volume                  volume;
    String                  line;
    Colour                  col;

    status = OK;

    if( get_slice_window_volume( display, &volume ) )
    {
        print( "Enter under colour name or r g b:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
        {
            col = convert_string_to_colour( line );

            slice_window = display->associated[SLICE_WINDOW];

            set_colour_coding_under_colour( &slice_window->slice.colour_coding,
                                            col );

            colour_coding_has_changed( slice_window );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_under_colour )   /* ARGSUSED */
{
    display_struct   *slice_window;
    Colour           col;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window == (display_struct  *) 0 )
        col = WHITE;
    else
        col = get_colour_coding_under_colour(
                        &slice_window->slice.colour_coding );

    set_menu_text_with_colour( menu_window, menu_entry, label, col );

    return( OK );
}

public  DEF_MENU_FUNCTION(set_over_colour )   /* ARGSUSED */
{
    Status                  status;
    display_struct          *slice_window;
    Volume                  volume;
    String                  line;
    Colour                  col;

    status = OK;

    if( get_slice_window_volume( display, &volume ) )
    {
        print( "Enter over colour name or r g b:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
        {
            col = convert_string_to_colour( line );

            slice_window = display->associated[SLICE_WINDOW];

            set_colour_coding_over_colour( &slice_window->slice.colour_coding,
                                           col );

            colour_coding_has_changed( slice_window );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_over_colour )   /* ARGSUSED */
{
    display_struct   *slice_window;
    Colour           col;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window == (display_struct  *) 0 )
        col = WHITE;
    else
        col = get_colour_coding_over_colour(&slice_window->slice.colour_coding);

    set_menu_text_with_colour( menu_window, menu_entry, label, col );

    return( OK );
}

public  DEF_MENU_FUNCTION(set_label_colour_ratio )   /* ARGSUSED */
{
    Volume           volume;
    Real             ratio;
    display_struct   *slice_window;

    if( get_slice_window_volume(display,&volume) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        print( "Enter new label colour ratio: " );

        if( input_real( stdin, &ratio ) == OK &&
            ratio >= 0.0 && ratio <= 1.0 )
        {
            slice_window->slice.label_colour_ratio = ratio;
    
            colour_coding_has_changed( slice_window );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_label_colour_ratio )   /* ARGSUSED */
{
    String           text;
    Real             ratio;
    display_struct   *slice_window;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window == (display_struct  *) 0 )
        ratio = 0.0;
    else
        ratio = display->associated[SLICE_WINDOW]->slice.label_colour_ratio;

    (void) sprintf( text, label, ratio );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}
