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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/deform.c,v 1.16 1995-10-19 15:50:55 david Exp $";
#endif

 
#include  <display.h>

/* ARGSUSED */

public  DEF_MENU_FUNCTION( start_deforming_object )
{
    Volume            volume;
    object_struct     *object;

    if( get_slice_window_volume( display, &volume ) &&
        get_current_object( display, &object ) )
    {
        turn_on_deformation( display, object, FALSE );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(start_deforming_object )
{
    return( get_n_volumes(display) > 0 &&
            (current_object_is_this_type( display, LINES ) ||
             current_object_is_this_type( display, POLYGONS )) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( start_annealing_deforming_object )
{
    Volume            volume;
    object_struct     *object;

    if( get_slice_window_volume( display, &volume ) &&
        get_current_object( display, &object ) )
    {
        turn_on_deformation( display, object, TRUE );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(start_annealing_deforming_object )
{
    return( get_n_volumes(display) > 0 &&
            (current_object_is_this_type( display, LINES ) ||
             current_object_is_this_type( display, POLYGONS )) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_annealing_parameters )
{
    Real              fifty_percent_threshold, temperature_factor;
    Real              max_trans, max_rotate, max_scale_offset;
    int               min_n_to_move, max_n_to_move, stop_criteria;

    print( "Enter 50%% thresh, temp factor, min_n_to_move, max_n_to_move,\n" );
    print( "      max_trans, max_rotate, max_scale, stop_criteria: " );

    if( input_real( stdin, &fifty_percent_threshold ) == OK &&
        input_real( stdin, &temperature_factor ) == OK &&
        input_int( stdin, &min_n_to_move ) == OK &&
        input_int( stdin, &max_n_to_move ) == OK &&
        input_real( stdin, &max_trans ) == OK &&
        input_real( stdin, &max_rotate ) == OK &&
        input_real( stdin, &max_scale_offset ) == OK &&
        input_int( stdin, &stop_criteria ) == OK )
    {
        display->three_d.deform.anneal.temperature = fifty_percent_threshold /
                                                           log( 2.0 );
        display->three_d.deform.anneal.temperature_factor = temperature_factor;
        display->three_d.deform.anneal.min_n_to_move = min_n_to_move;
        display->three_d.deform.anneal.max_n_to_move = max_n_to_move;
        display->three_d.deform.anneal.max_translation = max_trans;
        display->three_d.deform.anneal.max_angle_rotation = max_rotate;
        display->three_d.deform.anneal.max_scale_offset = max_scale_offset;
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_annealing_parameters )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( stop_deforming_object )
{
    turn_off_deformation( display );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(stop_deforming_object )
{
    return( display->three_d.deform.in_progress );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_deformation_boundary )
{
    Real   min_val, max_val, grad_threshold, angle, tolerance;
    char   ch;

    print( "Current boundary: min %g max %g  grad_mag %g  min_dot %g max_dot %g tol %g\n",
          display->three_d.deform.deform.boundary_definition.min_isovalue,
          display->three_d.deform.deform.boundary_definition.max_isovalue,
          display->three_d.deform.deform.boundary_definition.gradient_threshold,
          display->three_d.deform.deform.boundary_definition.min_dot_product,
          display->three_d.deform.deform.boundary_definition.max_dot_product,
          display->three_d.deform.deform.boundary_definition.tolerance );

    print( "Enter min_val, max_val, grad_mag_threshold, angle, [+,-,or none] tolerance: " );

    if( input_real( stdin, &min_val ) == OK &&
        input_real( stdin, &max_val ) == OK &&
        input_real( stdin, &grad_threshold ) == OK &&
        input_real( stdin, &angle ) == OK &&
        input_nonwhite_character( stdin, &ch ) == OK &&
        input_real( stdin, &tolerance ) == OK )
    {
        set_boundary_definition( &display->three_d.deform.deform.
                                          boundary_definition,
                                 min_val, max_val, grad_threshold, angle, ch,
                                 tolerance );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_deformation_boundary )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_deformation_model )
{
    Real           model_weight, min_curvature, max_curvature;
    int            up_to_n_points;
    STRING         model_name;
    Status         status;

    status = OK;
    print("Enter up_to_n_points, model_weight, deformation_model, \n" );
    print("      min and max curvature: ");

    model_name = NULL;

    if( input_int( stdin, &up_to_n_points ) == OK &&
        input_real( stdin, &model_weight ) == OK &&
        input_string( stdin, &model_name, ' ' ) == OK &&
        input_real( stdin, &min_curvature ) == OK &&
        input_real( stdin, &max_curvature ) == OK )
    {
        status = add_deformation_model(
                           &display->three_d.deform.deform.deformation_model,
                           up_to_n_points, model_weight, model_name,
                           min_curvature, max_curvature );
    }

    delete_string( model_name );

    (void) input_newline( stdin );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_deformation_model )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_deformation_original_positions )
{
    Real           max_position_offset;
    int            n_points;
    Point          *points;
    STRING         position_filename;
    Status         status;
    object_struct  *object;

    status = OK;
    print("Enter none|original_position_filename max_offset: " );

    if( input_string( stdin, &position_filename, ' ' ) == OK &&
        input_real( stdin, &max_position_offset ) == OK )
    {
        if( get_current_object( display, &object ) &&
            (object->object_type == LINES || object->object_type == POLYGONS) )
        {
            n_points = get_object_points( object, &points );

            status = input_original_positions( &display->three_d.deform.
                                           deform.deformation_model,
                                           position_filename,
                                           max_position_offset,
                                           n_points );
        }
        else
        {
            print( "Set the current object to be deformed.\n" );
            status = ERROR;
        }
    }

    (void) input_newline( stdin );

    delete_string( position_filename );

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_deformation_original_positions )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_deformation_parameters )
{
    Real           fractional_step, max_step, max_search_distance;
    Real           movement_threshold;
    int            degrees_continuity;

    print("Enter fractional_step, max_step, max_search_distance,\n" );
    print("      degrees_continuity, movement_threshold: " );

    if( input_real( stdin, &fractional_step ) == OK &&
        input_real( stdin, &max_step ) == OK &&
        input_real( stdin, &max_search_distance ) == OK &&
        input_int( stdin, &degrees_continuity ) == OK &&
        input_real( stdin, &movement_threshold ) == OK )
    {
        display->three_d.deform.deform.fractional_step = fractional_step;
        display->three_d.deform.deform.max_step = max_step;
        display->three_d.deform.deform.max_search_distance =
                                                  max_search_distance;
        display->three_d.deform.deform.degrees_continuity = degrees_continuity;
        display->three_d.deform.deform.movement_threshold = movement_threshold;
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_deformation_parameters )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( reset_deformation_model )
{
    delete_deformation_model(&display->three_d.deform.deform.deformation_model);
    initialize_deformation_model(
                  &display->three_d.deform.deform.deformation_model );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(reset_deformation_model )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( show_deformation_model )
{
    print_deformation_model( &display->three_d.deform.deform.deformation_model);

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(show_deformation_model )
{
    return( TRUE );
}
