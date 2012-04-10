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

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_atlas_on_or_off )
{
    BOOLEAN          state;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        state = !slice_window->slice.atlas.enabled;
        set_atlas_state( slice_window, state );
        set_atlas_update( slice_window, -1 );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_atlas_on_or_off )
{
    BOOLEAN          valid;
    display_struct   *slice_window;

    valid = get_slice_window( display, &slice_window ) &&
            get_n_volumes(slice_window) > 0;

    set_menu_text_on_off( menu_window, menu_entry,
                          valid && slice_window->slice.atlas.enabled );

    return( valid );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_atlas_opacity )
{
    Real             opacity;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter atlas opacity: " );
        if( input_real( stdin, &opacity ) == OK && opacity >= 0.0 &&
            opacity <= 1.0 )
        {
            slice_window->slice.atlas.opacity = opacity;
            set_atlas_update( slice_window, -1 );
        }
        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_atlas_opacity )
{
    return( slice_window_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_atlas_transparent_threshold )
{
    int              threshold;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter value above which atlas is transparent (e.g. 220): " );
        if( input_int( stdin, &threshold ) == OK && threshold >= 0 )
        {
            slice_window->slice.atlas.transparent_threshold = threshold;
            set_atlas_update( slice_window, -1 );
        }
        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_atlas_transparent_threshold )
{
    return( slice_window_exists(display) );
}

private  void  flip_atlas_on_an_axis(
    display_struct    *display,
    int               axis )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.atlas.flipped[axis] =
                             !slice_window->slice.atlas.flipped[axis];
        if( axis == X )
            regenerate_atlas_lookup( slice_window );
        set_atlas_update( slice_window, -1 );
    }
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( flip_atlas_x )
{
    flip_atlas_on_an_axis( display, X );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(flip_atlas_x )
{
    return( is_atlas_loaded(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( flip_atlas_y )
{
    flip_atlas_on_an_axis( display, Y );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(flip_atlas_y )
{
    return( is_atlas_loaded(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( flip_atlas_z )
{
    flip_atlas_on_an_axis( display, Z );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(flip_atlas_z )
{
    return( is_atlas_loaded(display) );
}

private  void  set_atlas_tolerance(
    display_struct    *display,
    int               axis )
{
    Real             distance_tolerance;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter new tolerance: " );

        if( input_real( stdin, &distance_tolerance ) == OK )
        {
            slice_window->slice.atlas.slice_tolerance[axis] =
                             distance_tolerance;
            regenerate_atlas_lookup( slice_window );
            set_atlas_update( slice_window, -1 );
        }
        (void) input_newline( stdin );
    }
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_atlas_tolerance_x )
{
    set_atlas_tolerance( display, X );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_atlas_tolerance_x )
{
    return( slice_window_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_atlas_tolerance_y )
{
    set_atlas_tolerance( display, Y );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_atlas_tolerance_y )
{
    return( slice_window_exists(display) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_atlas_tolerance_z )
{
    set_atlas_tolerance( display, Z );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_atlas_tolerance_z )
{
    return( slice_window_exists(display) );
}
