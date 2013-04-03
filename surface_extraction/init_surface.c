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

static  void  clear_surface_extraction(
    display_struct     *display );

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

    install_surface_extraction( display );

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

  void  reset_surface_extraction(
    display_struct    *display )
{
    delete_surface_extraction( display );

    clear_surface_extraction( display );
}

  void  tell_surface_extraction_volume_deleted(
    display_struct    *display,
    VIO_Volume            volume,
    VIO_Volume            label_volume )
{
    display = get_three_d_window( display );

    if( display->three_d.surface_extraction.volume == volume ||
        display->three_d.surface_extraction.volume == label_volume ||
        display->three_d.surface_extraction.label_volume == label_volume )
    {
        reset_surface_extraction( display );
    }
}

  void  start_surface_extraction(
    display_struct     *display )
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &display->three_d.surface_extraction;

    surface_extraction->extraction_in_progress = TRUE;
}

  void  stop_surface_extraction(
    display_struct     *display )
{
    if( display->three_d.surface_extraction.extraction_in_progress )
        display->three_d.surface_extraction.extraction_in_progress = FALSE;
}

  int  get_n_voxels(
    VIO_Volume            volume )
{
    int   n_voxels;
    int   sizes[VIO_N_DIMENSIONS];

    if( volume != NULL )
    {
        get_volume_sizes( volume, sizes );

        n_voxels = sizes[VIO_X] * sizes[VIO_Y] * sizes[VIO_Z];
    }
    else
    {
        n_voxels = 0;
    }

    return( n_voxels );
}

  void  set_invalid_label_range_for_surface_extraction(
    display_struct  *display,
    int             min_label,
    int             max_label )
{
    display->three_d.surface_extraction.min_invalid_label = (VIO_Real) min_label;
    display->three_d.surface_extraction.max_invalid_label = (VIO_Real) max_label;
}
