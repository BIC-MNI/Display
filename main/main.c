#include  <stdio.h>
#include  <def_standard.h>
#include  <def_graphics.h>

main()
{
    Status         status;
    render_struct  render;
    window_struct  window;
    Status         G_initialize();
    Status         G_create_window();
    Status         G_delete_window();
    Status         G_terminate();
    void           G_update_window();
    void           define_view();
    void           define_lights();
    void           define_render();
    void           draw_triangles();

    status = G_initialize();

    status = G_create_window( "Test Window", &window );

    if( status != OK )
    {
        PRINT_ERROR( "Shit\n" );
    }

    define_view( &window );

    define_lights( &window );

    define_render( &window, &render );

    draw_triangles( &window, &render );

    G_update_window( &window );

    PRINT( "Hit return\n" );

    while( getchar() != '\n' );

    status = G_delete_window( &window );

    status = G_terminate();

    return( (int) status );
}

private  void  draw_triangles( window, render )
    window_struct  *window;
    render_struct  *render;
{
    static  Colour   colour  = { 1.0, 1.0, 1.0 };
    static  Surfprop surfprop  = { 1.0, 1.0, 1.0, {1.0,1.0,1.0}, 1.0, 1.0 };
    static  Point    points[]  = { {0.0, 0.0, 0.0},
                                   {1.0, 0.0, 0.0},
                                   {0.0, 1.0, 0.0} };
    static  Vector   normals[] = { {0.5, 0.0, 1.0},
                                   {0.0, 0.5, 1.0},
                                   {-0.5, 0.0, 1.0} };
    static  int      indices[] = { 0, 1, 2 };
    triangles_struct triangles;
    void             G_draw_triangles();

    triangles.colour = colour;
    triangles.surfprop = surfprop;
    triangles.n_points = 3;
    triangles.points = points;
    triangles.normals = normals;
    triangles.n_triangles = 1;
    triangles.triangle_indices = indices;

    G_draw_triangles( window, &triangles, render );
}

private  void  define_view( window )
    window_struct  *window;
{
    view_struct   view;
    void          G_define_view();

    view.perspective_flag = TRUE;

    fill_Point( view.origin, 0.5, 0.5, 1.0 );
    fill_Vector( view.line_of_sight, 0.0, 0.0, -1.0 );
    fill_Vector( view.horizontal, 1.0, 0.0, 0.0 );
    fill_Vector( view.up, 0.0, 1.0, 0.0 );
    view.front_distance = 0.5;
    view.perspective_distance = 1.0;
    view.back_distance = 1.5;

    view.window_width = 1.1;
    view.window_height = 1.1;

    G_define_view( window, &view );
}

private  void  define_lights( window )
    window_struct  *window;
{
    light_struct   ambient, directional;
    void           G_define_light();
    void           G_set_light_state();

    ambient.light_type = AMBIENT_LIGHT;
    fill_Colour( ambient.colour, 0.3, 0.3, 0.3 );

    directional.light_type = DIRECTIONAL_LIGHT;
    fill_Colour( directional.colour, 1.0, 1.0, 0.3 );
    fill_Vector( directional.direction, 1.0, 0.0, 1.0 );

    G_define_light( window, &ambient, 0 );
    G_define_light( window, &directional, 1 );

    G_set_light_state( window, 0, ON );
    G_set_light_state( window, 1, ON );
}

private  void  define_render( window, render )
    window_struct  *window;
    render_struct  * render;
{
    void            G_set_render();

    render->render_mode = SHADED_MODE;
    render->shading_type = GOURAUD_SHADING;
    render->master_light_switch = ON;

    G_set_render( window, render );
}
