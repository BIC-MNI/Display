
#include  <def_graphics.h>

public  void  display_objects( window, object, interrupt )
    window_struct               *window;
    object_struct               *object;
    update_interrupted_struct   *interrupt;
{
    void           G_set_render();
    void           G_set_view_type();
    void           display_objects_recursive();
    Boolean        past_last_object;

    past_last_object = FALSE;

    display_objects_recursive( window,
                               object->ptr.model->n_objects,
                               object->ptr.model->object_list,
                               &object->ptr.model->render,
                               object->ptr.model->view_type,
                               interrupt, &past_last_object );
}

private  void  display_objects_recursive( window,
                                          n_objects, object_list,
                                          render, view_type,
                                          interrupt, past_last_object )
    window_struct                *window;
    int                          n_objects;
    object_struct                *object_list[];
    render_struct                *render;
    view_types                   view_type;
    update_interrupted_struct    *interrupt;
    Boolean                      *past_last_object;
{
    int            i, n_items_done, next_item;
    model_struct   *model;
    void           G_set_view_type();
    void           G_set_render();
    void           G_draw_text();
    void           G_draw_lines();
    void           G_draw_polygons();
    void           G_draw_volume();
    Real           current_realtime_seconds();

    G_set_render( window, render );
    G_set_view_type( window, view_type );

    for_less( i, 0, n_objects )
    {
        if( object_list[i]->visibility )
        {
            if( interrupt->last_was_interrupted &&
                !(*past_last_object) &&
                object_list[i] == interrupt->object_interrupted )
            {
                *past_last_object = TRUE;
            }

            if( !interrupt->last_was_interrupted || *past_last_object
                || object_list[i]->object_type == MODEL )
            {
                if( !(interrupt->current_interrupted) &&
                    current_realtime_seconds() > interrupt->interrupt_at )
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

                    display_objects_recursive( window,
                                               model->n_objects,
                                               model->object_list,
                                               &model->render,
                                               model->view_type,
                                               interrupt, past_last_object );

                    G_set_render( window, render );
                    G_set_view_type( window, view_type );
                    break;

                case TEXT:
                    G_draw_text( window, object_list[i]->ptr.text, render );
                    break;

                case LINES:
                    G_draw_lines( window, object_list[i]->ptr.lines, render,
                                  &interrupt->current_interrupted, &n_items_done, &next_item );
                    break;

                case POLYGONS:
                    G_draw_polygons( window, object_list[i]->ptr.polygons,
                                     render, &interrupt->current_interrupted,
                                     &n_items_done, &next_item );
                    break;

                case VOLUME:
                    G_draw_volume( window, object_list[i]->ptr.volume, render,
                                   &interrupt->current_interrupted, &n_items_done, &next_item);
                    break;
                }

                if( interrupt->current_interrupted )
                {
                    if( object_list[i]->object_type != MODEL )
                    {
                        interrupt->object_interrupted = object_list[i];
                        interrupt->n_items_done = n_items_done;
                        interrupt->next_item = next_item;
                    }
                    break;
                }
            }
        }
    }
}
