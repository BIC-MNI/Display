#ifndef  DEF_DISPLAY_TYPES
#define  DEF_DISPLAY_TYPES

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
static char display_types_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/display_types.h,v 1.20 1997-08-01 14:47:45 david Exp $";
#endif

#include  <graphics.h>

#define  N_MODELS    7

typedef  struct
{
    BOOLEAN     perspective_flag;
    Point       origin;
    Vector      x_axis, y_axis, line_of_sight;
    Real        front_distance, back_distance;
    Real        desired_aspect;
    Real        perspective_distance;
    Real        window_width, window_height;
    Real        scale_factors[N_DIMENSIONS];
    Transform   modeling_transform;
    BOOLEAN     stereo_flag;
    Real        eye_separation_ratio;
} view_struct;

typedef  struct
{
    BOOLEAN       state;
    Light_types   light_type;
    Colour        colour;
    Vector        direction;
    Point         position;
    Real          spot_exponent;
    Real          spot_angle;
} light_struct;

typedef  struct
{
    BOOLEAN         shaded_mode;
    Shading_types   shading_type;
    BOOLEAN         master_light_switch;
    BOOLEAN         backface_flag;
    BOOLEAN         two_sided_surface_flag;
    BOOLEAN         render_lines_as_curves;
    BOOLEAN         show_marker_labels;
    int             n_curve_segments;
}  render_struct;

struct display_struct;

typedef  Status  (*event_function_type)( struct display_struct *,
                                         Event_types,
                                         int );

#define  DEF_EVENT_FUNCTION( f )   Status   f( display_struct  *display, \
                                               Event_types     event_type, \
                                               int             key_pressed )

#define  MAX_ACTION_STACK  5
#define  MAX_ACTIONS       10

typedef  struct
{
    int                   stack_index;
    int                   last_index[MAX_ACTION_STACK];
    event_function_type   actions[MAX_ACTIONS];
} action_table_entry;

typedef  struct
{
    action_table_entry  event_info[N_EVENT_TYPES];
} action_table_struct;

typedef  struct
{
    BOOLEAN       line_active;
    Point         line_origin;
    Point         line_direction;
} point_position_struct;

typedef  struct
{
    int            object_index;
    object_struct  *model_object;
} selection_entry;

typedef  struct
{
    int                n_levels_alloced;
    int                max_levels;
    int                current_level;

    selection_entry    *stack;
} selection_struct;

typedef  struct
{
    BOOLEAN        last_was_interrupted;
    object_struct  *object_interrupted;
    BOOLEAN        current_interrupted;
} update_interrupted_struct;

typedef  struct
{
    Point   first_corner;
    Point   second_corner;
} viewport_picking_struct;

typedef  struct
{
    Point   origin;
    Real    box_size[N_DIMENSIONS];
    Real    axis_size;
} cursor_struct;

typedef  struct
{
    int           x_size;
    int           y_size;
    int           n_steps;
    int           current_step;
    STRING        base_filename;
    Transform     transform;
} film_loop_struct;

typedef  struct
{
    int              n_paint_polygons;

    Colour           visible_colour;
    Colour           invisible_colour;

    polygons_struct  *polygons;
    BOOLEAN          polygons_set;
    int              n_vertices;
    int              n_vertices_alloced;
    int              *vertices;

    int              n_undos;
    int              n_undos_alloced;
    int              *undo_indices;

    int              n_invisible;
    int              n_invisible_alloced;
    int              *invisible_indices;
} surface_edit_struct;

typedef  struct
{
    lines_struct  *lines;
    int           n_points_alloced;
    int           n_indices_alloced;
    int           n_end_indices_alloced;
} contour_struct;

typedef  struct
{
    BOOLEAN          picking_points;
    Real             line_curvature_weight;
    Real             min_curvature;
    Real             max_curvature;
    lines_struct     *lines;
    int              n_points_alloced;
    int              n_indices_alloced;
    int              n_end_indices_alloced;
    int              first_poly_index;
    BOOLEAN          prev_point_exists;
    Point            prev_point;
    int              prev_poly_index;
    polygons_struct  *prev_polygons;
} surface_curve_struct;

typedef  struct
{
    View_types                   view_type;
    Bitplane_types               bitplanes;
    render_struct                render;
    Transform                    transform;
} model_info_struct;

typedef  struct
{
    contour_struct           contours[N_DIMENSIONS];
    int                      models_changed_id;
    int                      axis;
    polygons_struct          *current_polygons;
    int                      poly_index;
    object_traverse_struct   object_traverse;
} cursor_contours_struct;

#endif
