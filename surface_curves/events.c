
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_marching_cubes.h>
#include  <def_splines.h>
#include  <def_bitlist.h>
#include  <def_files.h>

static    DECL_EVENT_FUNCTION( pick_point );

public  Status  initialize_surface_curve( graphics )
    graphics_struct    *graphics;
{
    Status         status;
    object_struct  *object;
    model_struct   *model;
    model_struct   *get_graphics_model();
    Status         create_lines_object();
    Status         add_object_to_model();

    model = get_graphics_model( graphics, OVERLAY_MODEL );

    status = create_lines_object( &object, &Surface_curve_colour, 0, 0, 0 );

    if( status == OK )
    {
        object->ptr.lines->line_thickness = Surface_curve_thickness;

        status = add_object_to_model( model, object );
    }

    graphics->three_d.surface_curve.lines = object->ptr.lines;
    graphics->three_d.surface_curve.n_points_alloced = 0;
    graphics->three_d.surface_curve.n_indices_alloced = 0;
    graphics->three_d.surface_curve.n_end_indices_alloced = 0;
    graphics->three_d.surface_curve.picking_points = FALSE;

    return( status );
}

public  void  start_surface_curve( graphics )
    graphics_struct    *graphics;
{
    void                    add_action_table_function();
    void                    push_action_table();
    void                    set_update_required();

    if( !graphics->three_d.surface_curve.picking_points )
    {
        push_action_table( &graphics->action_table, LEFT_MOUSE_DOWN_EVENT );
        add_action_table_function( &graphics->action_table,
                                   LEFT_MOUSE_DOWN_EVENT, pick_point );

        graphics->three_d.surface_curve.lines->colours[0] =
                                                  Surface_curve_colour;
        graphics->three_d.surface_curve.lines->line_thickness =
                                                  Surface_curve_thickness;
        graphics->three_d.surface_curve.lines->n_points = 0;
        graphics->three_d.surface_curve.lines->n_items = 0;
        graphics->three_d.surface_curve.prev_point_exists = FALSE;
        graphics->three_d.surface_curve.picking_points = TRUE;

        set_update_required( graphics, OVERLAY_PLANES );
    }
}

public  void  end_surface_curve( graphics )
    graphics_struct    *graphics;
{
    void   pop_action_table();

    if( graphics->three_d.surface_curve.picking_points )
    {
        pop_action_table( &graphics->action_table, LEFT_MOUSE_DOWN_EVENT );
        graphics->three_d.surface_curve.picking_points = FALSE;
    }
}

private  DEF_EVENT_FUNCTION( pick_point )    /* ARGSUSED */
{
    Status                status;
    polygons_struct       *polygons;
    int                   poly_index;
    Point                 point;
    Real                  dist;
    surface_curve_struct  *info;
    void                  set_update_required();

    status = OK;

    if( get_polygon_under_mouse( graphics, &polygons, &poly_index, &point ) )
    {
        info = &graphics->three_d.surface_curve;

        if( info->prev_point_exists )
        {
            if( distance_along_polygons( polygons, &info->prev_point,
                                         info->prev_poly_index,
                                         &point, poly_index,
                                         &dist, info->lines ) )
            {
                PRINT( "Distance %g\n", dist );
                set_update_required( graphics, OVERLAY_PLANES );
            }
        }

        graphics->three_d.surface_curve.prev_point_exists = TRUE;
        graphics->three_d.surface_curve.prev_polygons = polygons;
        graphics->three_d.surface_curve.prev_poly_index = poly_index;
        graphics->three_d.surface_curve.prev_point = point;
    }

    return( status );
}

public  void  reset_surface_curve( graphics )
    graphics_struct    *graphics;
{
    void                  set_update_required();

    graphics->three_d.surface_curve.lines->n_points = 0;
    graphics->three_d.surface_curve.lines->n_items = 0;
    graphics->three_d.surface_curve.prev_point_exists = FALSE;

    if( graphics->three_d.surface_curve.picking_points )
    {
        end_surface_curve( graphics );
    }

    set_update_required( graphics, OVERLAY_PLANES );
}

public  void  make_surface_curve_permanent( graphics )
    graphics_struct    *graphics;
{
    Status                status;
    Status                create_object();
    Status                add_object_to_model();
    model_struct          *get_current_model();
    object_struct         *object;
    surface_curve_struct  *info;
    void                  set_update_required();

    info = &graphics->three_d.surface_curve;

    if( info->picking_points )
    {
        end_surface_curve( graphics );
    }

    if( info->lines->n_points > 0 )
    {
        status = create_object( &object, LINES );

        if( status == OK )
        {
            *(object->ptr.lines) = *(info->lines);

            status = add_object_to_model( get_current_model(graphics),
                                          object );
        }

        ALLOC( status, info->lines->colours, 1 );

        info->lines->n_points = 0;
        info->lines->n_items = 0;
        info->prev_point_exists = FALSE;

        set_update_required( graphics, NORMAL_PLANES );
        set_update_required( graphics, OVERLAY_PLANES );
    }
}
