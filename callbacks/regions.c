
#include  <display.h>

public  DEF_MENU_FUNCTION( set_paint_x_brush_radius )   /* ARGSUSED */
{
    Real            x_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter x brush size: " );

        if( input_real( stdin, &x_brush_radius ) == OK &&
            x_brush_radius >= 0.0 )
            slice_window->slice.x_brush_radius = x_brush_radius;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_paint_x_brush_radius )   /* ARGSUSED */
{
    STRING           text;
    Real             x_brush_radius;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        x_brush_radius = slice_window->slice.x_brush_radius;
    else
        x_brush_radius = Default_x_brush_radius;

    (void) sprintf( text, label, x_brush_radius );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_paint_y_brush_radius )   /* ARGSUSED */
{
    Real            y_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter y brush size: " );

        if( input_real( stdin, &y_brush_radius ) == OK &&
            y_brush_radius >= 0.0 )
            slice_window->slice.y_brush_radius = y_brush_radius;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_paint_y_brush_radius )   /* ARGSUSED */
{
    STRING           text;
    Real             y_brush_radius;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        y_brush_radius = slice_window->slice.y_brush_radius;
    else
        y_brush_radius = Default_y_brush_radius;

    (void) sprintf( text, label, y_brush_radius );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
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
    STRING           text;
    Real             z_brush_radius;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        z_brush_radius = slice_window->slice.z_brush_radius;
    else
        z_brush_radius = Default_z_brush_radius;

    (void) sprintf( text, label, z_brush_radius );

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
            label >= 0 && label <= 255 )
            slice_window->slice.current_paint_label = label;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_current_paint_label )   /* ARGSUSED */
{
    STRING           text;
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
    STRING           line;
    Colour           col;

    if( get_slice_window( display, &slice_window ) )
    {
        if( slice_window->slice.current_paint_label <= 0 )
        {
            print( "First you must set the current paint label to > 0.\n" );
            return( OK );
        }

        print( "Enter the colour for label %d: ",
               slice_window->slice.current_paint_label );

        if( input_line( stdin, line, MAX_STRING_LENGTH ) == OK )
        {
            col = convert_string_to_colour( line );

            add_new_label( slice_window,
                           slice_window->slice.current_paint_label, col );

            set_slice_window_all_update( display->associated[SLICE_WINDOW] );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_label_colour )   /* ARGSUSED */
{
    return( OK );
}

private  save_labels_as_tags(
    display_struct  *display,
    display_struct  *slice_window,
    int             desired_label )
{
    Status         status;
    FILE           *file;
    STRING         filename;

    print( "Enter filename to save: " );
    if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
    {
        status = open_file_with_default_suffix( filename,
                         get_default_tag_file_suffix(),
                         WRITE_FILE, ASCII_FORMAT, &file );

        if( status == OK )
            status = output_labels_as_tags( file,
                      get_volume(slice_window),
                      get_label_volume(slice_window),
                      desired_label,
                      display->three_d.default_marker_size,
                      display->three_d.default_marker_patient_id );

        if( status == OK )
            status = close_file( file );

        print( "Done saving.\n" );
    }

    (void) input_newline( stdin );
}

public  DEF_MENU_FUNCTION( save_labels )   /* ARGSUSED */
{
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) )
        save_labels_as_tags( display, slice_window, -1 );

    return( OK );
}

public  DEF_MENU_UPDATE(save_labels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( save_current_label )   /* ARGSUSED */
{
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        if( slice_window->slice.current_paint_label > 0 )
        {
            save_labels_as_tags( display, slice_window,
                                 slice_window->slice.current_paint_label );
        }
        else
            print( "You first have to set the current label > 0.\n" );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(save_current_label )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( load_labels )   /* ARGSUSED */
{
    Status         status;
    BOOLEAN        landmark_format;
    FILE           *file;
    STRING         filename;
    display_struct *slice_window;
    Volume         volume;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename to load: " );
        if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            landmark_format = filename_extension_matches( filename,
                                 get_default_landmark_file_suffix() );

            status = open_file_with_default_suffix( filename,
                                get_default_tag_file_suffix(),
                                READ_FILE, ASCII_FORMAT, &file );

            if( status == OK )
            {
                if( landmark_format )
                    status = input_landmarks_as_labels( file, volume,
                                            get_label_volume(slice_window) );
                else
                    status = input_tags_as_labels( file, volume,
                                            get_label_volume(slice_window) );
            }

            if( status == OK )
                status = close_file( file );

            set_slice_window_all_update( slice_window );
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

private  void  copy_labels_from_adjacent_slice(
    display_struct   *display,
    int              src_offset )
{
    Real             real_dest_index[MAX_DIMENSIONS];
    int              src_index[N_DIMENSIONS], dest_index[MAX_DIMENSIONS];
    int              view_index, x_index, y_index, axis_index;
    display_struct   *slice_window;
    Volume           volume;

    if( get_voxel_under_mouse( display, real_dest_index, &view_index ) &&
        get_slice_window_volume( display, &volume) &&
        get_slice_window( display, &slice_window ) &&
        slice_has_ortho_axes( slice_window, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        convert_real_to_int_voxel( N_DIMENSIONS, real_dest_index, dest_index );

        src_index[X] = 0;
        src_index[Y] = 0;
        src_index[Z] = 0;
        src_index[axis_index] = dest_index[axis_index] + src_offset;

        if( int_voxel_is_within_volume( volume, src_index ) )
        {
            copy_labels_slice_to_slice(
                         volume,
                         get_label_volume(slice_window),
                         axis_index,
                         src_index[axis_index],
                         dest_index[axis_index],
                         slice_window->slice.segmenting.min_threshold,
                         slice_window->slice.segmenting.max_threshold );

            set_slice_window_all_update( slice_window );
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
    return( OK );
}

public  DEF_MENU_FUNCTION( copy_labels_from_higher_slice )   /* ARGSUSED */
{
    copy_labels_from_adjacent_slice( display, 1 );

    return( OK );
}

public  DEF_MENU_UPDATE(copy_labels_from_higher_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_display_labels )   /* ARGSUSED */
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.display_labels =!slice_window->slice.display_labels;
        set_slice_window_all_update( slice_window );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_display_labels )   /* ARGSUSED */
{
    STRING           text;
    BOOLEAN          display_labels;
    display_struct   *slice_window;

    if( !get_slice_window( display, &slice_window ) )
        display_labels = Initial_display_labels;
    else
        display_labels = slice_window->slice.display_labels;

    set_text_on_off( label, text, display_labels );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
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
            set_slice_window_all_update( slice_window );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(change_labels_in_range )   /* ARGSUSED */
{
    return( OK );
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
}

public  DEF_MENU_FUNCTION( calculate_volume )   /* ARGSUSED */
{
    display_struct  *slice_window;
    Status          status;
    Volume          volume;
    int             n_voxels;
    Real            cubic_millimetres;

    if( get_slice_window( display, &slice_window ) )
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
    return( OK );
}
