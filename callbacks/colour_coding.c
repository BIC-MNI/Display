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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/colour_coding.c,v 1.25 1996/12/09 20:21:24 david Exp $";
#endif


#include  <display.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_colour_limits )
{
    int              volume_index;
    STRING           line;
    BOOLEAN          do_it;
    Real             min_value, max_value;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        volume_index = get_current_volume_index( slice_window );

        print( "Current limits:\t%g\t%g\n",
               slice_window->slice.volumes[volume_index].
                                             colour_coding.min_value,
               slice_window->slice.volumes[volume_index].
                                             colour_coding.max_value );

        print( "Enter new values: " );

        if( input_line( stdin, &line ) == OK )
        {
            do_it = TRUE;
            if( sscanf( line, "%lf %lf", &min_value, &max_value ) != 2 )
            {
                if( sscanf( line, "%lf", &min_value ) != 1 )
                    do_it = FALSE;
                else
                    max_value = min_value;
            }

            if( do_it )
            {
                change_colour_coding_range( slice_window,
                                            volume_index, min_value, max_value);

                print( "    New limits:\t%g\t%g\n",
                   slice_window->slice.volumes[volume_index].
                                                  colour_coding.min_value,
                   slice_window->slice.volumes[volume_index].
                                                  colour_coding.max_value );
            }

            delete_string( line );
        }
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_colour_limits )
{
    return( get_n_volumes(display) > 0 );
}

private  void  set_the_colour_coding_type(
    display_struct       *display,
    Colour_coding_types  type )
{
    display_struct          *slice_window;
    colour_coding_struct    *colour_coding;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        colour_coding = &slice_window->slice.volumes
                 [get_current_volume_index(slice_window)].colour_coding;

        set_colour_coding_type( colour_coding, type );

        colour_coding_has_changed( slice_window,
                      get_current_volume_index(slice_window), UPDATE_SLICE );
    }
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_contour_colour_map )
{
    set_the_colour_coding_type( display, CONTOUR_COLOUR_MAP );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_contour_colour_map )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_hot_metal )
{
    set_the_colour_coding_type( display, HOT_METAL );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_hot_metal )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_gray_scale )
{
    set_the_colour_coding_type( display, GRAY_SCALE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_gray_scale )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_red )
{
    set_the_colour_coding_type( display, RED_COLOUR_MAP );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_red )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_green )
{
    set_the_colour_coding_type( display, GREEN_COLOUR_MAP );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_green )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_blue )
{
    set_the_colour_coding_type( display, BLUE_COLOUR_MAP );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_blue )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_spectral )
{
    set_the_colour_coding_type( display, SPECTRAL );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_spectral )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_arbitrary_colour_scale )
{
    set_the_colour_coding_type( display, SINGLE_COLOUR_SCALE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_arbitrary_colour_scale )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_user_defined_colour_scale )
{
    set_the_colour_coding_type( display, USER_DEFINED_COLOUR_MAP );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_user_defined_colour_scale )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_under_colour )
{
    Status                  status;
    display_struct          *slice_window;
    STRING                  line;
    Colour                  col;

    status = OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter under colour name or 3 or 4 colour components: " );

        status = input_line( stdin, &line );

        if( status == OK )
        {
            col = convert_string_to_colour( line );

            set_colour_coding_under_colour( &slice_window->slice.
                 volumes[get_current_volume_index(slice_window)].colour_coding,
                 col );

            colour_coding_has_changed( slice_window,
                      get_current_volume_index(slice_window), UPDATE_SLICE );
        }

        delete_string( line );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_under_colour )
{
    BOOLEAN          active;
    display_struct   *slice_window;
    Colour           col;

    active = get_slice_window( display, &slice_window ) &&
             get_n_volumes(slice_window) > 0;

    if( !active )
        col = WHITE;
    else
        col = get_colour_coding_under_colour( &slice_window->slice.
               volumes[get_current_volume_index(slice_window)].colour_coding );

    set_menu_text_with_colour( menu_window, menu_entry, col );

    return( active );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_over_colour )
{
    Status                  status;
    display_struct          *slice_window;
    STRING                  line;
    Colour                  col;

    status = OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter over colour name or 3 or 4 colour components: " );

        status = input_line( stdin, &line );

        if( status == OK )
        {
            col = convert_string_to_colour( line );

            set_colour_coding_over_colour( &slice_window->slice.
                volumes[get_current_volume_index(slice_window)].colour_coding,
                col );

            colour_coding_has_changed( slice_window,
                      get_current_volume_index(slice_window), UPDATE_SLICE );
        }

        delete_string( line );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_over_colour )
{
    BOOLEAN          active;
    display_struct   *slice_window;
    Colour           col;

    active = get_slice_window( display, &slice_window ) &&
             get_n_volumes(slice_window) > 0;

    if( !active )
        col = WHITE;
    else
        col = get_colour_coding_over_colour(
               &slice_window->slice.
               volumes[get_current_volume_index(slice_window)].colour_coding );

    set_menu_text_with_colour( menu_window, menu_entry, col );

    return( active );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_label_colour_ratio )
{
    Real             opacity;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter new label colour opacity: " );

        if( input_real( stdin, &opacity ) == OK &&
            opacity >= 0.0 && opacity <= 1.0 )
        {
            set_label_opacity( slice_window,
                      get_current_volume_index(slice_window), opacity );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_label_colour_ratio )
{
    BOOLEAN          state;
    Real             opacity;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window ) &&
            get_n_volumes(slice_window) > 0;

    if( state )
        opacity = slice_window->slice.volumes
              [get_current_volume_index(slice_window)].label_colour_opacity;
    else
        opacity = 0.0;

    set_menu_text_real( menu_window, menu_entry, opacity );

    return( state );
}

private  void  set_filter_type(
    display_struct   *display,
    Filter_types     filter_type )
{
    int             view_index, volume_index;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( display, &view_index ) &&
        get_n_volumes(slice_window) > 0 )
    {
        volume_index = get_current_volume_index( slice_window );
        slice_window->slice.volumes[volume_index].views[view_index].filter_type
                                                         = filter_type;

        set_slice_window_update( slice_window, volume_index, view_index,
                                 UPDATE_SLICE );
    }
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_nearest_neighbour )
{
    set_filter_type( display, NEAREST_NEIGHBOUR );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_nearest_neighbour )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_linear_interpolation )
{
    set_filter_type( display, LINEAR_INTERPOLATION );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_linear_interpolation )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_box_filter )
{
    set_filter_type( display, BOX_FILTER );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_box_filter )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_triangle_filter )
{
    set_filter_type( display, TRIANGLE_FILTER );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_triangle_filter )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_gaussian_filter )
{
    set_filter_type( display, GAUSSIAN_FILTER );
    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_gaussian_filter )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_filter_half_width )
{
    int             view_index, volume_index;
    display_struct  *slice_window;
    Real            filter_width;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( display, &view_index ) &&
        get_n_volumes(slice_window) > 0 )
    {
        volume_index = get_current_volume_index( slice_window );

        print( "Current filter full width half max: %g\n",
               slice_window->slice.volumes[volume_index].views[view_index]
                                                  .filter_width );

        print( "Enter new value: " );

        if( input_real( stdin, &filter_width ) == OK &&
            filter_width >= 0.0 )
        {
            slice_window->slice.volumes[volume_index].views[view_index]
                                                  .filter_width = filter_width;

            set_slice_window_update( slice_window, volume_index, view_index,
                                     UPDATE_SLICE );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_filter_half_width )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(set_slice_window_n_labels )
{
    int             n_labels;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter number of labels: " );

        if( input_int( stdin, &n_labels ) == OK )
        {
            set_slice_window_number_labels( slice_window,
                         get_current_volume_index(slice_window), n_labels );
            set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_slice_window_n_labels )
{
    BOOLEAN          state;
    int              n_labels;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window ) &&
            get_n_volumes(slice_window) > 0;

    if( state )
        n_labels = get_num_labels( display,
                                   get_current_volume_index(slice_window));
    else
        n_labels = Initial_num_labels;

    set_menu_text_int( menu_window, menu_entry, n_labels );

    return( state );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(toggle_share_labels )
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.share_labels_flag = 
                             !slice_window->slice.share_labels_flag;
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(toggle_share_labels )
{
    BOOLEAN          state;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    set_menu_text_on_off( menu_window, menu_entry,
                          state && slice_window->slice.share_labels_flag );

    return( state );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(save_colour_map )
{
    Status          status;
    STRING          filename;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter name of colour map file to save: " );

        status = input_string( stdin, &filename, ' ' );
        (void) input_newline( stdin );

        if( status == OK && check_clobber_file_default_suffix( filename,
                                           get_default_colour_map_suffix() ) )
        {
            (void) save_label_colour_map( slice_window, filename );
        }

        delete_string( filename );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(save_colour_map )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(load_colour_map )
{
    STRING          filename;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter name of colour map file to load: " );
        if( input_string( stdin, &filename, ' ' ) == OK )
        {
            (void) load_label_colour_map( slice_window, filename );
            set_slice_window_all_update( slice_window,
                       get_current_volume_index(slice_window), UPDATE_LABELS );

        }
        (void) input_newline( stdin );

        delete_string( filename );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(load_colour_map )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION(load_user_defined_colour_scale )
{
    STRING          filename;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter name of piecewise colour coding file to load: " );
        if( input_string( stdin, &filename, ' ' ) == OK )
        {
            if( load_user_defined_colour_coding( slice_window, filename ) == OK)
            {
                set_colour_coding_type( &slice_window->slice.volumes[
                        get_current_volume_index(slice_window)].colour_coding,
                        USER_DEFINED_COLOUR_MAP );
            }

            colour_coding_has_changed( slice_window,
                                       get_current_volume_index(slice_window),
                                       UPDATE_SLICE );

            set_slice_window_all_update( slice_window,
                       get_current_volume_index(slice_window), UPDATE_SLICE );

        }
        (void) input_newline( stdin );

        delete_string( filename );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(load_user_defined_colour_scale )
{
    return( get_n_volumes(display) > 0 );
}
