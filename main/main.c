#include  <stdio.h>
#include  <def_standard.h>
#include  <def_graphics.h>

main()
{
    Status         status;
    window_struct  window;
    Status         G_initialize();
    Status         G_create_window();
    Status         G_delete_window();
    Status         G_terminate();
    void           G_update_window();
    void           define_view();
    void           draw_triangles();

    status = G_initialize();

    status = G_create_window( "Test Window", &window );

    if( status != OK )
    {
        PRINT_ERROR( "Shit\n" );
    }

    define_view( &window );

    draw_triangles( &window );

    G_update_window( &window );

    PRINT( "Hit return\n" );

    while( getchar() != '\n' );

    status = G_delete_window( &window );

    status = G_terminate();
}

private  void  draw_triangles( window )
    window_struct  *window;
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

    G_draw_triangles( window, &triangles );
}

private  void  define_view( window )
    window_struct  *window;
{
    view_struct   view;
    void          G_define_view();

    view.perspective_flag = FALSE;

    fill_Point( view.origin, 0.5, 0.5, 1.0 );
    fill_Vector( view.line_of_sight, 0.0, 0.0, -1.0 );
    fill_Vector( view.horizontal, 1.0, 0.0, 0.0 );
    fill_Vector( view.up, 0.0, 1.0, 0.0 );
    view.front_distance = 0.5;
    view.back_distance = 1.5;

    view.window_width = 1.1;
    view.window_height = 1.1;

    G_define_view( window, &view );
}
