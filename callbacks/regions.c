
#include  <def_display.h>

public  DEF_MENU_FUNCTION( set_paint_brush_size )   /* ARGSUSED */
{
    Real            brush_size;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter brush size: " );

        if( input_real( stdin, &brush_size ) == OK && brush_size >= 0.0 )
            slice_window->slice.brush_size = brush_size;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_paint_brush_size )   /* ARGSUSED */
{
    String           text;
    Real             brush_size;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        brush_size = slice_window->slice.brush_size;
    else
        brush_size = Default_brush_size;

    (void) sprintf( text, label, brush_size );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_current_paint_label )   /* ARGSUSED */
{
    int             label;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter current paint label: " );

        if( input_int( stdin, &label ) == OK &&
            label >= 0 && label <= LOWER_AUXILIARY_BITS )
            slice_window->slice.current_paint_label = label;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_current_paint_label )   /* ARGSUSED */
{
    String           text;
    int              current_label;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        current_label = slice_window->slice.current_paint_label;
    else
        current_label = Default_paint_label;

    (void) sprintf( text, label, current_label );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_label_colour )   /* ARGSUSED */
{
    display_struct   *slice_window;
    String           line;
    Colour           col;

    if( get_slice_window( display, &slice_window ) &&
        slice_window->slice.current_paint_label > 0 )
    {
        print( "Enter the colour for label %d: ",
               slice_window->slice.current_paint_label );

        if( input_line( stdin, line, MAX_STRING_LENGTH ) == OK )
        {
            col = convert_string_to_colour( line );

            add_new_label( slice_window,
                   ACTIVE_BIT | slice_window->slice.current_paint_label, col );

            set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_label_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( save_labels )   /* ARGSUSED */
{
    Status         status;
    FILE           *file;
    int            i;
    String         filename;
    display_struct *slice_window;
    Volume         volume;
    Colour         colour_table[LOWER_AUXILIARY_BITS+1];

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename to save: " );
        if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            for_less( i, 0, LOWER_AUXILIARY_BITS+1 )
            {
                if( slice_window->slice.label_colours_used[i|ACTIVE_BIT] )
                    colour_table[i] = slice_window->slice.
                                        label_colours[i|ACTIVE_BIT];
                else
                    colour_table[i] = BLACK;
            }

            status = open_file_with_default_suffix( filename, "lmk",
                                            WRITE_FILE, ASCII_FORMAT, &file );

            if( status == OK )
                status = output_labels_as_landmarks( file, volume,
                      display->three_d.default_marker_size,
                      display->three_d.default_marker_patient_id,
                      colour_table );

            if( status == OK )
            status = close_file( file );

            print( "Done saving.\n" );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(save_labels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( load_labels )   /* ARGSUSED */
{
    Status         status;
    FILE           *file;
    String         filename;
    Volume         volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        print( "Enter filename to load: " );
        if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            status = open_file_with_default_suffix( filename, "lmk",
                                            READ_FILE, ASCII_FORMAT, &file );

            if( status == OK )
                status = input_landmarks_as_labels( file, volume );

            if( status == OK )
                status = close_file( file );

            print( "Done loading.\n" );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(load_labels )   /* ARGSUSED */
{
    return( OK );
}
