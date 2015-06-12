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

#include <display.h>

static  void  get_screen_point(
    view_struct  *view,
    VIO_Real         x,
    VIO_Real         y,
    VIO_Point        *point );

static  void  get_screen_centre(
    view_struct   *view,
    VIO_Point         *centre );

static  void  convert_point_from_coordinate_system(
    VIO_Point    *origin,
    VIO_Vector   *x_axis,
    VIO_Vector   *y_axis,
    VIO_Vector   *z_axis,
    VIO_Point    *point,
    VIO_Point    *transformed_point );

static  void  transform_world_to_screen(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point );


  void  initialize_view(
    view_struct  *view,
    VIO_Vector       *line_of_sight,
    VIO_Vector       *horizontal )
{
    static  VIO_Point   origin = { { 0.0f, 0.0f, 1.0f } };

    view->perspective_flag = FALSE;
    view->origin = origin;
    assign_view_direction( view, line_of_sight, horizontal );
    view->front_distance = 0.01;
    view->perspective_distance = 1.0;
    view->back_distance = 2.0;
    view->desired_aspect = 0.0;
    view->window_width = 1.0;
    view->window_height = 1.0;
    view->scale_factors[VIO_X] = 1.0;
    view->scale_factors[VIO_Y] = 1.0;
    view->scale_factors[VIO_Z] = 1.0;

    make_identity_transform( &view->modeling_transform );

    view->stereo_flag = FALSE;
    view->eye_separation_ratio = 0.07;
}

  void  assign_view_direction(
    view_struct    *view,
    VIO_Vector         *line_of_sight,
    VIO_Vector         *hor )
{
    NORMALIZE_VECTOR( view->line_of_sight, *line_of_sight );
    CROSS_VECTORS( view->y_axis, *hor, view->line_of_sight );

    if( null_Vector( &view->y_axis ) )
    {
        create_orthogonal_vector( &view->line_of_sight, &view->x_axis );
        CROSS_VECTORS( view->y_axis, view->x_axis, view->line_of_sight );
    }

    NORMALIZE_VECTOR( view->y_axis, view->y_axis );
    CROSS_VECTORS( view->x_axis, view->line_of_sight, view->y_axis );
    NORMALIZE_VECTOR( view->x_axis, view->x_axis );
}

  void  get_view_z_axis(
    view_struct   *view,
    VIO_Vector        *z_axis )
{
    Vector_x(*z_axis) = - Vector_x(view->line_of_sight);
    Vector_y(*z_axis) = - Vector_y(view->line_of_sight);
    Vector_z(*z_axis) = - Vector_z(view->line_of_sight);
}

  void  get_view_centre(
    view_struct   *view,
    VIO_Point         *centre )
{
    VIO_Real    offset;
    VIO_Vector  offset_vector;

    offset = (view->front_distance + view->back_distance) / 2.0;

    SCALE_VECTOR( offset_vector, view->line_of_sight, offset );
    ADD_POINT_VECTOR( *centre, view->origin, offset_vector );
}

static  void  get_screen_centre(
    view_struct   *view,
    VIO_Point         *centre )
{
    VIO_Real    offset;
    VIO_Vector  offset_vector;

    offset = view->perspective_distance;

    SCALE_VECTOR( offset_vector, view->line_of_sight, offset );
    ADD_POINT_VECTOR( *centre, view->origin, offset_vector );
}

  void  get_screen_axes(
    view_struct   *view,
    VIO_Vector        *hor,
    VIO_Vector        *vert )
{
    SCALE_VECTOR( *hor, view->x_axis, view->window_width );
    SCALE_VECTOR( *vert, view->y_axis, view->window_height );
}

static  void  get_screen_point(
    view_struct  *view,
    VIO_Real         x,
    VIO_Real         y,
    VIO_Point        *point )
{
    VIO_Vector   hor, vert;

    get_screen_axes( view, &hor, &vert );

    SCALE_VECTOR( hor, hor, x - 0.5 );
    SCALE_VECTOR( vert, vert, y - 0.5 );

    get_screen_centre( view, point );

    ADD_POINT_VECTOR( *point, *point, hor );
    ADD_POINT_VECTOR( *point, *point, vert );
}

  void  adjust_view_for_aspect(
    view_struct    *view,
    window_struct  *window )
{
    VIO_Real    width, height, prev_width;
    VIO_Real    new_aspect;
    VIO_Real    prev_persp_dist, eye_offset;
    VIO_Vector  eye_offset_vector;
    int     x_size, y_size;

    G_get_window_size( window, &x_size, &y_size );

    new_aspect = (VIO_Real) y_size / (VIO_Real) x_size;

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

    prev_width = view->window_width;
    prev_persp_dist = view->perspective_distance;

    view->window_width = width;
    view->window_height = height;

    view->perspective_distance *= width / prev_width;

    eye_offset = prev_persp_dist - view->perspective_distance;

    SCALE_VECTOR( eye_offset_vector, view->line_of_sight, eye_offset );

    ADD_POINT_VECTOR( view->origin, view->origin, eye_offset_vector );
    view->front_distance -= eye_offset;
    view->back_distance -= eye_offset;
}

  void  convert_point_from_coordinate_system(
    VIO_Point    *origin,
    VIO_Vector   *x_axis,
    VIO_Vector   *y_axis,
    VIO_Vector   *z_axis,
    VIO_Point    *point,
    VIO_Point    *transformed_point )
{
    VIO_Point    translated;

    fill_Point( translated, Point_x(*point) - Point_x(*origin),
                            Point_y(*point) - Point_y(*origin),
                            Point_z(*point) - Point_z(*origin) );

    Point_x(*transformed_point) = (VIO_Point_coord_type)
                                 DOT_POINT_VECTOR( *x_axis, translated );
    Point_y(*transformed_point) = (VIO_Point_coord_type)
                                 DOT_POINT_VECTOR( *y_axis, translated );
    Point_z(*transformed_point) = (VIO_Point_coord_type)
                                 DOT_POINT_VECTOR( *z_axis, translated );
}

  void  transform_point_to_world(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point )
{
    VIO_Real   x_scaled, y_scaled, z_scaled;
    VIO_Real   x, y, z;

    x_scaled = view->scale_factors[VIO_X] * (VIO_Real) Point_x(*p);
    y_scaled = view->scale_factors[VIO_Y] * (VIO_Real) Point_y(*p);
    z_scaled = view->scale_factors[VIO_Z] * (VIO_Real) Point_z(*p);

    transform_point( &view->modeling_transform, x_scaled, y_scaled, z_scaled,
                     &x, &y, &z );

    fill_Point( *transformed_point, x, y, z );
}

  void  transform_world_to_model(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point )
{
    VIO_Transform   inverse;
    VIO_Real        x, y, z;

    compute_transform_inverse( &view->modeling_transform, &inverse );
    transform_point( &inverse,
                     (VIO_Real) Point_x(*p), (VIO_Real) Point_y(*p), (VIO_Real) Point_z(*p),
                     &x, &y, &z);
    fill_Point( *transformed_point, x, y, z );
}

  void  transform_world_to_model_vector(
    view_struct   *view,
    VIO_Vector        *v,
    VIO_Vector        *transformed_vector )
{
    VIO_Transform   inverse;
    VIO_Real        x, y, z;

    compute_transform_inverse( &view->modeling_transform, &inverse );
    transform_vector( &inverse,
                      (VIO_Real) Vector_x(*v), (VIO_Real) Vector_y(*v),
                      (VIO_Real) Vector_z(*v),
                      &x, &y, &z);
    fill_Vector( *transformed_vector, x, y, z );
}

static  void  transform_world_to_screen(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point )
{
    convert_point_from_coordinate_system( &view->origin,
                &view->x_axis, &view->y_axis, &view->line_of_sight,
                p, transformed_point );
}

  void  transform_point_to_view_space(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point )
{
    transform_point_to_world( view, p, transformed_point );
    transform_world_to_screen( view, transformed_point, transformed_point );
}

  void  transform_point_to_screen(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point )
{
    VIO_Point  tmp;
    VIO_Real   x, y, z_factor;

    transform_point_to_view_space( view, p, &tmp );

    x = (VIO_Real) Point_x(tmp);
    y = (VIO_Real) Point_y(tmp);

    if( view->perspective_flag )
    {
        z_factor = (VIO_Real) Point_z(tmp) / view->perspective_distance;
        x /= z_factor;
        y /= z_factor;
    }

    x = (x + view->window_width / 2.0 ) / view->window_width;
    y = (y + view->window_height / 2.0 ) / view->window_height;

    fill_Point( *transformed_point, x, y, 0.0 );
}

  void  set_model_scale(
    view_struct   *view,
    VIO_Real          sx,
    VIO_Real          sy,
    VIO_Real          sz )
{
    view->scale_factors[VIO_X] = sx;
    view->scale_factors[VIO_Y] = sy;
    view->scale_factors[VIO_Z] = sz;
}

  void  convert_screen_to_ray(
    view_struct   *view,
    VIO_Real          x_screen,
    VIO_Real          y_screen,
    VIO_Point         *origin,
    VIO_Vector        *direction )
{
    VIO_Vector     hor, vert;
    VIO_Vector     x_offset, y_offset, z_offset;
    VIO_Point      pt;

    get_screen_axes( view, &hor, &vert );

    SCALE_VECTOR( x_offset, hor, x_screen - 0.5 );
    SCALE_VECTOR( y_offset, vert, y_screen - 0.5 );

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

  void  magnify_view_size(
    view_struct  *view,
    VIO_Real         factor )
{
    VIO_Real      dist, mid_dist;
    VIO_Vector    offset;

    if( view->perspective_flag || view->stereo_flag )
    {
        mid_dist = (view->front_distance + view->back_distance) / 2.0;
        dist = (1.0 - 1.0 / factor) * mid_dist;
        SCALE_VECTOR( offset, view->line_of_sight, dist );
        ADD_POINT_VECTOR( view->origin, view->origin, offset);
    }
    else
    {
        view->window_width /= factor;
        view->window_height /= factor;
        view->perspective_distance /= factor;
    }
}

  void  set_view_rectangle(
    view_struct   *view,
    VIO_Real          x_min,
    VIO_Real          x_max,
    VIO_Real          y_min,
    VIO_Real          y_max )
{
    VIO_Real   window_width, window_height, prev_dist;
    VIO_Real   x, y;
    VIO_Point  new_centre;
    VIO_Vector to_eye;

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
