
#include  <def_graphics.h>
#include  <def_math.h>
#include  <def_files.h>

public  DEF_MENU_FUNCTION(set_colour_limits )   /* ARGSUSED */
{
    volume_struct    *volume;
    Real             min_value, max_value;
    graphics_struct  *slice_window;
    void             set_slice_window_update();
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

            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_colour_limits )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_hot_metal )   /* ARGSUSED */
{
    graphics_struct         *slice_window;
    volume_struct           *volume;
    colour_coding_struct    *colour_coding;
    void                    build_hot_metal_coding();
    void                    set_slice_window_update();
    void                    rebuild_fast_lookup();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];
        colour_coding = &slice_window->slice.colour_coding;

        build_hot_metal_coding( colour_coding );

        rebuild_fast_lookup( slice_window );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_hot_metal )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_gray_scale )   /* ARGSUSED */
{
    graphics_struct         *slice_window;
    volume_struct           *volume;
    colour_coding_struct    *colour_coding;
    void                    build_gray_scale_coding();
    void                    set_slice_window_update();
    void                    rebuild_fast_lookup();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];
        colour_coding = &slice_window->slice.colour_coding;

        build_gray_scale_coding( colour_coding );

        rebuild_fast_lookup( slice_window );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_gray_scale )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_spectral )   /* ARGSUSED */
{
    graphics_struct         *slice_window;
    volume_struct           *volume;
    colour_coding_struct    *colour_coding;
    void                    build_spectral_coding();
    void                    set_slice_window_update();
    void                    rebuild_fast_lookup();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];
        colour_coding = &slice_window->slice.colour_coding;

        build_spectral_coding( colour_coding );

        rebuild_fast_lookup( slice_window );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_spectral )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_under_colour )   /* ARGSUSED */
{
    graphics_struct         *slice_window;
    volume_struct           *volume;
    colour_coding_struct    *colour_coding;
    void                    build_spectral_coding();
    void                    set_slice_window_update();
    void                    rebuild_fast_lookup();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];
        colour_coding = &slice_window->slice.colour_coding;

        build_spectral_coding( colour_coding );

        rebuild_fast_lookup( slice_window );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_under_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_over_colour )   /* ARGSUSED */
{
    graphics_struct         *slice_window;
    volume_struct           *volume;
    colour_coding_struct    *colour_coding;
    void                    build_spectral_coding();
    void                    set_slice_window_update();
    void                    rebuild_fast_lookup();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];
        colour_coding = &slice_window->slice.colour_coding;

        build_spectral_coding( colour_coding );

        rebuild_fast_lookup( slice_window );

        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_over_colour )   /* ARGSUSED */
{
    return( OK );
}
