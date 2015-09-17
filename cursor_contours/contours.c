/**
 * \file cursor_contours/contours.c
 * \brief Show current slice position as contours on the 3D model.
 *
 * The functions in this file normally do nothing visible, but will
 * superimpose the intersection of each slice view as a contour on the
 * 3D model if the Show_cursor_contours variable is set.

 \copyright
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
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>
#include  <assert.h>

static DEF_EVENT_FUNCTION( check_update_contour );
static void make_cursor_contours( display_struct   *display );
static void initialize_contour( display_struct   *display );

/**
 * Install the handler for the contour updates, and initialize some
 * of the data structures needed for the cursor plane outline
 * display.
 *
 * \param display The display_struct of the 3D view window.
 */
void
initialize_cursor_plane_outline( display_struct *display )
{
    assert( display->window_type == THREE_D_WINDOW );

    add_action_table_function( &display->action_table, NO_EVENT,
                               check_update_contour );

    display->three_d.cursor_contours.models_changed_id = -1;

    make_cursor_contours( display );
}

static void
make_cursor_contours( display_struct   *display )
{
    int             axis;
    VIO_Colour      col;
    object_struct   *object;
    lines_struct    *lines;
    model_struct    *model;
    cursor_contours_struct  *contours;

    contours = &display->three_d.cursor_contours;

    model = get_graphics_model( display, MISCELLANEOUS_MODEL );

    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        object = create_object( LINES );
        lines = get_lines_ptr( object );
        switch( axis )
        {
        case VIO_X:  col = RED;    break;
        case VIO_Y:  col = GREEN;  break;
        case VIO_Z:  col = BLUE;   break;
        }
        initialize_lines( lines, col );
        lines->line_thickness = (float) Cursor_contour_thickness;

        add_object_to_model( model, object );

        contours->contours[axis].lines = lines;
        contours->contours[axis].n_points_alloced = 0;
        contours->contours[axis].n_indices_alloced = 0;
        contours->contours[axis].n_end_indices_alloced = 0;
    }
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( check_update_contour )
{
    VIO_BOOL               found;
    object_traverse_struct object_traverse;
    object_struct          *obj_ptr;
    cursor_contours_struct *ccs_ptr;
    contour_struct         *contours;
    VIO_Point              origin;

    assert( display->window_type == THREE_D_WINDOW );

    ccs_ptr = &display->three_d.cursor_contours;
    contours = ccs_ptr->contours;

    if( ccs_ptr->models_changed_id == display->models_changed_id ||
        !Show_cursor_contours )
    {
        return VIO_OK;
    }

    ccs_ptr->models_changed_id = display->models_changed_id;

    initialize_contour( display );

    get_cursor_origin(display, &origin);

    found = FALSE;

    initialize_object_traverse( &object_traverse, TRUE, N_MODELS, 
                                display->models );

    while( get_next_object_traverse( &object_traverse, &obj_ptr ) )
    {
        polygons_struct *poly_ptr;

        if( obj_ptr->object_type == POLYGONS &&
            (poly_ptr = get_polygons_ptr( obj_ptr )) != NULL &&
            poly_ptr->n_items > 0)
        {
            int  axis;

            for_less ( axis, 0, VIO_N_DIMENSIONS )
            {
                VIO_Real   plane_constant = Point_coord( origin, axis );
                VIO_Vector plane_normal;
                int        poly_index;

                fill_Vector( plane_normal, 0.0, 0.0, 0.0 );
                Vector_coord( plane_normal, axis) = 1.0;

                for_less (poly_index, 0, poly_ptr->n_items )
                {
                    if (intersect_plane_one_polygon( &plane_normal,
                                  plane_constant,
                                  poly_ptr,
                                  poly_index,
                                  contours[axis].lines,
                                  &contours[axis].n_points_alloced,
                                  &contours[axis].n_indices_alloced,
                                  &contours[axis].n_end_indices_alloced ))
                    {
                        found = TRUE;
                    }
                }
            }
        }
    }

    if( found )
        set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

static  void
initialize_contour( display_struct *display )
{
    int             axis;
    contour_struct  *contours;

    contours = display->three_d.cursor_contours.contours;

    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        contours[axis].lines->n_points = 0;
        contours[axis].lines->n_items = 0;
    }
}
