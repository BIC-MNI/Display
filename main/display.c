
#include  <def_graphics.h>

public  void  display_objects( window, object, interrupt, bitplanes )
    window_struct               *window;
    object_struct               *object;
    update_interrupted_struct   *interrupt;
    Bitplane_types              bitplanes;
{
    void           display_objects_recursive();
    Boolean        past_last_object;

    past_last_object = FALSE;

    if( object->ptr.model->bitplanes == bitplanes )
    {
        display_objects_recursive( window, bitplanes,
                                   object->ptr.model->n_objects,
                                   object->ptr.model->object_list,
                                   &object->ptr.model->render,
                                   object->ptr.model->view_type,
                                   &object->ptr.model->transform,
                                   interrupt, &past_last_object );
    }
}

private  void  display_objects_recursive( window, bitplanes,
                                          n_objects, object_list,
                                          render, view_type, transform,
                                          interrupt, past_last_object )
    window_struct                *window;
    Bitplane_types               bitplanes;
    int                          n_objects;
    object_struct                *object_list[];
    render_struct                *render;
    view_types                   view_type;
    Transform                    *transform;
    update_interrupted_struct    *interrupt;
    Boolean                      *past_last_object;
{
    int            i;
    model_struct   *model;
    void           G_set_view_type();
    void           G_set_render();
    void           G_draw_text();
    void           G_draw_lines();
    void           G_draw_pixels();
    void           G_draw_polygons();
    void           G_draw_volume();
    void           G_push_transform();
    void           G_pop_transform();
    Real           current_realtime_seconds();
    Boolean        object_is_continuing;
    Boolean        G_events_pending();

    G_set_render( window, render );
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

                switch( object_list[i]->object_type )
                {
                case MODEL:
                    model = object_list[i]->ptr.model;

                    if( model->bitplanes == bitplanes )
                    {
                        display_objects_recursive( window, bitplanes,
                                                   model->n_objects,
                                                   model->object_list,
                                                   &model->render,
                                                   model->view_type,
                                                   &model->transform,
                                                   interrupt,
                                                   past_last_object );

                        G_set_render( window, render );
                        G_set_view_type( window, view_type );
                    }
                    break;

                case LINES:
                    G_draw_lines( window, object_list[i]->ptr.lines,
                                  interrupt, object_is_continuing );
                    break;

                case PIXELS:
                    G_draw_pixels( window, object_list[i]->ptr.pixels );
                    break;

                case POLYGONS:
                    G_draw_polygons( window, object_list[i]->ptr.polygons,
                                     render, interrupt,
                                     object_is_continuing );
                    break;

                case TEXT:
                    G_draw_text( window, object_list[i]->ptr.text, render );
                    break;

                case VOLUME:
                    G_draw_volume( window, object_list[i]->ptr.volume, render,
                                   interrupt, object_is_continuing );
                    break;
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
