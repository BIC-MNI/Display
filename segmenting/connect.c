
#include  <def_graphics.h>
#include  <def_alloc.h>
#include  <def_queue.h>

typedef  struct
{
    Boolean  inside;
    int      label;
    Boolean  queued;
    int      dist_transform;
    int      cutoff;
} pixel_struct;

#define  INFINITY          10000
#define  REGION_OF_INTEREST    1

public  Status  disconnect_components( volume, voxel_indices, axis,
                                       n_labels, labels,
                                       min_threshold, max_threshold )
    volume_struct   *volume;
    int             voxel_indices[3];
    int             axis[3];
    int             n_labels;
    label_struct    labels[];
    int             min_threshold;
    int             max_threshold;
{
    Status        status;
    int           conn_length, val, i;
    void          create_distance_transform();
    void          assign_region_flags();
    pixel_struct  **pixels;
    int           x, y, index[3], size[3];
    void          set_voxel_inactivity();
    void          get_volume_size();

    get_volume_size( volume, &size[X_AXIS], &size[Y_AXIS], &size[Z_AXIS] );

    ALLOC2( status, pixels, size[axis[X_AXIS]], size[axis[Y_AXIS]],
            pixel_struct );

    index[axis[Z_AXIS]] = voxel_indices[axis[Z_AXIS]];
    for_less( x, 0, size[axis[X_AXIS]] )
    {
        index[axis[X_AXIS]] = x;
        for_less( y, 0, size[axis[Y_AXIS]] )
        {
            index[axis[Y_AXIS]] = y;

            val = GET_VOLUME_DATA( *volume, index[0], index[1], index[2] );

            pixels[x][y].inside = (val >= min_threshold && val <=max_threshold);
            pixels[x][y].label = 0;
        }
    }

    for_less( i, 0, n_labels )
    {
        if( labels[i].voxel_indices[axis[Z_AXIS]] == voxel_indices[Z_AXIS] )
        {
            pixels[labels[i].voxel_indices[X_AXIS]]
                  [labels[i].voxel_indices[Y_AXIS]].label = labels[i].id;
        }
    }

    create_distance_transform( size[X_AXIS], size[Y_AXIS], pixels );

    conn_length = find_connectivity_length( size[X_AXIS], size[Y_AXIS],
                                            pixels );

    assign_region_flags( size[X_AXIS], size[Y_AXIS], pixels, conn_length );

    index[axis[Z_AXIS]] = voxel_indices[axis[Z_AXIS]];
    for_less( x, 0, size[axis[X_AXIS]] )
    {
        index[axis[X_AXIS]] = x;
        for_less( y, 0, size[axis[Y_AXIS]] )
        {
            index[axis[Y_AXIS]] = y;

            if( pixels[x][y].label != REGION_OF_INTEREST )
                set_voxel_inactivity( volume, index[0], index[1], index[2],
                                      TRUE );
            else
                set_voxel_inactivity( volume, index[0], index[1], index[2],
                                      FALSE );
        }
    }

    FREE2( status, pixels );

    return( status );
}

typedef  struct
{
    int  x,y;
} voxel_struct;

private   void   create_distance_transform( x_size, y_size, pixels )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
{
    Status                         status;
    int                            x, y, nx, ny, dist, dx, dy;
    voxel_struct                   insert, entry;
    QUEUE_STRUCT( voxel_struct )   queue;

    INITIALIZE_QUEUE( queue );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].inside &&
                is_border_pixel( x_size, y_size, pixels, x, y ) )
            {
                insert.x = x;
                insert.y = y;
                INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
                pixels[x][y].queued = TRUE;
                pixels[x][y].dist_transform = 1;
            }
            else
            {
                pixels[x][y].queued = FALSE;
                pixels[x][y].dist_transform = 0;
            }
        }
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;
        dist = pixels[x][y].dist_transform + 1;

        for_inclusive( dx, -1, 1 )
        {
            for_inclusive( dy, -1, 1 )
            {
                if( dx != 0 || dy != 0 )
                {
                    nx = x + dx;
                    ny = y + dy;

                    if( nx >= 0 && nx < x_size &&
                        ny >= 0 && ny < y_size && 
                        pixels[nx][ny].inside  &&
                        !pixels[nx][ny].queued )
                    {
                        insert.x = nx;
                        insert.y = ny;
                        INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
                        pixels[nx][ny].queued = TRUE;
                        pixels[nx][ny].dist_transform = dist;
                    }
                }
            }
        }
    }

    DELETE_QUEUE( status, queue );
}

Boolean  is_border_pixel( x_size, y_size, pixels, x, y )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            x;
    int            y;
{
    int   dx, dy, nx, ny;

    for_inclusive( dx, -1, 1 )
    {
        for_inclusive( dy, -1, 1 )
        {
            if( dx != 0 || dy != 0 )
            {
                nx = x + dx;
                ny = y + dy;

                if( nx < 0 || nx >= x_size ||
                    ny < 0 || ny >= y_size ||
                    !pixels[nx][ny].inside )
                {
                    return( TRUE );
                }
            }
        }
    }

    return( FALSE );
}

private  int  find_connectivity_length( x_size, y_size, pixels )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
{
    int                            x, y, nx, ny, conn_length;
    int                            dx, dy, max_dist;
    Status                         status;
    voxel_struct                   insert, entry;
    QUEUE_STRUCT( voxel_struct )   queue;

    INITIALIZE_QUEUE( queue );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].label == REGION_OF_INTEREST )
            {
                pixels[x][y].cutoff = INFINITY;
                pixels[x][y].queued = TRUE;
                insert.x = x;
                insert.y = y;
                INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
            }
            else
            {
                pixels[x][y].cutoff = 0;
                pixels[x][y].queued = FALSE;
            }
        }
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;

        pixels[x][y].queued = FALSE;

        for_inclusive( dx, -1, 1 )
        {
            for_inclusive( dy, -1, 1 )
            {
                if( dx != 0 || dy != 0 )
                {
                    nx = x + dx;
                    ny = y + dy;

                    if( nx >= 0 && nx < x_size &&
                        ny >= 0 && ny < y_size &&
                        pixels[nx][ny].inside )
                    {
                        if( pixels[nx][ny].label > 0 )
                            max_dist = INFINITY;
                        else
                            max_dist = pixels[nx][ny].dist_transform;

                        if( pixels[x][y].cutoff > pixels[nx][ny].cutoff &&
                            pixels[nx][ny].cutoff < max_dist )
                        {
                            pixels[nx][ny].cutoff = MIN( pixels[x][y].cutoff,
                                                         max_dist );

                            if( !pixels[nx][ny].queued )
                            {
                                insert.x = nx;
                                insert.y = ny;
                                INSERT_IN_QUEUE( status, queue, voxel_struct,
                                                 insert );
                                pixels[nx][ny].queued = TRUE;
                            }
                        }
                    }
                }
            }
        }
    }

    conn_length = 0;

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].label > 0 &&
                pixels[x][y].label != REGION_OF_INTEREST )
            {
                conn_length = MAX( conn_length, pixels[x][y].cutoff );
            }
        }
    }

    return( conn_length );
}

int  cutoff_from_neighbours( x_size, y_size, pixels, x, y )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            x;
    int            y;
{
    int   dx, dy, nx, ny, cutoff;

    cutoff = 0;

    for_inclusive( dx, -1, 1 )
    {
        for_inclusive( dy, -1, 1 )
        {
            if( dx != 0 || dy != 0 )
            {
                nx = x + dx;
                ny = y + dy;

                if( nx >= 0 && nx < x_size &&
                    ny >= 0 && ny < y_size &&
                    pixels[nx][ny].inside )
                {
                    if( pixels[nx][ny].cutoff > cutoff )
                        cutoff = pixels[nx][ny].cutoff;
                }
            }
        }
    }

    return( cutoff );
}


private   void   assign_region_flags( x_size, y_size, pixels, conn_length )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            conn_length;
{
    Status                         status;
    int                            x, y, nx, ny, dx, dy;
    voxel_struct                   insert, entry;
    QUEUE_STRUCT( voxel_struct )   queue;

    INITIALIZE_QUEUE( queue );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].label == REGION_OF_INTEREST )
            {
                insert.x = x;
                insert.y = y;
                INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
            }
        }
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;

        for_inclusive( dx, -1, 1 )
        {
            for_inclusive( dy, -1, 1 )
            {
                if( dx != 0 || dy != 0 )
                {
                    nx = x + dx;
                    ny = y + dy;

                    if( nx >= 0 && nx < x_size &&
                        ny >= 0 && ny < y_size && 
                        pixels[nx][ny].inside &&
                        pixels[nx][ny].label != REGION_OF_INTEREST &&
                        pixels[nx][ny].dist_transform > conn_length )
                    {
                        insert.x = nx;
                        insert.y = ny;
                        INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
                        pixels[nx][ny].label = REGION_OF_INTEREST;
                    }
                }
            }
        }
    }

    DELETE_QUEUE( status, queue );
}
