
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_marching_cubes.h>
#include  <def_splines.h>
#include  <def_bitlist.h>

public  Status  initialize_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    Status                      status;
    surface_extraction_struct   *surface_extraction;
    Status                      create_object();
    Status                      add_object_to_model();
    object_struct               *object;
    Status                      clear_surface_extraction();
    void                        install_surface_extraction();

    surface_extraction = &graphics->three_d.surface_extraction;

    status = create_object( &object, POLYGONS );

    if( status == OK )
    {
        status = add_object_to_model(
                     graphics->models[THREED_MODEL]->ptr.model, object );
    }

    if( status == OK )
    {
        surface_extraction->triangles = object->ptr.polygons;
    }

    if( status == OK )
    {
        install_surface_extraction( graphics );

        status = clear_surface_extraction( graphics );
    }

    return( status );
}

private  Status  clear_surface_extraction( graphics )
    graphics_struct    *graphics;
{
    Status                      status;
    void                        empty_polygons_struct();
    Status                      initialize_edge_points();
    void                        initialize_voxel_queue();
    void                        clear_voxel_flags();
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &graphics->three_d.surface_extraction;

    surface_extraction->extraction_in_progress = FALSE;
    surface_extraction->isovalue_selected = FALSE;

    status = initialize_edge_points( &surface_extraction->edge_points );

    if( status == OK )
    {
        initialize_voxel_queue( &surface_extraction->voxels_to_do );

        empty_polygons_struct( surface_extraction->triangles,
                               &Extracted_surface_colour,
                               &Default_surface_property );

        clear_voxel_flags( &surface_extraction->voxels_queued );

        clear_voxel_flags( &surface_extraction->voxels_done );
    }

    return( status );
}

private  Status  free_surface_extraction( graphics )
    graphics_struct   *graphics;
{
    Status                      status;
    Status                      delete_edge_points();
    Status                      delete_voxel_queue();
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &graphics->three_d.surface_extraction;

    status = delete_edge_points( &surface_extraction->edge_points );

    if( status == OK )
    {
        status = delete_voxel_queue( &surface_extraction->voxels_to_do );
    }

    return( status );
}

public  Status  delete_surface_extraction( graphics )
    graphics_struct   *graphics;
{
    Status                      status;
    Status                      delete_voxel_flags();
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &graphics->three_d.surface_extraction;

    status = free_surface_extraction( graphics );

    if( status == OK )
    {
        status = delete_voxel_flags( &surface_extraction->voxels_done );
    }

    if( status == OK )
    {
        status = delete_voxel_flags( &surface_extraction->voxels_queued );
    }

    return( status );
}

public  Status  reset_surface_extraction( graphics )
    graphics_struct   *graphics;
{
    Status    status;
    Status    free_surface_extraction();
    Status    clear_surface_extraction();

    status = free_surface_extraction( graphics );

    if( status == OK )
    {
        status = clear_surface_extraction( graphics );
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
    Status                      set_voxel_flag();
    Status                      reset_voxel_flag();
    void                        start_surface_extraction();
    void                        get_next_voxel_from_queue();

    surface_extraction = &graphics->three_d.surface_extraction;

    status = OK;

    if( cube_is_within_volume(
             graphics->associated[SLICE_WINDOW]->slice.volume, x, y, z ) )
    {
        if( !surface_extraction->isovalue_selected )
        {
            set_isosurface_value( surface_extraction );
        }
        else
        {
            while( voxels_remaining(
                     &graphics->three_d.surface_extraction.voxels_to_do ) )
            {
                get_next_voxel_from_queue( 
                     &graphics->three_d.surface_extraction.voxels_to_do,
                     &voxel_indices );

                if( status == OK )
                {
                    status = reset_voxel_flag(
                         graphics->associated[SLICE_WINDOW]->slice.volume,
                         &graphics->three_d.surface_extraction.voxels_queued,
                         &voxel_indices );
                }
            }
        }

        if( find_close_voxel_containing_value(
                  graphics->associated[SLICE_WINDOW]->slice.volume,
                  &graphics->three_d.surface_extraction.voxels_done,
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
                status = set_voxel_flag( 
                           graphics->associated[SLICE_WINDOW]->slice.volume,
                           &graphics->three_d.surface_extraction.voxels_queued,
                           &voxel_indices );
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

private  Boolean  find_close_voxel_containing_value( volume, voxels_done, value,
                                                     x, y, z, found_indices )
    volume_struct          *volume;
    bitlist_struct         *voxels_done;
    Real                   value;
    int                    x, y, z;
    voxel_index_struct     *found_indices;
{
    Status                                status;
    Boolean                               found, voxel_done, voxel_contains;
    Boolean                               voxel_contains_value();
    QUEUE_STRUCT( voxel_index_struct )    voxels_to_check;
    voxel_index_struct                    indices, insert;
    bitlist_struct                        voxels_searched;
    Status                                set_voxel_flag();
    Status                                delete_voxel_flags();
    Status                                insert_in_voxel_queue();
    Status                                delete_voxel_queue();
    Status                                initialize_voxel_flags();
    void                                  initialize_voxel_queue();
    void                                  get_next_voxel_from_queue();
    void                                  add_voxel_neighbours();

    insert.i[X_AXIS] = MIN( x, volume->size[X_AXIS]-2 );
    insert.i[Y_AXIS] = MIN( y, volume->size[Y_AXIS]-2 );
    insert.i[Z_AXIS] = MIN( z, volume->size[Z_AXIS]-2 );

    found = FALSE;

    status = initialize_voxel_flags( &voxels_searched, get_n_voxels(volume) );

    initialize_voxel_queue( &voxels_to_check );

    if( status == OK )
    {
        status = insert_in_voxel_queue( &voxels_to_check, &insert );
    }

    if( status == OK )
    {
        status = set_voxel_flag( volume, &voxels_searched, &insert );
    }

    while( !found && status == OK && voxels_remaining(&voxels_to_check) )
    {
        get_next_voxel_from_queue( &voxels_to_check, &indices );

        voxel_contains = voxel_contains_value( volume,
                                               indices.i[X_AXIS],
                                               indices.i[Y_AXIS],
                                               indices.i[Z_AXIS], value );

        voxel_done = get_voxel_flag( volume, voxels_done, &indices );

        if( voxel_contains && !voxel_done )
        {
            found_indices->i[X_AXIS] = indices.i[X_AXIS];
            found_indices->i[Y_AXIS] = indices.i[Y_AXIS];
            found_indices->i[Z_AXIS] = indices.i[Z_AXIS];
            found = TRUE;
        }
        else if( voxel_contains || !voxel_done )
        {
            add_voxel_neighbours( volume,
                                  indices.i[X_AXIS],
                                  indices.i[Y_AXIS],
                                  indices.i[Z_AXIS],
                                  voxel_done, value,
                                  &voxels_searched, &voxels_to_check );
        }
    }

    if( status == OK )
    {
        status = delete_voxel_flags( &voxels_searched );
    }

    if( status == OK )
    {
        status = delete_voxel_queue( &voxels_to_check );
    }

    return( found );
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
    Real                        stop_time;
    Real                        current_realtime_seconds();
    void    possibly_output();

    n_voxels_done = 0;

    surface_extraction = &graphics->three_d.surface_extraction;

    stop_time = current_realtime_seconds() + Max_seconds_per_voxel_update;

    while( (n_voxels_done < Min_voxels_per_update ||
           (n_voxels_done < Max_voxels_per_update &&
            current_realtime_seconds() < stop_time) ) &&
           voxels_remaining( &surface_extraction->voxels_to_do ) )
    {
        possibly_output( surface_extraction->triangles );

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
                    TRUE, surface_extraction->isovalue,
                    &surface_extraction->voxels_queued,
                    &surface_extraction->voxels_to_do );
        }
    }
}

private  Boolean   check_voxel( volume, surface_extraction, voxel_index )
    volume_struct               *volume;
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
{
    Status                 status;
    Status                 set_voxel_flag();
    Boolean                lookup_edge_point_id();
    Status                 record_edge_point_id();
    Status                 delete_edge_points_no_longer_needed();
    polygons_struct        *poly;
    triangle_point_type    *points_list, *pt;
    voxel_index_struct     corner_points[MAX_POINTS_PER_VOXEL];
    voxel_index_struct     corner;
    Real                   corner_values[2][2][2];
    Boolean                active, connected;
    Boolean                tri_connected[4], changed;
    int                    id, n_connected, axis, cache_pt[N_DIMENSIONS];
    struct
    {
        Boolean  in_cache;
        int      point_id;
    }                      corner_cache[2][2][2][N_DIMENSIONS];
    Boolean                are_voxel_corners_active();
    int                    n_tris, n_nondegenerate_tris, tri, p, next_end;
    int                    x, y, z, pt_index;
    int                    point_ids[3];
    Point_classes          pt_class;

    active = are_voxel_corners_active( volume,
                                       voxel_index->i[X_AXIS],
                                       voxel_index->i[Y_AXIS],
                                       voxel_index->i[Z_AXIS] );

    if( active )
    {
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
    }

    status = OK;

    n_nondegenerate_tris = 0;

    if( active && n_tris > 0 )
    {
        poly = surface_extraction->triangles;

        for_less( tri, 0, n_tris )
        {
            tri_connected[tri] = FALSE;
        }

        changed = TRUE;
        n_connected = 0;

        for_less( x, 0, 2 )
        {
            for_less( y, 0, 2 )
            {
                for_less( z, 0, 2 )
                {
                    for_less( axis, 0, N_DIMENSIONS )
                    {
                        corner_cache[x][y][z][axis].in_cache = FALSE;
                    }
                }
            }
        }

        for_less( tri, 0, n_tris )
        {
            for_less( p, 0, 3 )
            {
                pt_index = 3 * tri + p;
                pt = &points_list[pt_index];

                corner_points[pt_index].i[X_AXIS] = voxel_index->i[X_AXIS] +
                                                    pt->coord[X_AXIS];
                corner_points[pt_index].i[Y_AXIS] = voxel_index->i[Y_AXIS] +
                                                    pt->coord[Y_AXIS];
                corner_points[pt_index].i[Z_AXIS] = voxel_index->i[Z_AXIS] +
                                                    pt->coord[Z_AXIS];
                if( !corner_cache[pt->coord[X_AXIS]]
                                 [pt->coord[Y_AXIS]]
                                 [pt->coord[Z_AXIS]]
                                 [pt->edge_intersected].in_cache )
                {
                    if( !lookup_edge_point_id( volume,
                                              &surface_extraction->edge_points,
                                              &corner_points[pt_index],
                                              pt->edge_intersected, &id ) )
                    {
                        id = -1;
                    }

                    corner_cache[pt->coord[X_AXIS]]
                                [pt->coord[Y_AXIS]]
                                [pt->coord[Z_AXIS]]
                                [pt->edge_intersected].in_cache = TRUE;
                    corner_cache[pt->coord[X_AXIS]]
                                [pt->coord[Y_AXIS]]
                                [pt->coord[Z_AXIS]]
                                [pt->edge_intersected].point_id = id;
                }
            }
        }

        while( n_connected < n_tris && changed )
        {
            changed = FALSE;

            for_less( tri, 0, n_tris )
            {
                if( !tri_connected[tri] )
                {
                    connected = (poly->n_items == 0);
                    connected = TRUE;

                    for_less( p, 0, 3 )
                    {
                        pt_index = 3 * tri + p;
                        pt = &points_list[pt_index];

                        if( corner_cache[pt->coord[X_AXIS]]
                                        [pt->coord[Y_AXIS]]
                                        [pt->coord[Z_AXIS]]
                                        [pt->edge_intersected].point_id >= 0 )
                        {
                            connected = TRUE;
                            break;
                        }
                    }

                    if( connected )
                    {
                        for_less( p, 0, 3 )
                        {
                            pt_index = 3 * tri + p;
                            pt = &points_list[pt_index];

                            if( corner_cache[pt->coord[X_AXIS]]
                                            [pt->coord[Y_AXIS]]
                                            [pt->coord[Z_AXIS]]
                                            [pt->edge_intersected].point_id < 0)
                            {
                                id = create_point( volume,
                                         surface_extraction->isovalue,
                                         surface_extraction->triangles,
                                         &corner_points[pt_index],
                                         pt->edge_intersected, &pt_class );

                                if( pt_class == ON_FIRST_CORNER ||
                                    pt_class == ON_SECOND_CORNER )
                                {
                                    corner = corner_points[pt_index];

                                    cache_pt[X_AXIS] = pt->coord[X_AXIS];
                                    cache_pt[Y_AXIS] = pt->coord[Y_AXIS];
                                    cache_pt[Z_AXIS] = pt->coord[Z_AXIS];

                                    if( pt_class == ON_SECOND_CORNER )
                                    {
                                        ++corner.i[pt->edge_intersected];
                                        ++cache_pt[pt->edge_intersected];
                                    }

                                    for_less( axis, 0, N_DIMENSIONS )
                                    {
                                        if( corner.i[axis] > 0 )
                                        {
                                            --corner.i[axis];
                                            status = record_edge_point_id(
                                               volume,
                                               &surface_extraction->edge_points,
                                               &corner, axis, id );
                                            ++corner.i[axis];
                                        }

                                        if( cache_pt[axis] == 1 )
                                        {
                                            --cache_pt[axis];
                                            corner_cache[cache_pt[X_AXIS]]
                                                        [cache_pt[Y_AXIS]]
                                                        [cache_pt[Z_AXIS]]
                                                        [axis].point_id = id;
                                            ++cache_pt[axis];
                                        }

                                        status = record_edge_point_id( volume,
                                             &surface_extraction->edge_points,
                                             &corner, axis, id );

                                        corner_cache[cache_pt[X_AXIS]]
                                                    [cache_pt[Y_AXIS]]
                                                    [cache_pt[Z_AXIS]]
                                                    [axis].point_id = id;
                                    }
                                }
                                else
                                {
                                    status = record_edge_point_id( volume,
                                             &surface_extraction->edge_points,
                                             &corner_points[pt_index],
                                             pt->edge_intersected, id );

                                    corner_cache[pt->coord[X_AXIS]]
                                                [pt->coord[Y_AXIS]]
                                                [pt->coord[Z_AXIS]]
                                                [pt->edge_intersected].point_id
                                                = id;
                                }

                                if( status != OK )
                                {
                                    corner_cache[pt->coord[X_AXIS]]
                                                [pt->coord[Y_AXIS]]
                                                [pt->coord[Z_AXIS]]
                                                [pt->edge_intersected].point_id
                                                  = -1;
                                }
                            }
                        }

                        ++n_connected;
                        tri_connected[tri] = TRUE;
                        changed = TRUE;
                    }
                }
            }
        }

        for_less( tri, 0, n_tris )
        {
            if( tri_connected[tri] )
            {
                for_less( p, 0, 3 )
                {
                    pt_index = 3 * tri + p;
                    pt = &points_list[pt_index];

                    point_ids[p] = corner_cache[pt->coord[X_AXIS]]
                                               [pt->coord[Y_AXIS]]
                                               [pt->coord[Z_AXIS]]
                                               [pt->edge_intersected].point_id;
                }
            }

            if( tri_connected[tri] &&
                point_ids[0] != point_ids[1] &&
                point_ids[1] != point_ids[2] &&
                point_ids[2] != point_ids[0] )
            {
                next_end = NUMBER_INDICES( *poly ) + 3;

                if( status == OK )
                {
                    CHECK_ALLOC1( status, poly->indices,
                                  3 * poly->n_items,
                                  3 * poly->n_items + 3,
                                  int, DEFAULT_CHUNK_SIZE );
                    poly->indices[next_end-3] = point_ids[0];
                    poly->indices[next_end-2] = point_ids[1];
                    poly->indices[next_end-1] = point_ids[2];
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

    if( status == OK )
    {
        status = set_voxel_flag( volume, &surface_extraction->voxels_done,
                                 voxel_index);
    }

    if( status == OK )
    {
        status = delete_edge_points_no_longer_needed( volume,
                                       voxel_index,
                                       &surface_extraction->voxels_done,
                                       &surface_extraction->edge_points );
    }

    return( n_nondegenerate_tris > 0 );
}

private  void  add_voxel_neighbours( volume, x, y, z, surface_only, isovalue,
                                     voxels_queued, voxel_queue )
    volume_struct                       *volume;
    int                                 x, y, z;
    Boolean                             surface_only;
    Real                                isovalue;
    bitlist_struct                      *voxels_queued;
    QUEUE_STRUCT(voxel_index_struct)    *voxel_queue;
{
    Status                   status;
    int                      x_offset, y_offset, z_offset;
    voxel_index_struct       neighbour;
    Boolean                  cube_is_within_volume();
    Status                   insert_in_voxel_queue();
    Status                   set_voxel_flag();

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
                    !get_voxel_flag( volume, voxels_queued, &neighbour ) )
                {
                    status = set_voxel_flag( volume, voxels_queued, &neighbour);
                    if( status == OK &&
                        (!surface_only ||
                         voxel_contains_value( volume,
                                               neighbour.i[X_AXIS],
                                               neighbour.i[Y_AXIS],
                                               neighbour.i[Z_AXIS],
                                               isovalue )) )
                    {
                        status = insert_in_voxel_queue( voxel_queue,&neighbour);
                    }
                }
            }
        }
    }
}

private  void  add_square_neighbours( volume, x, y, z, axis_index,
                                      voxels_queued, voxel_queue )
    volume_struct                       *volume;
    int                                 x, y, z;
    int                                 axis_index;
    bitlist_struct                      *voxels_queued;
    QUEUE_STRUCT(voxel_index_struct)    *voxel_queue;
{
    Status                status;
    int                   axis, dir;
    voxel_index_struct    neighbour;
    Status                insert_in_voxel_queue();
    Status                set_voxel_flag();

    status = OK;

    for_less( axis, 0, N_DIMENSIONS )
    {
        for( dir = -1;  dir <= 1;  dir += 2 )
        {
            if( axis != axis_index )
            {
                neighbour.i[X_AXIS] = x;
                neighbour.i[Y_AXIS] = y;
                neighbour.i[Z_AXIS] = z;
                neighbour.i[axis] += dir;

                if( cube_is_within_volume( volume,
                                           neighbour.i[X_AXIS],
                                           neighbour.i[Y_AXIS],
                                           neighbour.i[Z_AXIS] ) &&
                    !get_voxel_flag( volume, voxels_queued, &neighbour ) )
                {
                    status = set_voxel_flag( volume, voxels_queued, &neighbour);
                    if( status == OK )
                    {
                        status = insert_in_voxel_queue( voxel_queue,
                                                        &neighbour );
                    }
                }
            }
        }
    }
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
    int       i, x, y, z, pt_index;
    Real      u_bar[N_DIMENSIONS], dx, dy, dz;
    Real      alpha1, alpha2, val1, val2, val, alpha;
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
    else if( val2 == 0.0 )
    {
        *pt_class = ON_SECOND_CORNER;
        alpha = 1.0;
    }
    else
    {
        *pt_class = ON_EDGE;
        alpha = val1 / (val1 - val2);
    }

    x = voxel->i[X_AXIS];
    y = voxel->i[Y_AXIS];
    z = voxel->i[Z_AXIS];

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


    c100 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y-1, z-1 );
    c101 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y-1, z+0 );
    c102 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y-1, z+1 );
    c103 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y-1, z+2 );

    c110 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+0, z-1 );
    c111 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+0, z+0 );
    c112 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+0, z+1 );
    c113 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+0, z+2 );

    c120 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+1, z-1 );
    c121 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+1, z+0 );
    c122 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+1, z+1 );
    c123 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+1, z+2 );

    c130 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+2, z-1 );
    c131 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+2, z+0 );
    c132 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+2, z+1 );
    c133 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+2, z+2 );


    c200 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y-1, z-1 );
    c201 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y-1, z+0 );
    c202 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y-1, z+1 );
    c203 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y-1, z+2 );

    c210 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+0, z-1 );
    c211 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+0, z+0 );
    c212 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+0, z+1 );
    c213 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+0, z+2 );

    c220 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+1, z-1 );
    c221 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+1, z+0 );
    c222 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+1, z+1 );
    c223 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+1, z+2 );

    c230 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+2, z-1 );
    c231 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+2, z+0 );
    c232 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+2, z+1 );
    c233 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+2, z+2 );


    c300 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y-1, z-1 );
    c301 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y-1, z+0 );
    c302 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y-1, z+1 );
    c303 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y-1, z+2 );

    c310 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+0, z-1 );
    c311 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+0, z+0 );
    c312 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+0, z+1 );
    c313 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+0, z+2 );

    c320 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+1, z-1 );
    c321 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+1, z+0 );
    c322 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+1, z+1 );
    c323 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+1, z+2 );

    c330 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+2, z-1 );
    c331 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+2, z+0 );
    c332 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+2, z+1 );
    c333 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+2, z+2 );

    u_bar[X_AXIS] = 0.0;
    u_bar[Y_AXIS] = 0.0;
    u_bar[Z_AXIS] = 0.0;

    alpha1 = 0.0;
    alpha2 = 1.0;

    for_less( i, 0, Max_surface_refinements )
    {
        u_bar[edge_intersected] = alpha;

        CUBIC_TRIVAR( c, u_bar[X_AXIS], u_bar[Y_AXIS], u_bar[Z_AXIS],
                      val );

        val = isovalue - val;

/*
        if( (alpha == 0.0 && val != val1) ||
            (alpha == 1.0 && val != val2) )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement val\n" );
        }
*/

        if( (val1 <= 0.0 && val <= 0.0) ||
            (val1 >= 0.0 && val >= 0.0) )
        {
            val1 = val;
            alpha1 = alpha;
        }
        else
        {
            val2 = val;
            alpha2 = alpha;
        }

        if( val1 != val2 )
        {
            alpha = alpha1 + val1 / (val1 - val2) * (alpha2 - alpha1);
        }
        else if( val != 0.0 )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement\n" );
        }

        if( alpha < 0.0 || alpha > 1.0 )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement alpha\n" );
        }

        if( ABS(val) < Max_surface_error )
        {
            break;
        }
    }

    u_bar[edge_intersected] = alpha;
  
    CUBIC_TRIVAR_DERIV( c, u_bar[X_AXIS], u_bar[Y_AXIS], u_bar[Z_AXIS],
                        dx, dy, dz );

    fill_Vector( normal, dx, dy, dz );

    Vector_x(normal) /= volume->slice_thickness[X_AXIS];
    Vector_y(normal) /= volume->slice_thickness[Y_AXIS];
    Vector_z(normal) /= volume->slice_thickness[Z_AXIS];


    NORMALIZE_VECTOR( normal, normal );

    if( Normal_towards_lower )
    {
        SCALE_VECTOR( normal, normal, -1.0 );
    }

    fill_Point( point, (Real) voxel->i[X_AXIS], (Real) voxel->i[Y_AXIS],
                       (Real) voxel->i[Z_AXIS] )

    Point_coord( point, edge_intersected ) += alpha;

    Point_x(point) *= volume->slice_thickness[X_AXIS];
    Point_y(point) *= volume->slice_thickness[Y_AXIS];
    Point_z(point) *= volume->slice_thickness[Z_AXIS];

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

public  int  get_n_voxels( volume )
    volume_struct  *volume;
{
    int   n_voxels;

    if( volume != (volume_struct *) 0 )
    {
        n_voxels = (volume->size[X_AXIS] - 1) *
                   (volume->size[Y_AXIS] - 1) *
                   (volume->size[Z_AXIS] - 1);
    }
    else
    {
        n_voxels = 0;
    }

    return( n_voxels );
}

public  Boolean  are_voxel_corners_active( volume, x, y, z )
    volume_struct   *volume;
    int             x, y, z;
{
    if( One_active_flag )
    return( get_voxel_activity( volume, x  , y  , z   ) ||
            get_voxel_activity( volume, x  , y  , z+1 ) ||
            get_voxel_activity( volume, x  , y+1, z   ) ||
            get_voxel_activity( volume, x  , y+1, z+1 ) ||
            get_voxel_activity( volume, x+1, y  , z   ) ||
            get_voxel_activity( volume, x+1, y  , z+1 ) ||
            get_voxel_activity( volume, x+1, y+1, z   ) ||
            get_voxel_activity( volume, x+1, y+1, z+1 ) );
    else
    return( get_voxel_activity( volume, x  , y  , z   ) &&
            get_voxel_activity( volume, x  , y  , z+1 ) &&
            get_voxel_activity( volume, x  , y+1, z   ) &&
            get_voxel_activity( volume, x  , y+1, z+1 ) &&
            get_voxel_activity( volume, x+1, y  , z   ) &&
            get_voxel_activity( volume, x+1, y  , z+1 ) &&
            get_voxel_activity( volume, x+1, y+1, z   ) &&
            get_voxel_activity( volume, x+1, y+1, z+1 ) );
}

public  Boolean  are_any_voxel_corners_inactive( volume, x, y, z )
    volume_struct   *volume;
    int             x, y, z;
{
    return( get_voxel_inactivity_flag( volume, x  , y  , z   ) ||
            get_voxel_inactivity_flag( volume, x  , y  , z+1 ) ||
            get_voxel_inactivity_flag( volume, x  , y+1, z   ) ||
            get_voxel_inactivity_flag( volume, x  , y+1, z+1 ) ||
            get_voxel_inactivity_flag( volume, x+1, y  , z   ) ||
            get_voxel_inactivity_flag( volume, x+1, y  , z+1 ) ||
            get_voxel_inactivity_flag( volume, x+1, y+1, z   ) ||
            get_voxel_inactivity_flag( volume, x+1, y+1, z+1 ) );
}

public  Boolean  are_any_square_corners_inactive( volume, x, y, z, axis_index )
    volume_struct   *volume;
    int             x, y, z;
    int             axis_index;
{
    Boolean  inactive;
    int      indices[N_DIMENSIONS], corner_indices[N_DIMENSIONS];
    int      x_index, y_index;

    x_index = (axis_index + 1) % N_DIMENSIONS;
    y_index = (axis_index + 2) % N_DIMENSIONS;

    indices[X_AXIS] = x;
    indices[Y_AXIS] = y;
    indices[Z_AXIS] = z;

    corner_indices[axis_index] = indices[axis_index];

    inactive = FALSE;

    for_less( corner_indices[x_index], indices[x_index], indices[x_index]+2 )
    {
        for_less( corner_indices[y_index], indices[y_index],
                  indices[y_index]+2 )
        {
            if( get_voxel_inactivity_flag( volume, corner_indices[X_AXIS],
                                                   corner_indices[Y_AXIS],
                                                   corner_indices[Z_AXIS] ) )
            {
                inactive = TRUE;
                break;
            }
        }

        if( inactive ) break;
    }

    return( inactive );
}

public  void  generate_activity_from_point( graphics, x, y, z )
    graphics_struct    *graphics;
    int                x, y, z;
{
    surface_extraction_struct   *surface_extraction;
    Boolean                     cube_is_within_volume();
    Boolean                     find_close_voxel_containing_value();
    voxel_index_struct          voxel_indices;
    void                        set_all_voxel_activities();
    void                        connect_active_voxels();

    surface_extraction = &graphics->three_d.surface_extraction;

    if( cube_is_within_volume(
             graphics->associated[SLICE_WINDOW]->slice.volume, x, y, z ) )
    {
        if( !surface_extraction->isovalue_selected )
        {
            set_isosurface_value( surface_extraction );
        }

        if( find_close_voxel_containing_value(
                  graphics->associated[SLICE_WINDOW]->slice.volume,
                  &graphics->three_d.surface_extraction.voxels_done,
                  graphics->three_d.surface_extraction.isovalue,
                  x, y, z, &voxel_indices ) )
        {
            set_all_voxel_activities( 
                     graphics->associated[SLICE_WINDOW]->slice.volume, FALSE );

            connect_active_voxels( 
                  graphics->associated[SLICE_WINDOW]->slice.volume,
                  graphics->three_d.surface_extraction.isovalue,
                  &voxel_indices );
        }
    }
}

private  void  connect_active_voxels( volume, isovalue, start_voxel )
    volume_struct          *volume;
    Real                   isovalue;
    voxel_index_struct     *start_voxel;
{
    Status                                status;
    Boolean                               voxel_contains_value();
    Boolean                               active;
    Boolean                               are_any_voxel_corners_inactive();
    QUEUE_STRUCT( voxel_index_struct )    voxels_to_check;
    voxel_index_struct                    indices;
    bitlist_struct                        voxels_searched;
    Status                                set_voxel_flag();
    Status                                delete_voxel_flags();
    Status                                insert_in_voxel_queue();
    Status                                delete_voxel_queue();
    Status                                initialize_voxel_flags();
    void                                  initialize_voxels_queue();
    void                                  get_next_voxel_from_queue();
    void                                  add_voxel_neighbours();
    void                                  set_voxel_corners_active();
    void                                  initialize_voxel_queue();
int    count;
Real   next_time;
Real   current_realtime_seconds();

    indices.i[X_AXIS] = MIN( start_voxel->i[X_AXIS], volume->size[X_AXIS]-2 );
    indices.i[Y_AXIS] = MIN( start_voxel->i[Y_AXIS], volume->size[Y_AXIS]-2 );
    indices.i[Z_AXIS] = MIN( start_voxel->i[Z_AXIS], volume->size[Z_AXIS]-2 );

    status = initialize_voxel_flags( &voxels_searched, get_n_voxels( volume ) );

    initialize_voxel_queue( &voxels_to_check );

    if( status == OK )
    {
        status = insert_in_voxel_queue( &voxels_to_check, &indices );
    }

    if( status == OK )
    {
        status = set_voxel_flag( volume, &voxels_searched, &indices );
    }

    if( status == OK )
    {
        set_voxel_corners_active( volume,
                     indices.i[X_AXIS], indices.i[Y_AXIS], indices.i[Z_AXIS] );
    }

next_time = current_realtime_seconds() + 5.0;
count = 100;

    while( status == OK && voxels_remaining(&voxels_to_check) )
    {
        --count;
        if( count == 0 )
        {
            count = 100;

            if( current_realtime_seconds() >= next_time )
            {
                next_time = current_realtime_seconds() + 5.0;
                PRINT( "Voxels in queue %d\n", NUMBER_IN_QUEUE(voxels_to_check) );
            }
        }

        get_next_voxel_from_queue( &voxels_to_check, &indices );

        active = !are_any_voxel_corners_inactive( volume,
                                                  indices.i[X_AXIS],
                                                  indices.i[Y_AXIS],
                                                  indices.i[Z_AXIS] );

        if( active )
        {
            if( voxel_contains_value( volume, indices.i[X_AXIS],
                                      indices.i[Y_AXIS], indices.i[Z_AXIS],
                                      isovalue ) )
            {
                if( status == OK )
                {
                    set_voxel_corners_active( volume,
                                              indices.i[X_AXIS],
                                              indices.i[Y_AXIS],
                                              indices.i[Z_AXIS] );
                }

                add_voxel_neighbours( volume,
                                      indices.i[X_AXIS],
                                      indices.i[Y_AXIS],
                                      indices.i[Z_AXIS],
                                      FALSE, 0.0,
                                      &voxels_searched, &voxels_to_check );
            }
        }
    }

    if( status == OK )
    {
        status = delete_voxel_flags( &voxels_searched );
    }

    if( status == OK )
    {
        status = delete_voxel_queue( &voxels_to_check );
    }
}

private  void  set_voxel_corners_active( volume, x, y, z )
    volume_struct  *volume;
    int            x, y, z;
{
    void   set_voxel_activity();

    set_voxel_activity( volume, x  , y  , z  , TRUE );
    set_voxel_activity( volume, x  , y  , z+1, TRUE );
    set_voxel_activity( volume, x  , y+1, z  , TRUE );
    set_voxel_activity( volume, x  , y+1, z+1, TRUE );
    set_voxel_activity( volume, x+1, y  , z  , TRUE );
    set_voxel_activity( volume, x+1, y  , z+1, TRUE );
    set_voxel_activity( volume, x+1, y+1, z  , TRUE );
    set_voxel_activity( volume, x+1, y+1, z+1, TRUE );
}

public  void  set_slice_activities( volume, axis, index, value )
    volume_struct   *volume;
    int             axis;
    int             index;
    Boolean         value;
{
    int   x_index, y_index, x_size, y_size;
    int   indices[N_DIMENSIONS];

    x_index = (axis + 1) % N_DIMENSIONS;
    y_index = (axis + 2) % N_DIMENSIONS;
    x_size = volume->size[x_index];
    y_size = volume->size[y_index];

    indices[axis] = index;

    for_less( indices[x_index], 0, x_size )
    {
        for_less( indices[y_index], 0, y_size )
        {
            set_voxel_activity( volume, indices[X_AXIS], indices[Y_AXIS],
                                indices[Z_AXIS], value );
        }
    }
}

public  void  generate_slice_activity( volume, isovalue, axis_index,
                                       slice_index )
    volume_struct    *volume;
    Real             isovalue;
    int              axis_index;
    int              slice_index;
{
    int      x_index, y_index, x_size, y_size, indices[N_DIMENSIONS];
    int      corner_indices[N_DIMENSIONS];
    void     set_voxel_activity();

    x_index = (axis_index + 1) % N_DIMENSIONS;
    y_index = (axis_index + 2) % N_DIMENSIONS;
    x_size = volume->size[x_index];
    y_size = volume->size[y_index];

    indices[axis_index] = slice_index;
    corner_indices[axis_index] = slice_index;

    for_less( indices[x_index], 0, x_size-1 )
    {
        for_less( indices[y_index], 0, y_size-1 )
        {
            if( !are_any_square_corners_inactive( volume, indices[X_AXIS],
                           indices[Y_AXIS], indices[Z_AXIS], axis_index ) &&
                square_contains_value( volume, indices, axis_index, isovalue ) )
            {
                for_less( corner_indices[x_index], indices[x_index],
                          indices[x_index]+2 )
                {
                    for_less( corner_indices[y_index], indices[y_index],
                              indices[y_index]+2 )
                    {
                        set_voxel_activity( volume,
                                            corner_indices[X_AXIS],
                                            corner_indices[Y_AXIS],
                                            corner_indices[Z_AXIS], TRUE );
                    }
                }
            }
        }
    }
}

private  Boolean  square_contains_value( volume, indices, axis_index, isovalue )
    volume_struct    *volume;
    int              indices[N_DIMENSIONS];
    int              axis_index;
    Real             isovalue;
{
    int      x_index, y_index, square_indices[N_DIMENSIONS];
    Real     val;
    Boolean  less, more;

    x_index = (axis_index + 1) % N_DIMENSIONS;
    y_index = (axis_index + 2) % N_DIMENSIONS;

    less = FALSE;
    more = FALSE;

    square_indices[axis_index] = indices[axis_index];

    for_less( square_indices[x_index], indices[x_index], indices[x_index]+2 )
    {
        for_less( square_indices[y_index], indices[y_index],
                  indices[y_index]+2 )
        {
            val = (Real) ACCESS_VOLUME_DATA( *volume,
                                             square_indices[X_AXIS],
                                             square_indices[Y_AXIS],
                                             square_indices[Z_AXIS] );
            if( val < isovalue )
                less = TRUE;
            else if( val > isovalue )
                more = TRUE;
        }
    }

    return( less && more );
}

public  void  set_connected_slice_inactivity( graphics, x, y, z, axis_index,
                                              activity )
    graphics_struct  *graphics;
    int              x, y, z;
    int              axis_index;
    Boolean          activity;
{
    Status                                status;
    Boolean                               square_contains_value();
    QUEUE_STRUCT( voxel_index_struct )    voxels_to_check;
    voxel_index_struct                    indices;
    int                                   n_voxels;
    bitlist_struct                        voxels_searched;
    volume_struct                         *volume;
    Status                                set_voxel_flag();
    Status                                delete_voxel_flags();
    Status                                insert_in_voxel_queue();
    Status                                delete_voxel_queue();
    Status                                initialize_voxel_flags();
    void                                  initialize_voxels_queue();
    void                                  get_next_voxel_from_queue();
    void                                  add_square_neighbours();
    void                                  set_voxel_inactivity();
    void                                  initialize_voxel_queue();

    volume = graphics->associated[SLICE_WINDOW]->slice.volume;

    indices.i[X_AXIS] = MIN( x, volume->size[X_AXIS]-2 );
    indices.i[Y_AXIS] = MIN( y, volume->size[Y_AXIS]-2 );
    indices.i[Z_AXIS] = MIN( z, volume->size[Z_AXIS]-2 );

    n_voxels = volume->size[X_AXIS] *
               volume->size[Y_AXIS] *
               volume->size[Z_AXIS];

    status = initialize_voxel_flags( &voxels_searched, n_voxels );

    initialize_voxel_queue( &voxels_to_check );

    if( status == OK )
    {
        status = insert_in_voxel_queue( &voxels_to_check, &indices );
    }

    if( status == OK )
    {
        status = set_voxel_flag( volume, &voxels_searched, &indices );
    }

    while( status == OK && voxels_remaining(&voxels_to_check) )
    {
        get_next_voxel_from_queue( &voxels_to_check, &indices );

        if( activity )
        {
            if( get_voxel_inactivity_flag( volume, indices.i[X_AXIS],
                                    indices.i[Y_AXIS], indices.i[Z_AXIS] ) &&
                get_voxel_activity_flag( volume, indices.i[X_AXIS],
                                    indices.i[Y_AXIS], indices.i[Z_AXIS] ) )
            {
                set_voxel_inactivity( volume,
                                      indices.i[X_AXIS],
                                      indices.i[Y_AXIS],
                                      indices.i[Z_AXIS],
                                      FALSE );
                add_square_neighbours( volume,
                                       indices.i[X_AXIS],
                                       indices.i[Y_AXIS],
                                       indices.i[Z_AXIS], axis_index,
                                       &voxels_searched, &voxels_to_check );
            }
        }
        else
        {
            if( !get_voxel_inactivity_flag( volume, indices.i[X_AXIS],
                                    indices.i[Y_AXIS], indices.i[Z_AXIS] ) &&
                get_voxel_activity_flag( volume, indices.i[X_AXIS],
                                    indices.i[Y_AXIS], indices.i[Z_AXIS] ) )
            {
                set_voxel_inactivity( volume,
                                      indices.i[X_AXIS],
                                      indices.i[Y_AXIS],
                                      indices.i[Z_AXIS],
                                      TRUE );
                add_square_neighbours( volume,
                                       indices.i[X_AXIS],
                                       indices.i[Y_AXIS],
                                       indices.i[Z_AXIS], axis_index,
                                       &voxels_searched, &voxels_to_check );
            }
        }
    }

    if( status == OK )
    {
        status = delete_voxel_flags( &voxels_searched );
    }

    if( status == OK )
    {
        status = delete_voxel_queue( &voxels_to_check );
    }
}

#include <def_stdio.h>

private  void  possibly_output( p )
    polygons_struct  *p;
{
    static   int  count  = 0;
    Status   status;
    FILE     *file;
    String   name;
    Status   open_output_file();
    Status   io_polygons();
    Status   close_file();

    if( (count-1) * Output_every > p->n_items )
    {
        count = p->n_items / Output_every;
    }

    if( p->n_items > count * Output_every )
    {
        ++count;
        (void) sprintf( name, Tmp_surface_name, count );

        status = open_output_file( name, &file );

        if( status == OK )
        {
            status = io_polygons( file, OUTPUTTING, BINARY_FORMAT, p );
        }

        if( status == OK )
        {
            status = close_file( file );
        }
    }
}

private  Status  delete_edge_points_no_longer_needed( volume, voxel_index,
                                                      voxels_done, edge_points )
    volume_struct       *volume;
    voxel_index_struct  *voxel_index;
    bitlist_struct      *voxels_done;
    hash_table_struct   *edge_points;
{
    Status              status;
    int                 axis_index, a1, a2;
    int                 x, y, dx, dy, dz;
    Boolean             all_four_done;
    Boolean             voxel_done[3][3][3];
    voxel_index_struct  indices;
    Status              remove_edge_point();

    status = OK;

    for_inclusive( dx, -1, 1 )
    {
        indices.i[X_AXIS] = voxel_index->i[X_AXIS] + dx;
        for_inclusive( dy, -1, 1 )
        {
            indices.i[Y_AXIS] = voxel_index->i[Y_AXIS] + dy;
            for_inclusive( dz, -1, 1 )
            {
                indices.i[Z_AXIS] = voxel_index->i[Z_AXIS] + dz;

                if( !cube_is_within_volume( volume, indices.i[X_AXIS],
                             indices.i[Y_AXIS], indices.i[Z_AXIS] ) ||
                    get_voxel_flag( volume, voxels_done, &indices ) )
                {
                    voxel_done[dx+1][dy+1][dz+1] = TRUE;
                }
                else
                {
                    voxel_done[dx+1][dy+1][dz+1] = FALSE;
                }
            }
        }
    }

    for_less( axis_index, 0, N_DIMENSIONS )
    {
        a1 = (axis_index + 1) % N_DIMENSIONS;
        a2 = (axis_index + 2) % N_DIMENSIONS;

        for_less( x, 0, 2 )
        {
            for_less( y, 0, 2 )
            {
                all_four_done = TRUE;

                for_less( dx, 0, 2 )
                {
                    for_less( dy, 0, 2 )
                    {
                        indices.i[axis_index] = 1;
                        indices.i[a1] = x+dx;
                        indices.i[a2] = y+dy;

                        if( !voxel_done[indices.i[X_AXIS]]
                                       [indices.i[Y_AXIS]]
                                       [indices.i[Z_AXIS]] )
                        {
                            all_four_done = FALSE;
                            break;
                        }
                    }
                }

                if( all_four_done )
                {
                    indices.i[axis_index] = voxel_index->i[axis_index];
                    indices.i[a1] = voxel_index->i[a1] + x;
                    indices.i[a2] = voxel_index->i[a2] + y;

                    status = remove_edge_point( volume, edge_points, &indices,
                                                axis_index );
                }
            }
        }
    }

    return( status );
}
