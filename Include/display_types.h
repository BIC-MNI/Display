#ifndef  DEF_DISPLAY_TYPES
#define  DEF_DISPLAY_TYPES

#include  <def_mni.h>

typedef  struct
{
    Boolean     perspective_flag;
    Point       origin;
    Vector      x_axis, y_axis, line_of_sight;
    Real        front_distance, back_distance;
    Real        desired_aspect;
    Real        perspective_distance;
    Real        window_width, window_height;
    Real        scale_factors[N_DIMENSIONS];
    Transform   modeling_transform;
} view_struct;

typedef  struct
{
    Boolean       state;
    Light_types   light_type;
    Colour        colour;
    Vector        direction;
    Point         position;
    Real          spot_exponent;
    Real          spot_angle;
} light_struct;

typedef  struct
{
    Boolean         shaded_mode;
    Shading_types   shading_type;
    Boolean         master_light_switch;
    Boolean         backface_flag;
    Boolean         two_sided_surface_flag;
    Boolean         render_lines_as_curves;
    Boolean         show_marker_labels;
    int             n_curve_segments;
}  render_struct;

typedef  Status  event_function_decl();

typedef  Status  (*event_function_type)();

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
    Boolean       line_active;
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
    Boolean        last_was_interrupted;
    object_struct  *object_interrupted;
    Real           interrupt_at;
    Boolean        current_interrupted;
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
    String        base_filename;
    Transform     transform;
    Colour        *image_storage;
} film_loop_struct;

typedef  struct
{
    int              n_paint_polygons;

    polygons_struct  *polygons;
    Boolean          polygons_set;
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
    Boolean          picking_points;
    lines_struct     *lines;
    int              n_points_alloced;
    int              n_indices_alloced;
    int              n_end_indices_alloced;
    Boolean          prev_point_exists;
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
    contour_struct           contours[3];
    int                      models_changed_id;
    int                      axis;
    polygons_struct          *current_polygons;
    int                      poly_index;
    object_traverse_struct   object_traverse;
} cursor_contours_struct;

typedef struct
{
    deform_struct     deform;
    object_struct     *deforming_object;
    int               iteration;
    Boolean           in_progress;
}  deformation_struct;

#endif
