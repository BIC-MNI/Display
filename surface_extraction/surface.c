
#include  <def_graphics.h>
#include  <def_globals.h>

#define  INITIAL_SIZE         1000
#define  ENLARGE_THRESHOLD    1.0
#define  ENLARGE_DENSITY      0.5

public  Status  initialize_surface_extraction( surface_extraction )
    surface_extraction_struct   *surface_extraction;
{
    Status  status;
    void    empty_polygons_struct();
    Status  initialize_voxels_done();
    void    initialize_voxel_queue();

    surface_extraction->extraction_started = FALSE;
    surface_extraction->isovalue_selected = FALSE;

    status = initialize_hash_table( &surface_extraction->edge_points,
                                    2, INITIAL_SIZE, ENLARGE_THRESHOLD,
                                    ENLARGE_DENSITY );

    if( status == OK )
    {
        status = initialize_voxels_done( &surface_extraction->voxels_done );
    }

    if( status == OK )
    {
        initialize_voxel_queue( &surface_extraction->voxels_to_do );

        empty_polygons_struct( &surface_extraction->triangles,
                               &Extracted_surface_colour,
                               &Default_surface_property );
    }

    return( status );
}

public  Status  delete_surface_extraction( surface_extraction )
    surface_extraction_struct   *surface_extraction;
{
    return( OK );
}

public  void  set_isosurface_value( surface_extraction )
    surface_extraction_struct   *surface_extraction;
{
    Real   value;

    if( !surface_extraction->extraction_started )
    {
        PRINT( "Enter isosurface value: " );
        if( scanf( "%f", &value ) == 1 && value >= 0.0 )
        {
            surface_extraction->isovalue = value;
            surface_extraction->isovalue_selected = TRUE;
        }
    }
}

public  void  start_surface_extraction( graphics, x, y, z )
    graphics_struct    *graphics;
    int                x, y, z;
{
    surface_extraction_struct   *surface_extraction;
    Boolean                     voxel_is_within_volume();
    void                        install_surface_extraction();
    Boolean                     find_close_voxel_containing_value();
    voxel_index_struct          voxel_indices;
    Status                      status;
    Status                      insert_in_voxel_queue();

    surface_extraction = &graphics->three_d.surface_extraction;

    if( voxel_is_within_volume(
             graphics->associated[SLICE_WINDOW]->slice.volume, x, y, z ) &&
        !surface_extraction->extraction_started )
    {
        if( !surface_extraction->isovalue_selected )
        {
            set_isosurface_value( surface_extraction );
        }

        surface_extraction->extraction_started = TRUE;

        install_surface_extraction( graphics );

        if( find_close_voxel_containing_value(
                  graphics->associated[SLICE_WINDOW]->slice.volume,
                  graphics->three_d.surface_extraction.isovalue,
                  x, y, z, &voxel_indices ) )
        {
            status = insert_in_voxel_queue(
                     &graphics->three_d.surface_extraction.voxels_to_do,
                     &voxel_indices );

PRINT( "%d %d %d  -> %d %d %d\n", x, y, z,
       voxel_indices.voxel_indices[X_AXIS],
       voxel_indices.voxel_indices[Y_AXIS],
       voxel_indices.voxel_indices[Z_AXIS] );
        }
    }
}

public  void  stop_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    void      uninstall_surface_extraction();

    if( graphics->three_d.surface_extraction.extraction_started )
    {
        uninstall_surface_extraction( graphics );
    }
}

public  void  install_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    void                    add_action_table_function();
    DECL_EVENT_FUNCTION(    add_to_surface );

    add_action_table_function( &graphics->action_table, NO_EVENT,
                               add_to_surface );
}

public  void  uninstall_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    void   remove_action_table_function();

    remove_action_table_function( &graphics->action_table, NO_EVENT );
}

private  DEF_EVENT_FUNCTION( add_to_surface )    /* ARGSUSED */
{
    void     extract_more_triangles();
    void     display_triangles();

    if( voxels_remaining( &graphics->three_d.surface_extraction.voxels_to_do ) )
    {
        extract_more_triangles( graphics );

        graphics->update_required = TRUE;
    }

    if( graphics->update_required )
    {
        display_triangles( graphics );
    }

    return( OK );
}

private  Boolean  find_close_voxel_containing_value( volume, value,
                        x, y, z, found_indices )
    volume_struct   *volume;
    Real            value;
    int             x, y, z;
    int             found_indices[N_DIMENSIONS];
{
    Status                                status;
    Boolean                               found;
    Boolean                               voxel_contains_value();
    QUEUE_STRUCT( voxel_index_struct )    voxels_to_check;
    voxel_index_struct                    indices, insert;
    hash_table_struct                     voxels_done;
    Status                                mark_voxel_done();
    Status                                delete_voxels_done();
    Status                                insert_in_voxel_queue();
    Status                                delete_voxel_queue();
    Status                                initialize_voxels_done();
    void                                  initialize_voxels_queue();
    void                                  get_next_voxel_from_queue();
    int                                   x_offset, y_offset, z_offset;

    insert.voxel_indices[X_AXIS] = MIN( x, volume->size[X_AXIS]-2 );
    insert.voxel_indices[Y_AXIS] = MIN( y, volume->size[Y_AXIS]-2 );
    insert.voxel_indices[Z_AXIS] = MIN( z, volume->size[Z_AXIS]-2 );

    found = FALSE;

    status = initialize_voxels_done( &voxels_done );

    initialize_voxel_queue( &voxels_to_check );

    if( status == OK )
    {
        status = insert_in_voxel_queue( &voxels_to_check, &insert );
    }

    if( status == OK )
    {
        status = mark_voxel_done( volume, &voxels_done, &insert );
    }

    while( !found && status == OK && voxels_remaining(&voxels_to_check) )
    {
        get_next_voxel_from_queue( &voxels_to_check, &indices );

        if( voxel_contains_value( volume,
                                  indices.voxel_indices[X_AXIS],
                                  indices.voxel_indices[Y_AXIS],
                                  indices.voxel_indices[Z_AXIS], value ) )
        {
            found_indices[X_AXIS] = indices.voxel_indices[X_AXIS];
            found_indices[Y_AXIS] = indices.voxel_indices[Y_AXIS];
            found_indices[Z_AXIS] = indices.voxel_indices[Z_AXIS];
            found = TRUE;
        }
        else
        {
            for_inclusive( x_offset, -1, 1 )
            {
                for_inclusive( y_offset, -1, 1 )
                {
                    for_inclusive( z_offset, -1, 1 )
                    {
                        if( x_offset != 0 || y_offset != 0 || z_offset != 0 )
                        {
                            insert.voxel_indices[X_AXIS] =
                                    indices.voxel_indices[X_AXIS] + x_offset;
                            insert.voxel_indices[Y_AXIS] =
                                    indices.voxel_indices[Y_AXIS] + y_offset;
                            insert.voxel_indices[Z_AXIS] =
                                    indices.voxel_indices[Z_AXIS] + z_offset;

                            if( voxel_is_within_volume( volume,
                                           insert.voxel_indices[X_AXIS],
                                           insert.voxel_indices[Y_AXIS],
                                           insert.voxel_indices[Z_AXIS] ) &&
                                !is_voxel_done( volume, &voxels_done, &insert ) )
                            {
                                status = insert_in_voxel_queue(
                                                   &voxels_to_check,
                                                   &insert );
                                if( status == OK )
                                {
                                    status = mark_voxel_done( volume,
                                                              &voxels_done,
                                                              &insert );
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if( status == OK )
    {
        status = delete_voxels_done( &voxels_done );
    }

    if( status == OK )
    {
        status = delete_voxel_queue( &voxels_to_check );
    }

    return( found );
}

private  void   initialize_voxel_queue( voxel_queue )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
{
    INITIALIZE_QUEUE( *voxel_queue );
}

private  Status   insert_in_voxel_queue( voxel_queue, voxel_indices )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
    voxel_index_struct                  *voxel_indices;
{
    Status   status;

    INSERT_IN_QUEUE( status, *voxel_queue, voxel_index_struct, *voxel_indices );

    return( status );
}

private  void   get_next_voxel_from_queue( voxel_queue, voxel_indices )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
    voxel_index_struct                  *voxel_indices;
{
    REMOVE_FROM_QUEUE( *voxel_queue, *voxel_indices );
}

private  Boolean  voxels_remaining( voxel_queue )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
{
    return( !IS_QUEUE_EMPTY( *voxel_queue ) );
}

private  Status  delete_voxel_queue( voxel_queue )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
{
    Status   status;

    DELETE_QUEUE( status, *voxel_queue );

    return( status );
}

public  void  extract_more_triangles( graphics )
    graphics_struct   *graphics;
{
    int                         n_voxels_done;
    voxel_index_struct          voxel_index;
    surface_extraction_struct   *surface_extraction;
    void                        get_next_voxel_from_queue();
    Boolean                     check_voxel();
    void                        add_voxel_neighbours();

    n_voxels_done = 0;

    surface_extraction = &graphics->three_d.surface_extraction;

    while( n_voxels_done < Voxels_per_update &&
           voxels_remaining( &surface_extraction->voxels_to_do ) )
    {
        get_next_voxel_from_queue( &surface_extraction->voxels_to_do,
                                   &voxel_index );

        if( check_voxel( surface_extraction, &voxel_index ) )
        {
            ++n_voxels_done;
            add_voxel_neighbours( surface_extraction, &voxel_index );
        }
    }
}

private  Status  initialize_voxels_done( voxels_done )
    hash_table_struct  *voxels_done;
{
    Status   status;

    status = initialize_hash_table( voxels_done,
                                    1, INITIAL_SIZE, ENLARGE_THRESHOLD,
                                    ENLARGE_DENSITY );

    return( status );
}

private  Status  delete_voxels_done( voxels_done )
    hash_table_struct  *voxels_done;
{
    Status   status;

    status = delete_hash_table( voxels_done );

    return( status );
}

private  void  make_voxel_hash_keys( volume, indices, keys )
    volume_struct       *volume;
    voxel_index_struct  *indices;
    int                 keys[];
{
    keys[0] = ijk( indices->voxel_indices[X_AXIS],
                   indices->voxel_indices[Y_AXIS],
                   indices->voxel_indices[Z_AXIS],
                   volume->size[Y_AXIS],
                   volume->size[Z_AXIS] );
}

private  Boolean  is_voxel_done( volume, voxels_done, indices )
    volume_struct       *volume;
    hash_table_struct   *voxels_done;
    voxel_index_struct  *indices;
{
    int   keys[1];

    make_voxel_hash_keys( volume, indices, keys );

    return( lookup_in_hash_table( voxels_done, keys, (char **) 0 ) );
}

private  Status  mark_voxel_done( volume, voxels_done, indices )
    volume_struct       *volume;
    hash_table_struct   *voxels_done;
    voxel_index_struct  *indices;
{
    Status   status;
    int      keys[1];

    make_voxel_hash_keys( volume, indices, keys );

    status = insert_in_hash_table( voxels_done, keys, (char *) 0 );

    return( status );
}

private  void  display_triangles( graphics )
    graphics_struct  *graphics;
{
}

private  Boolean   check_voxel( surface_extraction, voxel_index )
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
{
    return( FALSE );
}

private  void  add_voxel_neighbours( surface_extraction, voxel_index )
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
{
}
