
#include  <def_graphics.h>

public  void  draw_2d_line( graphics, view_type, colour, x1, y1, x2, y2 )
    graphics_struct   *graphics;
    view_types        view_type;
    Colour            *colour;
    Real              x1, y1, x2, y2;
{
    void      G_set_view_type();
    static    Point     end_points[2];
    static    int       end_indices[] = { 2 };
    static    int       indices[]     = { 0, 1 };
    static    lines_struct  lines = {
                                        { 0.0, 0.0, 0.0 },
                                        2,
                                        end_points,
                                        1,
                                        end_indices,
                                        indices
                                    };
    void      G_set_view_type();
    void      G_draw_lines();

    G_set_view_type( &graphics->window, view_type );

    fill_Point( end_points[0], x1, y1, 0.0 );
    fill_Point( end_points[1], x2, y2, 0.0 );

    lines.colour = *colour;

    G_draw_lines( &graphics->window, &lines,
                  (update_interrupted_struct *) 0, FALSE );
}

public  void  draw_2d_rectangle( graphics, view_type, colour, x1, y1, x2, y2 )
    graphics_struct   *graphics;
    view_types        view_type;
    Colour            *colour;
    Real              x1, y1, x2, y2;
{
    static    Point     corners[4];
    static    int       end_indices[] = { 5 };
    static    int       indices[]     = { 0, 1, 2, 3, 0 };
    static    lines_struct  lines = {
                                        { 0.0, 0.0, 0.0 },
                                        4,
                                        corners,
                                        1,
                                        end_indices,
                                        indices
                                    };
    void      G_set_view_type();
    void      G_draw_lines();

    G_set_view_type( &graphics->window, view_type );

    fill_Point( corners[0], x1, y1, 0.0 );
    fill_Point( corners[1], x2, y1, 0.0 );
    fill_Point( corners[2], x2, y2, 0.0 );
    fill_Point( corners[3], x1, y2, 0.0 );

    lines.colour = *colour;

    G_draw_lines( &graphics->window, &lines,
                  (update_interrupted_struct *) 0, FALSE );
}

public  void  draw_polygons( graphics, polygons )
    graphics_struct   *graphics;
    polygons_struct   *polygons;
{
    void           G_set_view_type();
    void           G_set_render();
    void           G_draw_polygons();
    render_struct  *get_main_render();

    G_set_view_type( &graphics->window, MODEL_VIEW );

    G_set_render( &graphics->window, get_main_render(graphics) );

    G_draw_polygons( &graphics->window, polygons, 
                     get_main_render( graphics ),
                     (update_interrupted_struct *) 0, FALSE );
}

private  render_struct  *get_main_render( graphics )
    graphics_struct   *graphics;
{
    return( &graphics->models[THREED_MODEL]->ptr.model->render );
}
