/**
 * \file init_surface.c
 * \brief Initialize, start, and stop surface extraction.
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

static  void  clear_surface_extraction(
    display_struct     *display );

/**
 * Initializes the data structures used by surface extraction. Surface
 * extraction runs asynchronously when it is operating, attached to the
 * timer event (i.e. NO_EVENT).
 */
void  initialize_surface_extraction(
    display_struct     *display )
{
    surface_extraction_struct   *surface_extraction;
    object_struct               *object;

    surface_extraction = &display->three_d.surface_extraction;

    surface_extraction->volume = NULL;
    surface_extraction->label_volume = NULL;

    surface_extraction->min_invalid_label = 0.0;
    surface_extraction->max_invalid_label = -1.0;

    object = create_object( POLYGONS );

    add_object_to_model( get_model_ptr(display->models[THREED_MODEL]),
                         object );

    surface_extraction->polygons = get_polygons_ptr( object );

    initialize_polygons( surface_extraction->polygons, WHITE, (VIO_Surfprop *) 0 );

    clear_surface_extraction( display );
}

static  void  clear_surface_extraction(
    display_struct     *display )
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &display->three_d.surface_extraction;

    surface_extraction->extraction_in_progress = FALSE;

    delete_polygons( surface_extraction->polygons );

    initialize_polygons( surface_extraction->polygons,
                         Extracted_surface_colour,
                         &Default_surface_property );
}

/**
 * Delete the extracted surface.
 */
void  delete_surface_extraction(
    display_struct    *display )
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &display->three_d.surface_extraction;

    if( surface_extraction->volume != NULL )
    {
        delete_edge_points( &surface_extraction->edge_points );
        delete_voxel_flags( &surface_extraction->voxel_state );
        if( surface_extraction->voxellate_flag )
        {
            delete_edge_points( &surface_extraction->faces_done );
            DELETE_QUEUE( surface_extraction->deleted_faces );
        }
        else
        {
            delete_voxel_done_flags( surface_extraction->voxel_done_flags );
            delete_voxel_queue( &surface_extraction->voxels_to_do );
        }
    }

    surface_extraction->volume = NULL;
    surface_extraction->label_volume = NULL;
}

/**
 * Stop surface extraction and delete the extracted surface.
 */
void  reset_surface_extraction( display_struct    *display )
{
    delete_surface_extraction( display );

    clear_surface_extraction( display );

    uninstall_surface_extraction( display );
}

/**
 * Called to give the surface extraction code a chance to clean up after
 * itself when the volume it is working on has been deleted.
 */
void  tell_surface_extraction_volume_deleted(
    display_struct    *display,
    VIO_Volume            volume,
    VIO_Volume            label_volume )
{
    display = get_three_d_window( display );
    if ( display == NULL )
        return;

    if( display->three_d.surface_extraction.volume == volume ||
        display->three_d.surface_extraction.volume == label_volume ||
        display->three_d.surface_extraction.label_volume == label_volume )
    {
        reset_surface_extraction( display );
    }
}

/**
 * Start surface extraction by installing the asynchronous event handler and
 * setting the flag.
 */
void start_surface_extraction( display_struct *display )
{
    install_surface_extraction( display );
    display->three_d.surface_extraction.extraction_in_progress = TRUE;
}

/**
 * Stop (or pause) surface extraction. Just clears the flag.
 */
void stop_surface_extraction( display_struct *display )
{
    display->three_d.surface_extraction.extraction_in_progress = FALSE;
}

/**
 * Sets the minimum and maximum label values for surface extraction.
 */
void  set_invalid_label_range_for_surface_extraction(
    display_struct  *display,
    int             min_label,
    int             max_label )
{
    display->three_d.surface_extraction.min_invalid_label = (VIO_Real) min_label;
    display->three_d.surface_extraction.max_invalid_label = (VIO_Real) max_label;
}
