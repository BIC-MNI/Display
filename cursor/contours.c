#include  <def_graphics.h>
#include  <def_globals.h>

public  Status  create_cursor_contours( graphics )
    graphics_struct  *graphics;
{
    Status          status;
    int             axis;
    Status          add_object_to_model();
    Status          create_lines_object();
    object_struct   *object;
    model_struct    *model;
    model_struct    *get_graphics_model();

    status = OK;
    model = get_graphics_model( graphics, OVERLAY_MODEL );

    for_less( axis, 0, 3 )
    {
        status = create_lines_object( &object, &Cursor_colour, 0, 0, 0 );

        if( status == OK )
            status = add_object_to_model( model, object );

        if( status == OK )
        {
            object->ptr.lines->line_thickness = 2;

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

#ifdef DELETE
public  Status  update_cursor_contours( graphics )
    graphics_struct  *graphics;
{
    Status                   status;
    int                      axis;
    Vector                   plane_normal;
    Real                     plane_constant;
    cursor_contours_struct   *contours;
    Status                   intersect_plane_with_polygons();

    status = OK;

    if( Show_cursor_contours )
    {
        contours = &graphics->three_d.cursor_contours;

        for_less( axis, 0, 3 )
        {
            fill_Vector( plane_normal, 0.0, 0.0, 0.0 );
            Vector_coord(plane_normal,axis) = 1.0;

            plane_constant = Point_coord( graphics->three_d.cursor.origin,
                                          axis );

            status = intersect_plane_with_polygons( graphics, &plane_normal,
                           plane_constant,
                           contours->contours[axis].lines,
                           &contours->contours[axis].n_points_alloced,
                           &contours->contours[axis].n_indices_alloced,
                           &contours->contours[axis].n_end_indices_alloced );

            if( status != OK )
                break;
        }
    }
    
    return( status );
}
#endif
