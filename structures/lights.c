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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

static  void  initialize_directional(
    light_struct  *light,
    VIO_BOOL       state );

  void  initialize_lights(
    light_struct  *lights )
{
    initialize_directional( &lights[0], TRUE );
    initialize_directional( &lights[1], FALSE );
    initialize_directional( &lights[2], FALSE );
    initialize_directional( &lights[3], FALSE );
    initialize_directional( &lights[4], FALSE );
    initialize_directional( &lights[5], FALSE );
    initialize_directional( &lights[6], FALSE );
    initialize_directional( &lights[7], FALSE );
}

static  void  initialize_directional(
    light_struct  *light,
    VIO_BOOL       state )
{
    light->state = state;

    light->light_type = DIRECTIONAL_LIGHT;

    light->colour = WHITE;
    fill_Vector( light->direction, 1.0, -1.0, -1.0 );
}
