#ifndef  DEF_GRAPHICS
#define  DEF_GRAPHICS

#include  <def_standard.h>
#include  <def_objects.h>
#include  <def_menu.h>
#include  <def_queue.h>

typedef  struct
{
    Boolean     perspective_flag;
    Point       origin;
    Vector      x_axis, y_axis, line_of_sight;
    Real        front_distance, back_distance;
    Real        perspective_distance;
    Real        desired_aspect;
    Real        window_width, window_height;
    Real        scale_factors[N_DIMENSIONS];
    Transform   modeling_transform;
} view_struct;

typedef enum { AMBIENT_LIGHT,
               DIRECTIONAL_LIGHT,
               POINT_LIGHT,
               SPOT_LIGHT
             } light_types;

typedef  struct
{
    Boolean       state;
    light_types   light_type;
    Colour        colour;
    Vector        direction;
    Point         position;
    Real          spot_exponent;
    Real          spot_angle;
} light_struct;

typedef  enum  {
                   NO_EVENT,
                   KEYBOARD_EVENT,
                   MOUSE_MOVEMENT_EVENT,
                   LEFT_MOUSE_DOWN_EVENT,
                   LEFT_MOUSE_UP_EVENT,
                   MIDDLE_MOUSE_DOWN_EVENT,
                   MIDDLE_MOUSE_UP_EVENT,
                   RIGHT_MOUSE_DOWN_EVENT,
                   RIGHT_MOUSE_UP_EVENT,
                   WINDOW_REDRAW_EVENT,
                   WINDOW_RESIZE_EVENT,
                   NUM_EVENT_TYPES
               } event_types;

typedef  struct
{
    event_types   event_type;
    Window_id     window_id;

    union
    {
        char   key_pressed;
        Point  mouse_position;
    } event_data;

} event_struct;

typedef  QUEUE_STRUCT( event_struct )   event_queue_struct;

typedef  Status  eft();

typedef  Status  (*event_function_type)();

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
    action_table_entry  event_info[(int) NUM_EVENT_TYPES];
} action_table_struct;

typedef  struct  graphics_struct
{
    struct  graphics_struct  *menu_window;
    struct  graphics_struct  *graphics_window;

    menu_window_struct   menu;

    window_struct        window;
    Point                mouse_position;
    Point                prev_mouse_position;
    view_struct          view;
    light_struct         lights[N_LIGHTS];
    action_table_struct  action_table;

    model_struct         model;
    Point                min_limit;
    Point                max_limit;
    Point                centre_of_objects;

    int                  frame_number;
    int                  update_required;
} graphics_struct;

#define  DECL_EVENT_FUNCTION( f )  eft   f

#define  DEF_EVENT_FUNCTION( f )   Status   f( graphics, event ) \
                                                graphics_struct  *graphics; \
                                                event_struct     *event;

#endif
