#ifndef  DEF_GRAPHICS
#define  DEF_GRAPHICS

#include  <def_graphics_types.h>
#include  <def_menu.h>
#include  <def_slice.h>
#include  <def_surface_extract.h>

/* for graphics windows */

#define  CURSOR_MODEL           0
#define  UNUSED_MODEL           1
#define  THREED_MODEL           2

/* for menu windows */

#define  MENU_BUTTONS_MODEL     0
#define  SELECTED_MODEL         1

/* for slice windows */

#define  SLICE_MODEL            0

#define  N_MODELS               3

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
} three_d_window_struct;


typedef  struct  graphics_struct
{
    window_types               window_type;

    struct  graphics_struct    *associated[N_WINDOW_TYPES];

    three_d_window_struct      three_d;    /* union of three */
    menu_window_struct         menu;
    slice_window_struct        slice;

    window_struct              window;
    Point                      mouse_position;
    Point                      prev_mouse_position;
    action_table_struct        action_table;
    object_struct              *models[N_MODELS];


    int                        frame_number;
    Boolean                    update_required;
    update_interrupted_struct  update_interrupted;

    viewport_picking_struct    viewport_picking;
} graphics_struct;

#endif
