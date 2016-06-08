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

#include  <graphics.h>

#define  N_MODELS    8

typedef  struct
{
    VIO_BOOL     perspective_flag;
    VIO_Point       origin;
    VIO_Vector      x_axis, y_axis, line_of_sight;
    VIO_Real        front_distance, back_distance;
    VIO_Real        desired_aspect;
    VIO_Real        perspective_distance;
    VIO_Real        window_width, window_height;
    VIO_Real        scale_factors[VIO_N_DIMENSIONS];
    VIO_Transform   modeling_transform;
    VIO_BOOL     stereo_flag;
    VIO_Real        eye_separation_ratio;
} view_struct;

typedef  struct
{
    VIO_BOOL       state;
    Light_types   light_type;
    VIO_Colour        colour;
    VIO_Vector        direction;
    VIO_Point         position;
    VIO_Real          spot_exponent;
    VIO_Real          spot_angle;
} light_struct;

typedef  struct
{
    Shading_modes   shaded_mode;
    Shading_types   shading_type;
    VIO_BOOL         master_light_switch;
    VIO_BOOL         backface_flag;
    VIO_BOOL         two_sided_surface_flag;
    VIO_BOOL         render_lines_as_curves;
    VIO_BOOL         show_marker_labels;
    int             n_curve_segments;
}  render_struct;

struct display_struct;

typedef  enum  {
                   NO_EVENT,
                   TERMINATE_INTERACTION_EVENT,
                   KEY_DOWN_EVENT,
                   KEY_UP_EVENT,
                   MOUSE_MOVEMENT_EVENT,
                   LEFT_MOUSE_DOWN_EVENT,
                   LEFT_MOUSE_UP_EVENT,
                   MIDDLE_MOUSE_DOWN_EVENT,
                   MIDDLE_MOUSE_UP_EVENT,
                   RIGHT_MOUSE_DOWN_EVENT,
                   RIGHT_MOUSE_UP_EVENT,
                   REDRAW_OVERLAY_EVENT,
                   WINDOW_REDRAW_EVENT,
                   WINDOW_RESIZE_EVENT,
                   WINDOW_ICONIZED_EVENT,
                   WINDOW_DEICONIZED_EVENT,
                   WINDOW_QUIT_EVENT,
                   WINDOW_LEAVE_EVENT,
                   WINDOW_ENTER_EVENT,
                   SCROLL_DOWN_EVENT,
                   SCROLL_UP_EVENT,
                   N_EVENT_TYPES
               } Event_types;

typedef  VIO_Status  (*event_function_type)( struct display_struct *,
                                         Event_types,
                                         int );

#define  DEF_EVENT_FUNCTION( f )   VIO_Status   f( display_struct  *display, \
                                               Event_types     event_type, \
                                               int             key_pressed )

#define  MAX_ACTION_STACK  5  /*!< Maximum depth of action_table_entry stack. */
#define  MAX_ACTIONS       10 /*!< Maximum actions per action_table_entry. */

/** \struct action_table_entry 
 * The action table is structured to permit
 * multiple actions per event, and support a global mechanism for "pushing" and 
 * "popping" actions.
 * Each table entry can contain up to \c MAX_ACTIONS in total, and each
 * entry maintains an individual stack of the previously registered actions.
 * This stack can be pushed at most \c MAX_ACTION_STACK times.
 * The \c last_index field stores the index of the last valid action table 
 * entry in the structure. Therefore it starts at -1 to indicate that there are
 * NO valid entries in an empty \c action_table_entry.
 */
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
    VIO_BOOL       line_active;
    VIO_Point         line_origin;
    VIO_Point         line_direction;
} point_position_struct;

/**
 * This structure represents the currently selected object at a particular
 * level. It represents the selection as an index into a model.
 */
typedef  struct
{
    int            object_index; /**< The index of the current object. */
    object_struct  *model_object; /**< The model containing the current object. */
} selection_entry;

/*
 * This structure represents the entire stack of object selections, so that
 * we can track back up through the hierarchy to the top level. At each
 * level we record the index and model that is selected. As we descend, we
 * add additional levels to the stack to represent the index and model of
 * substructure into which we've descended.
 */
typedef  struct
{
    int                n_levels_alloced;
    int                max_levels;
    int                current_level;

    selection_entry    *stack;
} selection_struct;

typedef  struct
{
    VIO_Point   first_corner;
    VIO_Point   second_corner;
} viewport_picking_struct;

typedef  struct
{
    VIO_Point   origin;
    VIO_Real    box_size[VIO_N_DIMENSIONS];
    VIO_Real    axis_size;
} cursor_struct;

typedef  struct
{
    int           x_size;
    int           y_size;
    int           n_steps;
    int           current_step;
    VIO_STR        base_filename;
    VIO_Transform     transform;
} film_loop_struct;

/**
 * Parameters used in surface segmenting. See callbacks/surf_segmenting.c
 */
typedef  struct
{
    int              n_paint_polygons;
    VIO_Colour       visible_colour;
    VIO_Colour       invisible_colour;
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
    VIO_BOOL          picking_points;
    VIO_Real             line_curvature_weight;
    VIO_Real             min_curvature;
    VIO_Real             max_curvature;
    lines_struct     *lines;
    int              n_points_alloced;
    int              n_indices_alloced;
    int              n_end_indices_alloced;
    int              first_poly_index;
    VIO_BOOL          prev_point_exists;
    VIO_Point            prev_point;
    int              prev_poly_index;
    polygons_struct  *prev_polygons;
} surface_curve_struct;

typedef  struct
{
    View_types                   view_type;
    Bitplane_types               bitplanes;
    render_struct                render;
    VIO_Transform                transform;
} model_info_struct;

typedef  struct
{
    contour_struct           contours[VIO_N_DIMENSIONS];
    int                      models_changed_id;
} cursor_contours_struct;

#endif
