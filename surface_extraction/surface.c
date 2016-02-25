/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

static  VIO_BOOL  find_close_voxel_containing_range(
    VIO_Volume                     volume,
    VIO_Volume                     label_volume,
    VIO_UCHAR              voxel_done_flags[],
    surface_extraction_struct  *surface_extraction,
    int                        x,
    int                        y,
    int                        z,
    int                        found_indices[] );
static  void  add_voxel_neighbours(
    VIO_Volume                              volume,
    VIO_Volume                              label_volume,
    int                                 x,
    int                                 y,
    int                                 z,
    VIO_BOOL                             surface_only,
    surface_extraction_struct           *surface_extraction,
    bitlist_3d_struct                   *voxels_queued,
    voxel_queue_struct                  *voxel_queue );
static  void  delete_edge_points_no_longer_needed(
    surface_extraction_struct       *surface_extraction,
    VIO_Volume                          volume,
    int                             voxel_index[],
    VIO_UCHAR                   voxel_done_flags[],
    hash_table_struct               *edge_points );

static  VIO_BOOL  surface_voxel_is_within_volume(
    surface_extraction_struct   *surface_extraction,
    int                         indices[] )
{

    return( indices[VIO_X] >= surface_extraction->min_limits[VIO_X] &&
            indices[VIO_X] <= surface_extraction->max_limits[VIO_X] &&
            indices[VIO_Y] >= surface_extraction->min_limits[VIO_Y] &&
            indices[VIO_Y] <= surface_extraction->max_limits[VIO_Y] &&
            indices[VIO_Z] >= surface_extraction->min_limits[VIO_Z] &&
            indices[VIO_Z] <= surface_extraction->max_limits[VIO_Z] );
}

  void  start_surface_extraction_at_point(
    display_struct     *display,
    VIO_Volume             volume,
    VIO_Volume             label_volume,
    VIO_BOOL            binary_flag,
    VIO_BOOL            voxellate_flag,
    VIO_Real               min_value,
    VIO_Real               max_value,
    int                x,
    int                y,
    int                z )
{
    int                         dim, sizes[VIO_MAX_DIMENSIONS];
    int                         indices[VIO_N_DIMENSIONS];
    surface_extraction_struct   *surf;
    int                         voxel_indices[VIO_N_DIMENSIONS];
    int                         offset;
    int                         min_crop[VIO_N_DIMENSIONS], max_crop[VIO_N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    surf = &display->three_d.surface_extraction;

    surf->volume = volume;
    surf->label_volume = label_volume;
    surf->binary_flag = binary_flag;
    surf->voxellate_flag = voxellate_flag;
    surf->min_value = min_value;
    surf->max_value = max_value;

    indices[VIO_X] = x;
    indices[VIO_Y] = y;
    indices[VIO_Z] = z;

    if( voxellate_flag )
        offset = 0;
    else
        offset = 1;

    get_volume_crop_limits( display, min_crop, max_crop );

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        surf->min_limits[dim] = min_crop[dim];
        surf->max_limits[dim] = MIN( sizes[dim]-1-offset, max_crop[dim] );

        if( !volume_is_alloced( volume ) && !volume_is_cached( volume ) )
        {
            surf->min_changed_limits[dim] = 0;
            surf->max_changed_limits[dim] = -1;
        }
        else
        {
            surf->min_changed_limits[dim] = surf->min_limits[dim];
            surf->max_changed_limits[dim] = surf->max_limits[dim];
        }
        surf->min_modified[dim] = 0;
        surf->max_modified[dim] = -1;
        surf->not_changed_since[dim] = surf->min_limits[dim];

        surf->min_block[dim] = surf->min_limits[dim];
        surf->max_block[dim] = surf->min_limits[dim] -
                               (surf->min_block[dim] % SURFACE_BLOCK_SIZE) +
                               SURFACE_BLOCK_SIZE - 1;
        if( surf->max_block[dim] > surf->max_limits[dim] )
            surf->max_block[dim] = surf->max_limits[dim];

        if( voxellate_flag )
            surf->current_voxel[dim] = surf->min_limits[dim];
        else
            surf->current_voxel[dim] = indices[dim];
    }

    if( !surface_voxel_is_within_volume( surf, indices ) )
    {
        /*--- turn off surface extraction */

        surf->volume = NULL;
        surf->label_volume = NULL;
        print( "Starting voxel is not within crop limits.\n" );
        return;
    }

    if( voxellate_flag )
    {
        initialize_edge_points( &surf->faces_done );
        INITIALIZE_QUEUE( surf->deleted_faces );
    }
    else
    {
        initialize_voxel_queue( &surf->voxels_to_do );
        initialize_voxel_done_flags( &surf->voxel_done_flags,
                                     surf->min_limits, surf->max_limits );
    }

    initialize_voxel_flags( &surf->voxel_state,
                            surf->min_limits, surf->max_limits );
    initialize_edge_points( &surf->edge_points );

    surf->n_voxels_with_surface = 0;

    if( !voxellate_flag )
    {
        if( find_close_voxel_containing_range( volume, label_volume,
                  surf->voxel_done_flags, surf, x, y, z, voxel_indices ) )
        {
            insert_in_voxel_queue( &surf->voxels_to_do, voxel_indices );

            set_voxel_flag( &surf->voxel_state, surf->min_limits,
                            voxel_indices );

            start_surface_extraction( display );
        }
        else
            print( "No surface voxels found.\n" );
    }
    else
        start_surface_extraction( display );
}

static  VIO_BOOL  find_close_voxel_containing_range(
    VIO_Volume                     volume,
    VIO_Volume                     label_volume,
    VIO_UCHAR              voxel_done_flags[],
    surface_extraction_struct  *surface_extraction,
    int                        x,
    int                        y,
    int                        z,
    int                        found_indices[] )
{
    VIO_BOOL                   found, voxel_contains;
    int                       sizes[VIO_MAX_DIMENSIONS], voxel[VIO_N_DIMENSIONS];
    VIO_BOOL                   voxel_done;
    voxel_queue_struct        voxels_to_check;
    int                       insert[VIO_N_DIMENSIONS];
    bitlist_3d_struct         voxels_searched;

    get_volume_sizes( volume, sizes );

    insert[VIO_X] = MIN( x, sizes[VIO_X]-2 );
    insert[VIO_Y] = MIN( y, sizes[VIO_Y]-2 );
    insert[VIO_Z] = MIN( z, sizes[VIO_Z]-2 );

    found = FALSE;

    initialize_voxel_flags( &voxels_searched, surface_extraction->min_limits,
                                              surface_extraction->max_limits );

    initialize_voxel_queue( &voxels_to_check );

    insert_in_voxel_queue( &voxels_to_check, insert );

    set_voxel_flag( &voxels_searched, surface_extraction->min_limits, insert );

    while( !found && voxels_remaining(&voxels_to_check) )
    {
        get_next_voxel_from_queue( &voxels_to_check, voxel );

        voxel_contains = voxel_contains_surface( volume, label_volume,
                                                 surface_extraction, voxel );

        voxel_done = (int) get_voxel_done_flag( surface_extraction->min_limits,
                                                surface_extraction->max_limits,
                                                voxel_done_flags, voxel );

        if( voxel_contains && voxel_done == 0 )
        {
            found_indices[VIO_X] = voxel[VIO_X];
            found_indices[VIO_Y] = voxel[VIO_Y];
            found_indices[VIO_Z] = voxel[VIO_Z];
            found = TRUE;
        }
        else if( voxel_contains || voxel_done == 0 )
        {
            add_voxel_neighbours( volume, label_volume,
                                  voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z],
                                  voxel_done, surface_extraction,
                                  &voxels_searched, &voxels_to_check );
        }
    }

    delete_voxel_flags( &voxels_searched );

    delete_voxel_queue( &voxels_to_check );

    return( found );
}

  VIO_BOOL  some_voxels_remaining_to_do(
    surface_extraction_struct   *surface_extraction )
{
    VIO_BOOL   remaining_to_do;

    if( surface_extraction->voxellate_flag )
    {
        remaining_to_do = surface_extraction->min_changed_limits[VIO_X] <=
                          surface_extraction->max_changed_limits[VIO_X] ||
                          surface_extraction->min_modified[VIO_X] <=
                          surface_extraction->max_modified[VIO_X];
    }
    else
        remaining_to_do = voxels_remaining( &surface_extraction->voxels_to_do );

    return( remaining_to_do );
}

static  void  update_changed_limits(
    surface_extraction_struct  *surf )
{
    int   dim, min_range[VIO_N_DIMENSIONS], max_range[VIO_N_DIMENSIONS];
    int   current_min, current_max;

    if( surf->min_modified[VIO_X] > surf->max_modified[VIO_X] )
        return;

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        min_range[dim] = MAX( surf->min_limits[dim], surf->min_modified[dim]-1);
        max_range[dim] = MIN( surf->max_limits[dim], surf->max_modified[dim]+1);
    }

    if( surf->min_changed_limits[VIO_X] > surf->max_changed_limits[VIO_X] )
    {
        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            surf->min_changed_limits[dim] = min_range[dim];
            surf->max_changed_limits[dim] = max_range[dim];
            surf->not_changed_since[dim] = min_range[dim];
            surf->current_voxel[dim] = min_range[dim];
            surf->min_block[dim] = min_range[dim];
            surf->max_block[dim] = min_range[dim] -
                                   (min_range[dim] % SURFACE_BLOCK_SIZE) +
                                   SURFACE_BLOCK_SIZE - 1;
            if( surf->max_block[dim] > max_range[dim] )
                surf->max_block[dim] = max_range[dim];
        }
    }
    else
    {
        current_min = surf->not_changed_since[VIO_X] -
                      (surf->not_changed_since[VIO_X] % SURFACE_BLOCK_SIZE);
        current_max = surf->current_voxel[VIO_X] -
                      (surf->current_voxel[VIO_X] % SURFACE_BLOCK_SIZE) +
                      SURFACE_BLOCK_SIZE - 1;

        if( min_range[VIO_X] <= current_max && max_range[VIO_X] >= current_min )
        {
            for_less( dim, 0, VIO_N_DIMENSIONS )
                surf->not_changed_since[dim] = surf->current_voxel[dim];
        }

        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            if( min_range[dim] < surf->min_changed_limits[dim] )
                surf->min_changed_limits[dim] = min_range[dim];
            if( max_range[dim] > surf->max_changed_limits[dim] )
                surf->max_changed_limits[dim] = max_range[dim];
        }
    }

    surf->min_modified[VIO_X] = 0;
    surf->max_modified[VIO_X] = -1;
}

static  void  advance_voxellated_index(
    surface_extraction_struct   *surf )
{
    int     dim, last_changed_dim, not_changed_since;

    dim = VIO_N_DIMENSIONS-1;

    while( dim >= 0 )
    {
        ++surf->current_voxel[dim];
        if( surf->current_voxel[dim] <= surf->max_block[dim] )
            break;

        surf->current_voxel[dim] = surf->min_block[dim];

        --dim;
    }

    if( dim < 0 )
    {
        dim = VIO_N_DIMENSIONS-1;

        while( dim >= 0 )
        {
            surf->min_block[dim] = surf->min_block[dim] -
                                   (surf->min_block[dim] % SURFACE_BLOCK_SIZE) +
                                   SURFACE_BLOCK_SIZE;
            surf->max_block[dim] = surf->min_block[dim] + SURFACE_BLOCK_SIZE-1;
            if( surf->max_block[dim] > surf->max_changed_limits[dim] )
                surf->max_block[dim] = surf->max_changed_limits[dim];
            surf->current_voxel[dim] = surf->min_block[dim];

            if( surf->min_block[dim] <= surf->max_changed_limits[dim] )
                break;

            surf->min_block[dim] = surf->min_changed_limits[dim];
            surf->max_block[dim] = surf->min_changed_limits[dim] -
                       (surf->min_changed_limits[dim] % SURFACE_BLOCK_SIZE)
                       + SURFACE_BLOCK_SIZE - 1;
            if( surf->max_block[dim] > surf->max_changed_limits[dim] )
                surf->max_block[dim] = surf->max_changed_limits[dim];
            surf->current_voxel[dim] = surf->min_block[dim];

            --dim;
        }

        last_changed_dim = dim;
    }
    else
        last_changed_dim = VIO_N_DIMENSIONS;

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        if( surf->current_voxel[dim] != surf->not_changed_since[dim] )
            break;
    }

    if( dim == VIO_N_DIMENSIONS )
    {
        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            surf->min_changed_limits[dim] = 0;
            surf->max_changed_limits[dim] = -1;
        }
    }
    else
    {
        not_changed_since = surf->not_changed_since[VIO_X];
        not_changed_since = not_changed_since -
                            (not_changed_since % SURFACE_BLOCK_SIZE) +
                            SURFACE_BLOCK_SIZE - 1;

        if( last_changed_dim < 0 &&
            not_changed_since < surf->max_changed_limits[VIO_X] )
        {
            surf->max_changed_limits[VIO_X] = not_changed_since;

            if( surf->not_changed_since[VIO_Y] != surf->min_changed_limits[VIO_Y] ||
                surf->not_changed_since[VIO_Z] != surf->min_changed_limits[VIO_Z] )
            {
                ++surf->max_changed_limits[VIO_X];
            }

            for_less( dim, 0, VIO_N_DIMENSIONS )
                surf->not_changed_since[dim] = surf->min_changed_limits[dim];
        }
        else if( last_changed_dim == 0 &&
                 surf->not_changed_since[VIO_X] == surf->min_changed_limits[VIO_X] &&
                 surf->not_changed_since[VIO_Y] == surf->min_changed_limits[VIO_Y] &&
                 surf->not_changed_since[VIO_Z] == surf->min_changed_limits[VIO_Z] )
        {
            surf->min_changed_limits[VIO_X] = surf->current_voxel[VIO_X];
            surf->not_changed_since[VIO_X] = surf->current_voxel[VIO_X];
        }
    }
}

  VIO_BOOL  extract_more_surface(
    display_struct    *display )
{
    int                         n_voxels_done, sizes[VIO_MAX_DIMENSIONS];
    int                         voxel_index[VIO_N_DIMENSIONS];
    surface_extraction_struct   *surf;
    VIO_Volume                      volume, label_volume;
    VIO_Real                        stop_time;
    VIO_BOOL                     voxellate_flag, changed;

    changed = FALSE;

    n_voxels_done = 0;

    surf = &display->three_d.surface_extraction;
    volume = surf->volume;
    label_volume = surf->label_volume;
    voxellate_flag = surf->voxellate_flag;

    if( voxellate_flag )
        update_changed_limits( surf );

    get_volume_sizes( volume, sizes );

    stop_time = current_realtime_seconds() + Max_seconds_per_voxel_update;

    while( (n_voxels_done < Min_voxels_per_update ||
           (n_voxels_done < Max_voxels_per_update &&
           current_realtime_seconds() < stop_time) ) &&
           some_voxels_remaining_to_do( surf ) )
    {
        if( voxellate_flag )
        {
            voxel_index[VIO_X] = surf->current_voxel[VIO_X];
            voxel_index[VIO_Y] = surf->current_voxel[VIO_Y];
            voxel_index[VIO_Z] = surf->current_voxel[VIO_Z];

            if( voxel_index[VIO_X] == surf->min_block[VIO_X] &&
                voxel_index[VIO_Y] == surf->min_block[VIO_Y] &&
                voxel_index[VIO_Z] == surf->min_block[VIO_Z] )
            {
                read_voxellation_block( surf );
            }
        }
        else
        {
            get_next_voxel_from_queue( &surf->voxels_to_do, voxel_index );
        }

        if( !voxellate_flag )
            reset_voxel_flag( &surf->voxel_state,
                              surf->min_limits,  voxel_index );

        if( extract_voxel_surface( volume, label_volume,
                                   surf, voxel_index,
                                   surf->n_voxels_with_surface == 0) )
        {
            changed = TRUE;

            ++surf->n_voxels_with_surface;

            if( !voxellate_flag )
            {
                delete_edge_points_no_longer_needed( surf,
                                     volume, voxel_index,
                                     surf->voxel_done_flags,
                                     &surf->edge_points );

                add_voxel_neighbours( volume, label_volume,
                            voxel_index[VIO_X], voxel_index[VIO_Y], voxel_index[VIO_Z],
                            TRUE, surf,
                            &surf->voxel_state,
                            &surf->voxels_to_do );
            }
        }

        if( voxellate_flag )
            advance_voxellated_index( surf );

        ++n_voxels_done;
    }

    if( !voxellate_flag &&
        !some_voxels_remaining_to_do( surf ) )
    {
        print( "Surface extraction finished\n" );
        stop_surface_extraction( display );
        update_all_menu_text( display );
        show_three_d_window( display, display->associated[MARKER_WINDOW] );
    }

    return( changed );
}

  void  tell_surface_extraction_range_of_labels_changed(
    display_struct    *display,
    int               volume_index,
    int               range[2][VIO_N_DIMENSIONS] )
{
    int                         dim;
    VIO_Volume                      label_volume;
    surface_extraction_struct   *surface_extraction;

    display = get_three_d_window( display );
    surface_extraction = &display->three_d.surface_extraction;

    label_volume = get_nth_label_volume( display, volume_index );

    if( surface_extraction->volume == label_volume ||
        surface_extraction->label_volume == label_volume )
    {
        if( surface_extraction->min_modified[VIO_X] >
            surface_extraction->max_modified[VIO_X] )
        {
            for_less( dim, 0, VIO_N_DIMENSIONS )
            {
                surface_extraction->min_modified[dim] = range[0][dim];
                surface_extraction->max_modified[dim] = range[1][dim];
            }
        }
        else
        {
            for_less( dim, 0, VIO_N_DIMENSIONS )
            {
                if( range[0][dim] < surface_extraction->min_modified[dim] )
                    surface_extraction->min_modified[dim] = range[0][dim];
                if( range[1][dim] > surface_extraction->max_modified[dim] )
                    surface_extraction->max_modified[dim] = range[1][dim];
            }
        }
    }
}

  void  tell_surface_extraction_label_changed(
    display_struct    *display,
    int               volume_index,
    int               x,
    int               y,
    int               z )
{
    int   range[2][VIO_N_DIMENSIONS];

    range[0][VIO_X] = x;
    range[1][VIO_X] = x;
    range[0][VIO_Y] = y;
    range[1][VIO_Y] = y;
    range[0][VIO_Z] = z;
    range[1][VIO_Z] = z;

    tell_surface_extraction_range_of_labels_changed( display, volume_index,
                                                     range );
}

static  void  add_voxel_neighbours(
    VIO_Volume                          volume,
    VIO_Volume                          label_volume,
    int                             x,
    int                             y,
    int                             z,
    VIO_BOOL                         surface_only,
    surface_extraction_struct       *surface_extraction,
    bitlist_3d_struct               *voxels_queued,
    voxel_queue_struct              *voxel_queue )
{
    int                      x_offset, y_offset, z_offset;
    int                      neighbour[VIO_N_DIMENSIONS];

    for_inclusive( x_offset, -1, 1 )
    {
        neighbour[VIO_X] = x + x_offset;

        for_inclusive( y_offset, -1, 1 )
        {
            neighbour[VIO_Y] = y + y_offset;
            for_inclusive( z_offset, -1, 1 )
            {
                neighbour[VIO_Z] = z + z_offset;
                if( (x_offset != 0 || y_offset != 0 || z_offset != 0) &&
                    surface_voxel_is_within_volume( surface_extraction,
                                                    neighbour ) &&
                    !get_voxel_flag( voxels_queued,
                                     surface_extraction->min_limits,
                                     neighbour ) &&
                    get_voxel_done_flag( surface_extraction->min_limits,
                                         surface_extraction->max_limits,
                                         surface_extraction->voxel_done_flags,
                                         neighbour ) != VOXEL_COMPLETELY_DONE )
                {
                    set_voxel_flag( voxels_queued,
                                    surface_extraction->min_limits, neighbour );
                    if( !surface_only ||
                        voxel_contains_surface( volume, label_volume,
                                                surface_extraction, neighbour ))
                    {
                        insert_in_voxel_queue( voxel_queue, neighbour );
                    }
                }
            }
        }
    }
}

static  void  delete_edge_points_no_longer_needed(
    surface_extraction_struct       *surface_extraction,
    VIO_Volume                          volume,
    int                             voxel_index[],
    VIO_UCHAR                   voxel_done_flags[],
    hash_table_struct               *edge_points )
{
    int                 axis_index, a1, a2;
    int                 x, y, dx, dy, dz;
    VIO_BOOL             all_four_done;
    VIO_BOOL             voxel_done[3][3][3];
    int                 indices[VIO_N_DIMENSIONS];
    int                 sizes[VIO_MAX_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    for_inclusive( dx, -1, 1 )
    {
        indices[VIO_X] = voxel_index[VIO_X] + dx;
        for_inclusive( dy, -1, 1 )
        {
            indices[VIO_Y] = voxel_index[VIO_Y] + dy;
            for_inclusive( dz, -1, 1 )
            {
                indices[VIO_Z] = voxel_index[VIO_Z] + dz;

                if( !surface_voxel_is_within_volume( surface_extraction,
                                                     indices ) ||
                    get_voxel_done_flag( surface_extraction->min_limits,
                                         surface_extraction->max_limits,
                                         voxel_done_flags, indices )
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

    for_less( axis_index, 0, VIO_N_DIMENSIONS )
    {
        a1 = (axis_index + 1) % VIO_N_DIMENSIONS;
        a2 = (axis_index + 2) % VIO_N_DIMENSIONS;

        for_less( x, 0, 2 )
        {
            for_less( y, 0, 2 )
            {
                all_four_done = TRUE;

                for_less( dx, 0, 2 )
                {
                    for_less( dy, 0, 2 )
                    {
                        indices[axis_index] = 1;
                        indices[a1] = x + dx;
                        indices[a2] = y + dy;

                        if( !voxel_done[indices[VIO_X]]
                                       [indices[VIO_Y]]
                                       [indices[VIO_Z]] )
                        {
                            all_four_done = FALSE;
                            break;
                        }
                    }
                }

                if( all_four_done )
                {
                    indices[axis_index] = voxel_index[axis_index];
                    indices[a1] = voxel_index[a1] + x;
                    indices[a2] = voxel_index[a2] + y;

                    remove_edge_point( sizes, edge_points,
                                       indices[VIO_X], indices[VIO_Y], indices[VIO_Z],
                                       axis_index );
                }
            }
        }
    }
}

  void  remove_empty_polygons(
    polygons_struct  *polygons )
{
    int    new_n_polys, new_indices, size, v, poly, p1, p2, old_indices;
    int    *new_point_index, new_n_points, p;

    new_n_polys = 0;
    new_indices = 0;
    old_indices = 0;

    /*--- remove any polygons that have less than 3 vertices, or all vertices
          are the same */

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );

        for_less( v, 0, size-1 )
        {
            p1 = polygons->indices[old_indices+v];
            p2 = polygons->indices[old_indices+v+1];
            if( p1 != p2 )
                break;
        }

        /*--- if not degenerate polygon, keep it in the set */

        if( size >= 3 && v < size-1 )
        {
            for_less( v, 0, size )
            {
                polygons->indices[new_indices+v] =
                              polygons->indices[old_indices+v];
            }

            new_indices += size;
            polygons->end_indices[new_n_polys] = new_indices;
            ++new_n_polys;
        }

        old_indices += size;
    }

    polygons->n_items = new_n_polys;

    /*--- remove any unused vertices */

    ALLOC( new_point_index, polygons->n_points );
    for_less( p, 0, polygons->n_points )
        new_point_index[p] = FALSE;

    for_less( v, 0, new_indices )
        new_point_index[polygons->indices[v]] = TRUE;

    new_n_points = 0;
    for_less( p, 0, polygons->n_points )
    {
        if( new_point_index[p] )
        {
            new_point_index[p] = new_n_points;
            polygons->points[new_n_points] = polygons->points[p];
            polygons->normals[new_n_points] = polygons->normals[p];
            ++new_n_points;
        }
    }

    for_less( v, 0, new_indices )
        polygons->indices[v] = new_point_index[polygons->indices[v]];

    polygons->n_points = new_n_points;

    FREE( new_point_index );
}
