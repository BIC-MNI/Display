 
#include  <display.h>

public  BOOLEAN  get_current_polygons(
    display_struct      *display,
    polygons_struct     **polygons )
{
    BOOLEAN                 found;
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    found = FALSE;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, 1, &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( !found && object->object_type == POLYGONS &&
                get_polygons_ptr(object)->n_items > 0 )
            {
                found = TRUE;
                *polygons = get_polygons_ptr(object);
            }
        }
    }

    return( found );
}

public  DEF_MENU_FUNCTION( input_polygons_bintree )   /* ARGSUSED */
{
    Status            status;
    polygons_struct   *polygons;
    STRING            filename;
    FILE              *file;

    status = OK;

    if( get_current_polygons(display,&polygons) &&
        polygons->bintree == (bintree_struct_ptr) 0 )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "btr", READ_FILE,
                                                    BINARY_FORMAT, &file );

        if( status == OK )
            polygons->bintree = allocate_bintree();

        if( status == OK )
        {
            status = io_bintree( file, READ_FILE, BINARY_FORMAT,
                                 polygons->bintree );
        }

        if( status == OK )
            status = close_file( file );

        print( "Done.\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(input_polygons_bintree )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( save_polygons_bintree )   /* ARGSUSED */
{
    Status            status;
    polygons_struct   *polygons;
    STRING            filename;
    FILE              *file;

    status = OK;

    if( get_current_polygons(display,&polygons) &&
        polygons->bintree != (bintree_struct_ptr) 0 )
    {
        print( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "btr",
                                            WRITE_FILE, BINARY_FORMAT, &file );

        if( status == OK )
        {
            status = io_bintree( file, WRITE_FILE, BINARY_FORMAT,
                                 polygons->bintree );
        }

        if( status == OK )
        {
            status = close_file( file );
        }

        print( "Done.\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(save_polygons_bintree )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( create_bintree_for_polygons )   /* ARGSUSED */
{
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        if( polygons->bintree != (bintree_struct_ptr) 0 )
            delete_the_bintree( (bintree_struct **) (&polygons->bintree) );

        create_polygons_bintree( polygons,
                     ROUND( (Real) polygons->n_items * Bintree_size_factor ) );
        print( "Done.\n" );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(create_bintree_for_polygons )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( create_normals_for_polygon )   /* ARGSUSED */
{
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        compute_polygon_normals( polygons );
        graphics_models_have_changed( display );

        print( "Done computing polygon normals.\n" );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(create_normals_for_polygon )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( average_normals_for_polygon )   /* ARGSUSED */
{
    int               n_iters;
    Real              neighbour_weight;
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        print( "Enter #iterations  neighbour_weight: " );

        if( input_int( stdin, &n_iters ) == OK &&
            input_real( stdin, &neighbour_weight ) == OK )
        {
            average_polygon_normals( polygons, n_iters, neighbour_weight );

            graphics_models_have_changed( display );
        }

        (void) input_newline( stdin );

        print( "Done averaging polygon normals.\n" );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(average_normals_for_polygon )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( smooth_current_polygon )   /* ARGSUSED */
{
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        smooth_polygon( polygons, Max_smoothing_distance,
                        Smoothing_ratio, Smoothing_threshold,
                        Smoothing_normal_ratio,
                        FALSE, (Volume) NULL, 0, 0 );

        compute_polygon_normals( polygons );

        delete_the_bintree( (bintree_struct **) (&polygons->bintree) );

        set_update_required( display, NORMAL_PLANES );

        print( "Done smoothing polygon.\n" );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(smooth_current_polygon )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( smooth_current_polygon_with_volume )   /* ARGSUSED */
{
    polygons_struct   *polygons;
    Volume            volume;

    if( get_current_polygons( display, &polygons ) &&
        get_slice_window_volume( display, &volume ) )
    {
        smooth_polygon( polygons, Max_smoothing_distance,
                                 Smoothing_ratio, Smoothing_threshold,
                                 Smoothing_normal_ratio, TRUE, volume,
            display->associated[SLICE_WINDOW]->slice.segmenting.min_threshold,
            display->associated[SLICE_WINDOW]->slice.segmenting.max_threshold);

        compute_polygon_normals( polygons );

        delete_the_bintree( (bintree_struct **) (&polygons->bintree) );

        set_update_required( display, NORMAL_PLANES );

        print( "Done smoothing polygon.\n" );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(smooth_current_polygon_with_volume )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reverse_polygons_order )   /* ARGSUSED */
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        reverse_polygons_vertices( polygons );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reverse_polygons_order )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_polygon_sphere )   /* ARGSUSED */
{
    Point             centre;
    Real              x_size, y_size, z_size;
    int               n_up, n_around;
    object_struct     *object;

    print( "Enter x_centre, y_centre, z_centre, x_size, y_size, z_size,\n" );
    print( "      n_up, n_around: " );
    
    if( input_float( stdin, &Point_x(centre) ) == OK &&
        input_float( stdin, &Point_y(centre) ) == OK &&
        input_float( stdin, &Point_z(centre) ) == OK &&
        input_real( stdin, &x_size ) == OK &&
        input_real( stdin, &y_size ) == OK &&
        input_real( stdin, &z_size ) == OK &&
        input_int( stdin, &n_up ) == OK &&
        input_int( stdin, &n_around ) == OK )
    {
        object = create_object( POLYGONS );

        create_polygons_sphere( &centre, x_size, y_size, z_size,
                                n_up, n_around, FALSE,
                                get_polygons_ptr(object) );

        get_polygons_ptr(object)->colours[0] = WHITE;
        compute_polygon_normals( get_polygons_ptr(object) );

        add_object_to_current_model( display, object );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(make_polygon_sphere )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_tetrahedral_sphere )   /* ARGSUSED */
{
    Point             centre;
    Real              x_size, y_size, z_size;
    int               n_triangles;
    object_struct     *object;

    print( "Enter x_centre, y_centre, z_centre, x_size, y_size, z_size,\n" );
    print( "      n_triangles: " );
    
    if( input_float( stdin, &Point_x(centre) ) == OK &&
        input_float( stdin, &Point_y(centre) ) == OK &&
        input_float( stdin, &Point_z(centre) ) == OK &&
        input_real( stdin, &x_size ) == OK &&
        input_real( stdin, &y_size ) == OK &&
        input_real( stdin, &z_size ) == OK &&
        input_int( stdin, &n_triangles ) == OK )
    {
        object = create_object( POLYGONS );

        create_tetrahedral_sphere( &centre, x_size, y_size, z_size,
                                   n_triangles, get_polygons_ptr(object) );

        get_polygons_ptr(object)->colours[0] = WHITE;
        compute_polygon_normals( get_polygons_ptr(object) );

        add_object_to_current_model( display, object );
    }

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(make_tetrahedral_sphere )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( subdivide_current_polygon )   /* ARGSUSED */
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        subdivide_polygons( polygons );

        compute_polygon_normals( polygons );

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(subdivide_current_polygon )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_polygon_neighbours )   /* ARGSUSED */
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        if( polygons->neighbours != (int *) NULL )
            FREE( polygons->neighbours );
        polygons->neighbours = (int *) NULL;

        check_polygons_neighbours_computed( polygons );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_polygon_neighbours )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( cut_polygon_neighbours )   /* ARGSUSED */
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        print( "Cutting polygon neighbours:\n" );
        cut_polygon_neighbours_from_lines( display, polygons );
        print( "...done\n" );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(cut_polygon_neighbours )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_polygon_line_thickness )   /* ARGSUSED */
{
    Real              line_thickness;
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        print( "Enter line thickness: " );
        if( input_real( stdin, &line_thickness ) )
        {
            polygons->line_thickness = (float) line_thickness;
            graphics_models_have_changed( display );
        }
        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_polygon_line_thickness )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( print_polygons_surface_area )   /* ARGSUSED */
{
    Real              surface_area;
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        surface_area = get_polygons_surface_area( polygons );
        print( "Surface area of polygons: %g\n", surface_area );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(print_polygons_surface_area )   /* ARGSUSED */
{
    return( OK );
}
