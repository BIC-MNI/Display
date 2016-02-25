/**
 * \file callbacks/colour_coding.c
 * \brief Commands to manipulate the appearance of the slice views.
 *
 * \copyright
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
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

/* ARGSUSED */

DEF_MENU_FUNCTION(set_colour_limits )
{
    int              volume_index;
    VIO_Real         min_value, max_value;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        char prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];
        volume_index = get_current_volume_index( slice_window );

        sprintf(prompt, "Current limits:\t%g\t%g\nEnter new values:",
               slice_window->slice.volumes[volume_index].
                                             colour_coding.min_value,
               slice_window->slice.volumes[volume_index].
                                             colour_coding.max_value );

        if( get_user_input( prompt, "rr", &min_value, &max_value ) == VIO_OK )
        {
            change_colour_coding_range( slice_window,
                                        volume_index, min_value, max_value);

            print( "    New limits:\t%g\t%g\n",
                   slice_window->slice.volumes[volume_index].
                                                  colour_coding.min_value,
                   slice_window->slice.volumes[volume_index].
                                                  colour_coding.max_value );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_colour_limits )
{
    return( get_n_volumes(display) > 0 );
}

static  void  set_the_colour_coding_type(
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

DEF_MENU_FUNCTION(set_contour_colour_map )
{
    set_the_colour_coding_type( display, CONTOUR_COLOUR_MAP );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_contour_colour_map )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_hot_metal )
{
    set_the_colour_coding_type( display, HOT_METAL );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_hot_metal )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_gray_scale )
{
    set_the_colour_coding_type( display, GRAY_SCALE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_gray_scale )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_red )
{
    set_the_colour_coding_type( display, RED_COLOUR_MAP );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_red )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_green )
{
    set_the_colour_coding_type( display, GREEN_COLOUR_MAP );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_green )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_blue )
{
    set_the_colour_coding_type( display, BLUE_COLOUR_MAP );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_blue )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_spectral )
{
    set_the_colour_coding_type( display, SPECTRAL );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_spectral )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_arbitrary_colour_scale )
{
    set_the_colour_coding_type( display, SINGLE_COLOUR_SCALE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_arbitrary_colour_scale )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_user_defined_colour_scale )
{
    set_the_colour_coding_type( display, USER_DEFINED_COLOUR_MAP );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_user_defined_colour_scale )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_under_colour )
{
    VIO_Status              status;
    display_struct          *slice_window;
    VIO_STR                 line;
    VIO_Colour              col;

    status = VIO_OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        status = get_user_input("Enter under colour name or 3 or 4 colour components: ", "s", &line);

        if( status == VIO_OK )
        {
            col = convert_string_to_colour( line );

            set_colour_coding_under_colour( &slice_window->slice.
                 volumes[get_current_volume_index(slice_window)].colour_coding,
                 col );

            colour_coding_has_changed( slice_window,
                      get_current_volume_index(slice_window), UPDATE_SLICE );

            delete_string( line );
        }
    }

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_under_colour )
{
    VIO_BOOL          active;
    display_struct   *slice_window;
    VIO_Colour           col;

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

DEF_MENU_FUNCTION(set_over_colour )
{
    VIO_Status              status;
    display_struct          *slice_window;
    VIO_STR                 line;
    VIO_Colour              col;

    status = VIO_OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        status = get_user_input( "Enter over colour name or 3 or 4 colour components: ", "s", &line );

        if( status == VIO_OK )
        {
            col = convert_string_to_colour( line );

            set_colour_coding_over_colour( &slice_window->slice.
                volumes[get_current_volume_index(slice_window)].colour_coding,
                col );

            colour_coding_has_changed( slice_window,
                      get_current_volume_index(slice_window), UPDATE_SLICE );

            delete_string( line );
        }
    }

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_over_colour )
{
    VIO_BOOL          active;
    display_struct   *slice_window;
    VIO_Colour           col;

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

DEF_MENU_FUNCTION(set_label_colour_ratio )
{
    VIO_Real             opacity;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if (get_user_input( "Enter new label colour opacity: " , "r", &opacity) == VIO_OK &&

            opacity >= 0.0 && opacity <= 1.0 )
        {
            set_label_opacity( slice_window,
                      get_current_volume_index(slice_window), opacity );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_label_colour_ratio )
{
    VIO_BOOL          state;
    VIO_Real             opacity;
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

/**
 * \brief Return a human-readable description of a filter.
 * \param filter_type The value (VIO_Filter_types) to describe.
 * \returns A constant string describing the filter type, e.g. "Gaussian".
 */
const char *
get_filter_name( VIO_Filter_types filter_type )
{
  switch ( filter_type )
  {
  case GAUSSIAN_FILTER:
    return "Gaussian";
  case BOX_FILTER:
    return "box";
  case LINEAR_INTERPOLATION:
    return "linear";
  case NEAREST_NEIGHBOUR:
    return "nearest neighbour";
  case TRIANGLE_FILTER:
    return "triangle";
  default:
    return "unknown";
  }
}

/**
 * \brief Sets the filter type used for the slice under the mouse cursor.
 *
 * Each slice view can have its own independent setting for a slice filter
 * that averages slices across the axis perpendicular to the view. If the
 * mouse is not over a slice view, this function will not change anything.
 *
 * \param display A pointer to the display_struct of a top-level window.
 * \param filter_type The desired filter type.
 */
static  void  set_filter_type(
    display_struct   *display,
    VIO_Filter_types filter_type )
{
    int             view_index, volume_index;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( display, &view_index ) &&
        get_n_volumes(slice_window) > 0 )
    {
        volume_view_struct *vv_ptr;
        volume_index = get_current_volume_index( slice_window );

        vv_ptr = &slice_window->slice.volumes[volume_index].views[view_index];
        vv_ptr->filter_type = filter_type;

        print("The %s view is now using a %s filter",
              get_view_name( slice_window, volume_index, view_index ),
              get_filter_name( filter_type ));
        if ( filter_type != NEAREST_NEIGHBOUR &&
             filter_type != LINEAR_INTERPOLATION )
            print(" with FWHM %.2f", vv_ptr->filter_width );
        print(".\n");

        set_slice_window_update( slice_window, volume_index, view_index,
                                 UPDATE_SLICE );
    }
    else
    {
        print("The filter type was not changed.\n");
    }
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_nearest_neighbour )
{
    set_filter_type( display, NEAREST_NEIGHBOUR );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_nearest_neighbour )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_linear_interpolation )
{
    set_filter_type( display, LINEAR_INTERPOLATION );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_linear_interpolation )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_box_filter )
{
    set_filter_type( display, BOX_FILTER );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_box_filter )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_triangle_filter )
{
    set_filter_type( display, TRIANGLE_FILTER );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_triangle_filter )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_gaussian_filter )
{
    set_filter_type( display, GAUSSIAN_FILTER );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_gaussian_filter )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_filter_half_width )
{
    int             view_index, volume_index;
    display_struct  *slice_window;
    VIO_Real        filter_width;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_view_index_under_mouse( display, &view_index ) &&
        get_n_volumes(slice_window) > 0 )
    {
        char prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

        volume_index = get_current_volume_index( slice_window );

        sprintf( prompt, 
                 "Current filter full width half max: %g\nEnter new value: ",
                slice_window->slice.volumes[volume_index].views[view_index]
                                                  .filter_width );

        if( get_user_input( prompt, "r", &filter_width ) == VIO_OK &&
            filter_width >= 0.0 )
        {
            slice_window->slice.volumes[volume_index].views[view_index]
                                                  .filter_width = filter_width;

            set_slice_window_update( slice_window, volume_index, view_index,
                                     UPDATE_SLICE );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_filter_half_width )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(set_slice_window_n_labels )
{
    int             n_labels;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if (get_user_input( "Enter number of labels: ", "d",
                            &n_labels) == VIO_OK)
        {
            set_slice_window_number_labels( slice_window,
                         get_current_volume_index(slice_window), n_labels );
            set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );
        }
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_slice_window_n_labels )
{
    VIO_BOOL          state;
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

DEF_MENU_FUNCTION(toggle_share_labels )
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.share_labels_flag = 
                             !slice_window->slice.share_labels_flag;
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_share_labels )
{
    VIO_BOOL          state;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    set_menu_text_on_off( menu_window, menu_entry,
                          state && slice_window->slice.share_labels_flag );

    return( state );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(save_colour_map )
{
    VIO_Status          status;
    VIO_STR          filename;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        status = get_user_file( "Enter name of colour map file to save: ",
                                TRUE,
                                get_default_colour_map_suffix(),
                                &filename);
        if (status == VIO_OK)
        {
            (void) save_label_colour_map( slice_window, filename );
            delete_string( filename );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(save_colour_map )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(load_colour_map )
{
    VIO_STR          filename;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if (get_user_file( "Enter name of colour map file to load: ",
                           FALSE, NULL, &filename ) == VIO_OK )
        {
            (void) load_label_colour_map( slice_window, filename );
            set_slice_window_all_update( slice_window,
                       get_current_volume_index(slice_window), UPDATE_LABELS );

            delete_string( filename );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(load_colour_map )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION(load_user_defined_colour_scale )
{
    VIO_STR          filename;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if (get_user_file("Enter name of piecewise colour coding file to load: ",
                          FALSE, NULL, &filename) == VIO_OK)
        {
            if( load_user_defined_colour_coding( slice_window, filename ) == VIO_OK)
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

            delete_string( filename );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(load_user_defined_colour_scale )
{
    return( get_n_volumes(display) > 0 );
}
