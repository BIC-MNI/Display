#ifndef  DEF_GRAPHICS
#define  DEF_GRAPHICS

#include  <def_standard.h>
#include  <def_geometry.h>

#define  OFF  0
#define  ON   1

typedef  struct
{
    Boolean     perspective_flag;
    Point       origin;
    Vector      line_of_sight;
    Vector      horizontal;
    Vector      up;
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
    Vector        x_axis;
    Vector        y_axis;
    Point         position;
    Real          spot_exponent;
    Real          spot_angle;
} light_struct;

typedef  enum  { WIREFRAME_MODE, SHADED_MODE } render_modes;

typedef  enum  { FLAT_SHADING, GOURAUD_SHADING, PHONG_SHADING } shading_types;

typedef  struct
{
    render_modes    render_mode;
    shading_types   shading_type;
    Boolean         master_light_switch;
}  render_struct;

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

    int                  update_required;
} graphics_struct;

#define  DECL_EVENT_FUNCTION( f )  Status   f()
#define  DEF_EVENT_FUNCTION( f )   Status   f( graphics, event ) \
                                                graphics_struct  *graphics; \
                                                event_struct     *event;

#endif
