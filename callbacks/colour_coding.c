
#include  <display.h>

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
    return( slice_window_exists(display) );
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
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_hot_metal )   /* ARGSUSED */
{
    set_the_colour_coding_type( display, HOT_METAL );

    return( OK );
}

public  DEF_MENU_UPDATE(set_hot_metal )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_gray_scale )   /* ARGSUSED */
{
    set_the_colour_coding_type( display, GRAY_SCALE );

    return( OK );
}

public  DEF_MENU_UPDATE(set_gray_scale )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_spectral )   /* ARGSUSED */
{
    set_the_colour_coding_type( display, SPECTRAL );

    return( OK );
}

public  DEF_MENU_UPDATE(set_spectral )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_under_colour )   /* ARGSUSED */
{
    Status                  status;
    display_struct          *slice_window;
    Volume                  volume;
    STRING                  line;
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
    BOOLEAN          active;
    display_struct   *slice_window;
    Colour           col;

    active = get_slice_window( display, &slice_window );

    if( !active )
        col = WHITE;
    else
        col = get_colour_coding_under_colour(
                        &slice_window->slice.colour_coding );

    set_menu_text_with_colour( menu_window, menu_entry, col );

    return( active );
}

public  DEF_MENU_FUNCTION(set_over_colour )   /* ARGSUSED */
{
    Status                  status;
    display_struct          *slice_window;
    Volume                  volume;
    STRING                  line;
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
    BOOLEAN          active;
    display_struct   *slice_window;
    Colour           col;

    active = get_slice_window( display, &slice_window );

    if( !active )
        col = WHITE;
    else
        col = get_colour_coding_over_colour(
                        &slice_window->slice.colour_coding );

    set_menu_text_with_colour( menu_window, menu_entry, col );

    return( active );
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
    BOOLEAN          state;
    Real             ratio;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    if( state )
        ratio = slice_window->slice.label_colour_ratio;
    else
        ratio = 0.0;

    set_menu_text_real( menu_window, menu_entry, ratio );

    return( state );
}

private  void  set_filter_type(
    display_struct   *display,
    Filter_types     filter_type )
{
    int             view_index;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_axis_index_under_mouse( display, &view_index ) )
    {
        slice_window->slice.slice_views[view_index].filter_type = filter_type;

        set_slice_window_update( slice_window, view_index );
    }
}

public  DEF_MENU_FUNCTION(set_nearest_neighbour )   /* ARGSUSED */
{
    set_filter_type( display, NEAREST_NEIGHBOUR );
    return( OK );
}

public  DEF_MENU_UPDATE(set_nearest_neighbour )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_linear_interpolation )   /* ARGSUSED */
{
    set_filter_type( display, LINEAR_INTERPOLATION );
    return( OK );
}

public  DEF_MENU_UPDATE(set_linear_interpolation )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_box_filter )   /* ARGSUSED */
{
    set_filter_type( display, BOX_FILTER );
    return( OK );
}

public  DEF_MENU_UPDATE(set_box_filter )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_triangle_filter )   /* ARGSUSED */
{
    set_filter_type( display, TRIANGLE_FILTER );
    return( OK );
}

public  DEF_MENU_UPDATE(set_triangle_filter )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_gaussian_filter )   /* ARGSUSED */
{
    set_filter_type( display, GAUSSIAN_FILTER );
    return( OK );
}

public  DEF_MENU_UPDATE(set_gaussian_filter )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}

public  DEF_MENU_FUNCTION(set_filter_half_width )   /* ARGSUSED */
{
    int             view_index;
    display_struct  *slice_window;
    Real            filter_width;

    if( get_slice_window( display, &slice_window ) &&
        get_axis_index_under_mouse( display, &view_index ) )
    {
        print( "Current filter full width half max: %g\n",
               slice_window->slice.slice_views[view_index].filter_width );

        print( "Enter new value: " );

        if( input_real( stdin, &filter_width ) == OK &&
            filter_width >= 0.0 )
        {
            slice_window->slice.slice_views[view_index].filter_width =
                                                      filter_width;

            set_slice_window_update( slice_window, view_index );
        }

        (void) input_newline( stdin );
    }
}

public  DEF_MENU_UPDATE(set_filter_half_width )   /* ARGSUSED */
{
    return( slice_window_exists(display) );
}
