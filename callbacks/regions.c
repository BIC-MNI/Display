
#include  <display.h>

public  DEF_MENU_FUNCTION( set_paint_xy_brush_radius )   /* ARGSUSED */
{
    Real            xy_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter xy brush size: " );

        if( input_real( stdin, &xy_brush_radius ) == OK &&
            xy_brush_radius >= 0.0 )
        {
            slice_window->slice.x_brush_radius = xy_brush_radius;
            slice_window->slice.y_brush_radius = xy_brush_radius;
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_paint_xy_brush_radius )   /* ARGSUSED */
{
    BOOLEAN          state;
    Real             x_brush_radius;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );
    if( state )
        x_brush_radius = slice_window->slice.x_brush_radius;
    else
        x_brush_radius = Default_x_brush_radius;

    set_menu_text_real( menu_window, menu_entry, x_brush_radius );

    return( state );
}

public  DEF_MENU_FUNCTION( set_paint_z_brush_radius )   /* ARGSUSED */
{
    Real            z_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter out of plane brush size: " );

        if( input_real( stdin, &z_brush_radius ) == OK &&
            z_brush_radius >= 0.0 )
            slice_window->slice.z_brush_radius = z_brush_radius;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_paint_z_brush_radius )   /* ARGSUSED */
{
    BOOLEAN          state;
    Real             z_brush_radius;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );
    if( state )
        z_brush_radius = slice_window->slice.z_brush_radius;
    else
        z_brush_radius = Default_z_brush_radius;

    set_menu_text_real( menu_window, menu_entry, z_brush_radius );

    return( state );
}


public  DEF_MENU_FUNCTION( set_current_paint_label )   /* ARGSUSED */
{
    int             label;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter current paint label: " );

        if( input_int( stdin, &label ) == OK &&
            label >= 0 && label < get_num_labels(slice_window,
                            get_current_volume_index(slice_window)) )
            slice_window->slice.current_paint_label = label;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_current_paint_label )   /* ARGSUSED */
{
    BOOLEAN          state;
    int              current_label;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    if( state )
        current_label = slice_window->slice.current_paint_label;
    else
        current_label = Default_paint_label;

    set_menu_text_int( menu_window, menu_entry, current_label );

    return( state );
}

public  DEF_MENU_FUNCTION( set_label_colour )   /* ARGSUSED */
{
    display_struct   *slice_window;
    int              label;
    STRING           line;
    Colour           col;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter the label number and colour for this label: " );

        if( input_int( stdin, &label ) == OK &&
            label >= 1 && label < get_num_labels(slice_window,
                            get_current_volume_index(slice_window)) )
        {
            if( input_line( stdin, line, MAX_STRING_LENGTH ) == OK )
            {
                col = convert_string_to_colour( line );

                set_colour_of_label( slice_window,
                                get_current_volume_index(slice_window),
                                label, col );

                set_slice_window_all_update( slice_window,
                       get_current_volume_index(slice_window), UPDATE_LABELS );
            }
        }
        else
            (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_label_colour )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

private  void  copy_labels_from_adjacent_slice(
    display_struct   *display,
    int              src_offset )
{
    Real             real_dest_index[MAX_DIMENSIONS];
    int              src_index[N_DIMENSIONS], dest_index[MAX_DIMENSIONS];
    int              view_index, x_index, y_index, axis_index;
    int              volume_index;
    display_struct   *slice_window;
    Volume           volume;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( display, &volume_index,
                               &view_index, real_dest_index ) &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        volume = get_nth_volume( slice_window, volume_index );

        convert_real_to_int_voxel( N_DIMENSIONS, real_dest_index, dest_index );

        src_index[X] = 0;
        src_index[Y] = 0;
        src_index[Z] = 0;
        src_index[axis_index] = dest_index[axis_index] + src_offset;

        if( int_voxel_is_within_volume( volume, src_index ) )
        {
            record_slice_labels( display, volume_index, axis_index,
                                 dest_index[axis_index] );

            copy_labels_slice_to_slice(
                         volume,
                         get_nth_label_volume(slice_window,volume_index),
                         axis_index,
                         src_index[axis_index],
                         dest_index[axis_index],
                         slice_window->slice.segmenting.min_threshold,
                         slice_window->slice.segmenting.max_threshold );

            set_slice_window_all_update( slice_window, volume_index,
                                         UPDATE_LABELS );
        }
    }
}

public  DEF_MENU_FUNCTION( copy_labels_from_lower_slice )   /* ARGSUSED */
{
    copy_labels_from_adjacent_slice( display, -1 );

    return( OK );
}

public  DEF_MENU_UPDATE(copy_labels_from_lower_slice )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( copy_labels_from_higher_slice )   /* ARGSUSED */
{
    copy_labels_from_adjacent_slice( display, 1 );

    return( OK );
}

public  DEF_MENU_UPDATE(copy_labels_from_higher_slice )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( toggle_display_labels )   /* ARGSUSED */
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        slice_window->slice.volumes[get_current_volume_index(slice_window)].
                        display_labels =
           !slice_window->slice.volumes[get_current_volume_index(slice_window)].
                           display_labels;
        set_slice_window_all_update( slice_window,
                        get_current_volume_index(slice_window), UPDATE_LABELS );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_display_labels )   /* ARGSUSED */
{
    BOOLEAN          display_labels, state;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window ) &&
            get_n_volumes(slice_window) > 0;

    if( state )
        display_labels = slice_window->slice.
                volumes[get_current_volume_index(slice_window)].display_labels;
    else
        display_labels = Initial_display_labels;

    set_menu_text_on_off( menu_window, menu_entry, display_labels );

    return( state );
}

public  DEF_MENU_FUNCTION( change_labels_in_range )   /* ARGSUSED */
{
    display_struct  *slice_window;
    Status          status;
    int             src_label, dest_label;
    Real            min_threshold, max_threshold;
    Volume          volume;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        status = OK;
        print( "Label to change from: " );
        status = input_int( stdin, &src_label );

        if( status == OK )
        {
            print( "Label to change to: " );
            status = input_int( stdin, &dest_label );
        }

        if( status == OK )
        {
            print( "Min and max of value range: " );
            status = input_real( stdin, &min_threshold );
        }

        if( status == OK )
            status = input_real( stdin, &max_threshold );

        (void) input_newline( stdin );

        if( status == OK )
        {
            modify_labels_in_range( volume, get_label_volume(slice_window),
                                    src_label, dest_label,
                                    min_threshold, max_threshold );
            delete_slice_undo( &slice_window->slice.undo,
                               get_current_volume_index(slice_window) );
            set_slice_window_all_update( slice_window,
                       get_current_volume_index(slice_window), UPDATE_LABELS );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_labels_in_range )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

private  void  calculate_label_volume(
    Volume    label_volume,
    int       label,
    int       *n_voxels,
    Real      *cubic_mm )
{
    int   x, y, z, sizes[MAX_DIMENSIONS];
    int   n_vox;
    Real  separations[MAX_DIMENSIONS];

    get_volume_sizes( label_volume, sizes );
    n_vox = 0;

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                if( get_3D_volume_label_data( label_volume, x, y, z ) == label )
                    ++n_vox;
            }
        }
    }

    get_volume_separations( label_volume, separations );

    *n_voxels = n_vox;
    *cubic_mm = (Real) n_vox * separations[X] * separations[Y] * separations[Z];
    *cubic_mm = ABS( *cubic_mm );

    print( "Voxel size: %g mm by %g mm by %g mm\n",
           ABS( separations[X] ),
           ABS( separations[Y] ),
           ABS( separations[Z] ) );
}

public  DEF_MENU_FUNCTION( calculate_volume )   /* ARGSUSED */
{
    display_struct  *slice_window;
    int             n_voxels;
    Real            cubic_millimetres;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        calculate_label_volume( get_label_volume(slice_window),
                                slice_window->slice.current_paint_label,
                                &n_voxels, &cubic_millimetres );

        print( "Region %d:    number of voxels: %d\n",
                  slice_window->slice.current_paint_label, n_voxels );
        print( "             cubic millimetres: %g\n", cubic_millimetres );
        print( "             cubic centimetres: %g\n",
               cubic_millimetres / 1000.0 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(calculate_volume )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( flip_labels_in_x )   /* ARGSUSED */
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        flip_labels_around_zero( get_label_volume( slice_window ) );
        delete_slice_undo( &slice_window->slice.undo,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                        get_current_volume_index(slice_window), UPDATE_LABELS );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(flip_labels_in_x )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

private  void  translate_labels_callback(
    display_struct   *display,
    int              x_delta,
    int              y_delta )
{
    display_struct  *slice_window;
    int             view_index, axis_index, x_index, y_index;
    int             delta[N_DIMENSIONS];

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 &&
        get_slice_view_index_under_mouse( display, &view_index ) &&
        slice_has_ortho_axes( slice_window,
                       get_current_volume_index( slice_window ), view_index,
                       &x_index, &y_index, &axis_index ) )
    {
        delta[X] = 0;
        delta[Y] = 0;
        delta[Z] = 0;

        delta[x_index] = x_delta;
        delta[y_index] = y_delta;

        translate_labels( get_label_volume(slice_window), delta );
        delete_slice_undo( &slice_window->slice.undo,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                        get_current_volume_index(slice_window), UPDATE_LABELS);
    }
}

public  DEF_MENU_FUNCTION( translate_labels_up )   /* ARGSUSED */
{
    translate_labels_callback( display, 0, 1 );

    return( OK );
}

public  DEF_MENU_UPDATE(translate_labels_up )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( translate_labels_down )   /* ARGSUSED */
{
    translate_labels_callback( display, 0, -1 );

    return( OK );
}

public  DEF_MENU_UPDATE(translate_labels_down )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( translate_labels_left )   /* ARGSUSED */
{
    translate_labels_callback( display, -1, 0 );

    return( OK );
}

public  DEF_MENU_UPDATE(translate_labels_left )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( translate_labels_right )   /* ARGSUSED */
{
    translate_labels_callback( display, 1, 0 );

    return( OK );
}

public  DEF_MENU_UPDATE(translate_labels_right )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}

public  DEF_MENU_FUNCTION( undo_slice_labels )   /* ARGSUSED */
{
    int   volume_index;

    volume_index = undo_slice_labels_if_any( display );

    if( volume_index >= 0 )
    {
        set_slice_window_all_update( display->associated[SLICE_WINDOW],
                                     volume_index, UPDATE_LABELS );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(undo_slice_labels )   /* ARGSUSED */
{
    return( slice_labels_to_undo(display) );
}

public  DEF_MENU_FUNCTION( translate_labels_arbitrary )   /* ARGSUSED */
{
    int              delta[MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        print( "Enter offset to translate for x, y and z: " );

        if( input_int( stdin, &delta[X] ) == OK &&
            input_int( stdin, &delta[Y] ) == OK &&
            input_int( stdin, &delta[Z] ) == OK )
        {
            translate_labels( get_label_volume(slice_window), delta );
            set_slice_window_all_update( slice_window,
                        get_current_volume_index(slice_window), UPDATE_LABELS );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(translate_labels_arbitrary )   /* ARGSUSED */
{
    return( get_n_volumes(display) > 0 );
}
