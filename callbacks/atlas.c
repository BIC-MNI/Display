
#include  <def_graphics.h>
#include  <def_files.h>

public  DEF_MENU_FUNCTION( set_atlas_on_or_off )   /* ARGSUSED */
{
    Boolean          state;
    graphics_struct  *slice_window;
    void             set_slice_window_update();
    void             set_atlas_state();

    if( get_slice_window( graphics, &slice_window ) )
    {
        state = !slice_window->slice.atlas.enabled;
        set_atlas_state( slice_window, state );
        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_atlas_on_or_off )   /* ARGSUSED */
{
    void             set_text_on_off();
    String           text;
    void             set_menu_text();
    graphics_struct  *slice_window;

    if( get_slice_window( graphics, &slice_window ) )
        set_text_on_off( label, text, slice_window->slice.atlas.enabled );
    else
        (void) sprintf( text, label, "none" );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_atlas_opacity )   /* ARGSUSED */
{
    Real             opacity;
    graphics_struct  *slice_window;
    void             set_slice_window_update();

    if( get_slice_window( graphics, &slice_window ) )
    {
        PRINT( "Enter atlas opacity: " );
        if( input_real( stdin, &opacity ) == OK && opacity >= 0.0 &&
            opacity <= 1.0 )
        {
            slice_window->slice.atlas.opacity = opacity;
            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_atlas_opacity )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_atlas_transparent_threshold )   /* ARGSUSED */
{
    int              threshold;
    graphics_struct  *slice_window;
    void             set_slice_window_update();

    if( get_slice_window( graphics, &slice_window ) )
    {
        PRINT( "Enter value above which atlas is transparent (e.g. 220): " );
        if( input_int( stdin, &threshold ) == OK && threshold >= 0 )
        {
            slice_window->slice.atlas.transparent_threshold = threshold;
            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_atlas_transparent_threshold )   /* ARGSUSED */
{
    return( OK );
}

private  void  flip_atlas_on_an_axis( graphics, axis )
    graphics_struct   *graphics;
    int               axis;
{
    graphics_struct  *slice_window;
    void             set_slice_window_update();

    if( get_slice_window( graphics, &slice_window ) )
    {
        slice_window->slice.atlas.flipped[axis] =
                             !slice_window->slice.atlas.flipped[axis];
        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }
}

public  DEF_MENU_FUNCTION( flip_atlas_x )   /* ARGSUSED */
{
    flip_atlas_on_an_axis( graphics, X );
    return( OK );
}

public  DEF_MENU_UPDATE(flip_atlas_x )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( flip_atlas_y )   /* ARGSUSED */
{
    flip_atlas_on_an_axis( graphics, Y );
    return( OK );
}

public  DEF_MENU_UPDATE(flip_atlas_y )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( flip_atlas_z )   /* ARGSUSED */
{
    flip_atlas_on_an_axis( graphics, Z );
    return( OK );
}

public  DEF_MENU_UPDATE(flip_atlas_z )   /* ARGSUSED */
{
    return( OK );
}

private  void  set_atlas_tolerance( graphics, axis )
    graphics_struct   *graphics;
    int               axis;
{
    Real             distance_tolerance;
    graphics_struct  *slice_window;
    void             regenerate_atlas_lookup();
    void             set_slice_window_update();

    if( get_slice_window( graphics, &slice_window ) )
    {
        PRINT( "Enter new tolerance: " );

        if( input_real( stdin, &distance_tolerance ) == OK )
        {
            slice_window->slice.atlas.slice_tolerance[axis] =
                             distance_tolerance;
            regenerate_atlas_lookup( slice_window );
            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
        (void) input_newline( stdin );
    }
}

public  DEF_MENU_FUNCTION( set_atlas_tolerance_x )   /* ARGSUSED */
{
    set_atlas_tolerance( graphics, X );
    return( OK );
}

public  DEF_MENU_UPDATE(set_atlas_tolerance_x )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_atlas_tolerance_y )   /* ARGSUSED */
{
    set_atlas_tolerance( graphics, Y );
    return( OK );
}

public  DEF_MENU_UPDATE(set_atlas_tolerance_y )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_atlas_tolerance_z )   /* ARGSUSED */
{
    set_atlas_tolerance( graphics, Z );
    return( OK );
}

public  DEF_MENU_UPDATE(set_atlas_tolerance_z )   /* ARGSUSED */
{
    return( OK );
}
