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
#include  <priority_queue.h>

public  void  initialize_marker_segmentation(
    marker_segment_struct *seg )
{
    seg->threshold_distance = Marker_threshold;
    seg->model = (model_struct *) NULL;
    seg->must_be_reinitialized = TRUE;
}

public  void  delete_marker_segmentation(
    marker_segment_struct *seg )
{
    if( seg->model != (model_struct *) NULL && seg->n_objects > 0 )
    {
        FREE2D( seg->distances );
    }
}

public  Real  get_marker_threshold(
    marker_segment_struct  *seg )
{
    return( seg->threshold_distance );
}

public  void  set_marker_threshold(
    marker_segment_struct  *seg,
    Real                   threshold )
{
    if( threshold >= 0.0 )
    {
        seg->threshold_distance = threshold;
        seg->must_be_reinitialized = TRUE;
    }
}

private  BOOLEAN  markers_are_neighbours(
    marker_segment_struct  *seg,
    Real                   threshold_distance,
    int                    i,
    int                    j,
    Real                   closest_dists[],
    Real                   *dist )
{
    BOOLEAN   neigh_flag, dist_to_marker;

    dist_to_marker = seg->distances[i][j];

    neigh_flag = (dist_to_marker <= threshold_distance);

    if( neigh_flag )
    {
        *dist = MAX( dist_to_marker, closest_dists[i] );
    }

    return( neigh_flag );
#ifdef OLD
    BOOLEAN   neigh_flag;

    *dist = seg->distances[i][j];

    neigh_flag = (*dist <= Marker_threshold);

    if( neigh_flag && !Use_marker_distances )
        *dist = 1.0;

    return( neigh_flag );
#endif
}


private  void  classify(
    marker_segment_struct    *seg,
    Real                     threshold_distance,
    model_struct             *model,
    int                      n_indices,
    int                      indices[] )
{
    Real                             *closest_dist;
    BOOLEAN                          *in_queue;
    int                              i, ind, marker_index;
    marker_struct                    *marker1, *marker2;
    Real                             dist;
    PRIORITY_QUEUE_STRUCT( int )     queue;

    ALLOC( closest_dist, model->n_objects );
    ALLOC( in_queue, model->n_objects );

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
            if( model->objects[i]->object_type == MARKER )
            {
                marker1 = get_marker_ptr(model->objects[i]);

                if( marker1->structure_id < Marker_segment_id )
                {
                    INSERT_IN_PRIORITY_QUEUE( queue, i, 0.0 );
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
            if( model->objects[ind]->object_type == MARKER )
            {
                marker1 = get_marker_ptr(model->objects[ind]);

                INSERT_IN_PRIORITY_QUEUE( queue, ind, 0.0 );
                closest_dist[ind] = 0.0;
                in_queue[ind] = TRUE;
            }
        }
    }

    while( !IS_PRIORITY_QUEUE_EMPTY(queue) )
    {
        REMOVE_FROM_PRIORITY_QUEUE( queue, marker_index, dist );
        in_queue[marker_index] = FALSE;

        marker1 = get_marker_ptr( model->objects[marker_index] );

        for_less( i, 0, model->n_objects )
        {
            if( markers_are_neighbours( seg, threshold_distance,
                                        marker_index, i, closest_dist,
                                        &dist ) &&
                (closest_dist[i] < 0.0 || dist < closest_dist[i]) )
            {
                marker2 = get_marker_ptr( model->objects[i] );
                marker2->structure_id = marker1->structure_id;
                if( marker2->structure_id < Marker_segment_id )
                    marker2->structure_id += Marker_segment_id;
                marker2->colour = marker1->colour;
                marker2->patient_id = marker1->patient_id;
                replace_string( &marker2->label,
                                create_string(marker1->label) );
                closest_dist[i] = dist;
                if( !in_queue[i] )
                {
                    INSERT_IN_PRIORITY_QUEUE( queue, i, -closest_dist[i] );
                    in_queue[i] = TRUE;
                }
            }
        }
    }

    DELETE_PRIORITY_QUEUE( queue );

    FREE( closest_dist );
    FREE( in_queue );
}

private  void  set_up_new_model(
    marker_segment_struct    *seg,
    model_struct             *model )
{
    int                   i, j;
    marker_struct         *marker1, *marker2;
    Real                  dist;

    delete_marker_segmentation( seg );

    seg->model = model;
    seg->n_objects = model->n_objects;

    ALLOC2D( seg->distances, model->n_objects, model->n_objects );

    for_less( i, 0, model->n_objects )
    {
        seg->distances[i][i] = 0.0;
        if( model->objects[i]->object_type == MARKER )
        {
            marker1 = get_marker_ptr( model->objects[i] );

            for_less( j, i+1, model->n_objects )
            {
                if( model->objects[j]->object_type == MARKER )
                {
                    marker2 = get_marker_ptr( model->objects[j] );

                    dist = distance_between_points( &marker1->position,
                                                    &marker2->position );
                    seg->distances[i][j] = (float) dist;
                    seg->distances[j][i] = (float) dist;
                }
            }
        }
    }
}

public  void  make_guess_classification(
    marker_segment_struct    *seg,
    Real                     threshold_dist,
    model_struct             *model )
{
    int                   i, n_ids, *marker_indices;
    marker_struct         *marker;
    static  BOOLEAN       first = TRUE;
    static  Colour        colours[13];
    progress_struct       progress;

    if( first )
    {
        first = FALSE;
        i = 0;

        colours[i++] = RED;
        colours[i++] = GREEN;
        colours[i++] = BLUE;
        colours[i++] = CYAN;
        colours[i++] = MAGENTA;
        colours[i++] = YELLOW;
        colours[i++] = WHITE;
        colours[i++] = LIGHT_BLUE;
        colours[i++] = ORANGE;
        colours[i++] = BLACK;
        colours[i++] = BLUE_VIOLET;
        colours[i++] = DARK_SEA_GREEN;
        colours[i++] = NAVY_BLUE;
    }

    n_ids = 0;

    for_less( i, 0, model->n_objects )
    {
        if( model->objects[i]->object_type == MARKER )
        {
            marker = get_marker_ptr( model->objects[i] );

            if( marker->structure_id >= Marker_segment_id )
                marker->structure_id -= Marker_segment_id;

        }
    }

    initialize_progress_report( &progress, FALSE, model->n_objects,
                                "Initial Segmenting Guess" );

    for_less( i, 0, model->n_objects )
    {
        if( model->objects[i]->object_type == MARKER )
        {
            marker = get_marker_ptr( model->objects[i] );

            if( marker->structure_id < Marker_segment_id )
            {
                marker->structure_id = Marker_segment_id + n_ids + 1;
                marker->colour = colours[n_ids % SIZEOF_STATIC_ARRAY(colours)];

                ADD_ELEMENT_TO_ARRAY( marker_indices, n_ids, i,
                                      DEFAULT_CHUNK_SIZE );

                classify( seg, threshold_dist, model, n_ids, marker_indices );
            }
        }

        update_progress_report( &progress, i+1 );
    }

    terminate_progress_report( &progress );

    if( n_ids > 0 )
        FREE( marker_indices );
}

private  Real  get_threshold_distance(
    display_struct   *display )
{
    Real            threshold;

    threshold = display->three_d.marker_segmentation.threshold_distance;

    return( threshold );
}

public  void  segment_markers(
    display_struct   *display,
    model_struct     *model )
{
    Real                      threshold_distance;
    marker_segment_struct     *seg;

    seg = &display->three_d.marker_segmentation;

    if( seg->model != model || seg->n_objects != model->n_objects )
    {
        set_up_new_model( seg, model );
        seg->must_be_reinitialized = TRUE;
    }

    threshold_distance = get_threshold_distance( display );

    if( seg->must_be_reinitialized )
    {
        make_guess_classification( seg, threshold_distance, model );
        seg->must_be_reinitialized = FALSE;
    }
    else
        classify( seg, threshold_distance, model, 0, (int *) NULL );
}
