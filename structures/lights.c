
#include  <def_graphics.h>

public  void  initialize_lights( lights )
    light_struct  *lights;
{
    void  initialize_ambient();
    void  initialize_directional();
/*
    void  initialize_point();
    void  initialize_spot();
*/

    initialize_ambient( &lights[0], ON );
    initialize_directional( &lights[1], ON );
    initialize_directional( &lights[2], OFF );
    initialize_directional( &lights[3], OFF );
    initialize_directional( &lights[4], OFF );
    initialize_directional( &lights[5], OFF );
    initialize_directional( &lights[6], OFF );
    initialize_directional( &lights[7], OFF );
}

private  void  initialize_ambient( light, state )
    light_struct  *light;
    Boolean       state;
{
    light->state = state;

    light->light_type = AMBIENT_LIGHT;

    fill_Colour( light->colour, 0.4, 0.4, 0.4 );
}

private  void  initialize_directional( light, state )
    light_struct  *light;
    Boolean       state;
{
    light->state = state;

    light->light_type = DIRECTIONAL_LIGHT;

    fill_Colour( light->colour, 1.0, 1.0, 1.0 );
    fill_Vector( light->direction, 1.0, -1.0, -1.0 );
}
