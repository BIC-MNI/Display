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
    graphics_struct  graphics;
    Status           status;
    Status           G_initialize();
    Status           initialize_globals();
    Status           input_graphics_file();
    Status           create_graphics_window();
    Status           delete_graphics_window();
    Status           main_event_loop();
    Status           G_terminate();
    void             fit_view_to_domain();
    void             update_view();
    void             get_range_of_objects();
    Point            min_coord, max_coord;

    if( argc != 2 )
    {
        PRINT_ERROR( "Argument.\n" );
        (void) abort();
    }

    status = initialize_globals();

    if( status == OK )
    {
        status = G_initialize();
    }

    if( status == OK )
    {
        status = create_graphics_window( &graphics );
    }

    if( status == OK )
    {
        PRINT( "Inputting objects.\n" );

        status = input_graphics_file( argv[1], &graphics.objects );

        graphics.update_required = TRUE;

        PRINT( "Objects input.\n" );
    }

    if( status == OK )
    {
        get_range_of_objects( graphics.objects, &min_coord, &max_coord );

        ADD_POINTS( graphics.centre_of_objects, min_coord, max_coord );
        SCALE_POINT( graphics.centre_of_objects, graphics.centre_of_objects,
                     0.5 );

        fit_view_to_domain( &graphics.view,
                            Point_x(min_coord),
                            Point_y(min_coord),
                            Point_z(min_coord),
                            Point_x(max_coord),
                            Point_y(max_coord),
                            Point_z(max_coord) );

        update_view( &graphics );
    }

    if( status == OK )
    {
        status = main_event_loop( &graphics );
    }

    if( status == OK )
    {
        status = delete_graphics_window( &graphics );
    }

    if( status == OK )
    {
        status = G_terminate();
    }

    return( (int) status );
}
