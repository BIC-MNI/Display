#ifndef  DEF_GRAPHICS
#define  DEF_GRAPHICS

#include  <def_standard.h>
#include  <def_objects.h>

typedef  struct
{
    Boolean     perspective_flag;
    Point       origin;
    Vector      x_axis, y_axis, line_of_sight;
    Real        front_distance, back_distance;
    Real        perspective_distance;
    Real        window_width, window_height;
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

typedef  struct
{
    int           head, tail;
    event_struct  *events;
} event_queue_struct;

typedef  Status  (*event_function_type)();

typedef  struct
{
    event_function_type   actions[(int) NUM_EVENT_TYPES];
} action_table_struct;

#include  <def_objects.h>

typedef  struct
{
    window_struct        window;
    Point                mouse_position;
    Point                prev_mouse_position;
    view_struct          view;
    light_struct         lights[N_LIGHTS];
    action_table_struct  action_table;

    render_struct        render;
    object_struct        *objects;
    Point                centre_of_objects;

    int                  update_required;
} graphics_struct;

#define  DECL_EVENT_FUNCTION( f )  Status   f()
#define  DEF_EVENT_FUNCTION( f )   Status   f( graphics, event ) \
                                                graphics_struct  *graphics; \
                                                event_struct     *event;

#endif
