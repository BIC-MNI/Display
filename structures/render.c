
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_render( render )
    render_struct  *render;
{
    render->render_mode = (render_modes) Initial_render_mode;
    render->shading_type = (shading_types) Initial_shading_type;
    render->master_light_switch = Initial_light_switch;
    render->backface_flag = Initial_backface_flag;
    render->two_sided_surface_flag = Initial_2_sided_flag;
}
