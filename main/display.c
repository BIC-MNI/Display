/**
 * \file display.c
 * \brief High-level functions for painting the objects that make up the
 * user interface.
 *
 * \copyright
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
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

/**
 * Draw a single object.
 * \param window The window where the object is to be drawn.
 * \param object The object to draw.
 */
static void
draw_one_object(Gwindow                   window,
                object_struct             *object )
{
    draw_object( window, object );
}

/**
 * Display the objects, descending into any model objects and
 * recursively displaying their contents.
 *
 * \param window The window to update.
 * \param bitplanes Either NORMAL_PLANES or OVERLAY_PLANES.
 * \param n_objects The number of objects in the list.
 * \param object_list An array of objects to draw.
 * \param render A render_struct controlling the rendering of this model.
 * \param view_type The View_type of the model.
 * \param transform A transform which is applied to this model.
 * \param opaque_flag If bit 0 set, display opaque objects. If bit 1 set,
 * display translucent objects.
 */
static  void
display_objects_recursive(
    Gwindow                      window,
    Bitplane_types               bitplanes,
    int                          n_objects,
    object_struct                *object_list[],
    render_struct                *render,
    View_types                   view_type,
    VIO_Transform                *transform,
    int                          opaque_flag )
{
    int                  i;
    model_struct         *model;
    model_info_struct    *model_info;

    set_render_info( window, render );
    G_set_view_type( window, view_type );
    G_set_zbuffer_state( window,
                         view_type == WORLD_VIEW || view_type == MODEL_VIEW );
    G_push_transform( window, transform );

    for_less( i, 0, n_objects )
    {
        if( object_list[i]->visibility )
        {
            if( object_list[i]->object_type == MODEL )
            {
                model = get_model_ptr( object_list[i] );
                model_info = get_model_info( model );

                if( model_info->bitplanes == bitplanes )
                {
                    display_objects_recursive( window, bitplanes,
                                               model->n_objects,
                                               model->objects,
                                               &model_info->render,
                                               model_info->view_type,
                                               &model_info->transform,
                                               opaque_flag );

                    /* Restore the previous state.
                     */
                    set_render_info( window, render );
                    G_set_view_type( window, view_type );
                    G_set_zbuffer_state( window,
                         view_type == WORLD_VIEW || view_type == MODEL_VIEW );
                }
            }
            else if ( object_list[i]->object_type == POLYGONS )
            {
                polygons_struct *polygons = get_polygons_ptr( object_list[i] );
                if (((opaque_flag & DISPLAY_OPAQUE) != 0 &&
                     Surfprop_t(polygons->surfprop) == 1.0) ||
                    ((opaque_flag & DISPLAY_TRANSLUCENT) != 0 &&
                     Surfprop_t(polygons->surfprop) < 1.0))
                {
                    draw_one_object( window, object_list[i] );
                }
            }
            else if ( object_list[i]->object_type == QUADMESH )
            {
                quadmesh_struct *quadmesh = get_quadmesh_ptr( object_list[i] );
                if (((opaque_flag & DISPLAY_OPAQUE) != 0 &&
                     Surfprop_t(quadmesh->surfprop) == 1.0) ||
                    ((opaque_flag & DISPLAY_TRANSLUCENT) != 0 &&
                     Surfprop_t(quadmesh->surfprop) < 1.0))
                {
                    draw_one_object( window, object_list[i] );
                }
            }
            else if ((opaque_flag & DISPLAY_OPAQUE) != 0)
            {
                draw_one_object( window, object_list[i] );
            }
        }
    }

    G_pop_transform( window );
}

/**
 * Given a pointer to a window and a model object, draw the objects contained
 * within the model.
 * \param window The window to update.
 * \param object The model object whose contents we want to draw.
 * \param bitplanes Either NORMAL_PLANES or OVERLAY_PLANES.
 */
void  display_objects(
    Gwindow                     window,
    object_struct               *object,
    Bitplane_types              bitplanes,
    VIO_BOOL                    opaque_flag )
{
    model_struct         *model;
    model_info_struct    *model_info;

    model = get_model_ptr( object );
    model_info = get_model_info( model );

    if( get_object_visibility( object ) && model_info->bitplanes == bitplanes )
    {
        display_objects_recursive( window, bitplanes,
                                   model->n_objects,
                                   model->objects,
                                   &model_info->render,
                                   model_info->view_type,
                                   &model_info->transform,
                                   opaque_flag );
    }
}
