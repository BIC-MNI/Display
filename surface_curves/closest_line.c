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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_curves/closest_line.c,v 1.3 1995-10-19 15:52:34 david Exp $";
#endif


#include  <display.h>

public  BOOLEAN  find_closest_line_point_to_point(
    display_struct    *display,
    Point             *point,
    Point             *closest_line_point )
{
    BOOLEAN                  found;
    int                      i;
    Real                     dist, closest_dist;
    lines_struct             *lines;
    object_struct            *current_object;
    object_traverse_struct   object_traverse;

    found = TRUE;
    closest_dist = 1.0e30;

    initialize_object_traverse( &object_traverse, N_MODELS, display->models );

    while( get_next_object_traverse(&object_traverse,&current_object) )
    {
        if( current_object->visibility &&
            get_object_type(current_object) == LINES )
        {
            lines = get_lines_ptr( current_object );

            for_less( i, 0, lines->n_points )
            {
                dist = distance_between_points( point, &lines->points[i] );
                if( !found || dist < closest_dist )
                {
                    found = TRUE;
                    closest_dist = dist;
                    *closest_line_point = lines->points[i];
                }
            }
        }
    }

    return( found );
}
