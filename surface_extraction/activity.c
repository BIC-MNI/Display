
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_bitlist.h>

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
    void                        set_isosurface_value();

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
                  graphics->three_d.surface_extraction.voxel_done_flags,
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
            val = (Real) GET_VOLUME_DATA( *volume,
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
