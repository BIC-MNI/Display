
#include  <display.h>

private  void  initialize_directional(
    light_struct  *light,
    BOOLEAN       state );

public  void  initialize_lights(
    light_struct  *lights )
{
    initialize_directional( &lights[0], ON );
    initialize_directional( &lights[1], OFF );
    initialize_directional( &lights[2], OFF );
    initialize_directional( &lights[3], OFF );
    initialize_directional( &lights[4], OFF );
    initialize_directional( &lights[5], OFF );
    initialize_directional( &lights[6], OFF );
    initialize_directional( &lights[7], OFF );
}

private  void  initialize_directional(
    light_struct  *light,
    BOOLEAN       state )
{
    light->state = state;

    light->light_type = DIRECTIONAL_LIGHT;

    light->colour = WHITE;
    fill_Vector( light->direction, 1.0, -1.0, -1.0 );
}
