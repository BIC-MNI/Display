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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/structures/lights.c,v 1.6 1995/10/19 15:52:31 david Exp $";
#endif


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
