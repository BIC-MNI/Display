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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/edit_surface/edit.c,v 1.8 1995-07-31 19:54:00 david Exp $";
#endif

#include  <display.h>

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
    surface_edit->visible_colour = Visible_segmenting_colour;
    surface_edit->invisible_colour = Invisible_segmenting_colour;
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

public  BOOLEAN  get_edited_polygons(
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
