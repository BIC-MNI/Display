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

static  DEF_EVENT_FUNCTION( check_update_contour );
static  VIO_BOOL  add_to_contour(
    display_struct   *display );
static  VIO_BOOL  contour_not_finished(
    display_struct   *display );
static  polygons_struct  *get_next_polygons(
    object_traverse_struct  *object_traverse );
static  void  advance_polygon(
    display_struct   *display );
static  void  make_cursor_contours(
    display_struct   *display );
static  void   initialize_contour(
    display_struct   *display );
static  void   start_new_axis(
    display_struct   *display );

  void  initialize_cursor_plane_outline(
    display_struct    *display )
{
    add_action_table_function( &display->action_table, NO_EVENT,
                            check_update_contour );

    display->three_d.cursor_contours.models_changed_id = -1;

    initialize_object_traverse(
         &display->three_d.cursor_contours.object_traverse,
         FALSE, 0, (object_struct **) NULL );

    make_cursor_contours( display );
}

  void  delete_cursor_plane_outline(
    display_struct    *display )
{
    if( display->three_d.cursor_contours.object_traverse.top_of_stack > 0 )
    {
        terminate_object_traverse(
                  &display->three_d.cursor_contours.object_traverse );
    }

}

static  Bitplane_types   get_cursor_contour_bitplane( void )
{
    if( Cursor_contour_overlay_flag && G_has_overlay_planes() )
        return( OVERLAY_PLANES );
    else
        return( NORMAL_PLANES );
}

static  void  make_cursor_contours(
    display_struct   *display )
{
    int             axis;
    VIO_Colour          col;
    object_struct   *object;
    lines_struct    *lines;
    model_struct    *model;

    if( get_cursor_contour_bitplane() == OVERLAY_PLANES )
        model = get_graphics_model( display, OVERLAY_MODEL );
    else
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

        display->three_d.cursor_contours.contours[axis].lines = lines;
        display->three_d.cursor_contours.contours[axis].n_points_alloced = 0;
        display->three_d.cursor_contours.contours[axis].n_indices_alloced = 0;
        display->three_d.cursor_contours.contours[axis].
                                    n_end_indices_alloced = 0;
    }
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( check_update_contour )
{
    VIO_Real     end_time;
    VIO_BOOL  found;

    end_time = current_realtime_seconds() + Max_background_seconds;
    
    if( display->three_d.cursor_contours.models_changed_id !=
        display->models_changed_id )
    {
        initialize_contour( display );

        display->three_d.cursor_contours.models_changed_id =
                                        display->models_changed_id;
    }
    else if( Show_cursor_contours )
    {
        found = FALSE;

        while( contour_not_finished( display ) &&
               current_realtime_seconds() < end_time )
        {
            if( add_to_contour( display ) )
                found = TRUE;
        }

        if( found )
            set_update_required( display, get_cursor_contour_bitplane() );
    }

    return( VIO_OK );
}

static  void   initialize_contour(
    display_struct   *display )
{
    int                     axis;
    cursor_contours_struct  *contours;

    contours = &display->three_d.cursor_contours;

    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        contours->contours[axis].lines->n_points = 0;
        contours->contours[axis].lines->n_items = 0;
    }

    contours->axis = 0;

    start_new_axis( display );
}

static  void   start_new_axis(
    display_struct   *display )
{
    cursor_contours_struct  *contours;

    contours = &display->three_d.cursor_contours;

    delete_cursor_plane_outline( display );

    initialize_object_traverse( &contours->object_traverse,
                                TRUE, N_MODELS, display->models );

    contours->current_polygons =
                    get_next_polygons( &contours->object_traverse );

    contours->poly_index = 0;
}

static  polygons_struct  *get_next_polygons(
    object_traverse_struct  *object_traverse )
{
    object_struct    *object;
    polygons_struct  *polygons;

    polygons = (polygons_struct *) 0;

    while( get_next_object_traverse( object_traverse, &object ) )
    {
        if( object->object_type == POLYGONS )
        {
            polygons = get_polygons_ptr( object );
            if( polygons->n_items > 0 )
                break;
        }
    }

    return( polygons );
}

static  VIO_BOOL  contour_not_finished(
    display_struct   *display )
{
    return( display->three_d.cursor_contours.current_polygons !=
            (polygons_struct *) 0 );
}

static  VIO_BOOL  add_to_contour(
    display_struct   *display )
{
    VIO_BOOL                 found;
    VIO_Vector                  plane_normal;
    VIO_Real                    plane_constant;
    int                     axis;
    cursor_contours_struct  *contours;

    contours = &display->three_d.cursor_contours;
    axis = contours->axis;

    fill_Vector( plane_normal, 0.0, 0.0, 0.0 );
    Vector_coord(plane_normal,axis) = 1.0f;

    plane_constant = (VIO_Real) Point_coord( display->three_d.cursor.origin, axis );

    found = intersect_plane_one_polygon( &plane_normal,
                              plane_constant,
                              contours->current_polygons,
                              contours->poly_index,
                              contours->contours[axis].lines,
                              &contours->contours[axis].n_points_alloced,
                              &contours->contours[axis].n_indices_alloced,
                              &contours->contours[axis].n_end_indices_alloced );

    advance_polygon( display );

    return( found );
}

static  void  advance_polygon(
    display_struct   *display )
{
    cursor_contours_struct  *contours;

    contours = &display->three_d.cursor_contours;

    ++contours->poly_index;

    if( contours->poly_index >= contours->current_polygons->n_items )
    {
        contours->current_polygons =
                        get_next_polygons( &contours->object_traverse );
        contours->poly_index = 0;

        if( contours->current_polygons == (polygons_struct *) 0 )
        {
            ++contours->axis;

            if( contours->axis < VIO_N_DIMENSIONS )
                start_new_axis( display );
        }
    }
}
