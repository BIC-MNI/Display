
#include  <def_graphics.h>

public  void  display_objects( window, model )
    window_struct   *window;
    model_struct    *model;
{
    void           G_set_render();
    void           G_set_view_type();
    void           display_objects_recursive();

    display_objects_recursive( window, model->objects, &model->render,
                               model->view_type );
}

private  void  display_objects_recursive( window, objects, render, view_type )
    window_struct   *window;
    object_struct   *objects;
    render_struct   *render;
    view_types      view_type;
{
    model_struct   *model;
    void           G_set_view_type();
    void           G_set_render();
    void           G_draw_text();
    void           G_draw_triangles();
    void           G_draw_rectangles();
    void           G_draw_lines();

    G_set_render( window, render );
    G_set_view_type( window, view_type );

    while( objects != (object_struct *) 0 )
    {
        if( objects->visibility )
        {
            switch( objects->object_type )
            {
            case MODEL:
                model = objects->ptr.model;

                display_objects_recursive( window, model->objects,
                                           &model->render, model->view_type );

                G_set_render( window, render );
                G_set_view_type( window, view_type );
                break;

            case TEXT:
                G_draw_text( window, objects->ptr.text, render );
                break;

            case LINES:
                G_draw_lines( window, objects->ptr.lines, render );
                break;

            case TRIANGLES:
                G_draw_triangles( window, objects->ptr.triangles, render );
                break;

            case RECTANGLES:
                G_draw_rectangles( window, objects->ptr.rectangles, render );
                break;
            }
        }

        objects = objects->next;
    }
}
