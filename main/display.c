
#include  <def_display.h>

private  void  display_objects_recursive(
    window_struct                *window,
    Bitplane_types               bitplanes,
    int                          n_objects,
    object_struct                *object_list[],
    render_struct                *render,
    View_types                   view_type,
    Transform                    *transform,
    update_interrupted_struct    *interrupt,
    Boolean                      *past_last_object );

public  void  display_objects(
    window_struct               *window,
    object_struct               *object,
    update_interrupted_struct   *interrupt,
    Bitplane_types              bitplanes )
{
    Boolean              past_last_object;
    model_struct         *model;
    model_info_struct    *model_info;

    past_last_object = FALSE;

    model = get_model_ptr( object );
    model_info = get_model_info( model );

    if( model_info->bitplanes == bitplanes )
    {
        display_objects_recursive( window, bitplanes,
                                   model->n_objects,
                                   model->objects,
                                   &model_info->render,
                                   model_info->view_type,
                                   &model_info->transform,
                                   interrupt, &past_last_object );
    }
}

private  void  display_objects_recursive(
    window_struct                *window,
    Bitplane_types               bitplanes,
    int                          n_objects,
    object_struct                *object_list[],
    render_struct                *render,
    View_types                   view_type,
    Transform                    *transform,
    update_interrupted_struct    *interrupt,
    Boolean                      *past_last_object )
{
    int                  i;
    Boolean              object_is_continuing;
    model_struct         *model;
    model_info_struct    *model_info;

    set_render_info( window, render );
    G_set_view_type( window, view_type );
    G_push_transform( window, transform );

    for_less( i, 0, n_objects )
    {
        if( object_list[i]->visibility )
        {
            object_is_continuing = FALSE;

            if( interrupt != (update_interrupted_struct *) 0 &&
                interrupt->last_was_interrupted &&
                !(*past_last_object) &&
                object_list[i] == interrupt->object_interrupted )
            {
                object_is_continuing = TRUE;
                *past_last_object = TRUE;
            }

            if( interrupt == (update_interrupted_struct *) 0 ||
                !interrupt->last_was_interrupted ||
                *past_last_object ||
                object_list[i]->object_type == MODEL )
            {
/*
                if( interrupt != (update_interrupted_struct *) 0 &&
                    !interrupt->current_interrupted &&
                    current_realtime_seconds() > interrupt->interrupt_at &&
                    G_events_pending() )
                {
                    interrupt->current_interrupted = TRUE;
                    interrupt->object_interrupted = object_list[i];
                    interrupt->n_items_done = 0;
                    interrupt->next_item = 0;
                    break;
                }
*/

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

                        set_render_info( window, render );
                        G_set_view_type( window, view_type );
                    }
                }
                else
                {
                    draw_object( window, object_list[i] );
                }

                if( interrupt != (update_interrupted_struct *) 0 &&
                    interrupt->current_interrupted )
                {
                    if( object_list[i]->object_type != MODEL )
                    {
                        interrupt->object_interrupted = object_list[i];
                    }
                    break;
                }
            }
        }
    }

    G_pop_transform( window );
}
