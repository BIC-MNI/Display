#include  <def_display.h>

public  void  initialize_surface_edit(
    surface_edit_struct   *surface_edit )
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

public  void  delete_surface_edit(
    surface_edit_struct   *surface_edit )
{
    if( surface_edit->n_vertices_alloced > 0 )
    {
        FREE( surface_edit->vertices );
        surface_edit->n_vertices = 0;
        surface_edit->n_vertices_alloced = 0;
    }

    if( surface_edit->n_undos_alloced > 0 )
    {
        FREE( surface_edit->undo_indices );
        surface_edit->n_undos = 0;
        surface_edit->n_undos_alloced = 0;
    }

    if( surface_edit->n_invisible_alloced > 0 )
    {
        FREE( surface_edit->invisible_indices );
        surface_edit->n_invisible = 0;
        surface_edit->n_invisible_alloced = 0;
    }
}

public  void  set_edited_polygons(
    surface_edit_struct   *surface_edit,
    polygons_struct       *polygons )
{
    surface_edit->polygons = polygons;
    surface_edit->polygons_set = TRUE;

    check_polygons_neighbours_computed( surface_edit->polygons );

    create_polygons_visibilities( surface_edit->polygons );
}

public  Boolean  get_edited_polygons(
    surface_edit_struct   *surface_edit,
    polygons_struct       **polygons )
{
    if( surface_edit->polygons_set )
        *polygons = surface_edit->polygons;

    return( surface_edit->polygons_set );
}

public  void  reset_edited_polygons(
    surface_edit_struct   *surface_edit )
{
    polygons_struct  *polygons;
    int              i;

    if( get_edited_polygons( surface_edit, &polygons ) )
    {
        for_less( i, 0, polygons->n_items )
            polygons->visibilities[i] = TRUE;
    }
}
