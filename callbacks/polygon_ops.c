
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_stdio.h>

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
    Status            open_file();
    Status            io_bintree();
    Status            close_file();

    status = OK;

    if( get_current_polygons(graphics,&polygons) &&
        polygons->bintree == (bintree_struct *) 0 )
    {
        PRINT( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_file( filename, READ_FILE, BINARY_FORMAT, &file );

        if( status == OK )
        {
            ALLOC1( status, polygons->bintree, 1, bintree_struct );
        }

        if( status == OK )
        {
            status = io_bintree( file, READ_FILE, BINARY_FORMAT,
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
    Status            open_file();
    Status            io_bintree();
    Status            close_file();

    status = OK;

    if( get_current_polygons(graphics,&polygons) &&
        polygons->bintree != (bintree_struct *) 0 )
    {
        PRINT( "Enter filename: " );
        (void) scanf( "%s", filename );

        status = open_file( filename, WRITE_FILE, BINARY_FORMAT, &file );

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
    void              graphics_models_have_changed();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = smooth_polygon( polygons, Max_smoothing_distance,
                                 Smoothing_ratio, Smoothing_threshold );

        if( status == OK )
            status = compute_polygon_normals( polygons );

        if( status == OK )
            status = delete_polygons_bintree( polygons );

        graphics_models_have_changed( graphics );

        PRINT( "Done smoothing polygon.\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(smooth_current_polygon )   /* ARGSUSED */
{
    return( OK );
}
