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

static struct display_stats {
  int n_past_last_object;
  int n_interrupted;
  int n_continuing;
} stats = { 0, 0, 0 };

/**
 * Draw a single object.
 * \param window The window where the object is to be drawn.
 * \param object The object to draw.
 * \param interrupt Container for information need to restart the drawing
 * if it is interrupted.
 * \param past_last_object Pointer to boolean used to flag whether the current
 * object was interrupted and therefore should be redrawn.
 */
static void
draw_one_object(Gwindow                   window,
                object_struct             *object,
                update_interrupted_struct *interrupt,
                VIO_BOOL                  *past_last_object )
{
    VIO_BOOL  continuing;

    continuing = (G_get_drawing_interrupt_state(window) &&
                  interrupt != NULL &&
                  interrupt->last_was_interrupted);

    if (continuing)
      stats.n_continuing++;

    if( continuing && object == interrupt->object_interrupted )
    {
        *past_last_object = TRUE;
        stats.n_past_last_object++;
    }

    if( !continuing || *past_last_object )
    {
        draw_object( window, object );

        if( G_get_drawing_interrupt_state(window) &&
            interrupt != NULL &&
            G_get_interrupt_occurred( window ) )
        {
            stats.n_interrupted++;

            interrupt->current_interrupted = TRUE;
            G_clear_drawing_interrupt_flag( window );
            interrupt->object_interrupted = object;
        }
    }
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
 * \param interrupt Container for information need to restart the drawing
 * if it is interrupted.
 * \param past_last_object Pointer to boolean used to flag whether the current
 * object was interrupted and therefore should be redrawn.
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
    update_interrupted_struct    *interrupt,
    VIO_BOOL                     *past_last_object )
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
                                               interrupt,
                                               past_last_object );

                    /* Restore the previous state.
                     */
                    set_render_info( window, render );
                    G_set_view_type( window, view_type );
                    G_set_zbuffer_state( window,
                         view_type == WORLD_VIEW || view_type == MODEL_VIEW );
                }
            }
            else
            {
                draw_one_object( window, object_list[i], interrupt,
                                 past_last_object );
            }
        }

        if( interrupt != NULL && interrupt->current_interrupted )
            break;
    }

    G_pop_transform( window );
}

/**
 * Given a pointer to a window and a model object, draw the objects contained
 * within the model.
 * \param window The window to update.
 * \param object The model object whose contents we want to draw.
 * \param interrupt Container for information need to restart the drawing
 * if it is interrupted.
 * \param bitplanes Either NORMAL_PLANES or OVERLAY_PLANES.
 * \param past_last_object Pointer to boolean used to flag whether the current
 * object was interrupted and therefore should be redrawn.
 */
void  display_objects(
    Gwindow                     window,
    object_struct               *object,
    update_interrupted_struct   *interrupt,
    Bitplane_types              bitplanes,
    VIO_BOOL                    *past_last_object )
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
                                   interrupt, past_last_object );
    }
}
