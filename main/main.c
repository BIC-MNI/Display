#include  <stdio.h>
#include  <math.h>
#include  <def_standard.h>
#include  <def_graphics.h>
#include  <def_display.h>

int  main( argc, argv )
    int     argc;
    char    *argv[];
{
    Status         status;
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

    if( argc != 2 )
    {
        PRINT_ERROR( "Argument.\n" );
        abort();
    }

    status = G_initialize();

    status = G_create_window( "Test Window", &window );

    if( status != OK )
    {
        PRINT_ERROR( "Shit\n" );
    }

    create_view( &view );

    G_define_view( &window, &view );

    define_lights( &window );

    define_render( &window, &render );

    create_objects( argv[1], &objects );

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

    view->perspective_flag = TRUE;

    fill_Point( view->origin, 0.5, 0.5, 1.0 );
    fill_Vector( view->line_of_sight, 0.0, 0.0, -1.0 );
    fill_Vector( view->horizontal, 1.0, 0.0, 0.0 );
    fill_Vector( view->up, 0.0, 1.0, 0.0 );
    view->front_distance = 0.5;
    view->perspective_distance = 1.0;
    view->back_distance = 1.5;

    view->window_width = 1.1;
    view->window_height = 1.1;
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

private  void  rotate_view( window, view, render, objects )
    window_struct  *window;
    view_struct    *view;
    render_struct  *render;
    object_struct  *objects;
{
    Real   angle, s, c;
    Vector line_of_sight;
    void   G_define_view();
    void   G_update_window();
    void   assign_view_direction();
    void   display_objects();

    angle = 0.0;

    while( TRUE )
    {
        c = cos( (double) angle );
        s = sin( (double) angle );

        Point_z( view->origin ) = c;
        Point_x( view->origin ) = 0.5 + s;

        fill_Vector( line_of_sight, -s, 0.0, -c );

        assign_view_direction( view, &line_of_sight, &view->up );

        G_define_view( window, view );

        display_objects( window, objects, render );

        G_update_window( window );

        angle += 0.05;
    }
}
