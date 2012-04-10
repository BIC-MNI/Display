#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

const  Surface_representations  INITIAL_REPRESENTATION = SUPERQUADRIC;

private  void  alloc_surface_fitting_parameters(
    surface_fitting_struct   *surface_fitting,
    double                   descriptors[] );

public  void  initialize_surface_fitting(
    surface_fitting_struct  *surface_fitting )
{
    if( !lookup_surface_representation( INITIAL_REPRESENTATION,
                                   &surface_fitting->surface_representation ) )
    {
        print( "Could not find representation.\n" );
        return;
    }

    alloc_surface_fitting_parameters( surface_fitting, (double *) 0 );

    surface_fitting->n_surface_points = 0;
}

private  void  alloc_surface_fitting_parameters(
    surface_fitting_struct   *surface_fitting,
    double                   descriptors[] )
{
    int     i, n_parameters;

    if( surface_fitting->surface_representation->n_descriptors > 0 )
    {
        ALLOC( surface_fitting->descriptors,
               surface_fitting->surface_representation->n_descriptors );
    }

    if( descriptors == (double *) 0 )
    {
        surface_fitting->surface_representation->get_default_descriptors(
                         surface_fitting->descriptors );
    }
    else
    {
        for_less( i, 0, surface_fitting->surface_representation->
                        n_descriptors )
            surface_fitting->descriptors[i] = descriptors[i];
    }

    n_parameters = surface_fitting->surface_representation->
                   get_num_parameters( surface_fitting->descriptors );

    ALLOC( surface_fitting->parameters, n_parameters );
    ALLOC( surface_fitting->max_parameter_deltas, n_parameters );
    ALLOC( surface_fitting->parameter_deltas, n_parameters );

    for_less( i, 0, n_parameters )
    {
        surface_fitting->max_parameter_deltas[i] = 1.0;
        surface_fitting->parameter_deltas[i] = 0.25 *
                      surface_fitting->max_parameter_deltas[i];
    }
}

public  void  free_surface_fitting_parameters(
    surface_fitting_struct  *surface_fitting )
{
    FREE( surface_fitting->parameters );
    FREE( surface_fitting->max_parameter_deltas );
    FREE( surface_fitting->parameter_deltas );

    if( surface_fitting->surface_representation->n_descriptors > 0 )
    {
        FREE( surface_fitting->descriptors );
    }
}

public  void  delete_surface_fitting(
    surface_fitting_struct  *surface_fitting )
{
    free_surface_fitting_parameters( surface_fitting );

    delete_surface_fitting_points( surface_fitting );
}

public  void  display_parameters(
    surface_fitting_struct   *surface_fitting,
    double                   parameters[] )
{
    int   i, n_parameters;

    n_parameters = surface_fitting->surface_representation->get_num_parameters(
                                 surface_fitting->descriptors );

    for_less( i, 0, n_parameters )
        print( " %g", parameters[i] );
    print( "\n" );
}

public  void  add_surface_fitting_point(
    surface_fitting_struct  *surface_fitting,
    Point                   *point )
{
    ADD_ELEMENT_TO_ARRAY( surface_fitting->surface_points,
                          surface_fitting->n_surface_points, *point,
                          DEFAULT_CHUNK_SIZE );
}

public  void  delete_surface_fitting_point(
    surface_fitting_struct  *surface_fitting,
    Point                   *point )
{
    int     index, i;

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
        DELETE_ELEMENT_FROM_ARRAY( surface_fitting->surface_points,
                                   surface_fitting->n_surface_points, index,
                                   DEFAULT_CHUNK_SIZE );
    }
}

public  void  delete_surface_fitting_points(
    surface_fitting_struct  *surface_fitting )
{
    if( surface_fitting->n_surface_points > 0 )
    {
        FREE( surface_fitting->surface_points );

        surface_fitting->n_surface_points = 0;
    }
}

public  void  convert_to_new_surface_representation(
    surface_fitting_struct   *surface_fitting,
    surface_rep_struct       *new_rep,
    double                   new_descriptors[] )
{
    surface_fitting_struct  prev_surface_fitting;

    prev_surface_fitting = *surface_fitting;

    surface_fitting->surface_representation = new_rep;

    alloc_surface_fitting_parameters( surface_fitting, new_descriptors );

    surface_fitting->surface_representation->convert_from_representation(
                prev_surface_fitting.surface_representation,
                prev_surface_fitting.descriptors,
                prev_surface_fitting.parameters,
                surface_fitting->descriptors, surface_fitting->parameters );
    
    free_surface_fitting_parameters( &prev_surface_fitting );
}
