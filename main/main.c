#include  <stdio.h>
#include  <math.h>
#include  <def_graphics.h>

#define  X_SIZE  256.0
#define  Y_SIZE  256.0
#define  Z_SIZE  64.0

int  main( argc, argv )
    int     argc;
    char    *argv[];
{
    Status         status;
    Real           G_get_window_aspect();
    void           adjust_view_for_aspect();
    view_struct    view;
    render_struct  render;
    window_struct  window;
    object_struct  *objects;
    Status         G_initialize();
    Status         G_create_window();
    Status         G_delete_window();
    Status         G_terminate();
    void           G_update_window();
    void           G_define_view();
    void           create_view();
    void           define_lights();
    void           define_render();
    void           draw_triangles();
    void           rotate_view();
    void           create_objects();
    void           display_objects();

    if( argc != 2 )
    {
        PRINT_ERROR( "Argument.\n" );
        (void) abort();
    }

    status = G_initialize();

    status = G_create_window( "Test Window", &window );

    if( status != OK )
    {
        PRINT_ERROR( "Shit\n" );
    }

    create_view( &view );

    adjust_view_for_aspect( &view, G_get_window_aspect(&window) );

    G_define_view( &window, &view );

    define_lights( &window );

    define_render( &window, &render );

    PRINT( "Inputting objects.\n" );

    create_objects( argv[1], &objects );

    PRINT( "Objects input.\n" );

    G_define_view( &window, &view );

    display_objects( &window, objects, &render );

    G_update_window( &window );

    PRINT( "Okay\n" );

    rotate_view( &window, &view, &render, objects );

    PRINT( "Hit return\n" );

    while( getchar() != '\n' );

    status = G_delete_window( &window );

    status = G_terminate();

    return( (int) status );
}

private  void  create_objects( filename, objects )
    char           filename[];
    object_struct  **objects;
{
    Status           status;
    Status           input_file();

    status = input_file( filename, objects );
}

private  void  create_view( view )
    view_struct    *view;
{
    void          G_define_view();
    void          make_identity_transform();
    void          make_translation_transform();
    static Vector trans = {100.0, 0.0, 0.0};

    view->perspective_flag = TRUE;

    fill_Point( view->origin, X_SIZE / 2.0, Y_SIZE / 2.0, 2 * Z_SIZE );
    fill_Vector( view->line_of_sight, 0.0, 0.0, -1.0 );
    fill_Vector( view->horizontal, 1.0, 0.0, 0.0 );
    fill_Vector( view->up, 0.0, 1.0, 0.0 );
    view->front_distance = Z_SIZE / 10.0;
    view->perspective_distance = Point_z(view->origin) - Z_SIZE / 2.0;
    view->back_distance = 2.5 * Z_SIZE;

    view->window_width = X_SIZE * 1.1;
    view->window_height = Y_SIZE * 1.1;

    make_identity_transform( &view->modeling_transform );
/*
    make_translation_transform( &trans, &view->modeling_transform );
*/
}

private  void  define_lights( window )
    window_struct  *window;
{
    light_struct   ambient, directional;
    void           G_define_light();
    void           G_set_light_state();

    ambient.light_type = AMBIENT_LIGHT;
    fill_Colour( ambient.colour, 0.4, 0.4, 0.4 );

    directional.light_type = DIRECTIONAL_LIGHT;
    fill_Colour( directional.colour, 1.0, 1.0, 1.0 );
    fill_Vector( directional.direction, 1.0, -1.0, -1.0 );

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

private  void  rotate_view( window, view, render, objects )
    window_struct  *window;
    view_struct    *view;
    render_struct  *render;
    object_struct  *objects;
{
    Real          angle;
    Vector        line_of_sight;
    void          G_define_view();
    void          G_update_window();
    void          assign_view_direction();
    void          display_objects();
    void          make_translation_transform();
    void          make_rotation_transform();
    void          make_transform_relative_to_point();
    Transform     rotation_trans;
    Vector        trans;
    static Point  centre = { X_SIZE/2.0, Y_SIZE/2.0, Z_SIZE/2.0 };

    angle = 0.0;

    while( TRUE )
    {
        make_rotation_transform( angle, Y_AXIS, &rotation_trans );

        make_transform_relative_to_point( &centre, &rotation_trans,
                                          &view->modeling_transform );

        G_define_view( window, view );

        display_objects( window, objects, render );

        G_update_window( window );

        angle += 0.03;
    }
}
