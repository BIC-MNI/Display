
#include  <def_display.h>

static    DEF_EVENT_FUNCTION( pick_point );

public  void  initialize_surface_curve(
    display_struct     *display )
{
    object_struct  *object;
    lines_struct   *lines;
    model_struct   *model;

    model = get_graphics_model( display, OVERLAY_MODEL );

    object = create_object( LINES );
    lines = get_lines_ptr( object );
    initialize_lines( lines, Surface_curve_colour );
    lines->line_thickness = Surface_curve_thickness;

    add_object_to_model( model, object );

    display->three_d.surface_curve.lines = lines;
    display->three_d.surface_curve.line_curvature_weight =
                                           Line_curvature_weight;
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
                                                  Surface_curve_thickness;
        display->three_d.surface_curve.lines->n_points = 0;
        display->three_d.surface_curve.lines->n_items = 0;
        display->three_d.surface_curve.prev_point_exists = FALSE;
        display->three_d.surface_curve.picking_points = TRUE;

        set_update_required( display, OVERLAY_PLANES );
    }
}

public  void  end_surface_curve(
    display_struct     *display )
{
    if( display->three_d.surface_curve.picking_points )
    {
        pop_action_table( &display->action_table, LEFT_MOUSE_DOWN_EVENT );
        display->three_d.surface_curve.picking_points = FALSE;
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
                     &info->prev_point,
                     info->prev_poly_index,
                     point, poly_index,
                     &dist, info->lines ) )
        {
            print( "Distance %g\n", dist );
            set_update_required( display, OVERLAY_PLANES );
        }
    }

    if( !display->three_d.surface_curve.prev_point_exists )
    {
        display->three_d.surface_curve.first_poly_index = poly_index;
        display->three_d.surface_curve.prev_point_exists = TRUE;
    }

    display->three_d.surface_curve.prev_polygons = polygons;
    display->three_d.surface_curve.prev_poly_index = poly_index;
    display->three_d.surface_curve.prev_point = *point;
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

private  DEF_EVENT_FUNCTION( pick_point )    /* ARGSUSED */
{
    polygons_struct       *polygons;
    int                   poly_index;
    Point                 point, closest_vertex;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        if( Snap_to_polygon_vertex )
        {
            find_polygon_vertex_nearest_point( polygons, poly_index, &point,
                                               &closest_vertex );
            point = closest_vertex;
        }

        add_point_to_curve( display, polygons, poly_index, &point );
    }

    return( OK );
}

public  void  reset_surface_curve(
    display_struct     *display )
{
    display->three_d.surface_curve.lines->n_points = 0;
    display->three_d.surface_curve.lines->n_items = 0;
    display->three_d.surface_curve.prev_point_exists = FALSE;

    if( display->three_d.surface_curve.picking_points )
        end_surface_curve( display );

    set_update_required( display, OVERLAY_PLANES );
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

        set_update_required( display, NORMAL_PLANES );
        set_update_required( display, OVERLAY_PLANES );
    }
}
