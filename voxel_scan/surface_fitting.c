#include  <def_arrays.h>
#include  <def_surface_fitting.h>
#include  <def_files.h>

const  Surface_representations  INITIAL_REPRESENTATION = SUPERQUADRIC;

public  Status  initialize_surface_fitting( surface_fitting )
    surface_fitting_struct  *surface_fitting;
{
    Status  status;
    Status  alloc_surface_fitting_parameters();

    status = OK;

    if( !lookup_surface_representation( INITIAL_REPRESENTATION,
                                   &surface_fitting->surface_representation ) )
    {
        PRINT( "Could not find representation.\n" );
        status = ERROR;
    }

    if( status == OK )
        status = alloc_surface_fitting_parameters( surface_fitting );

    return( status );
}

public  Status  alloc_surface_fitting_parameters( surface_fitting )
    surface_fitting_struct   *surface_fitting;
{
    int     i, n_parameters;
    Status  status;

    status = OK;

    if( surface_fitting->surface_representation->n_descriptors > 0 )
    {
        ALLOC( status, surface_fitting->descriptors,
               surface_fitting->surface_representation->n_descriptors );
    }

    if( status == OK )
    {
        surface_fitting->surface_representation->get_default_descriptors(
                         surface_fitting->descriptors );
    }

    if( status == OK )
    {
        n_parameters = surface_fitting->surface_representation->
                       get_num_parameters( surface_fitting->descriptors );

        ALLOC( status, surface_fitting->parameters, n_parameters );
    }

    if( status == OK )
        ALLOC( status, surface_fitting->max_parameter_deltas, n_parameters );

    if( status == OK )
        ALLOC( status, surface_fitting->parameter_deltas, n_parameters );

    if( status == OK )
    {
        for_less( i, 0, n_parameters )
        {
            surface_fitting->max_parameter_deltas[i] = 1.0;
            surface_fitting->parameter_deltas[i] = 0.25 *
                          surface_fitting->max_parameter_deltas[i];
        }
    }

    return( status );
}

public  Status  free_surface_fitting_parameters( surface_fitting )
    surface_fitting_struct  *surface_fitting;
{
    Status  status;

    FREE( status, surface_fitting->parameters );

    if( status == OK )
        FREE( status, surface_fitting->max_parameter_deltas );

    if( status == OK )
        FREE( status, surface_fitting->parameter_deltas );

    if( status == OK &&
        surface_fitting->surface_representation->n_descriptors > 0 )
    {
        FREE( status, surface_fitting->descriptors );
    }

    return( status );
}

public  Status  delete_surface_fitting( surface_fitting )
    surface_fitting_struct  *surface_fitting;
{
    Status  status;

    status = free_surface_fitting_parameters( surface_fitting );

    if( status == OK && surface_fitting->n_surface_points > 0 )
        FREE( status, surface_fitting->surface_points );

    return( status );
}

public  void  display_parameters( surface_fitting, parameters )
    surface_fitting_struct   *surface_fitting;
    double                   parameters[];
{
    int   i, n_parameters;

    n_parameters = surface_fitting->surface_representation->get_num_parameters(
                                 surface_fitting->descriptors );

    for_less( i, 0, n_parameters )
        PRINT( " %g", parameters[i] );
    PRINT( "\n" );
}

public  Status  add_surface_fitting_point( surface_fitting, point )
    surface_fitting_struct  *surface_fitting;
    Point                   *point;
{
    Status  status;

    ADD_ELEMENT_TO_ARRAY( status, surface_fitting->n_surface_points,
                          surface_fitting->surface_points, *point,
                          DEFAULT_CHUNK_SIZE );

    return( status );
}

public  Status  delete_surface_fitting_point( surface_fitting, point )
    surface_fitting_struct  *surface_fitting;
    Point                   *point;
{
    Status  status;
    int     index, i;

    status = OK;

    index = -1;

    for_less( i, 0, surface_fitting->n_surface_points )
    {
        if( EQUAL_POINTS(surface_fitting->surface_points[i],*point) )
        {
            index = i;
            break;
        }
    }

    if( index >= 0 )
    {
        DELETE_ELEMENT_FROM_ARRAY( status, surface_fitting->n_surface_points,
                                   surface_fitting->surface_points, index,
                                   DEFAULT_CHUNK_SIZE );
    }

    return( status );
}

public  Status  delete_surface_fitting_points( surface_fitting )
    surface_fitting_struct  *surface_fitting;
{
    Status  status;

    status = OK;

    if( surface_fitting->n_surface_points > 0 )
    {
        FREE( status, surface_fitting->surface_points );

        surface_fitting->n_surface_points = 0;
    }

    return( status );
}

public  Status  convert_to_new_surface_representation( surface_fitting,
                                                     new_rep, new_descriptors )
    surface_fitting_struct   *surface_fitting;
    surface_rep_struct       *new_rep;
    double                   new_descriptors[];
{
    Status              status;
    int                 i;
    surface_rep_struct  *prev_rep_struct;
    double              *prev_parameters, *prev_descriptors;

    prev_rep_struct = surface_fitting->surface_representation;
    prev_parameters = surface_fitting->parameters;
    prev_descriptors = surface_fitting->descriptors;

    surface_fitting->surface_representation = new_rep;

    status = alloc_surface_fitting_parameters( surface_fitting );

    for_less( i, 0, (int) new_rep->n_descriptors )
        surface_fitting->descriptors[i] = new_descriptors[i];

    if( status == OK )
    {
        surface_fitting->surface_representation->convert_from_representation(
                prev_rep_struct, prev_descriptors, prev_parameters,
                surface_fitting->descriptors, surface_fitting->parameters );
    }
    
    if( status == OK )
        FREE( status, prev_parameters );
    
    if( status == OK )
        FREE( status, prev_descriptors );

    return( status );
}
