#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_priority_queue.h>

public  void  initialize_marker_segmentation( seg )
    marker_segment_struct *seg;
{
    seg->model = (model_struct *) NULL;
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

private  Boolean  markers_are_neighbours( seg, i, j, closest_dists, dist )
    marker_segment_struct  *seg;
    int                    i;
    int                    j;
    Real                   closest_dists[];
    Real                   *dist;
{
    Boolean   neigh_flag, dist_to_marker;

    dist_to_marker = seg->distances[i][j];

    neigh_flag = (dist_to_marker <= Marker_threshold);

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

public  void  segment_markers( graphics, model )
    graphics_struct  *graphics;
    model_struct     *model;
{
    Status                           status;
    Real                             *closest_dist;
    Boolean                          *in_queue;
    int                              i, j, marker_index;
    marker_struct                    *marker1, *marker2;
    Real                             dist;
    Real                             distance_between_points();
    marker_segment_struct            *seg;
    PRIORITY_QUEUE_STRUCT( int )     queue;

    seg = &graphics->three_d.marker_segmentation;

    if( seg->model != model || seg->n_objects != model->n_objects )
    {
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

                if( marker1->structure_id < Marker_segment_id )
                    marker1->structure_id += Marker_segment_id;

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

    ALLOC( status, closest_dist, model->n_objects );
    ALLOC( status, in_queue, model->n_objects );

    INITIALIZE_PRIORITY_QUEUE( queue );

    for_less( i, 0, model->n_objects )
    {
        closest_dist[i] = -1.0;
        in_queue[i] = FALSE;
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

    while( !IS_PRIORITY_QUEUE_EMPTY(queue) )
    {
        REMOVE_FROM_PRIORITY_QUEUE( queue, marker_index, dist );
        in_queue[marker_index] = FALSE;

        marker1 = model->object_list[marker_index]->ptr.marker;

        for_less( i, 0, model->n_objects )
        {
            if( markers_are_neighbours( seg, marker_index, i, closest_dist,
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
