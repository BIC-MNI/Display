
#include  <display.h>

private  void  clear_surface_extraction(
    display_struct     *display );

public  void  initialize_surface_extraction(
    display_struct     *display )
{
    surface_extraction_struct   *surface_extraction;
    object_struct               *object;

    surface_extraction = &display->three_d.surface_extraction;

    surface_extraction->volume = (Volume) NULL;
    surface_extraction->label_volume = (Volume) NULL;

    surface_extraction->x_voxel_max_distance = Default_x_voxel_max_distance;
    surface_extraction->y_voxel_max_distance = Default_y_voxel_max_distance;
    surface_extraction->z_voxel_max_distance = Default_z_voxel_max_distance;

    surface_extraction->min_invalid_label = 0.0;
    surface_extraction->max_invalid_label = -1.0;

    object = create_object( POLYGONS );

    add_object_to_model( get_model_ptr(display->models[THREED_MODEL]),
                         object );

    surface_extraction->polygons = get_polygons_ptr( object );

    initialize_polygons( surface_extraction->polygons, WHITE, (Surfprop *) 0 );

    install_surface_extraction( display );

    clear_surface_extraction( display );
}

private  void  clear_surface_extraction(
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

public  void  delete_surface_extraction(
    display_struct    *display )
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &display->three_d.surface_extraction;

    if( surface_extraction->volume != (Volume) NULL )
    {
        delete_edge_points( &surface_extraction->edge_points );
        delete_voxel_queue( &surface_extraction->voxels_to_do );
        delete_voxel_done_flags( surface_extraction->voxel_done_flags);
        delete_voxel_flags( &surface_extraction->voxels_queued );
    }

    surface_extraction->volume = (Volume) NULL;
    surface_extraction->label_volume = (Volume) NULL;
}

public  void  reset_surface_extraction(
    display_struct    *display )
{
    delete_surface_extraction( display );

    clear_surface_extraction( display );
}

public  void  tell_surface_extraction_volume_deleted(
    display_struct    *display,
    Volume            volume,
    Volume            label_volume )
{
    display = get_three_d_window( display );

    if( display->three_d.surface_extraction.volume == volume ||
        display->three_d.surface_extraction.volume == label_volume ||
        display->three_d.surface_extraction.label_volume == label_volume )
    {
        reset_surface_extraction( display );
    }
}

public  void  initialize_surface_extraction_for_volume(
    display_struct    *display,
    Volume            volume,
    Volume            label_volume )
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &display->three_d.surface_extraction;
    if( surface_extraction->volume != volume ||
        surface_extraction->label_volume != label_volume )
    {
        reset_surface_extraction( display );
        surface_extraction->volume = volume;
        surface_extraction->label_volume = label_volume;
        surface_extraction->n_voxels_with_surface = 0;

        initialize_voxel_queue( &surface_extraction->voxels_to_do );
        initialize_edge_points( &surface_extraction->edge_points );
        initialize_voxel_flags( &surface_extraction->voxels_queued,
                                get_n_voxels(get_volume(display)) );
        initialize_voxel_done_flags( &surface_extraction->voxel_done_flags,
                                get_n_voxels(get_volume(display)) );
    }
}

public  void  start_surface_extraction(
    display_struct     *display )
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &display->three_d.surface_extraction;

    surface_extraction->extraction_in_progress = TRUE;
}

public  void  stop_surface_extraction(
    display_struct     *display )
{
    if( display->three_d.surface_extraction.extraction_in_progress )
        display->three_d.surface_extraction.extraction_in_progress = FALSE;
}

public  int  get_n_voxels(
    Volume            volume )
{
    int   n_voxels;
    int   sizes[N_DIMENSIONS];

    if( volume != (Volume) NULL )
    {
        get_volume_sizes( volume, sizes );

        n_voxels = (sizes[X] - 1) * (sizes[Y] - 1) * (sizes[Z] - 1);
    }
    else
    {
        n_voxels = 0;
    }

    return( n_voxels );
}

public  void  set_invalid_label_range_for_surface_extraction(
    display_struct  *display,
    int             min_label,
    int             max_label )
{
    display->three_d.surface_extraction.min_invalid_label = min_label;
    display->three_d.surface_extraction.max_invalid_label = max_label;
}
