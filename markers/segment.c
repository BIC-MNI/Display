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
    int     i;

    if( seg->model != (model_struct *) NULL && seg->n_objects > 0 )
    {
        for_less( i, 0, seg->n_objects )
            status = delete_bitlist( &seg->connected[i] );

        FREE( status, seg->connected );

        if( status ) {}
    }
}

private  Boolean  markers_close_enough( marker1, marker2, threshold )
    marker_struct  *marker1;
    marker_struct  *marker2;
    Real           threshold;
{
    Real   distance_between_points();

    return( distance_between_points( &marker1->position, &marker2->position) <=
            threshold );
}

public  void  segment_markers( graphics, model )
    graphics_struct  *graphics;
    model_struct     *model;
{
    Status                           status;
    Boolean                          *marked;
    int                              i, j, marker_index;
    marker_struct                    *marker1, *marker2;
    Real                             dist;
    marker_segment_struct            *seg;
    PRIORITY_QUEUE_STRUCT( int )     queue;

    seg = &graphics->three_d.marker_segmentation;

    if( seg->model != model || seg->n_objects != model->n_objects )
    {
        delete_marker_segmentation( seg );

        seg->model = model;
        seg->n_objects = model->n_objects;

        ALLOC( status, seg->connected, model->n_objects );

        for_less( i, 0, model->n_objects )
            status = create_bitlist( model->n_objects, &seg->connected[i] );

        for_less( i, 0, model->n_objects )
        {
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

                        if( markers_close_enough( marker1, marker2,
                                                  Marker_threshold ) )
                        {
                            set_bitlist_bit( &seg->connected[i], j, ON );
                            set_bitlist_bit( &seg->connected[j], i, ON );
                        }
                    }
                }
            }
        }
    }

    ALLOC( status, marked, model->n_objects );

    INITIALIZE_PRIORITY_QUEUE( queue );

    for_less( i, 0, model->n_objects )
    {
        marked[i] = FALSE;
        if( model->object_list[i]->object_type == MARKER )
        {
            marker1 = model->object_list[i]->ptr.marker;

            if( marker1->structure_id < Marker_segment_id )
            {
                INSERT_IN_PRIORITY_QUEUE( status, queue, i, 0 );
                marked[i] = TRUE;
            }
        }
    }

    while( !IS_PRIORITY_QUEUE_EMPTY(queue) )
    {
        REMOVE_FROM_PRIORITY_QUEUE( queue, marker_index, dist );

        marker1 = model->object_list[marker_index]->ptr.marker;

        for_less( i, 0, model->n_objects )
        {
            if( !marked[i] &&
                get_bitlist_bit( &seg->connected[marker_index], i ) )
            {
                marker2 = model->object_list[i]->ptr.marker;
                marker2->structure_id = marker1->structure_id;
                if( marker2->structure_id < Marker_segment_id )
                    marker2->structure_id += Marker_segment_id;
                marker2->colour = marker1->colour;
                INSERT_IN_PRIORITY_QUEUE( status, queue, i, dist-1 );
                marked[i] = TRUE;
            }
        }
    }

    DELETE_PRIORITY_QUEUE( status, queue );

    FREE( status, marked );
}
