#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_priority_queue.h>
#include  <def_progress.h>

public  void  initialize_marker_segmentation( seg )
    marker_segment_struct *seg;
{
    seg->threshold_distance = Marker_threshold;
    seg->model = (model_struct *) NULL;
    seg->must_be_reinitialized = TRUE;
}

public  void  delete_marker_segmentation( seg )
    marker_segment_struct *seg;
{
    Status  status;

    if( seg->model != (model_struct *) NULL && seg->n_objects > 0 )
    {
        FREE2D( status, seg->distances );

        if( status ) {}
    }
}

public  Real  get_marker_threshold( seg )
    marker_segment_struct  *seg;
{
    return( seg->threshold_distance );
}

public  void  set_marker_threshold( seg, threshold )
    marker_segment_struct  *seg;
    Real                   threshold;
{
    if( threshold >= 0.0 )
    {
        seg->threshold_distance = threshold;
        seg->must_be_reinitialized = TRUE;
    }
}

private  Boolean  markers_are_neighbours( seg, threshold_distance,
                                          i, j, closest_dists, dist )
    marker_segment_struct  *seg;
    Real                   threshold_distance;
    int                    i;
    int                    j;
    Real                   closest_dists[];
    Real                   *dist;
{
    Boolean   neigh_flag, dist_to_marker;

    dist_to_marker = seg->distances[i][j];

    neigh_flag = (dist_to_marker <= threshold_distance);

    if( neigh_flag )
    {
        *dist = MAX( dist_to_marker, closest_dists[i] );
    }

    return( neigh_flag );
#ifdef OLD
    Boolean   neigh_flag;

    *dist = seg->distances[i][j];

    neigh_flag = (*dist <= Marker_threshold);

    if( neigh_flag && !Use_marker_distances )
        *dist = 1.0;

    return( neigh_flag );
#endif
}


private  void  classify( seg, threshold_distance, model, n_indices, indices )
    marker_segment_struct    *seg;
    Real                     threshold_distance;
    model_struct             *model;
    int                      n_indices;
    int                      indices[];
{
    Status                           status;
    Real                             *closest_dist;
    Boolean                          *in_queue;
    int                              i, ind, marker_index;
    marker_struct                    *marker1, *marker2;
    Real                             dist;
    PRIORITY_QUEUE_STRUCT( int )     queue;

    ALLOC( status, closest_dist, model->n_objects );
    ALLOC( status, in_queue, model->n_objects );

    INITIALIZE_PRIORITY_QUEUE( queue );

    for_less( i, 0, model->n_objects )
    {
        closest_dist[i] = -1.0;
        in_queue[i] = FALSE;
    }

    if( n_indices == 0 )
    {
        for_less( i, 0, model->n_objects )
        {
            if( model->object_list[i]->object_type == MARKER )
            {
                marker1 = model->object_list[i]->ptr.marker;

                if( marker1->structure_id < Marker_segment_id )
                {
                    INSERT_IN_PRIORITY_QUEUE( status, queue, i, 0.0 );
                    closest_dist[i] = 0.0;
                    in_queue[i] = TRUE;
                }
            }
        }
    }
    else
    {
        for_less( i, 0, n_indices )
        {
            ind = indices[i];
            if( model->object_list[ind]->object_type == MARKER )
            {
                marker1 = model->object_list[ind]->ptr.marker;

                INSERT_IN_PRIORITY_QUEUE( status, queue, ind, 0.0 );
                closest_dist[ind] = 0.0;
                in_queue[ind] = TRUE;
            }
        }
    }

    while( !IS_PRIORITY_QUEUE_EMPTY(queue) )
    {
        REMOVE_FROM_PRIORITY_QUEUE( queue, marker_index, dist );
        in_queue[marker_index] = FALSE;

        marker1 = model->object_list[marker_index]->ptr.marker;

        for_less( i, 0, model->n_objects )
        {
            if( markers_are_neighbours( seg, threshold_distance,
                                        marker_index, i, closest_dist,
                                        &dist ) &&
                (closest_dist[i] < 0.0 || dist < closest_dist[i]) )
            {
                marker2 = model->object_list[i]->ptr.marker;
                marker2->structure_id = marker1->structure_id;
                if( marker2->structure_id < Marker_segment_id )
                    marker2->structure_id += Marker_segment_id;
                marker2->colour = marker1->colour;
                closest_dist[i] = dist;
                if( !in_queue[i] )
                {
                    INSERT_IN_PRIORITY_QUEUE( status, queue, i,
                                              -closest_dist[i] );
                    in_queue[i] = TRUE;
                }
            }
        }
    }

    DELETE_PRIORITY_QUEUE( status, queue );

    FREE( status, closest_dist );
    FREE( status, in_queue );
}

private  void  set_up_new_model( seg, model )
    marker_segment_struct    *seg;
    model_struct             *model;
{
    Status                status;
    int                   i, j;
    marker_struct         *marker1, *marker2;
    Real                  dist;
    Real                  distance_between_points();

    delete_marker_segmentation( seg );

    seg->model = model;
    seg->n_objects = model->n_objects;

    ALLOC2D( status, seg->distances, model->n_objects, model->n_objects );

    for_less( i, 0, model->n_objects )
    {
        seg->distances[i][i] = 0.0;
        if( model->object_list[i]->object_type == MARKER )
        {
            marker1 = model->object_list[i]->ptr.marker;

            for_less( j, i+1, model->n_objects )
            {
                if( model->object_list[j]->object_type == MARKER )
                {
                    marker2 = model->object_list[j]->ptr.marker;

                    dist = distance_between_points( &marker1->position,
                                                    &marker2->position );
                    seg->distances[i][j] = (float) dist;
                    seg->distances[j][i] = (float) dist;
                }
            }
        }
    }
}

public  void  make_guess_classification( seg, threshold_dist, model )
    marker_segment_struct    *seg;
    Real                     threshold_dist;
    model_struct             *model;
{
    Status                status;
    int                   i, n_ids, *marker_indices;
    marker_struct         *marker;
    Real                  distance_between_points();
    static  Colour        colours[] = { {RED_COL}, {GREEN_COL}, {BLUE_COL},
                                        {CYAN_COL}, {MAGENTA_COL},
                                        {YELLOW_COL}, {WHITE_COL},
                                        {LIGHT_BLUE_COL}, {ORANGE_COL},
                                        {BLACK_COL}, {BLUE_VIOLET_COL},
                                        {DARK_SEA_GREEN_COL}, {NAVY_BLUE_COL} };
    progress_struct       progress;

    n_ids = 0;

    for_less( i, 0, model->n_objects )
    {
        if( model->object_list[i]->object_type == MARKER )
        {
            marker = model->object_list[i]->ptr.marker;

            if( marker->structure_id >= Marker_segment_id )
                marker->structure_id -= Marker_segment_id;

        }
    }

    initialize_progress_report( &progress, FALSE, model->n_objects,
                                "Initial Segmenting Guess" );

    for_less( i, 0, model->n_objects )
    {
        if( model->object_list[i]->object_type == MARKER )
        {
            marker = model->object_list[i]->ptr.marker;

            if( marker->structure_id < Marker_segment_id )
            {
                marker->structure_id = Marker_segment_id + n_ids + 1;
                marker->colour = colours[n_ids % SIZEOF_STATIC_ARRAY(colours)];

                ADD_ELEMENT_TO_ARRAY( status, n_ids, marker_indices, i,
                                      DEFAULT_CHUNK_SIZE );

                classify( seg, threshold_dist, model, n_ids, marker_indices );
            }
        }

        update_progress_report( &progress, i+1 );
    }

    terminate_progress_report( &progress );

    if( n_ids > 0 )
        FREE( status, marker_indices );
}

private  Real  get_threshold_distance( graphics )
    graphics_struct  *graphics;
{
    Real            threshold;
    volume_struct   *volume;
    Point           position0, position1;
    int             nx, ny, nz;
    void            get_volume_size();
    void            convert_talairach_to_voxel();
    void            convert_voxel_to_point();

    threshold = graphics->three_d.marker_segmentation.threshold_distance;

    if( get_slice_window_volume( graphics, &volume ) )
    {
        get_volume_size( volume, &nx, &ny, &nz );

        convert_talairach_to_voxel( 0.0, 0.0, 0.0, nx, ny, nz,
                                    &Point_x(position0),
                                    &Point_y(position0),
                                    &Point_z(position0) );

        convert_voxel_to_point( volume,
                                Point_x(position0),
                                Point_y(position0),
                                Point_z(position0), &position0 );

        convert_talairach_to_voxel( 1.0, 1.0, 1.0, nx, ny, nz,
                                    &Point_x(position1),
                                    &Point_y(position1),
                                    &Point_z(position1) );

        convert_voxel_to_point( volume,
                                Point_x(position1),
                                Point_y(position1),
                                Point_z(position1), &position1 );

        threshold *= (ABS(Point_x(position0) - Point_x(position1)) +
                      ABS(Point_y(position0) - Point_y(position1)) +
                      ABS(Point_z(position0) - Point_z(position1))) / 3.0;
    }

    return( threshold );
}

public  void  segment_markers( graphics, model )
    graphics_struct  *graphics;
    model_struct     *model;
{
    Real                      threshold_distance;
    Real                      get_threshold_distance();
    marker_segment_struct     *seg;

    seg = &graphics->three_d.marker_segmentation;

    if( seg->model != model || seg->n_objects != model->n_objects )
    {
        set_up_new_model( seg, model );
        seg->must_be_reinitialized = TRUE;
    }

    threshold_distance = get_threshold_distance( graphics );

    if( seg->must_be_reinitialized )
    {
        make_guess_classification( seg, threshold_distance, model );
        seg->must_be_reinitialized = FALSE;
    }
    else
        classify( seg, threshold_distance, model, 0, (int *) NULL );
}
