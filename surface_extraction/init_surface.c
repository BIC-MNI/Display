
#include  <display.h>

private  void  clear_surface_extraction(
    display_struct     *display );

public  void  initialize_surface_extraction(
    display_struct     *display )
{
    surface_extraction_struct   *surface_extraction;
    object_struct               *object;

    surface_extraction = &display->three_d.surface_extraction;

    surface_extraction->x_voxel_max_distance = Default_x_voxel_max_distance;
    surface_extraction->y_voxel_max_distance = Default_y_voxel_max_distance;
    surface_extraction->z_voxel_max_distance = Default_z_voxel_max_distance;

    object = create_object( POLYGONS );

    add_object_to_model( get_model_ptr(display->models[THREED_MODEL]),
                         object );

    surface_extraction->polygons = get_polygons_ptr( object );

    initialize_polygons( surface_extraction->polygons, WHITE, (Surfprop *) 0 );

    create_bitlist( 0, &surface_extraction->voxels_queued );

    install_surface_extraction( display );

    clear_surface_extraction( display );
}

private  void  clear_surface_extraction(
    display_struct     *display )
{
    surface_extraction_struct   *surface_extraction;
    Volume                      volume;

    surface_extraction = &display->three_d.surface_extraction;

    surface_extraction->extraction_in_progress = FALSE;
    surface_extraction->n_voxels_with_surface = 0;

    initialize_edge_points( &surface_extraction->edge_points );

    delete_polygons( surface_extraction->polygons );

    initialize_voxel_queue( &surface_extraction->voxels_to_do );

    initialize_polygons( surface_extraction->polygons, Extracted_surface_colour,
                         &Default_surface_property );

    clear_voxel_flags( &surface_extraction->voxels_queued );

    if( get_slice_window_volume( display, &volume ) )
    {
        clear_voxel_done_flags( surface_extraction->voxel_done_flags,
                                get_n_voxels(volume) );
    }
    else
    {
        surface_extraction->voxel_done_flags = (unsigned_byte *) 0;
    }
}

private  void  free_surface_extraction(
    display_struct    *display )
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &display->three_d.surface_extraction;

    delete_edge_points( &surface_extraction->edge_points );

    delete_voxel_queue( &surface_extraction->voxels_to_do );
}

public  void  delete_surface_extraction(
    display_struct    *display )
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &display->three_d.surface_extraction;

    free_surface_extraction( display );

    delete_voxel_done_flags( surface_extraction->voxel_done_flags);

    delete_voxel_flags( &surface_extraction->voxels_queued );
}

public  void  reset_surface_extraction(
    display_struct    *display )
{
    free_surface_extraction( display );

    clear_surface_extraction( display );
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
