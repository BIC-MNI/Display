#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_surface_edit( surface_edit )
    surface_edit_struct   *surface_edit;
{
    surface_edit->n_paint_polygons = N_painting_polygons;

    surface_edit->polygons_set = FALSE;
    surface_edit->n_vertices = 0;
    surface_edit->n_vertices_alloced = 0;
    surface_edit->n_undos = 0;
    surface_edit->n_undos_alloced = 0;
    surface_edit->n_invisible = 0;
    surface_edit->n_invisible_alloced = 0;
}

public  Status  delete_surface_edit( surface_edit )
    surface_edit_struct   *surface_edit;
{
    Status   status;

    status = OK;

    if( surface_edit->n_vertices_alloced > 0 )
    {
        FREE1( status, surface_edit->vertices );
        surface_edit->n_vertices = 0;
        surface_edit->n_vertices_alloced = 0;
    }

    if( surface_edit->n_undos_alloced > 0 )
    {
        FREE1( status, surface_edit->undo_indices );
        surface_edit->n_undos = 0;
        surface_edit->n_undos_alloced = 0;
    }

    if( surface_edit->n_invisible_alloced > 0 )
    {
        FREE1( status, surface_edit->invisible_indices );
        surface_edit->n_invisible = 0;
        surface_edit->n_invisible_alloced = 0;
    }

    return( status );
}

public  void  set_edited_polygons( surface_edit, polygons )
    surface_edit_struct   *surface_edit;
    polygons_struct       *polygons;
{
    Status   create_polygon_neighbours();
    Status   create_polygons_visibilities();

    surface_edit->polygons = polygons;
    surface_edit->polygons_set = TRUE;

    if( surface_edit->polygons->neighbours == (int *) 0 )
    {
        (void) create_polygon_neighbours( surface_edit->polygons->n_items,
                                          surface_edit->polygons->indices,
                                          surface_edit->polygons->end_indices,
                                          &surface_edit->polygons->neighbours );
    }

    if( surface_edit->polygons->visibilities == (Smallest_int *) 0 )
    {
        (void) create_polygons_visibilities( surface_edit->polygons );
    }
}

public  Boolean  get_edited_polygons( surface_edit, polygons )
    surface_edit_struct   *surface_edit;
    polygons_struct       **polygons;
{
    if( surface_edit->polygons_set )
    {
        *polygons = surface_edit->polygons;
    }

    return( surface_edit->polygons_set );
}

public  void  reset_edited_polygons( surface_edit )
    surface_edit_struct   *surface_edit;
{
    polygons_struct  *polygons;
    int              i;

    if( get_edited_polygons( surface_edit, &polygons ) )
    {
        for_less( i, 0, polygons->n_items )
        {
            polygons->visibilities[i] = TRUE;
        }
    }
}
