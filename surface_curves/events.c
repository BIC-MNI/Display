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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_curves/events.c,v 1.20 1995-10-19 15:52:34 david Exp $";
#endif


#include  <display.h>

static    DEF_EVENT_FUNCTION( pick_point );

private  Bitplane_types   get_surface_curve_bitplane()
{
    if( Surface_curve_overlay_flag && G_has_overlay_planes() )
        return( OVERLAY_PLANES );
    else
        return( NORMAL_PLANES );
}

public  void  initialize_surface_curve(
    display_struct     *display )
{
    object_struct  *object;
    lines_struct   *lines;
    model_struct   *model;

    if( get_surface_curve_bitplane() == OVERLAY_PLANES )
        model = get_graphics_model( display, OVERLAY_MODEL );
    else
        model = get_graphics_model( display, MISCELLANEOUS_MODEL );

    object = create_object( LINES );
    lines = get_lines_ptr( object );
    initialize_lines( lines, Surface_curve_colour );
    lines->line_thickness = (float) Surface_curve_thickness;

    add_object_to_model( model, object );

    display->three_d.surface_curve.lines = lines;
    display->three_d.surface_curve.line_curvature_weight =
                                           Line_curvature_weight;
    display->three_d.surface_curve.min_curvature = Min_surface_curve_curvature;
    display->three_d.surface_curve.max_curvature = Max_surface_curve_curvature;
    display->three_d.surface_curve.n_points_alloced = 0;
    display->three_d.surface_curve.n_indices_alloced = 0;
    display->three_d.surface_curve.n_end_indices_alloced = 0;
    display->three_d.surface_curve.picking_points = FALSE;
}

public  void  start_surface_curve(
    display_struct     *display )
{
    if( !display->three_d.surface_curve.picking_points )
    {
        push_action_table( &display->action_table, LEFT_MOUSE_DOWN_EVENT );
        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_DOWN_EVENT, pick_point );

        display->three_d.surface_curve.lines->colours[0] =
                                                  Surface_curve_colour;
        display->three_d.surface_curve.lines->line_thickness =
                                             (float) Surface_curve_thickness;
        display->three_d.surface_curve.lines->n_points = 0;
        display->three_d.surface_curve.lines->n_items = 0;
        display->three_d.surface_curve.prev_point_exists = FALSE;
        display->three_d.surface_curve.picking_points = TRUE;

/*
        set_update_required( display, get_surface_curve_bitplane() );
*/
    }
}

public  void  end_surface_curve(
    display_struct     *display )
{
    Real  len;

    if( display->three_d.surface_curve.picking_points )
    {
        pop_action_table( &display->action_table, LEFT_MOUSE_DOWN_EVENT );
        display->three_d.surface_curve.picking_points = FALSE;
        len = get_lines_length( display->three_d.surface_curve.lines );
        print( "Total length: %g\n", len );
    }
}

private  void  add_point_to_curve(
    display_struct   *display,
    polygons_struct  *polygons,
    int              poly_index,
    Point            *point )
{
    Real                  dist;
    surface_curve_struct  *info;

    info = &display->three_d.surface_curve;

    if( info->prev_point_exists )
    {
        if( distance_along_polygons( polygons,
                     display->three_d.surface_curve.line_curvature_weight,
                     display->three_d.surface_curve.min_curvature,
                     display->three_d.surface_curve.max_curvature,
                     &info->prev_point,
                     info->prev_poly_index,
                     point, poly_index,
                     &dist, info->lines ) )
        {
            print( "Distance %g\n", dist );
            set_update_required( display, get_surface_curve_bitplane() );
            display->three_d.surface_curve.prev_point = *point;
        }
        else
        {
            print( "No path can be found.\n" );
        }
    }
    else
    {
        display->three_d.surface_curve.prev_point = *point;
        display->three_d.surface_curve.first_poly_index = poly_index;
        display->three_d.surface_curve.prev_point_exists = TRUE;
    }

    display->three_d.surface_curve.prev_polygons = polygons;
    display->three_d.surface_curve.prev_poly_index = poly_index;
}

public  void  close_surface_curve(
    display_struct     *display )
{
    Point                 point;
    surface_curve_struct  *info;

    info = &display->three_d.surface_curve;

    if( info->prev_point_exists && info->lines->n_points > 0 )
    {
        point = info->lines->points[0];
        add_point_to_curve( display, info->prev_polygons, 
                            info->first_poly_index, &point );
    }
}

private  void  pick_surface_point(
    display_struct   *display,
    BOOLEAN          snap_to_line )
{
    polygons_struct       *polygons;
    int                   poly_index;
    Point                 point, line_point, closest_vertex;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        if( Snap_to_polygon_vertex )
        {
            find_polygon_vertex_nearest_point( polygons, poly_index, &point,
                                               &closest_vertex );
            point = closest_vertex;
        }

        if( snap_to_line &&
            find_closest_line_point_to_point( display, &point, &line_point ) )
            point = line_point;

        add_point_to_curve( display, polygons, poly_index, &point );
    }
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( pick_point )
{
    pick_surface_point( display, FALSE );

    return( OK );
}

public  void  pick_surface_point_near_a_line(
    display_struct   *display )
{
    pick_surface_point( display, TRUE );
}

public  void  reset_surface_curve(
    display_struct     *display )
{
    display->three_d.surface_curve.lines->n_points = 0;
    display->three_d.surface_curve.lines->n_items = 0;
    display->three_d.surface_curve.prev_point_exists = FALSE;

    if( display->three_d.surface_curve.picking_points )
        end_surface_curve( display );

    set_update_required( display, get_surface_curve_bitplane() );
}

public  void  make_surface_curve_permanent(
    display_struct     *display )
{
    object_struct         *object;
    surface_curve_struct  *info;

    info = &display->three_d.surface_curve;

    if( info->picking_points )
        end_surface_curve( display );

    if( info->lines->n_points > 0 )
    {
        object = create_object( LINES );

        *(get_lines_ptr(object)) = *(info->lines);

        add_object_to_model( get_current_model(display), object );

        ALLOC( info->lines->colours, 1 );

        info->lines->n_points = 0;
        info->lines->n_items = 0;
        info->prev_point_exists = FALSE;

        graphics_models_have_changed( display );

        set_update_required( display, get_surface_curve_bitplane() );
    }
}
