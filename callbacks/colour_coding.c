
#include  <def_graphics.h>
#include  <def_math.h>
#include  <def_files.h>
#include  <def_colours.h>

public  DEF_MENU_FUNCTION(set_colour_limits )   /* ARGSUSED */
{
    volume_struct    *volume;
    Real             min_value, max_value;
    graphics_struct  *slice_window;
    void             change_colour_coding_range();

    if( get_current_volume(graphics,&volume) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        PRINT( "Current limits:\t%g\t%g\n",
               slice_window->slice.colour_coding.min_value,
               slice_window->slice.colour_coding.max_value );

        PRINT( "Enter new values: " );

        if( input_real( stdin, &min_value ) == OK &&
            input_real( stdin, &max_value ) == OK &&
            input_newline( stdin ) == OK &&
            min_value <= max_value )
        {
            change_colour_coding_range( slice_window, min_value, max_value );

            PRINT( "    New limits:\t%g\t%g\n",
                   slice_window->slice.colour_coding.min_value,
                   slice_window->slice.colour_coding.max_value );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_colour_limits )   /* ARGSUSED */
{
    return( OK );
}

private  void  set_colour_coding_type( graphics, type )
    graphics_struct      *graphics;
    Colour_coding_types  type;
{
    graphics_struct         *slice_window;
    volume_struct           *volume;
    colour_coding_struct    *colour_coding;
    void                    colour_coding_has_changed();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];
        colour_coding = &slice_window->slice.colour_coding;

        colour_coding->type = type;

        colour_coding_has_changed( slice_window );
    }
}

public  DEF_MENU_FUNCTION(set_contour_colour_map )   /* ARGSUSED */
{
    set_colour_coding_type( graphics, CONTOUR_COLOUR_MAP );

    return( OK );
}

public  DEF_MENU_UPDATE(set_contour_colour_map )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_hot_metal )   /* ARGSUSED */
{
    set_colour_coding_type( graphics, HOT_METAL );

    return( OK );
}

public  DEF_MENU_UPDATE(set_hot_metal )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_gray_scale )   /* ARGSUSED */
{
    set_colour_coding_type( graphics, GRAY_SCALE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_gray_scale )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_spectral )   /* ARGSUSED */
{
    set_colour_coding_type( graphics, SPECTRAL );

    return( OK );
}

public  DEF_MENU_UPDATE(set_spectral )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_user_defined_colour_coding )   /* ARGSUSED */
{
    set_colour_coding_type( graphics, USER_DEFINED );

    return( OK );
}

public  DEF_MENU_UPDATE(set_user_defined_colour_coding )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_under_colour )   /* ARGSUSED */
{
    Status                  status;
    Status                  convert_string_to_colour();
    graphics_struct         *slice_window;
    volume_struct           *volume;
    String                  line;
    Colour                  col;
    void                    colour_coding_has_changed();

    status = OK;

    if( get_slice_window_volume( graphics, &volume ) )
    {
        PRINT( "Enter under colour name or r g b:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
            status = convert_string_to_colour( line, &col );

        if( status == OK )
        {
            slice_window = graphics->associated[SLICE_WINDOW];

            slice_window->slice.colour_coding.colour_below = col;

            colour_coding_has_changed( slice_window );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_under_colour )   /* ARGSUSED */
{
    void             set_menu_text_with_colour();
    graphics_struct  *slice_window;
    Colour           *col;

    slice_window = graphics->associated[SLICE_WINDOW];

    if( slice_window == (graphics_struct *) 0 )
        col = &WHITE;
    else
        col = &slice_window->slice.colour_coding.colour_below;

    set_menu_text_with_colour( menu_window, menu_entry, label, col );

    return( OK );
}

public  DEF_MENU_FUNCTION(set_over_colour )   /* ARGSUSED */
{
    Status                  status;
    Status                  convert_string_to_colour();
    graphics_struct         *slice_window;
    volume_struct           *volume;
    String                  line;
    Colour                  col;
    void                    colour_coding_has_changed();

    status = OK;

    if( get_slice_window_volume( graphics, &volume ) )
    {
        PRINT( "Enter over colour name or r g b:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
            status = convert_string_to_colour( line, &col );

        if( status == OK )
        {
            slice_window = graphics->associated[SLICE_WINDOW];

            slice_window->slice.colour_coding.colour_above = col;

            colour_coding_has_changed( slice_window );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_over_colour )   /* ARGSUSED */
{
    void             set_menu_text_with_colour();
    graphics_struct  *slice_window;
    Colour           *col;

    slice_window = graphics->associated[SLICE_WINDOW];

    if( slice_window == (graphics_struct *) 0 )
        col = &WHITE;
    else
        col = &slice_window->slice.colour_coding.colour_above;

    set_menu_text_with_colour( menu_window, menu_entry, label, col );


    return( OK );
}

public  DEF_MENU_FUNCTION(set_user_defined_min_colour )   /* ARGSUSED */
{
    Status                  status;
    Status                  convert_string_to_colour();
    graphics_struct         *slice_window;
    volume_struct           *volume;
    String                  line;
    Colour                  col;
    void                    colour_coding_has_changed();

    status = OK;

    if( get_slice_window_volume( graphics, &volume ) )
    {
        PRINT( "Enter user defined min colour name or r g b:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
            status = convert_string_to_colour( line, &col );

        if( status == OK )
        {
            slice_window = graphics->associated[SLICE_WINDOW];

            slice_window->slice.colour_coding.user_defined_min_colour = col;

            colour_coding_has_changed( slice_window );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_user_defined_min_colour )   /* ARGSUSED */
{
    void             set_menu_text_with_colour();
    graphics_struct  *slice_window;
    Colour           *col;

    slice_window = graphics->associated[SLICE_WINDOW];

    if( slice_window == (graphics_struct *) 0 )
        col = &WHITE;
    else
        col = &slice_window->slice.colour_coding.user_defined_min_colour;

    set_menu_text_with_colour( menu_window, menu_entry, label, col );

    return( OK );
}

public  DEF_MENU_FUNCTION(set_user_defined_max_colour )   /* ARGSUSED */
{
    Status                  status;
    Status                  convert_string_to_colour();
    graphics_struct         *slice_window;
    volume_struct           *volume;
    String                  line;
    Colour                  col;
    void                    colour_coding_has_changed();

    status = OK;

    if( get_slice_window_volume( graphics, &volume ) )
    {
        PRINT( "Enter user defined max colour name or r g b:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
            status = convert_string_to_colour( line, &col );

        if( status == OK )
        {
            slice_window = graphics->associated[SLICE_WINDOW];

            slice_window->slice.colour_coding.user_defined_max_colour = col;

            colour_coding_has_changed( slice_window );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_user_defined_max_colour )   /* ARGSUSED */
{
    void             set_menu_text_with_colour();
    graphics_struct  *slice_window;
    Colour           *col;

    slice_window = graphics->associated[SLICE_WINDOW];

    if( slice_window == (graphics_struct *) 0 )
        col = &WHITE;
    else
        col = &slice_window->slice.colour_coding.user_defined_max_colour;

    set_menu_text_with_colour( menu_window, menu_entry, label, col );

    return( OK );
}
