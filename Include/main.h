#ifndef  DEF_GRAPHICS
#define  DEF_GRAPHICS

#include  <def_graphics_types.h>
#include  <def_menu.h>
#include  <def_slice.h>
#include  <def_surface_extract.h>
#include  <def_surface_fitting.h>
#include  <def_marker_segment.h>

#define  LEFT_ARROW_KEY    1
#define  RIGHT_ARROW_KEY   2
#define  DOWN_ARROW_KEY    3
#define  UP_ARROW_KEY      4

/* for graphics windows */

#define  THREED_MODEL           2
#define  CURSOR_MODEL           0
#define  OVERLAY_MODEL          1
#define  CUT_BUFFER_MODEL       3

/* for menu windows */

#define  MENU_BUTTONS_MODEL     0
#define  SELECTED_MODEL         1

/* for slice windows */

#define  SLICE_MODEL            0
#define  COLOUR_BAR_MODEL       1

#define  N_MODELS               4

typedef  enum  { THREE_D_WINDOW,
                 MENU_WINDOW,
                 SLICE_WINDOW,
                 N_WINDOW_TYPES }
               window_types;

typedef  struct
{
    cursor_struct              cursor;
    view_struct                view;
    light_struct               lights[N_LIGHTS];
    selection_struct           current_object;
    Point                      min_limit;
    Point                      max_limit;
    Point                      centre_of_objects;
    surface_extraction_struct  surface_extraction;
    film_loop_struct           film_loop;
    surface_edit_struct        surface_edit;
    cursor_contours_struct     cursor_contours;
    surface_curve_struct       surface_curve;
    surface_fitting_struct     surface_fitting;
    int                        default_marker_structure_id;
    int                        default_marker_patient_id;
    Marker_types               default_marker_type;
    Real                       default_marker_size;
    Colour                     default_marker_colour;
    String                     default_marker_label;
    marker_segment_struct      marker_segmentation;
} three_d_window_struct;


typedef  struct  graphics_struct
{
    window_types               window_type;

    struct  graphics_struct    *associated[N_WINDOW_TYPES];

    three_d_window_struct      three_d;    /* should be a union of three */
    menu_window_struct         menu;
    slice_window_struct        slice;

    window_struct              window;
    Point                      mouse_position;
    Point                      prev_mouse_position;
    action_table_struct        action_table;
    int                        models_changed_id;
    object_struct              *models[N_MODELS];


    int                        frame_number;
    Boolean                    update_required[N_BITPLANE_TYPES];
    update_interrupted_struct  update_interrupted;

    viewport_picking_struct    viewport_picking;
} graphics_struct;

#endif
