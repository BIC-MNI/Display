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

public  DEF_MENU_FUNCTION( transform_current_volume )
{
    display_struct    *slice_window;
    VIO_STR            filename;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter transform filename: " );

        if( input_string( stdin, &filename, ' ' ) == OK )
        {
            transform_current_volume_from_file( slice_window, filename );
        }

        (void) input_newline( stdin );

        delete_string( filename );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(transform_current_volume )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( reset_volume_transform )
{
    display_struct    *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        reset_current_volume_transform( slice_window );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(reset_volume_transform )
{
    return( get_n_volumes(display) > 0 );
}

private  void  linear_transform_volume(
    display_struct  *slice_window,
    VIO_Transform       *transform )
{
    VIO_General_transform   gen_transform;

    create_linear_transform( &gen_transform, transform );

    concat_transform_to_volume( slice_window,
                                get_current_volume_index(slice_window),
                                &gen_transform );

    delete_general_transform( &gen_transform );
}

private  void  translate_current_volume(
    display_struct  *display,
    int             axis,
    int             dir )
{
    display_struct  *slice_window;
    VIO_Transform       linear_transform;
    VIO_Real            trans[VIO_N_DIMENSIONS];

    if( !get_slice_window( display, &slice_window ) )
        return;

    trans[X] = 0.0;
    trans[Y] = 0.0;
    trans[Z] = 0.0;
    trans[axis] = slice_window->slice.volume_translation_step * (VIO_Real) dir;

    make_translation_transform( trans[X], trans[Y], trans[Z],
                                &linear_transform );

    linear_transform_volume( slice_window, &linear_transform );
}

private  void  rotate_current_volume(
    display_struct  *display,
    int             axis,
    int             dir )
{
    display_struct  *slice_window;
    VIO_Transform       translation, linear_transform;
    VIO_Real            angle;
    VIO_Point           origin;

    if( !get_slice_window( display, &slice_window ) )
        return;

    angle = (VIO_Real) dir * slice_window->slice.volume_rotation_step;

    make_rotation_transform( angle * DEG_TO_RAD, axis, &translation );

    get_cursor_origin( display, &origin );

    make_transform_relative_to_point( &origin, &translation,
                                      &linear_transform );

    linear_transform_volume( slice_window, &linear_transform );
}

private  void  scale_current_volume(
    display_struct  *display,
    int             dir )
{
    display_struct  *slice_window;
    VIO_Transform       scale_trans, linear_transform;
    VIO_Real            scale;
    VIO_Point           origin;

    if( !get_slice_window( display, &slice_window ) )
        return;

    if( dir < 0 )
        scale = 1.0 / slice_window->slice.volume_scale_step;
    else
        scale = slice_window->slice.volume_scale_step;

    make_scale_transform( scale, scale, scale, &scale_trans );

    get_cursor_origin( display, &origin );

    make_transform_relative_to_point( &origin, &scale_trans,
                                      &linear_transform );

    linear_transform_volume( slice_window, &linear_transform );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( translate_volume_plus_x)
{
    translate_current_volume( display, X, +1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(translate_volume_plus_x )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( translate_volume_minus_x)
{
    translate_current_volume( display, X, -1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(translate_volume_minus_x )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( translate_volume_plus_y)
{
    translate_current_volume( display, Y, +1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(translate_volume_plus_y )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( translate_volume_minus_y)
{
    translate_current_volume( display, Y, -1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(translate_volume_minus_y )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( translate_volume_plus_z)
{
    translate_current_volume( display, Z, +1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(translate_volume_plus_z )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( translate_volume_minus_z)
{
    translate_current_volume( display, Z, -1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(translate_volume_minus_z )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( magnify_volume)
{
    scale_current_volume( display, +1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(magnify_volume )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( shrink_volume)
{
    scale_current_volume( display, -1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(shrink_volume )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( rotate_volume_plus_x)
{
    rotate_current_volume( display, X, +1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(rotate_volume_plus_x )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( rotate_volume_minus_x)
{
    rotate_current_volume( display, X, -1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(rotate_volume_minus_x )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( rotate_volume_plus_y)
{
    rotate_current_volume( display, Y, +1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(rotate_volume_plus_y )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( rotate_volume_minus_y)
{
    rotate_current_volume( display, Y, -1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(rotate_volume_minus_y )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( rotate_volume_plus_z)
{
    rotate_current_volume( display, Z, +1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(rotate_volume_plus_z )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( rotate_volume_minus_z)
{
    rotate_current_volume( display, Z, -1 );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(rotate_volume_minus_z )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_volume_rotation_step)
{
    VIO_Real            new_step;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Current rotation step: %g\n",
               slice_window->slice.volume_rotation_step );
        print( "Enter rotation step: " );
        if( input_real( stdin, &new_step ) == OK )
            slice_window->slice.volume_rotation_step = new_step;

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_volume_rotation_step )
{
    VIO_Real            step;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
        step = slice_window->slice.volume_rotation_step;
    else
        step = Initial_volume_rotation_step;

    set_menu_text_real( menu_window, menu_entry, step );

    return( TRUE );

}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_volume_scale_step)
{
    VIO_Real            new_step;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Current scale step: %g\n",
               slice_window->slice.volume_scale_step );
        print( "Enter scale step: " );
        if( input_real( stdin, &new_step ) == OK )
            slice_window->slice.volume_scale_step = new_step;

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_volume_scale_step )
{
    VIO_Real            step;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
        step = slice_window->slice.volume_scale_step;
    else
        step = Initial_volume_scale_step;

    set_menu_text_real( menu_window, menu_entry, step );

    return( TRUE );

}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_volume_translation_step)
{
    VIO_Real            new_step;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Current translation step: %g\n",
               slice_window->slice.volume_translation_step );
        print( "Enter translation step: " );
        if( input_real( stdin, &new_step ) == OK )
            slice_window->slice.volume_translation_step = new_step;

        (void) input_newline( stdin );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_volume_translation_step )
{
    VIO_Real            step;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
        step = slice_window->slice.volume_translation_step;
    else
        step = Initial_volume_translation_step;

    set_menu_text_real( menu_window, menu_entry, step );

    return( TRUE );

}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( save_current_volume_transform )
{
    VIO_Status             status;
    display_struct     *slice_window;
    VIO_Volume             volume;
    VIO_STR             filename, comments;
    VIO_General_transform  *original_transform, *volume_transform;
    VIO_General_transform  incremental_transform, inverse;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        print( "Enter output filename: " );

        status = input_string( stdin, &filename, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
        {
            original_transform = &slice_window->slice.volumes
                   [get_current_volume_index(slice_window)].original_transform;

            volume_transform = get_voxel_to_world_transform( volume );

            create_inverse_general_transform( original_transform, &inverse );

            concat_general_transforms( &inverse, volume_transform,
                                       &incremental_transform );

            comments = create_string( "Transform created by Display" );

            if( check_clobber_file_default_suffix( filename,
                                     get_default_transform_file_suffix() ) )
            {
                (void) output_transform_file( filename, comments,
                                              &incremental_transform );
            }

            delete_string( comments );
            delete_general_transform( &inverse );
            delete_general_transform( &incremental_transform );
        }

        delete_string( filename );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(save_current_volume_transform )
{
    return( get_n_volumes(display) > 0 );
}

