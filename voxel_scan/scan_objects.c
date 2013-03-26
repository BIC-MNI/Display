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

  void  scan_object_to_current_volume(
    display_struct   *slice_window,
    object_struct    *object )
{
    int                     label;
    object_struct           *current_object;
    object_traverse_struct  object_traverse;

    label = get_current_paint_label( slice_window );

    initialize_object_traverse( &object_traverse, FALSE, 1, &object );

    while( get_next_object_traverse(&object_traverse,&current_object) )
    {
        scan_object_to_volume( current_object,
                               get_volume(slice_window),
                               get_label_volume(slice_window),
                               label,
                               Max_polygon_scan_distance );
    }
}
