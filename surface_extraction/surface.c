
#include  <def_display.h>

private  Boolean  find_close_voxel_containing_value(
    Volume                     volume,
    unsigned_byte              voxel_done_flags[],
    Real                       value,
    surface_extraction_struct  *surface_extraction,
    int                        x,
    int                        y,
    int                        z,
    voxel_index_struct         *found_indices );
private  void  add_voxel_neighbours(
    Volume                              volume,
    int                                 x,
    int                                 y,
    int                                 z,
    Boolean                             surface_only,
    Real                                isovalue,
    surface_extraction_struct           *surface_extraction,
    bitlist_struct                      *voxels_queued,
    voxel_queue_struct                  *voxel_queue );
private  Boolean  cube_is_within_distance(
    surface_extraction_struct           *surface_extraction,
    int                                 x,
    int                                 y,
    int                                 z );
private  void  possibly_output(
    polygons_struct  *p );
private  void  delete_edge_points_no_longer_needed(
    Volume              volume,
    voxel_index_struct  *voxel_index,
    unsigned_byte       voxel_done_flags[],
    hash_table_struct   *edge_points );

public  void  start_surface_extraction_at_point(
    display_struct     *display,
    int                x,
    int                y,
    int                z )
{
    int                         indices[N_DIMENSIONS];
    Volume                      volume;
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          voxel_indices;

    surface_extraction = &display->three_d.surface_extraction;

    indices[X] = x;
    indices[Y] = y;
    indices[Z] = z;
    if( get_slice_window_volume( display, &volume ) &&
        cube_is_within_volume( volume, indices ) )
    {
        display->three_d.surface_extraction.x_starting_voxel = x;
        display->three_d.surface_extraction.y_starting_voxel = y;
        display->three_d.surface_extraction.z_starting_voxel = z;

        if( !surface_extraction->isovalue_selected )
        {
            set_isosurface_value( surface_extraction );
        }
        else
        {
            while( voxels_remaining(
                     &display->three_d.surface_extraction.voxels_to_do ) )
            {
                get_next_voxel_from_queue( 
                     &display->three_d.surface_extraction.voxels_to_do,
                     &voxel_indices );

                reset_voxel_flag( volume,
                         &display->three_d.surface_extraction.voxels_queued,
                         &voxel_indices );
            }
        }

        if( find_close_voxel_containing_value( volume,
                  display->three_d.surface_extraction.voxel_done_flags,
                  display->three_d.surface_extraction.isovalue,
                  &display->three_d.surface_extraction,
                  x, y, z, &voxel_indices ) )
        {
            insert_in_voxel_queue(
                         &display->three_d.surface_extraction.voxels_to_do,
                         &voxel_indices );

            set_voxel_flag( volume,
                            &display->three_d.surface_extraction.voxels_queued,
                            &voxel_indices );

            start_surface_extraction( display );
        }
    }
}

private  Boolean  find_close_voxel_containing_value(
    Volume                     volume,
    unsigned_byte              voxel_done_flags[],
    Real                       value,
    surface_extraction_struct  *surface_extraction,
    int                        x,
    int                        y,
    int                        z,
    voxel_index_struct         *found_indices )
{
    Boolean                   found, voxel_contains;
    int                       sizes[N_DIMENSIONS];
    unsigned_byte             voxel_done;
    voxel_queue_struct        voxels_to_check;
    voxel_index_struct        indices, insert;
    bitlist_struct            voxels_searched;

    get_volume_sizes( volume, sizes );

    insert.i[X] = MIN( x, sizes[X]-2 );
    insert.i[Y] = MIN( y, sizes[Y]-2 );
    insert.i[Z] = MIN( z, sizes[Z]-2 );

    found = FALSE;

    initialize_voxel_flags( &voxels_searched, get_n_voxels(volume) );

    initialize_voxel_queue( &voxels_to_check );

    insert_in_voxel_queue( &voxels_to_check, &insert );

    set_voxel_flag( volume, &voxels_searched, &insert );

    while( !found && voxels_remaining(&voxels_to_check) )
    {
        get_next_voxel_from_queue( &voxels_to_check, &indices );

        voxel_contains = voxel_contains_value( volume,
                                               indices.i[X],
                                               indices.i[Y],
                                               indices.i[Z], value );

        voxel_done = get_voxel_done_flag( volume, voxel_done_flags, &indices );

        if( voxel_contains && !voxel_done )
        {
            found_indices->i[X] = indices.i[X];
            found_indices->i[Y] = indices.i[Y];
            found_indices->i[Z] = indices.i[Z];
            found = TRUE;
        }
        else if( voxel_contains || !voxel_done )
        {
            add_voxel_neighbours( volume,
                                  indices.i[X],
                                  indices.i[Y],
                                  indices.i[Z],
                                  (Boolean) voxel_done, value,
                                  surface_extraction,
                                  &voxels_searched, &voxels_to_check );
        }
    }

    delete_voxel_flags( &voxels_searched );

    delete_voxel_queue( &voxels_to_check );

    return( found );
}

public  void  extract_more_surface(
    display_struct    *display )
{
    int                         n_voxels_done;
    voxel_index_struct          voxel_index;
    surface_extraction_struct   *surface_extraction;
    Volume                      volume;
    Real                        stop_time;

    n_voxels_done = 0;

    surface_extraction = &display->three_d.surface_extraction;
    volume = get_volume( display );

    stop_time = current_realtime_seconds() + Max_seconds_per_voxel_update;

    while( (n_voxels_done < Min_voxels_per_update ||
           (n_voxels_done < Max_voxels_per_update &&
            current_realtime_seconds() < stop_time) ) &&
           voxels_remaining( &surface_extraction->voxels_to_do ) )
    {
        possibly_output( surface_extraction->polygons );

        get_next_voxel_from_queue( &surface_extraction->voxels_to_do,
                                   &voxel_index );

        reset_voxel_flag( volume, &surface_extraction->voxels_queued,
                          &voxel_index);

        if( extract_voxel_surface( volume, surface_extraction, &voxel_index,
                            surface_extraction->n_voxels_with_surface == 0) )
        {
            ++n_voxels_done;
            ++surface_extraction->n_voxels_with_surface;

            delete_edge_points_no_longer_needed( volume, &voxel_index,
                                 surface_extraction->voxel_done_flags,
                                 &surface_extraction->edge_points );

            if( Display_surface_in_slices )
            {
                label_voxel_as_done( volume,
                                     voxel_index.i[X],
                                     voxel_index.i[Y],
                                     voxel_index.i[Z] );
            }

            add_voxel_neighbours( volume,
                        voxel_index.i[X], voxel_index.i[Y], voxel_index.i[Z],
                        TRUE, surface_extraction->isovalue,
                        surface_extraction,
                        &surface_extraction->voxels_queued,
                        &surface_extraction->voxels_to_do );
        }
    }

    if( !voxels_remaining( &surface_extraction->voxels_to_do ) )
    {
        print( "Surface extraction finished\n" );
        stop_surface_extraction( display );
    }
}

private  void  add_voxel_neighbours(
    Volume                          volume,
    int                             x,
    int                             y,
    int                             z,
    Boolean                         surface_only,
    Real                            isovalue,
    surface_extraction_struct       *surface_extraction,
    bitlist_struct                      *voxels_queued,
    voxel_queue_struct                  *voxel_queue )
{
    int                      x_offset, y_offset, z_offset;
    int                      indices[N_DIMENSIONS];
    voxel_index_struct       neighbour;

    for_inclusive( x_offset, -1, 1 )
    {
        neighbour.i[X] = x + x_offset;

        for_inclusive( y_offset, -1, 1 )
        {
            neighbour.i[Y] = y + y_offset;
            for_inclusive( z_offset, -1, 1 )
            {
                neighbour.i[Z] = z + z_offset;
                indices[X] = neighbour.i[X];
                indices[Y] = neighbour.i[Y];
                indices[Z] = neighbour.i[Z];
                if( (x_offset != 0 || y_offset != 0 || z_offset != 0) &&
                    cube_is_within_volume( volume, indices ) &&
                    cube_is_within_distance( surface_extraction,
                                             neighbour.i[X],
                                             neighbour.i[Y],
                                             neighbour.i[Z] ) &&
                    !get_voxel_flag( volume, voxels_queued, &neighbour ) )
                {
                    set_voxel_flag( volume, voxels_queued, &neighbour);
                    if( !surface_only ||
                        voxel_contains_value( volume,
                                              neighbour.i[X],
                                              neighbour.i[Y],
                                              neighbour.i[Z],
                                              isovalue ) )
                    {
                        insert_in_voxel_queue( voxel_queue, &neighbour );
                    }
                }
            }
        }
    }
}

private  Boolean  cube_is_within_distance(
    surface_extraction_struct           *surface_extraction,
    int                                 x,
    int                                 y,
    int                                 z )
{
    Boolean  within_dist;
    int      dx, dy, dz;

    dx = ABS( x - surface_extraction->x_starting_voxel );
    dy = ABS( y - surface_extraction->y_starting_voxel );
    dz = ABS( z - surface_extraction->z_starting_voxel );

    within_dist = dx <= surface_extraction->x_voxel_max_distance &&
                  dy <= surface_extraction->y_voxel_max_distance &&
                  dz <= surface_extraction->z_voxel_max_distance;

    return( within_dist );
}

private  void  possibly_output(
    polygons_struct  *p )
{
    static   int  count  = 0;
    Status   status;
    FILE     *file;
    String   name;

    if( Output_every > 0 )
    {
        if( (count-1) * Output_every > p->n_items )
        {
            count = p->n_items / Output_every;
        }

        if( p->n_items > count * Output_every )
        {
            ++count;
            (void) sprintf( name, Tmp_surface_name, count );

            status = open_file_with_default_suffix( name, "obj",
                                            WRITE_FILE, BINARY_FORMAT, &file );

            if( status == OK )
            {
                status = io_polygons( file, WRITE_FILE, BINARY_FORMAT, p );
            }

            if( status == OK )
            {
                status = close_file( file );
            }
        }
    }
}

private  void  delete_edge_points_no_longer_needed(
    Volume              volume,
    voxel_index_struct  *voxel_index,
    unsigned_byte       voxel_done_flags[],
    hash_table_struct   *edge_points )
{
    int                 axis_index, a1, a2;
    int                 x, y, dx, dy, dz;
    Boolean             all_four_done;
    Boolean             voxel_done[3][3][3];
    voxel_index_struct  indices;
    int                 int_indices[N_DIMENSIONS];

    for_inclusive( dx, -1, 1 )
    {
        indices.i[X] = voxel_index->i[X] + dx;
        for_inclusive( dy, -1, 1 )
        {
            indices.i[Y] = voxel_index->i[Y] + dy;
            for_inclusive( dz, -1, 1 )
            {
                indices.i[Z] = voxel_index->i[Z] + dz;
                int_indices[X] = indices.i[X];
                int_indices[Y] = indices.i[Y];
                int_indices[Z] = indices.i[Z];

                if( !cube_is_within_volume( volume, int_indices ) ||
                    get_voxel_done_flag( volume, voxel_done_flags, &indices )
                    == VOXEL_COMPLETELY_DONE )
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

                        if( !voxel_done[indices.i[X]]
                                       [indices.i[Y]]
                                       [indices.i[Z]] )
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

                    remove_edge_point( volume, edge_points, &indices,
                                       axis_index );
                }
            }
        }
    }
}
