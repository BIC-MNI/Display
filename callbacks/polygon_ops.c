 
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_files.h>

public  Boolean  get_current_polygons( graphics, polygons )
    graphics_struct     *graphics;
    polygons_struct     **polygons;
{
    Status          status;
    Boolean         found;
    object_struct   *current_object, *object;
    Boolean         get_current_object();
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();

    found = FALSE;

    if( get_current_object( graphics, &current_object ) )
    {
        status = initialize_object_traverse( &object_traverse, 1,
                                             &current_object );

        if( status == OK )
        {
            while( get_next_object_traverse(&object_traverse,&object) )
            {
                if( !found && object->object_type == POLYGONS &&
                    object->ptr.polygons->n_items > 0 )
                {
                    found = TRUE;
                    *polygons = object->ptr.polygons;
                }
            }
        }
    }

    return( found );
}

public  DEF_MENU_FUNCTION( input_polygons_bintree )   /* ARGSUSED */
{
    Status            status;
    polygons_struct   *polygons;
    String            filename;
    FILE              *file;
    Status            io_bintree();

    status = OK;

    if( get_current_polygons(graphics,&polygons) &&
        polygons->bintree == (bintree_struct *) 0 )
    {
        PRINT( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "btr", READ_FILE,
                                                    BINARY_FORMAT, &file );

        if( status == OK )
            ALLOC( status, polygons->bintree, 1 );

        if( status == OK )
        {
            status = io_bintree( file, READ_FILE, BINARY_FORMAT,
                                 polygons->n_items, polygons->bintree );
        }

        if( status == OK )
            status = close_file( file );

        PRINT( "Done.\n" );
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
    String            filename;
    FILE              *file;
    Status            io_bintree();

    status = OK;

    if( get_current_polygons(graphics,&polygons) &&
        polygons->bintree != (bintree_struct *) 0 )
    {
        PRINT( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "btr",
                                            WRITE_FILE, BINARY_FORMAT, &file );

        if( status == OK )
        {
            status = io_bintree( file, WRITE_FILE, BINARY_FORMAT,
                                 polygons->n_items, polygons->bintree );
        }

        if( status == OK )
        {
            status = close_file( file );
        }

        PRINT( "Done.\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(save_polygons_bintree )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( create_bintree_for_polygons )   /* ARGSUSED */
{
    Status            status;
    Status            create_polygons_bintree();
    Status            delete_polygons_bintree();
    polygons_struct   *polygons;

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        if( polygons->bintree != (bintree_struct *) 0 )
            status = delete_polygons_bintree( polygons );

        if( status == OK )
            status = create_polygons_bintree( polygons,
                   ROUND( (Real) polygons->n_items * Bintree_size_factor ) );
        PRINT( "Done.\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(create_bintree_for_polygons )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( create_normals_for_polygon )   /* ARGSUSED */
{
    Status            status;
    Status            compute_polygon_normals();
    polygons_struct   *polygons;
    void              graphics_models_have_changed();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = compute_polygon_normals( polygons );
        graphics_models_have_changed( graphics );

        PRINT( "Done computing polygon normals.\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(create_normals_for_polygon )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( smooth_current_polygon )   /* ARGSUSED */
{
    Status            status;
    Status            smooth_polygon();
    Status            compute_polygon_normals();
    Status            delete_polygons_bintree();
    polygons_struct   *polygons;
    void              set_update_required();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = smooth_polygon( polygons, Max_smoothing_distance,
                                 Smoothing_ratio, Smoothing_threshold,
                                 Smoothing_normal_ratio,
                                 FALSE, (volume_struct *) 0, 0, 0 );

        if( status == OK )
            status = compute_polygon_normals( polygons );

        if( status == OK )
            status = delete_polygons_bintree( polygons );

        set_update_required( graphics, NORMAL_PLANES );

        PRINT( "Done smoothing polygon.\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(smooth_current_polygon )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( smooth_current_polygon_with_volume )   /* ARGSUSED */
{
    Status            status;
    Status            smooth_polygon();
    Status            compute_polygon_normals();
    Status            delete_polygons_bintree();
    polygons_struct   *polygons;
    volume_struct     *volume;
    void              set_update_required();

    status = OK;

    if( get_current_polygons( graphics, &polygons ) &&
        get_current_volume( graphics, &volume ) )
    {
        status = smooth_polygon( polygons, Max_smoothing_distance,
                                 Smoothing_ratio, Smoothing_threshold,
                                 Smoothing_normal_ratio, TRUE, volume,
            graphics->associated[SLICE_WINDOW]->slice.segmenting.min_threshold,
            graphics->associated[SLICE_WINDOW]->slice.segmenting.max_threshold);

        if( status == OK )
            status = compute_polygon_normals( polygons );

        if( status == OK )
            status = delete_polygons_bintree( polygons );

        set_update_required( graphics, NORMAL_PLANES );

        PRINT( "Done smoothing polygon.\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(smooth_current_polygon_with_volume )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reverse_polygons_order )   /* ARGSUSED */
{
    polygons_struct   *polygons;
    void              reverse_polygons_vertices();
    void              set_update_required();

    if( get_current_polygons( graphics, &polygons ) )
    {
        reverse_polygons_vertices( polygons );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reverse_polygons_order )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( deform_polygon_to_volume )   /* ARGSUSED */
{
    Status            status;
    Status            deform_polygons();
    Status            compute_polygon_normals();
    Status            delete_polygons_bintree();
    Real              boundary_factor, max_step, max_search_distance;
    Real              search_increment, min_size, min_isovalue, max_isovalue;
    Real              stop_threshold;
    int               max_iterations;
    volume_struct     *volume;
    polygons_struct   *polygons;
    void              set_update_required();

    status = OK;

    if( get_current_polygons( graphics, &polygons ) &&
        get_current_volume( graphics, &volume ) )
    {
        PRINT( "Enter boundary_factor, max_step, max_search_distance,\n" );
        PRINT( "      search_increment, min_size,\n" );
        PRINT( "      min_isovalue, max_isovalue,\n" );
        PRINT( "      max_iterations, stop_threshold: " );

        if( input_real( stdin, &boundary_factor ) == OK &&
            input_real( stdin, &max_step ) == OK &&
            input_real( stdin, &max_search_distance ) == OK &&
            input_real( stdin, &search_increment ) == OK &&
            input_real( stdin, &min_size ) == OK &&
            input_real( stdin, &min_isovalue ) == OK &&
            input_real( stdin, &max_isovalue ) == OK &&
            input_int( stdin, &max_iterations ) == OK &&
            input_real( stdin, &stop_threshold ) == OK )
        {
            status = deform_polygons( polygons, volume,
                                      boundary_factor, max_step,
                                      max_search_distance, search_increment,
                                      min_size,
                                      min_isovalue, max_isovalue,
                                      max_iterations, stop_threshold );

            if( status == OK )
                status = compute_polygon_normals( polygons );

            if( status == OK )
                status = delete_polygons_bintree( polygons );

            set_update_required( graphics, NORMAL_PLANES );

            PRINT( "Done deforming polygon.\n" );
        }

        (void) input_newline( stdin );
    }

    return( status );
}

public  DEF_MENU_UPDATE(deform_polygon_to_volume )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_polygon_sphere )   /* ARGSUSED */
{
    Status            status;
    Status            compute_polygon_normals();
    Status            create_polygons_sphere();
    Status            create_object();
    Status            add_object_to_current_model();
    Point             centre;
    Real              x_size, y_size, z_size;
    int               n_up, n_around;
    object_struct     *object;
    void              set_object_colour();
    void              get_default_surfprop();

    status = OK;

    PRINT( "Enter x_centre, y_centre, z_centre, x_size, y_size, z_size,\n" );
    PRINT( "      n_up, n_around: " );
    
    if( input_real( stdin, &Point_x(centre) ) == OK &&
        input_real( stdin, &Point_y(centre) ) == OK &&
        input_real( stdin, &Point_z(centre) ) == OK &&
        input_real( stdin, &x_size ) == OK &&
        input_real( stdin, &y_size ) == OK &&
        input_real( stdin, &z_size ) == OK &&
        input_int( stdin, &n_up ) == OK &&
        input_int( stdin, &n_around ) == OK )
    {
        status = create_object( &object, POLYGONS );

        if( status == OK )
            status = create_polygons_sphere( &centre, x_size, y_size, z_size,
                              n_up, n_around, object->ptr.polygons );

        if( status == OK )
            ALLOC( status, object->ptr.polygons->colours, 1 );

        if( status == OK )
        {
            object->ptr.polygons->colours[0] = WHITE;
            get_default_surfprop( &object->ptr.polygons->surfprop );
            status = compute_polygon_normals( object->ptr.polygons );
        }

        if( status == OK )
            status = add_object_to_current_model( graphics, object );
    }

    (void) input_newline( stdin );

    return( status );
}

public  DEF_MENU_UPDATE(make_polygon_sphere )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( subdivide_current_polygon )   /* ARGSUSED */
{
    Status            status;
    Status            subdivide_polygons();
    polygons_struct   *polygons;
    Status            compute_polygon_normals();
    void              graphics_models_have_changed();

    status = OK;

    if( get_current_polygons( graphics, &polygons ) )
    {
        status = subdivide_polygons( polygons );

        if( status == OK )
            status = compute_polygon_normals( polygons );

        if( status == OK )
            graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(subdivide_current_polygon )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( scan_current_polygon_to_volume )   /* ARGSUSED */
{
    Status            status;
    void              scan_polygons_to_voxels();
    polygons_struct   *polygons;
    volume_struct     *volume;
    void              set_all_volume_auxiliary_data_bit();
    void              set_slice_window_update();

    status = OK;

    if( get_current_polygons( graphics, &polygons ) &&
        get_current_volume( graphics, &volume ) )
    {
        set_all_volume_auxiliary_data_bit( volume, Scanned_polygons_label,
                                           FALSE );

        scan_polygons_to_voxels( polygons, volume,
                                 Scanned_polygons_label | ACTIVE_BIT,
                                 Max_polygon_scan_distance );

        PRINT( " done.\n" );

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }

    return( status );
}

public  DEF_MENU_UPDATE(scan_current_polygon_to_volume )   /* ARGSUSED */
{
    return( OK );
}
