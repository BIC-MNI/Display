
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_marching_cubes.h>
#include  <def_splines.h>

#define  INITIAL_SIZE         1000
#define  ENLARGE_THRESHOLD    1.0
#define  ENLARGE_DENSITY      0.5

typedef  enum  { ON_FIRST_CORNER, ON_EDGE, ON_SECOND_CORNER } Point_classes;

public  Status  initialize_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    Status                      status;
    void                        empty_polygons_struct();
    Status                      initialize_edge_points();
    Status                      initialize_voxels_done();
    void                        initialize_voxel_queue();
    void                        install_surface_extraction();
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &graphics->three_d.surface_extraction;

    surface_extraction->extraction_in_progress = FALSE;
    surface_extraction->isovalue_selected = FALSE;

    status = initialize_edge_points( &surface_extraction->edge_points );

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

        install_surface_extraction( graphics );
    }

    return( status );
}

public  Status  delete_surface_extraction( graphics )
    graphics_struct   *graphics;
{
    Status                      status;
    Status                      delete_edge_points();
    Status                      delete_voxels_done();
    Status                      delete_voxel_queue();
    Status                      delete_polygons();
    surface_extraction_struct   *surface_extraction;
    void                        uninstall_surface_extraction();

    uninstall_surface_extraction( graphics );

    surface_extraction = &graphics->three_d.surface_extraction;

    status = delete_edge_points( &surface_extraction->edge_points );

    if( status == OK )
    {
        status = delete_voxels_done( &surface_extraction->voxels_done );
    }

    if( status == OK )
    {
        status = delete_voxel_queue( &surface_extraction->voxels_to_do );
    }

    if( status == OK )
    {
        status = delete_polygons( &surface_extraction->triangles );
    }

    return( status );
}

public  Status  reset_surface_extraction( graphics )
    graphics_struct   *graphics;
{
    Status    status;

    status = delete_surface_extraction( graphics );

    if( status == OK )
    {
        status = initialize_surface_extraction( graphics );
    }

    return( status );
}

public  void  set_isosurface_value( surface_extraction )
    surface_extraction_struct   *surface_extraction;
{
    Real   value;

    if( !surface_extraction->extraction_in_progress )
    {
        PRINT( "Enter isosurface value: " );
        if( scanf( "%f", &value ) == 1 && value >= 0.0 )
        {
            surface_extraction->isovalue = value;
            surface_extraction->isovalue_selected = TRUE;
        }
    }
}

public  void  start_surface_extraction_at_point( graphics, x, y, z )
    graphics_struct    *graphics;
    int                x, y, z;
{
    surface_extraction_struct   *surface_extraction;
    Boolean                     cube_is_within_volume();
    Boolean                     find_close_voxel_containing_value();
    voxel_index_struct          voxel_indices;
    Status                      status;
    Status                      insert_in_voxel_queue();
    Status                      mark_voxel_done();
    void                        add_voxel_neighbours();
    void                        start_surface_extraction();

    surface_extraction = &graphics->three_d.surface_extraction;

    status = OK;

    if( cube_is_within_volume(
             graphics->associated[SLICE_WINDOW]->slice.volume, x, y, z ) &&
        !surface_extraction->extraction_in_progress )
    {
        if( !surface_extraction->isovalue_selected )
        {
            set_isosurface_value( surface_extraction );
        }

        if( find_close_voxel_containing_value(
                  graphics->associated[SLICE_WINDOW]->slice.volume,
                  graphics->three_d.surface_extraction.isovalue,
                  x, y, z, &voxel_indices ) )
        {
            if( status == OK )
            {
                status = insert_in_voxel_queue(
                         &graphics->three_d.surface_extraction.voxels_to_do,
                         &voxel_indices );
            }

            if( status == OK )
            {
                status = mark_voxel_done( 
                           graphics->associated[SLICE_WINDOW]->slice.volume,
                           &graphics->three_d.surface_extraction.voxels_done,
                           &voxel_indices );
            }

            if( FALSE && status == OK )
            {
                add_voxel_neighbours(
                        graphics->associated[SLICE_WINDOW]->slice.volume,
                        voxel_indices.i[X_AXIS], voxel_indices.i[Y_AXIS],
                        voxel_indices.i[Z_AXIS],
                        &graphics->three_d.surface_extraction.voxels_done,
                        &graphics->three_d.surface_extraction.voxels_to_do );
            }

            start_surface_extraction( graphics );
        }
    }
}

public  void  start_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &graphics->three_d.surface_extraction;

    if( !surface_extraction->extraction_in_progress &&
        surface_extraction->isovalue_selected )
    {
        surface_extraction->extraction_in_progress = TRUE;
    }
}

public  void  stop_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    if( graphics->three_d.surface_extraction.extraction_in_progress )
    {
        graphics->three_d.surface_extraction.extraction_in_progress = FALSE;
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

    if( graphics->three_d.surface_extraction.extraction_in_progress &&
        voxels_remaining( &graphics->three_d.surface_extraction.voxels_to_do ) )
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
    volume_struct          *volume;
    Real                   value;
    int                    x, y, z;
    voxel_index_struct     *found_indices;
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
    void                                  add_voxel_neighbours();

    insert.i[X_AXIS] = MIN( x, volume->size[X_AXIS]-2 );
    insert.i[Y_AXIS] = MIN( y, volume->size[Y_AXIS]-2 );
    insert.i[Z_AXIS] = MIN( z, volume->size[Z_AXIS]-2 );

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
                                  indices.i[X_AXIS],
                                  indices.i[Y_AXIS],
                                  indices.i[Z_AXIS], value ) )
        {
            found_indices->i[X_AXIS] = indices.i[X_AXIS];
            found_indices->i[Y_AXIS] = indices.i[Y_AXIS];
            found_indices->i[Z_AXIS] = indices.i[Z_AXIS];
            found = TRUE;
        }
        else
        {
            add_voxel_neighbours( volume,
                                  indices.i[X_AXIS],
                                  indices.i[Y_AXIS],
                                  indices.i[Z_AXIS],
                                  &voxels_done, &voxels_to_check );
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

        if( check_voxel( graphics->associated[SLICE_WINDOW]->slice.volume,
                         surface_extraction, &voxel_index ) )
        {
            ++n_voxels_done;
            add_voxel_neighbours(
                    graphics->associated[SLICE_WINDOW]->slice.volume,
                    voxel_index.i[X_AXIS], voxel_index.i[Y_AXIS],
                    voxel_index.i[Z_AXIS],
                    &surface_extraction->voxels_done,
                    &surface_extraction->voxels_to_do );
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
    keys[0] = ijk( indices->i[X_AXIS],
                   indices->i[Y_AXIS],
                   indices->i[Z_AXIS],
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

private  Boolean   check_voxel( volume, surface_extraction, voxel_index )
    volume_struct               *volume;
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
{
    Status                 status;
    polygons_struct        *poly;
    triangle_point_type    *points_list;
    Real                   corner_values[2][2][2];
    int                    n_tris, n_nondegenerate_tris, tri, p, next_end;
    int                    x, y, z, pt_index;
    int                    point_id[3];

    for_less( x, 0, 2 )
    {
        for_less( y, 0, 2 )
        {
            for_less( z, 0, 2 )
            {
                corner_values[x][y][z] = (Real) ACCESS_VOLUME_DATA( *volume,
                                                  voxel_index->i[X_AXIS]+x,
                                                  voxel_index->i[Y_AXIS]+y,
                                                  voxel_index->i[Z_AXIS]+z );
            }
        }
    }

    n_tris = compute_isotriangles_in_voxel( corner_values,
                                            surface_extraction->isovalue,
                                            &points_list );

    n_nondegenerate_tris = 0;

    if( n_tris > 0 )
    {
        poly = &surface_extraction->triangles;

        status = OK;

        for_less( tri, 0, n_tris )
        {
            for_less( p, 0, 3 )
            {
                pt_index = 3 * tri + p;
                point_id[p] = get_edge_point_id(
                                      volume,
                                      &surface_extraction->triangles,
                                      surface_extraction->isovalue,
                                      &surface_extraction->edge_points,
                                      voxel_index,
                                      points_list[pt_index].coord,
                                      points_list[pt_index].edge_intersected );
            }

            if( point_id[0] != point_id[1] &&
                point_id[1] != point_id[2] &&
                point_id[2] != point_id[0] )
            {
                next_end = NUMBER_INDICES( *poly ) + 3;

                if( status == OK )
                {
                    CHECK_ALLOC1( status, poly->indices,
                                  3 * poly->n_items,
                                  3 * poly->n_items + 3,
                                  int, DEFAULT_CHUNK_SIZE );
                    poly->indices[next_end-3] = point_id[0];
                    poly->indices[next_end-2] = point_id[1];
                    poly->indices[next_end-1] = point_id[2];
                }

                if( status == OK )
                {
                    ADD_ELEMENT_TO_ARRAY( status, poly->n_items,
                                          poly->end_indices,
                                          next_end, int, DEFAULT_CHUNK_SIZE );
                }

                ++n_nondegenerate_tris;
            }
        }
    }

    return( n_nondegenerate_tris > 0 );
}

private  void  add_voxel_neighbours( volume, x, y, z, voxels_done, voxel_queue )
    volume_struct                       *volume;
    int                                 x, y, z;
    hash_table_struct                   *voxels_done;
    QUEUE_STRUCT(voxel_index_struct)    *voxel_queue;
{
    Status                   status;
    int                      x_offset, y_offset, z_offset;
    voxel_index_struct       neighbour;
    Boolean                  cube_is_within_volume();

    status = OK;

    for_inclusive( x_offset, -1, 1 )
    {
        neighbour.i[X_AXIS] = x + x_offset;

        for_inclusive( y_offset, -1, 1 )
        {
            neighbour.i[Y_AXIS] = y + y_offset;

            for_inclusive( z_offset, -1, 1 )
            {
                neighbour.i[Z_AXIS] = z + z_offset;

                if( (x_offset != 0 || y_offset != 0 || z_offset != 0) &&
                    cube_is_within_volume( volume,
                                           neighbour.i[X_AXIS],
                                           neighbour.i[Y_AXIS],
                                           neighbour.i[Z_AXIS] ) &&
                    !is_voxel_done( volume, voxels_done, &neighbour ) )
                {
                    status = mark_voxel_done( volume, voxels_done, &neighbour );
                    if( status == OK )
                    {
                        status = insert_in_voxel_queue( voxel_queue,&neighbour);
                    }
                }
            }
        }
    }
}

private  Status  initialize_edge_points( hash_table )
    hash_table_struct  *hash_table;
{
    Status     status;

    status = initialize_hash_table( hash_table,
                                    2, INITIAL_SIZE, ENLARGE_THRESHOLD,
                                    ENLARGE_DENSITY );

    return( status );
}

private  Status  delete_edge_points( hash_table )
    hash_table_struct  *hash_table;
{
    Status               status;
    hash_table_pointer   hash_ptr;
    edge_point_struct    *edge_info;

    initialize_hash_pointer( &hash_ptr );

    status = OK;

    while( status == OK && get_next_hash_entry( hash_table, &hash_ptr,
                                                (char **) &edge_info ) )
    {
        FREE1( status, edge_info );
    }

    if( status == OK )
    {
        status = delete_hash_table( hash_table );
    }

    return( status );
}

private  void  get_edge_point_keys( volume, voxel, edge_intersected, keys )
    volume_struct        *volume;
    voxel_index_struct   *voxel;
    int                  edge_intersected;
    int                  keys[];
{
    keys[0] = ijk( voxel->i[X_AXIS],
                   voxel->i[Y_AXIS],
                   voxel->i[Z_AXIS],
                   volume->size[Y_AXIS],
                   volume->size[Z_AXIS] );

    keys[1] = edge_intersected;
}

private  int  get_edge_point_id( volume, tris, isovalue, hash_table,
                                 voxel, offset_coord,
                                 edge_intersected )
    volume_struct       *volume;
    polygons_struct     *tris;
    Real                isovalue;
    hash_table_struct   *hash_table;
    voxel_index_struct  *voxel;
    int                 offset_coord[N_DIMENSIONS];
    int                 edge_intersected;
{
    Status               status;
    int                  point_id, keys[2];
    edge_point_struct    *edge_info;
    Point_classes        pt_class;
    voxel_index_struct   corner;

    corner.i[X_AXIS] = voxel->i[X_AXIS] + offset_coord[X_AXIS];
    corner.i[Y_AXIS] = voxel->i[Y_AXIS] + offset_coord[Y_AXIS];
    corner.i[Z_AXIS] = voxel->i[Z_AXIS] + offset_coord[Z_AXIS];

    get_edge_point_keys( volume, &corner, edge_intersected, keys );

    if( lookup_in_hash_table( hash_table, keys, (char **) &edge_info ) )
    {
        point_id = edge_info->point_index;
    }
    else
    {
        point_id = create_point( volume, isovalue, tris, &corner,
                                 edge_intersected, &pt_class );

        CALLOC1( status, edge_info, 1, edge_point_struct );

        edge_info->point_index = point_id;

        if( status == OK )
        {
            status = insert_in_hash_table( hash_table, keys,
                                           (char *) edge_info );
        }
    }

    return( point_id );
}

private  int   create_point( volume, isovalue, tris, voxel, edge_intersected,
                             pt_class )
    volume_struct       *volume;
    Real                isovalue;
    polygons_struct     *tris;
    voxel_index_struct  *voxel;
    int                 edge_intersected;
    Point_classes       *pt_class;
{
    Status    status;
    int       x, y, z, pt_index;
    Real      x_bar, y_bar, z_bar, dx, dy, dz;
    Real      val1, val2, alpha;
    Point     point;
    Vector    normal;
    int       corner[N_DIMENSIONS];
    Real      c000, c001, c002, c003, c010, c011, c012, c013;
    Real      c020, c021, c022, c023, c030, c031, c032, c033;
    Real      c100, c101, c102, c103, c110, c111, c112, c113;
    Real      c120, c121, c122, c123, c130, c131, c132, c133;
    Real      c200, c201, c202, c203, c210, c211, c212, c213;
    Real      c220, c221, c222, c223, c230, c231, c232, c233;
    Real      c300, c301, c302, c303, c310, c311, c312, c313;
    Real      c320, c321, c322, c323, c330, c331, c332, c333;

    corner[X_AXIS] = voxel->i[X_AXIS];
    corner[Y_AXIS] = voxel->i[Y_AXIS];
    corner[Z_AXIS] = voxel->i[Z_AXIS];

    val1 = isovalue - (Real) ACCESS_VOLUME_DATA( *volume,
                             corner[X_AXIS], corner[Y_AXIS], corner[Z_AXIS] );

    ++corner[edge_intersected];

    val2 = isovalue - (Real) ACCESS_VOLUME_DATA( *volume,
                             corner[X_AXIS], corner[Y_AXIS], corner[Z_AXIS] );

    if( val1 == 0.0 )
    {
        *pt_class = ON_FIRST_CORNER;
        alpha = 0.0;
    }
    else if( val1 == 1.0 )
    {
        *pt_class = ON_SECOND_CORNER;
        alpha = 1.0;
    }
    else
    {
        *pt_class = ON_EDGE;
        alpha = val1 / (val1 - val2);
    }

    fill_Point( point, (Real) voxel->i[X_AXIS],
                       (Real) voxel->i[Y_AXIS],
                       (Real) voxel->i[Z_AXIS] )

    Point_coord( point, edge_intersected ) += alpha;

    x = (int) Point_x( point );
    y = (int) Point_y( point );
    z = (int) Point_z( point );

    x_bar = FRACTION( Point_x(point) );
    y_bar = FRACTION( Point_y(point) );
    z_bar = FRACTION( Point_z(point) );


    c000 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z-1 );
    c001 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+0 );
    c002 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+1 );
    c003 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+2 );

    c010 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z-1 );
    c011 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+0 );
    c012 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+1 );
    c013 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+2 );

    c020 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z-1 );
    c021 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+0 );
    c022 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+1 );
    c023 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+2 );

    c030 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z-1 );
    c031 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+0 );
    c032 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+1 );
    c033 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+2 );


    c100 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z-1 );
    c101 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+0 );
    c102 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+1 );
    c103 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+2 );

    c110 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z-1 );
    c111 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+0 );
    c112 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+1 );
    c113 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+2 );

    c120 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z-1 );
    c121 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+0 );
    c122 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+1 );
    c123 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+2 );

    c130 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z-1 );
    c131 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+0 );
    c132 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+1 );
    c133 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+2 );


    c200 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z-1 );
    c201 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+0 );
    c202 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+1 );
    c203 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+2 );

    c210 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z-1 );
    c211 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+0 );
    c212 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+1 );
    c213 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+2 );

    c220 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z-1 );
    c221 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+0 );
    c222 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+1 );
    c223 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+2 );

    c230 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z-1 );
    c231 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+0 );
    c232 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+1 );
    c233 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+2 );


    c300 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z-1 );
    c301 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+0 );
    c302 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+1 );
    c303 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+2 );

    c310 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z-1 );
    c311 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+0 );
    c312 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+1 );
    c313 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+2 );

    c320 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z-1 );
    c321 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+0 );
    c322 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+1 );
    c323 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+2 );

    c330 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z-1 );
    c331 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+0 );
    c332 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+1 );
    c333 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+2 );
  
    CUBIC_TRIVAR_DERIV( c, x_bar, y_bar, z_bar, dx, dy, dz );

    fill_Vector( normal, dx, dy, dz );

    NORMALIZE_VECTOR( normal, normal );

    if( Normal_towards_lower )
    {
        SCALE_VECTOR( normal, normal, -1.0 );
    }

    CHECK_ALLOC1( status, tris->points, tris->n_points, tris->n_points+1,
                  Point, DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        CHECK_ALLOC1( status, tris->normals,
                      tris->n_points, tris->n_points+1,
                      Vector, DEFAULT_CHUNK_SIZE );
    }

    if( status == OK )
    {
        pt_index = tris->n_points;
        tris->points[pt_index] = point;
        tris->normals[pt_index] = normal;
        ++tris->n_points;
    }

    return( pt_index );
}

private  void  display_triangles( graphics )
    graphics_struct  *graphics;
{
    void  draw_polygons();

    draw_polygons( graphics, &graphics->three_d.surface_extraction.triangles );
}
