 
#include  <def_display.h>

public  DEF_MENU_FUNCTION( start_deforming_object )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(start_deforming_object )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( start_annealing_deforming_object )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(start_annealing_deforming_object )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_annealing_parameters )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(set_annealing_parameters )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( stop_deforming_object )   /* ARGSUSED */
{
    turn_off_deformation( display );

    return( OK );
}

public  DEF_MENU_UPDATE(stop_deforming_object )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_deformation_boundary )   /* ARGSUSED */
{
    Real   threshold;
    char   ch;

    print( "Current boundary threshold: %g\n",
          display->three_d.deform.deform.boundary_definition.min_isovalue );
    print( "Enter new value and boundary_dir[+,-,or none]: " );

    if( input_real( stdin, &threshold ) == OK &&
        input_nonwhite_character( stdin, &ch ) == OK )
    {
        display->three_d.deform.deform.boundary_definition.min_isovalue =
               threshold;
        display->three_d.deform.deform.boundary_definition.max_isovalue =
               threshold;
        switch( ch )
        {
        case '-':  
            display->three_d.deform.deform.boundary_definition.normal_direction=
                   TOWARDS_LOWER;
            break;
        case '+':  
            display->three_d.deform.deform.boundary_definition.normal_direction=
                   TOWARDS_HIGHER;
            break;
        default:  
            display->three_d.deform.deform.boundary_definition.normal_direction=
                   ANY_DIRECTION;
            break;
        }
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_deformation_boundary )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_deformation_model )   /* ARGSUSED */
{
    Real           model_weight, min_curvature, max_curvature;
    Real           max_position_offset;
    int            n_points;
    Point          *points;
    String         model_name, position_filename;
    Status         status;
    object_struct  *object;

    status = OK;
    print("Enter new model_weight, deformation_model, min and max curvature,");
    print("      and none|original_position_filename max_offset: " );

    if( input_real( stdin, &model_weight ) == OK &&
        input_string( stdin, model_name, MAX_STRING_LENGTH, ' ' ) == OK &&
        input_real( stdin, &min_curvature ) == OK &&
        input_real( stdin, &max_curvature ) == OK &&
        input_string( stdin, position_filename, MAX_STRING_LENGTH, ' ' ) == OK &&
        input_real( stdin, &max_position_offset ) == OK )
    {
        delete_deformation_parameters( &display->three_d.deform.deform );

        display->three_d.deform.deform.model_weight = model_weight;
        display->three_d.deform.deform.deformation_model.min_curvature_offset =
                               min_curvature;
        display->three_d.deform.deform.deformation_model.max_curvature_offset =
                               max_curvature;
        display->three_d.deform.deform.deformation_model.max_position_offset =
                               max_position_offset;

        if( get_current_object( display, &object ) &&
            (object->object_type == LINES || object->object_type == POLYGONS) )
        {
            status = get_deformation_model( model_name, object,
                           &display->three_d.deform.deform.deformation_model );
        }
        else
        {
            print( "Set the current object to be deformed.\n" );
            status = ERROR;
        }
    }

    if( status == OK )
    {
        if( strcmp( position_filename, "none" ) == 0 )
            display->three_d.deform.deform.deformation_model.
                     position_constrained = FALSE;
        else
        {
            n_points = get_object_points( object, &points );
            status = input_original_positions( &display->three_d.deform.
                          deform.deformation_model, position_filename,
                          n_points );
        }
    }

    (void) input_newline( stdin );

    return( status );
}

public  DEF_MENU_UPDATE(set_deformation_model )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_deformation_parameters )   /* ARGSUSED */
{
    Real           fractional_step, max_step, max_search_distance;
    int            degrees_continuity;

    print("Enter fractional_step, max_step, max_search_distance,\n" );
    print("      degrees_continuity: " );

    if( input_real( stdin, &fractional_step ) == OK &&
        input_real( stdin, &max_step ) == OK &&
        input_real( stdin, &max_search_distance ) == OK &&
        input_int( stdin, &degrees_continuity ) == OK )
    {
        display->three_d.deform.deform.fractional_step = fractional_step;
        display->three_d.deform.deform.max_step = max_step;
        display->three_d.deform.deform.max_search_distance =
                                                  max_search_distance;
        display->three_d.deform.deform.degrees_continuity = degrees_continuity;
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_deformation_parameters )   /* ARGSUSED */
{
    return( OK );
}
