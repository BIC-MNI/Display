#include <def_standard.h>
#include <def_graphics.h>
#include <def_geometry.h>
#include <def_globals.h>

public  void  initialize_view( view, view_x, view_y, view_z )
    view_struct  *view;
    Real         view_x, view_y, view_z;
{
    static  Point   origin = { 0.0, 0.0, 1.0 };
    static  Vector  hor = { 1.0, 0.0, 0.0 };
    Vector          line_of_sight;
    void            assign_view_direction();
    void            make_identity_transform();

    view->perspective_flag = Initial_perspective_flag;
    view->origin = origin;
    fill_Vector( line_of_sight, view_x, view_y, view_z );
    assign_view_direction( view, &line_of_sight, &hor );
    view->front_distance = 0.0;
    view->perspective_distance = 4.0;
    view->back_distance = 2.0;
    view->desired_aspect = 0.0;
    view->window_width = 1.0;
    view->window_height = 1.0;
    view->scale_factors[X_AXIS] = 1.0;
    view->scale_factors[Y_AXIS] = 1.0;
    view->scale_factors[Z_AXIS] = 1.0;

    make_identity_transform( &view->modeling_transform );
}

public  void  assign_view_direction( view, line_of_sight, hor )
    view_struct    *view;
    Vector         *line_of_sight;
    Vector         *hor;
{
    Boolean   null_Vector();
    void      create_noncolinear_vector();

    NORMALIZE_VECTOR( view->line_of_sight, *line_of_sight );
    CROSS_VECTORS( view->y_axis, *hor, view->line_of_sight );

    if( null_Vector( &view->y_axis ) )
    {
        create_noncolinear_vector( &view->line_of_sight, &view->x_axis );
        CROSS_VECTORS( view->y_axis, view->x_axis, view->line_of_sight );
    }

    NORMALIZE_VECTOR( view->y_axis, view->y_axis );
    CROSS_VECTORS( view->x_axis, view->line_of_sight, view->y_axis );
    NORMALIZE_VECTOR( view->x_axis, view->x_axis );
}

public  void  adjust_view_for_aspect( view, window )
    view_struct    *view;
    window_struct  *window;
{
    Real    width, height;
    Real    new_aspect;
    int     G_get_monitor_width();
    Real    prev_persp_dist, eye_offset;
    Vector  eye_offset_vector;

    new_aspect = (Real) window->y_size / (Real) window->x_size;

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

    prev_persp_dist = view->perspective_distance;

    view->perspective_distance = width * Monitor_widths_to_eye *
                                 (Real) G_get_monitor_width() /
                                 (Real) window->x_size;

    eye_offset = prev_persp_dist - view->perspective_distance;

    SCALE_VECTOR( eye_offset_vector, view->line_of_sight, eye_offset );

    ADD_POINT_VECTOR( view->origin, view->origin, eye_offset_vector );
    view->front_distance -= eye_offset;
    view->back_distance -= eye_offset;
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
    void   transform_point();
    Point  scaled;

    Point_x(scaled) = view->scale_factors[X_AXIS] * Point_x(*p);
    Point_y(scaled) = view->scale_factors[Y_AXIS] * Point_y(*p);
    Point_z(scaled) = view->scale_factors[Z_AXIS] * Point_z(*p);

    transform_point( &view->modeling_transform, &scaled, transformed_point );
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

public  void  get_screen_axes( view, hor, vert )
    view_struct   *view;
    Vector        *hor;
    Vector        *vert;
{
    SCALE_VECTOR( *hor, view->x_axis, view->window_width );
    SCALE_VECTOR( *vert, view->y_axis, view->window_height );
}

public  void  set_model_scale( view, sx, sy, sz )
    view_struct   *view;
    Real          sx, sy, sz;
{
    view->scale_factors[X_AXIS] = sx;
    view->scale_factors[Y_AXIS] = sy;
    view->scale_factors[Z_AXIS] = sz;
}
