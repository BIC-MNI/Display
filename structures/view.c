#include <def_standard.h>
#include <def_graphics.h>
#include <def_geometry.h>
#include <def_globals.h>

public  void  initialize_view( view )
    view_struct  *view;
{
    static  Point   origin = { 0.0, 0.0, 1.0 };
    static  Vector  line_of_sight = { 0.0, 0.0, -1.0 };
    static  Vector  hor = { 1.0, 0.0, 0.0 };
    void            make_identity_transform();
    void            assign_view_direction();

    view->perspective_flag = Initial_perspective_flag;
    view->origin = origin;
    assign_view_direction( view, &line_of_sight, &hor );
    view->front_distance = 0.05;
    view->perspective_distance = 2.0;
    view->back_distance = 2.0;
    view->desired_aspect = 0.0;
    view->window_width = 1.0;
    view->window_height = 1.0;

    make_identity_transform( &view->modeling_transform );
}

public  void  assign_view_direction( view, line_of_sight, hor )
    view_struct    *view;
    Vector         *line_of_sight;
    Vector         *hor;
{
    NORMALIZE_VECTOR( view->line_of_sight, *line_of_sight );
    CROSS_VECTORS( view->y_axis, *hor, view->line_of_sight );
    NORMALIZE_VECTOR( view->y_axis, view->y_axis );
    CROSS_VECTORS( view->x_axis, view->line_of_sight, view->y_axis );
    NORMALIZE_VECTOR( view->x_axis, view->x_axis );
}

public  void  adjust_view_for_aspect( view, new_aspect )
    view_struct   *view;
    Real          new_aspect;
{
    Real  width, height;

    if( view->desired_aspect <= 0.0 )
    {
        view->desired_aspect = new_aspect;
    }

    width = view->window_width;
    height = view->window_height;

    if( width * view->desired_aspect < height )
    {
        height = width * view->desired_aspect;
    }
    else
    {
        width = height / view->desired_aspect;
    }

    if( width * new_aspect < height )
    {
        width = height / new_aspect;
    }
    else
    {
        height = width * new_aspect;
    }

    view->window_width = width;
    view->window_height = height;
}

public  void  convert_point_from_coordinate_system( origin, x_axis, y_axis,
                                 z_axis, point, transformed_point )
    Point    *origin;
    Vector   *x_axis;
    Vector   *y_axis;
    Vector   *z_axis;
    Point    *point;
    Point    *transformed_point;
{
    Point    translated;

    fill_Point( translated, Point_x(*point) - Point_x(*origin),
                            Point_y(*point) - Point_y(*origin),
                            Point_z(*point) - Point_z(*origin) );

    Point_x(*transformed_point) = DOT_POINT_VECTOR( *x_axis, translated );
    Point_y(*transformed_point) = DOT_POINT_VECTOR( *y_axis, translated );
    Point_z(*transformed_point) = DOT_POINT_VECTOR( *z_axis, translated );
}

public  void  transform_point_to_world( view, p, transformed_point )
    view_struct   *view;
    Point         *p;
    Point         *transformed_point;
{
    void  transform_point();

    transform_point( &view->modeling_transform, p, transformed_point );
}

public  void  transform_world_to_screen( view, p, transformed_point )
    view_struct   *view;
    Point         *p;
    Point         *transformed_point;
{
    void  convert_point_from_coordinate_system();

    convert_point_from_coordinate_system( &view->origin,
                &view->x_axis, &view->y_axis, &view->line_of_sight,
                p, transformed_point );
}

public  void  transform_point_to_screen( view, p, transformed_point )
    view_struct   *view;
    Point         *p;
    Point         *transformed_point;
{
    void  transform_point_to_world();
    void  transform_world_to_screen();

    transform_point_to_world( view, p, transformed_point );
    transform_world_to_screen( view, transformed_point, transformed_point );
}
