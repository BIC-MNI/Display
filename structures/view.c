#include <display.h>

public  void  initialize_view(
    view_struct  *view,
    Vector       *line_of_sight,
    Vector       *horizontal )
{
    static  Point   origin = { 0.0, 0.0, 1.0 };

    view->perspective_flag = FALSE;
    view->origin = origin;
    assign_view_direction( view, line_of_sight, horizontal );
    view->front_distance = 0.0;
    view->perspective_distance = 4.0;
    view->back_distance = 2.0;
    view->desired_aspect = 0.0;
    view->window_width = 1.0;
    view->window_height = 1.0;
    view->scale_factors[X] = 1.0;
    view->scale_factors[Y] = 1.0;
    view->scale_factors[Z] = 1.0;

    make_identity_transform( &view->modeling_transform );
}

public  void  assign_view_direction(
    view_struct    *view,
    Vector         *line_of_sight,
    Vector         *hor )
{
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

public  void  get_view_z_axis(
    view_struct   *view,
    Vector        *z_axis )
{
    Vector_x(*z_axis) = - Vector_x(view->line_of_sight);
    Vector_y(*z_axis) = - Vector_y(view->line_of_sight);
    Vector_z(*z_axis) = - Vector_z(view->line_of_sight);
}

public  void  get_view_centre(
    view_struct   *view,
    Point         *centre )
{
    Real    offset;
    Vector  offset_vector;

    offset = (view->front_distance + view->back_distance) / 2.0;

    SCALE_VECTOR( offset_vector, view->line_of_sight, offset );
    ADD_POINT_VECTOR( *centre, view->origin, offset_vector );
}

public  void  get_screen_centre(
    view_struct   *view,
    Point         *centre )
{
    Real    offset;
    Vector  offset_vector;

    offset = view->perspective_distance;

    SCALE_VECTOR( offset_vector, view->line_of_sight, offset );
    ADD_POINT_VECTOR( *centre, view->origin, offset_vector );
}

public  void  get_screen_axes(
    view_struct   *view,
    Vector        *hor,
    Vector        *vert )
{
    SCALE_VECTOR( *hor, view->x_axis, view->window_width );
    SCALE_VECTOR( *vert, view->y_axis, view->window_height );
}

public  void  get_screen_point(
    view_struct  *view,
    Real         x,
    Real         y,
    Point        *point )
{
    Vector   hor, vert;

    get_screen_axes( view, &hor, &vert );

    SCALE_VECTOR( hor, hor, x - 0.5 );
    SCALE_VECTOR( vert, vert, y - 0.5 );

    get_screen_centre( view, point );

    ADD_POINT_VECTOR( *point, *point, hor );
    ADD_POINT_VECTOR( *point, *point, vert );
}

public  void  adjust_view_for_aspect(
    view_struct    *view,
    window_struct  *window )
{
    Real    width, height;
    Real    new_aspect;
    Real    prev_persp_dist, eye_offset;
    Vector  eye_offset_vector;
    int     x_size, y_size;

    G_get_window_size( window, &x_size, &y_size );

    new_aspect = (Real) y_size / (Real) x_size;

    if( view->desired_aspect <= 0.0 )
        view->desired_aspect = new_aspect;

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

    view->perspective_distance = width * G_get_monitor_widths_to_eye() *
                                 (Real) G_get_monitor_width() /
                                 (Real) x_size;

    eye_offset = prev_persp_dist - view->perspective_distance;

    SCALE_VECTOR( eye_offset_vector, view->line_of_sight, eye_offset );

    ADD_POINT_VECTOR( view->origin, view->origin, eye_offset_vector );
    view->front_distance -= eye_offset;
    view->back_distance -= eye_offset;
}

public  void  convert_point_from_coordinate_system(
    Point    *origin,
    Vector   *x_axis,
    Vector   *y_axis,
    Vector   *z_axis,
    Point    *point,
    Point    *transformed_point )
{
    Point    translated;

    fill_Point( translated, Point_x(*point) - Point_x(*origin),
                            Point_y(*point) - Point_y(*origin),
                            Point_z(*point) - Point_z(*origin) );

    Point_x(*transformed_point) = DOT_POINT_VECTOR( *x_axis, translated );
    Point_y(*transformed_point) = DOT_POINT_VECTOR( *y_axis, translated );
    Point_z(*transformed_point) = DOT_POINT_VECTOR( *z_axis, translated );
}

public  void  transform_point_to_world(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point )
{
    Real   x_scaled, y_scaled, z_scaled;
    Real   x, y, z;

    x_scaled = view->scale_factors[X] * Point_x(*p);
    y_scaled = view->scale_factors[Y] * Point_y(*p);
    z_scaled = view->scale_factors[Z] * Point_z(*p);

    transform_point( &view->modeling_transform, x_scaled, y_scaled, z_scaled,
                     &x, &y, &z );

    fill_Point( *transformed_point, x, y, z );
}

public  void  transform_world_to_model(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point )
{
    Real   x, y, z;

    inverse_transform_point( &view->modeling_transform,
                             Point_x(*p), Point_y(*p), Point_z(*p), &x, &y, &z);
    fill_Point( *transformed_point, x, y, z );
}

public  void  transform_world_to_model_vector(
    view_struct   *view,
    Vector        *v,
    Vector        *transformed_vector )
{
    Real  x, y, z;

    inverse_transform_vector( &view->modeling_transform,
                              Vector_x(*v), Vector_y(*v), Vector_z(*v),
                              &x, &y, &z);
    fill_Vector( *transformed_vector, x, y, z );
}

public  void  transform_world_to_screen(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point )
{
    convert_point_from_coordinate_system( &view->origin,
                &view->x_axis, &view->y_axis, &view->line_of_sight,
                p, transformed_point );
}

public  void  transform_point_to_view_space(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point )
{
    transform_point_to_world( view, p, transformed_point );
    transform_world_to_screen( view, transformed_point, transformed_point );
}

public  void  transform_point_to_screen(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point )
{
    Point  tmp;
    Real   x, y, z_factor;

    transform_point_to_view_space( view, p, &tmp );

    x = Point_x(tmp);
    y = Point_y(tmp);

    if( view->perspective_flag )
    {
        z_factor = Point_z(tmp) / view->perspective_distance;
        x /= z_factor;
        y /= z_factor;
    }

    x = (x + view->window_width / 2.0 ) / view->window_width;
    y = (y + view->window_height / 2.0 ) / view->window_height;

    fill_Point( *transformed_point, x, y, 0.0 );
}

public  void  set_model_scale(
    view_struct   *view,
    Real          sx,
    Real          sy,
    Real          sz )
{
    view->scale_factors[X] = sx;
    view->scale_factors[Y] = sy;
    view->scale_factors[Z] = sz;
}

public  void  convert_mouse_to_ray(
    view_struct   *view,
    Real          x_mouse,
    Real          y_mouse,
    Point         *origin,
    Vector        *direction )
{
    Vector     hor, vert;
    Vector     x_offset, y_offset, z_offset;
    Point      pt;

    get_screen_axes( view, &hor, &vert );

    SCALE_VECTOR( x_offset, hor, x_mouse - 0.5 );
    SCALE_VECTOR( y_offset, vert, y_mouse - 0.5 );

    if( view->perspective_flag )
    {
        *origin = view->origin;

        SCALE_VECTOR( z_offset, view->line_of_sight,
                      view->perspective_distance );

        ADD_POINT_VECTOR( pt, view->origin, z_offset );
        ADD_POINT_VECTOR( pt, pt, x_offset );
        ADD_POINT_VECTOR( pt, pt, y_offset );

        SUB_POINTS( *direction, pt, view->origin );
        NORMALIZE_VECTOR( *direction, *direction );
    }
    else
    {
        *direction = view->line_of_sight;
        SCALE_VECTOR( z_offset, view->line_of_sight, view->front_distance );

        ADD_POINT_VECTOR( *origin, view->origin, x_offset );
        ADD_POINT_VECTOR( *origin, *origin, y_offset );
        ADD_POINT_VECTOR( *origin, *origin, z_offset );
    }
}

public  void  magnify_view_size(
    view_struct  *view,
    Real         factor )
{
    Real      dist;
    Vector    offset;

    if( view->perspective_flag )
    {
        dist = (1.0 - factor) * view->perspective_distance;
        SCALE_VECTOR( offset, view->line_of_sight, dist );
        ADD_POINT_VECTOR( view->origin, view->origin, offset);
    }
    else
    {
        view->window_width *= factor;
        view->window_height *= factor;
        view->perspective_distance *= factor;
    }
}

public  void  set_view_rectangle(
    view_struct   *view,
    Real          x_min,
    Real          x_max,
    Real          y_min,
    Real          y_max )
{
    Real   window_width, window_height, prev_dist;
    Real   x, y;
    Point  new_centre;
    Vector to_eye;

    window_width = (x_max - x_min) * view->window_width;
    window_height = (y_max - y_min) * view->window_height;

    x = (x_min + x_max) / 2.0;
    y = (y_min + y_max) / 2.0;

    get_screen_point( view, x, y, &new_centre );

    prev_dist = view->perspective_distance;
    view->perspective_distance *= window_width / view->window_width;
    view->window_width = window_width;
    view->window_height = window_height;
    view->front_distance += view->perspective_distance - prev_dist;
    view->back_distance += view->perspective_distance - prev_dist;

    SCALE_VECTOR( to_eye, view->line_of_sight, -view->perspective_distance );
    ADD_POINT_VECTOR( view->origin, new_centre, to_eye );

    view->desired_aspect = window_height / window_width;
}
