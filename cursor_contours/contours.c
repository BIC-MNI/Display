
#include  <def_graphics.h>
#include  <def_colours.h>
#include  <def_globals.h>

static    Status   create_cursor_contours();
static    DECL_EVENT_FUNCTION( check_update_contour );
static    Boolean  add_to_contour();
static    Status   initialize_contour();
static    Boolean  contour_not_finished();
static    Status   start_new_axis();
static    polygons_struct    *get_next_polygons();
static    Status             advance_polygon();

public  Status  initialize_cursor_plane_outline( graphics )
    graphics_struct   *graphics;
{
    void   add_action_table_function();

    add_action_table_function( &graphics->action_table, NO_EVENT,
                            check_update_contour );

    graphics->three_d.cursor_contours.models_changed_id = -1;

    return( create_cursor_contours(graphics) );
}

private  Status  create_cursor_contours( graphics )
    graphics_struct  *graphics;
{
    Status          status;
    int             axis;
    Status          add_object_to_model();
    Status          create_lines_object();
    static Colour   colours[N_DIMENSIONS] = { {RED_COL}, {GREEN_COL},
                                              {BLUE_COL} };
    object_struct   *object;
    model_struct    *model;
    model_struct    *get_graphics_model();

    status = OK;
    model = get_graphics_model( graphics, OVERLAY_MODEL );

    for_less( axis, 0, 3 )
    {
        status = create_lines_object( &object, &colours[axis], 0, 0, 0 );

        if( status == OK )
            status = add_object_to_model( model, object );

        if( status == OK )
        {
            object->ptr.lines->line_thickness = Cursor_contour_thickness;

            graphics->three_d.cursor_contours.contours[axis].
                                        lines = object->ptr.lines;
            graphics->three_d.cursor_contours.contours[axis].
                                        n_points_alloced = 0;
            graphics->three_d.cursor_contours.contours[axis].
                                        n_indices_alloced = 0;
            graphics->three_d.cursor_contours.contours[axis].
                                        n_end_indices_alloced = 0;
        }
    }

    return( status );
}

private  DEF_EVENT_FUNCTION( check_update_contour )
    /* ARGSUSED */
{
    Status   status;
    Real     end_time;
    Real     current_realtime_seconds();
    Boolean  found;
    void     set_update_required();

    status = OK;

    end_time = current_realtime_seconds() + Max_background_seconds;
    
    if( graphics->three_d.cursor_contours.models_changed_id !=
        graphics->models_changed_id )
    {
        status = initialize_contour( graphics );

        graphics->three_d.cursor_contours.models_changed_id =
                                        graphics->models_changed_id;
    }
    else if( Show_cursor_contours )
    {
        found = FALSE;

        while( status == OK && contour_not_finished( graphics ) &&
               current_realtime_seconds() < end_time )
        {
            if( add_to_contour( graphics ) )
                found = TRUE;
        }

        if( found )
            set_update_required( graphics, OVERLAY_PLANES );
    }

    return( status );
}

private  Status   initialize_contour( graphics )
    graphics_struct  *graphics;
{
    Status                  status;
    int                     axis;
    cursor_contours_struct  *contours;

    contours = &graphics->three_d.cursor_contours;

    for_less( axis, 0, 3 )
    {
        contours->contours[axis].lines->n_points = 0;
        contours->contours[axis].lines->n_items = 0;
    }

    contours->axis = 0;

    status = start_new_axis( graphics );

    return( status );
}

private  Status   start_new_axis( graphics )
    graphics_struct  *graphics;
{
    Status                  status;
    cursor_contours_struct  *contours;
    Status                  initialize_object_traverse();

    contours = &graphics->three_d.cursor_contours;

    status = initialize_object_traverse( &contours->object_traverse,
                                         N_MODELS, graphics->models );

    contours->current_polygons =
                    get_next_polygons( &contours->object_traverse );

    contours->poly_index = 0;

    return( status );
}

private  polygons_struct  *get_next_polygons( object_traverse )
    object_traverse_struct  *object_traverse;
{
    object_struct    *object;
    polygons_struct  *polygons;

    polygons = (polygons_struct *) 0;

    while( get_next_object_traverse( object_traverse, &object ) )
    {
        if( object->object_type == POLYGONS &&
            object->visibility &&
            object->ptr.polygons->n_items > 0 )
        {
            polygons = object->ptr.polygons;
            break;
        }
    }

    return( polygons );
}

private  Boolean  contour_not_finished( graphics )
    graphics_struct  *graphics;
{
    return( graphics->three_d.cursor_contours.current_polygons !=
            (polygons_struct *) 0 );
}

private  Boolean  add_to_contour( graphics )
    graphics_struct  *graphics;
{
    Boolean                 found;
    Boolean                 intersect_plane_one_polygon();
    Vector                  plane_normal;
    Real                    plane_constant;
    int                     axis;
    cursor_contours_struct  *contours;

    contours = &graphics->three_d.cursor_contours;
    axis = contours->axis;

    fill_Vector( plane_normal, 0.0, 0.0, 0.0 );
    Vector_coord(plane_normal,axis) = 1.0;

    plane_constant = Point_coord( graphics->three_d.cursor.origin, axis );

    found = intersect_plane_one_polygon( &plane_normal,
                              plane_constant,
                              contours->current_polygons,
                              contours->poly_index,
                              contours->contours[axis].lines,
                              &contours->contours[axis].n_points_alloced,
                              &contours->contours[axis].n_indices_alloced,
                              &contours->contours[axis].n_end_indices_alloced );

    (void) advance_polygon( graphics );

    return( found );
}

private  Status  advance_polygon( graphics )
    graphics_struct  *graphics;
{
    Status                  status;
    cursor_contours_struct  *contours;

    status = OK;

    contours = &graphics->three_d.cursor_contours;

    ++contours->poly_index;

    if( contours->poly_index >= contours->current_polygons->n_items )
    {
        contours->current_polygons =
                        get_next_polygons( &contours->object_traverse );
        contours->poly_index = 0;

        if( contours->current_polygons == (polygons_struct *) 0 )
        {
            ++contours->axis;

            if( contours->axis < N_DIMENSIONS )
                status = start_new_axis( graphics );
        }
    }

    return( status );
}
