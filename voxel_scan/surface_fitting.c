#include  <def_graphics.h>

const  Surface_representations  INITIAL_REPRESENTATION = SUPERQUADRIC;

public  Status  initialize_surface_fitting( surface_fitting )
    surface_fitting_struct  *surface_fitting;
{
    Status  status;

    surface_fitting->n_parameters = get_num_parameters();

    ALLOC1( status, surface_fitting->parameters, surface_fitting->n_parameters, 
            double );

    if( !lookup_surface_representation( INITIAL_REPRESENTATION,
                                   &surface_fitting->surface_representation ) )
    {
        PRINT( "Could not find representation.\n" );
        status = ERROR;
    }

    return( status );
}

public  Status  delete_surface_fitting( surface_fitting )
    surface_fitting_struct  *surface_fitting;
{
    Status  status;

    FREE1( status, surface_fitting->parameters );

    if( status == OK && surface_fitting->n_surface_points > 0 )
        FREE1( status, surface_fitting->surface_points );

    return( status );
}

public  void  display_parameters( parameters )
    double   parameters[];
{
    int   i, n_parameters;

    n_parameters = get_num_parameters();

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
                          Point, DEFAULT_CHUNK_SIZE );

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
                              Point, DEFAULT_CHUNK_SIZE );
    }

    return( status );
}
