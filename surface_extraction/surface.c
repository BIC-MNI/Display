
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
    void                        clear_voxels_done();
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

        clear_voxels_done( &surface_extraction->voxels_done );
    }

    return( status );
}

private  Status  free_surface_extraction( graphics )
    graphics_struct   *graphics;
{
    Status                      status;
    Status                      delete_edge_points();
    Status                      delete_voxels_done();
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
    Status                      delete_edge_points();
    Status                      delete_voxels_done();
    Status                      delete_voxel_queue();
    surface_extraction_struct   *surface_extraction;

    surface_extraction = &graphics->three_d.surface_extraction;

    status = free_surface_extraction( graphics );

    if( status == OK )
    {
        status = delete_voxels_done( &surface_extraction->voxels_done );
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
    Status                      mark_voxel_done();
    void                        add_voxel_neighbours();
    void                        start_surface_extraction();
    Status                      delete_voxels_done();
    Status                      initialize_voxels_done();

    surface_extraction = &graphics->three_d.surface_extraction;

    status = OK;

    if( cube_is_within_volume(
             graphics->associated[SLICE_WINDOW]->slice.volume, x, y, z ) &&
        !surface_extraction->extraction_in_progress )
    {
        if( surface_extraction->n_voxels_alloced <
            get_n_voxels(graphics->associated[SLICE_WINDOW]->slice.volume) )
        {
            status = delete_voxels_done( 
                       &graphics->three_d.surface_extraction.voxels_done );

            if( status == OK )
            {
                status = initialize_voxels_done( 
                   &surface_extraction->voxels_done,
                        get_n_voxels(graphics->associated[SLICE_WINDOW]->
                       slice.volume) );
            }
        }

        if( !surface_extraction->isovalue_selected )
        {
            set_isosurface_value( surface_extraction );
        }

        if( find_close_voxel_containing_value(
                  graphics->associated[SLICE_WINDOW]->slice.volume,
                  &graphics->associated[SLICE_WINDOW]->slice.voxel_activity,
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

private  Boolean  find_close_voxel_containing_value( volume, voxel_activity,
                        value,
                        x, y, z, found_indices )
    volume_struct          *volume;
    bitlist_struct         *voxel_activity;
    Real                   value;
    int                    x, y, z;
    voxel_index_struct     *found_indices;
{
    Status                                status;
    Boolean                               found;
    Boolean                               voxel_contains_value();
    Boolean                               are_voxel_corners_active();
    Boolean                               active;
    QUEUE_STRUCT( voxel_index_struct )    voxels_to_check;
    voxel_index_struct                    indices, insert;
    bitlist_struct                        voxels_done;
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

    status = initialize_voxels_done( &voxels_done, get_n_voxels( volume ) );

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

        active = are_voxel_corners_active( volume, voxel_activity,
                                           indices.i[X_AXIS],
                                           indices.i[Y_AXIS],
                                           indices.i[Z_AXIS] );

        if( active )
        {
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

    n_voxels_done = 0;

    surface_extraction = &graphics->three_d.surface_extraction;

    stop_time = current_realtime_seconds() + Max_seconds_per_voxel_update;

    while( (n_voxels_done < Min_voxels_per_update ||
           (n_voxels_done < Max_voxels_per_update &&
            current_realtime_seconds() < stop_time) ) &&
           voxels_remaining( &surface_extraction->voxels_to_do ) )
    {
        get_next_voxel_from_queue( &surface_extraction->voxels_to_do,
                                   &voxel_index );

        if( check_voxel( graphics->associated[SLICE_WINDOW]->slice.volume,
                    &graphics->associated[SLICE_WINDOW]->slice.voxel_activity,
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

private  Boolean   check_voxel( volume, voxel_activity, surface_extraction,
                                voxel_index )
    volume_struct               *volume;
    bitlist_struct              *voxel_activity;
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
{
    Status                 status;
    Boolean                lookup_edge_point_id();
    Status                 record_edge_point_id();
    polygons_struct        *poly;
    triangle_point_type    *points_list;
    voxel_index_struct     corner_points[3];
    Real                   corner_values[2][2][2];
    Boolean                active, connected;
    Boolean                tri_done[4], changed;
    int                    n_done;
    Boolean                are_voxel_corners_active();
    int                    n_tris, n_nondegenerate_tris, tri, p, next_end;
    int                    x, y, z, pt_index;
    int                    point_id[3];
    Point_classes          pt_class;

    active = are_voxel_corners_active( volume, voxel_activity,
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

    n_nondegenerate_tris = 0;

    if( active && n_tris > 0 )
    {
        poly = surface_extraction->triangles;

        status = OK;

        for_less( tri, 0, n_tris )
        {
            tri_done[tri] = FALSE;
        }

        changed = TRUE;
        n_done = 0;

        while( n_done < n_tris && changed )
        {
            changed = FALSE;

            for_less( tri, 0, n_tris )
            {
            if( !tri_done[tri] )
            {
            connected = (poly->n_items == 0);

            for_less( p, 0, 3 )
            {
                pt_index = 3 * tri + p;

                corner_points[p].i[X_AXIS] = voxel_index->i[X_AXIS] +
                                   points_list[pt_index].coord[X_AXIS];
                corner_points[p].i[Y_AXIS] = voxel_index->i[Y_AXIS] +
                                   points_list[pt_index].coord[Y_AXIS];
                corner_points[p].i[Z_AXIS] = voxel_index->i[Z_AXIS] +
                                   points_list[pt_index].coord[Z_AXIS];

                if( lookup_edge_point_id( volume,
                                       &surface_extraction->edge_points,
                                       &corner_points[p],
                                       points_list[pt_index].edge_intersected,
                                       &point_id[p] ) )
                {
                    connected = TRUE;
                }
                else
                {
                    point_id[p] = -1;
                }
            }

            if( connected )
            {
                for_less( p, 0, 3 )
                {
                    pt_index = 3 * tri + p;

                    if( point_id[p] < 0 )
                    {
                        point_id[p] = create_point( volume,
                                 surface_extraction->isovalue,
                                 surface_extraction->triangles,
                                 &corner_points[p],
                                 points_list[pt_index].edge_intersected,
                                 &pt_class );

                        status = record_edge_point_id( volume,
                                     &surface_extraction->edge_points,
                                     &corner_points[p],
                                     points_list[pt_index].edge_intersected,
                                     point_id[p] );

                        if( status != OK )
                        {
                            point_id[p] = -123456789;
                        }
                    }
                }

                ++n_done;
                tri_done[tri] = TRUE;
                changed = TRUE;
            }

            if( connected && point_id[0] != point_id[1] &&
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
        }
    }

    return( n_nondegenerate_tris > 0 );
}

private  void  add_voxel_neighbours( volume, x, y, z, voxels_done, voxel_queue )
    volume_struct                       *volume;
    int                                 x, y, z;
    bitlist_struct                      *voxels_done;
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

        if( (alpha == 0.0 && val != val1) ||
            (alpha == 1.0 && val != val2) )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement val\n" );
        }

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

    fill_Point( point, (Real) voxel->i[X_AXIS], (Real) voxel->i[Y_AXIS],
                       (Real) voxel->i[Z_AXIS] )

    Point_coord( point, edge_intersected ) += alpha;

    Point_x(point) *= volume->slice_thickness[X_AXIS];
    Point_y(point) *= volume->slice_thickness[Y_AXIS];
    Point_z(point) *= volume->slice_thickness[Z_AXIS];

    u_bar[X_AXIS] = FRACTION( Point_x(point) );
    u_bar[Y_AXIS] = FRACTION( Point_y(point) );
    u_bar[Z_AXIS] = FRACTION( Point_z(point) );
  
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

public  Boolean  are_voxel_corners_active( volume, voxel_activity, x, y, z )
    volume_struct   *volume;
    bitlist_struct  *voxel_activity;
    int             x, y, z;
{
    return( get_voxel_activity( volume, voxel_activity, x  , y  , z   ) &&
            get_voxel_activity( volume, voxel_activity, x  , y  , z+1 ) &&
            get_voxel_activity( volume, voxel_activity, x  , y+1, z   ) &&
            get_voxel_activity( volume, voxel_activity, x  , y+1, z+1 ) &&
            get_voxel_activity( volume, voxel_activity, x+1, y  , z   ) &&
            get_voxel_activity( volume, voxel_activity, x+1, y  , z+1 ) &&
            get_voxel_activity( volume, voxel_activity, x+1, y+1, z   ) &&
            get_voxel_activity( volume, voxel_activity, x+1, y+1, z+1 ) );
}
