
#include  <def_connect.h>
#include  <def_alloc.h>
#include  <def_queue.h>

#define  N_4_NEIGHBOURS    4

static   int   Dx4[N_4_NEIGHBOURS] = { 1, 0, -1,  0 };
static   int   Dy4[N_4_NEIGHBOURS] = { 0, 1,  0, -1 };

#define  N_8_NEIGHBOURS    8

static   int   Dx8[N_8_NEIGHBOURS] = {  1,  1,  0, -1, -1, -1,  0,  1 };
static   int   Dy8[N_8_NEIGHBOURS] = {  0,  1,  1,  1,  0, -1, -1, -1 };

#define  INFINITY          10000
#define  INVALID_DISTANCE     -1 

public  Status  label_components( x_size, y_size, pixels, label_of_interest )
    int             x_size, y_size;
    pixel_struct    **pixels;
    int             label_of_interest;
{
    Status        status;
    int           conn_length, x, y;
    void          create_distance_transform();
    Status        add_a_cut();
    void          expand_region_of_interest();

    status = OK;

    create_distance_transform( x_size, y_size, pixels );

    while( (conn_length = find_connectivity_length( x_size, y_size, pixels,
                       label_of_interest,&x,&y)) > 0 &&
           status == OK )
    {
        status = add_a_cut( x_size, y_size, pixels, conn_length, x, y );

        create_distance_transform( x_size, y_size, pixels );
    }

    expand_region_of_interest( x_size, y_size, pixels, label_of_interest );

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
    int                            x, y, nx, ny, dist, dir;
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

        for_less( dir, 0, N_4_NEIGHBOURS )
        {
            nx = x + Dx4[dir];
            ny = y + Dy4[dir];

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

    DELETE_QUEUE( status, queue );
}

Boolean  is_border_pixel( x_size, y_size, pixels, x, y )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            x;
    int            y;
{
    int   nx, ny, dir;

    for_less( dir, 0, N_4_NEIGHBOURS )
    {
        nx = x + Dx4[dir];
        ny = y + Dy4[dir];

        if( nx < 0 || nx >= x_size ||
            ny < 0 || ny >= y_size ||
            !pixels[nx][ny].inside )
        {
            return( TRUE );
        }
    }

    return( FALSE );
}

private  int  find_connectivity_length( x_size, y_size, pixels,
                                        label_of_interest, x_label, y_label )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            label_of_interest;
    int            *x_label;
    int            *y_label;
{
    int                            x, y, nx, ny, conn_length;
    int                            dir, max_dist;
    Status                         status;
    voxel_struct                   insert, entry;
    QUEUE_STRUCT( voxel_struct )   queue;

    INITIALIZE_QUEUE( queue );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].label == label_of_interest )
            {
                pixels[x][y].cutoff = pixels[x][y].dist_transform;
                pixels[x][y].queued = TRUE;
                pixels[x][y].dist_from_region = 0;
                insert.x = x;
                insert.y = y;
                INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
            }
            else
            {
                pixels[x][y].cutoff = 0;
                pixels[x][y].queued = FALSE;
                pixels[x][y].dist_from_region = INVALID_DISTANCE;
            }
        }
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;

        pixels[x][y].queued = FALSE;

        for_less( dir, 0, N_8_NEIGHBOURS )
        {
            nx = x + Dx8[dir];
            ny = y + Dy8[dir];

            if( nx >= 0 && nx < x_size &&
                ny >= 0 && ny < y_size &&
                pixels[nx][ny].label == INVALID_LABEL &&
                pixels[nx][ny].inside )
            {
#ifdef OLD
                if( pixels[nx][ny].label != INVALID_LABEL )
                    max_dist = INFINITY;
                else
#endif
                    max_dist = pixels[nx][ny].dist_transform;

                if( pixels[x][y].cutoff > pixels[nx][ny].cutoff &&
                    pixels[nx][ny].cutoff < max_dist )
                {
                    pixels[nx][ny].cutoff = MIN( pixels[x][y].cutoff,
                                                 max_dist );
                    pixels[nx][ny].dist_from_region =
                             pixels[x][y].dist_from_region + 1;

                    if( !pixels[nx][ny].queued &&
                        pixels[nx][ny].label == INVALID_LABEL)
                    {
                        insert.x = nx;
                        insert.y = ny;
                        INSERT_IN_QUEUE( status, queue, voxel_struct,
                                         insert );
                        pixels[nx][ny].queued = TRUE;
                        pixels[nx][ny].dist_from_region =
                             pixels[x][y].dist_from_region + 1;
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
            if( pixels[x][y].label != INVALID_LABEL &&
                pixels[x][y].label != label_of_interest &&
                pixels[x][y].cutoff > conn_length )
            {
                conn_length = pixels[x][y].cutoff;
                *x_label = x;
                *y_label = y;
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
    int   dir, nx, ny, cutoff;

    cutoff = 0;

    for_less( dir, 0, N_8_NEIGHBOURS )
    {
        nx = x + Dx8[dir];
        ny = y + Dy8[dir];

        if( nx >= 0 && nx < x_size &&
            ny >= 0 && ny < y_size &&
            pixels[nx][ny].inside )
        {
            if( pixels[nx][ny].cutoff > cutoff )
                cutoff = pixels[nx][ny].cutoff;
        }
    }

    return( cutoff );
}

private   Status   add_a_cut( x_size, y_size, pixels, conn_length, x, y )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            conn_length;
    int            x, y;
{
    Status         status;
    int            i, j, path_length, path_index;
    voxel_struct   *path;
    void           find_path();
    void           shrink_boundary_to_path();
    void           find_pixel_to_cut();
    void           cut_at_pixel();

    path_length = pixels[x][y].dist_from_region + 1;

    ALLOC1( status, path, path_length, voxel_struct );

    if( status == OK )
    {
        find_path( x_size, y_size, pixels, x, y, path );

        for_less( i, 0, x_size )
        {
            for_less( j, 0, y_size )
            {
                pixels[i][j].on_path = FALSE;
            }
        }

        for_less( i, 0, path_length )
        {
            pixels[path[i].x][path[i].y].on_path = TRUE;
        }

        shrink_boundary_to_path( x_size, y_size, pixels );

        find_pixel_to_cut( x_size, y_size, pixels, path_length, path,
                           &path_index );

        cut_at_pixel( x_size, y_size, pixels, path_length, path, path_index );
    }

    if( status == OK )
        FREE1( status, path )

    return( status );
}

private   void   find_path( x_size, y_size, pixels, x, y, path )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            x;
    int            y;
    voxel_struct   path[];
{
    int   nx, ny, dir, length;
    Boolean  found;

    length = pixels[x][y].dist_from_region;

    path[length].x = x;
    path[length].y = y;

    for( length = pixels[x][y].dist_from_region-1;  length >= 0;  --length )
    {
        found = FALSE;
        for_less( dir, 0, N_8_NEIGHBOURS )
        {
            nx = x + Dx8[dir];
            ny = y + Dy8[dir];

            if( nx >= 0 && nx < x_size &&
                ny >= 0 && ny < y_size &&
                pixels[nx][ny].inside &&
                (pixels[nx][ny].label == INVALID_LABEL || length == 0) &&
                pixels[nx][ny].dist_from_region == length )
            {
                x = nx;
                y = ny;
                found = TRUE;
                break;
            }
        }

        if( !found )
        {
             (void) fprintf( stderr, "Error in following path\n" );
             (void) abort();
        }

#ifdef DEBUG
pixels[x][y].label = 9;
#endif

        path[length].x = x;
        path[length].y = y;
    }
}

private   void   shrink_boundary_to_path( x_size, y_size, pixels )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
{
    Status                         status;
    int                            x, y, nx, ny, dist, dir;
    voxel_struct                   insert, entry;
    QUEUE_STRUCT( voxel_struct )   queue;

    INITIALIZE_QUEUE( queue );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].inside &&
                !pixels[x][y].on_path &&
                is_border_pixel( x_size, y_size, pixels, x, y ) )
            {
                insert.x = x;
                insert.y = y;
                INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
                pixels[x][y].queued = TRUE;
                pixels[x][y].path_dist_transform = 1;

#ifdef DEBUG
pixels[x][y].label = 1;
#endif
            }
            else
            {
                pixels[x][y].queued = FALSE;
                pixels[x][y].path_dist_transform = 0;
            }
        }
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;
        dist = pixels[x][y].path_dist_transform + 1;

        for_less( dir, 0, N_4_NEIGHBOURS )
        {
            nx = x + Dx4[dir];
            ny = y + Dy4[dir];

            if( nx >= 0 && nx < x_size &&
                ny >= 0 && ny < y_size && 
                pixels[nx][ny].inside  &&
                !pixels[nx][ny].on_path  &&
                !pixels[nx][ny].queued )
            {
                insert.x = nx;
                insert.y = ny;
                INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
                pixels[nx][ny].queued = TRUE;
                pixels[nx][ny].path_dist_transform = dist;

#ifdef DEBUG
pixels[nx][ny].label = dist;
#endif
            }
        }
    }

    DELETE_QUEUE( status, queue );
}

private  void  find_pixel_to_cut( x_size, y_size, pixels, path_length, path,
                                  path_index )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            path_length;
    voxel_struct   path[];
    int            *path_index;
{
    int      i, min_cut, cut, prev_cut;
    int      start, end;

#define TAKE_GLOBAL_MIN
#ifdef  TAKE_GLOBAL_MIN
    start = path_length-1;
    end = 0;
#else
    prev_cut = 0;

    for( start = path_length-1;  start >= 0;  --start )
    {
        cut = find_min_cut( x_size, y_size, pixels, path_length, path, start );

        if( cut < prev_cut )
            break;

        prev_cut = cut;
    }

    prev_cut = 0;

    for( end = 0;  end < path_length;  ++end )
    {
        cut = find_min_cut( x_size, y_size, pixels, path_length, path, end );

        if( cut < prev_cut )
            break;

        prev_cut = cut;
    }

    if( start < end )
    {
        start = (start + end) / 2;
        end = start;
    }
#endif

    min_cut = 0;

    for( i = start;  i >= end;  --i )
    {
        cut = find_min_cut( x_size, y_size, pixels, path_length, path, i );

        if( (i == start) || cut < min_cut )
        {
            min_cut = cut;
            *path_index = i;
        }
    }
}

private  int  find_min_cut( x_size, y_size, pixels, path_length, path, i )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            path_length;
    voxel_struct   path[];
    int            i;
{
    int   dir1, dir2, cut1, cut2;
    void  find_two_cut_neighbours();

    find_two_cut_neighbours( x_size, y_size, pixels, path_length, path, i,
                             &dir1, &dir2, &cut1, &cut2 );

    return( cut1 + cut2 + 1 );
}

private  void  find_two_cut_neighbours( x_size, y_size, pixels,
                                        path_length, path, i, dir1, dir2,
                                        cut1, cut2 )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            path_length;
    voxel_struct   path[];
    int            i;
    int            *dir1;
    int            *dir2;
    int            *cut1;
    int            *cut2;
{
    int   x, y, dir, start_dir, end_dir, prev_dir, next_dir;
    int   nx, ny;

    x = path[i].x;
    y = path[i].y;

    if( i > 0 )
        prev_dir = get_dir( path[i-1].x - x, path[i-1].y - y );

    if( i < path_length-1 )
        next_dir = get_dir( path[i+1].x - x, path[i+1].y - y );

    if( i == 0 )
        prev_dir = next_dir;

    if( i == path_length-1 )
        next_dir = prev_dir;

    if( ((prev_dir + 1) % N_8_NEIGHBOURS == next_dir) ||
        ((prev_dir - 1 + N_8_NEIGHBOURS) % N_8_NEIGHBOURS == next_dir) )
    {
        (void) printf( "Error in find_min_cut\n" );
        (void) abort();
    }

    *cut1 = -1;

    start_dir = (next_dir + 1) % N_8_NEIGHBOURS;
    end_dir = prev_dir;

    dir = start_dir;
    
    while( dir != end_dir )
    {
        if( dir % 2 == 0 )
        {
            nx = x + Dx8[dir];
            ny = y + Dy8[dir];

            if( nx < 0 || nx >= x_size ||
                ny < 0 || ny >= y_size ||
                !pixels[nx][ny].inside )
            {
                *cut1 = 0;
                *dir1 = dir;
            }
            else if( *cut1 == -1 ||
                     pixels[nx][ny].path_dist_transform < *cut1 )
            {
                *cut1 = pixels[nx][ny].path_dist_transform;
                *dir1 = dir;
            }
        }

        ++dir;
        if( dir == N_8_NEIGHBOURS )  dir = 0;
    }

    *cut2 = -1;

    end_dir = prev_dir;
    start_dir = (next_dir - 1 + N_8_NEIGHBOURS) % N_8_NEIGHBOURS;

    dir = start_dir;
    
    while( dir != end_dir )
    {
        if( dir % 2 == 0 )
        {
            nx = x + Dx8[dir];
            ny = y + Dy8[dir];

            if( nx < 0 || nx >= x_size ||
                ny < 0 || ny >= y_size ||
                !pixels[nx][ny].inside )
            {
                *cut2 = 0;
                *dir2 = dir;
            }
            else if( *cut2 == -1 ||
                     pixels[nx][ny].path_dist_transform < *cut2 )
            {
                *cut2 = pixels[nx][ny].path_dist_transform;
                *dir2 = dir;
            }
        }

        --dir;
        if( dir == -1 )  dir = N_8_NEIGHBOURS-1;
    }

    if( *cut1 == -1 || *cut2 == -1 )
    {
        (void) printf( "Error in find_min_cut %d %d\n", *cut1, *cut2 );
        (void) abort();
    }
}

private  int  get_dir( dx, dy )
    int   dx, dy;
{
    int   dir;

    for_less( dir, 0, N_8_NEIGHBOURS )
    {
        if( dx == Dx8[dir] && dy == Dy8[dir] )
            return( dir );
    }

    (void) printf( "get_dir error\n" );
    (void) abort();

    return( 987654321 );
}

private  void  cut_at_pixel( x_size, y_size, pixels, path_length, path,
                             path_index )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            path_length;
    voxel_struct   path[];
    int            path_index;
{
    int    i, x, y, which;
    int    dirs[2], cut[2];
    void   find_path_transform_neighbour();

    find_two_cut_neighbours( x_size, y_size, pixels, path_length, path,
                             path_index, &dirs[0], &dirs[1], &cut[0], &cut[1] );

    x = path[path_index].x;
    y = path[path_index].y;

    pixels[x][y].inside = FALSE;

    for_less( which, 0, 2 )
    {
        if( cut[which] > 0 )
        {
            x = path[path_index].x + Dx8[dirs[which]];
            y = path[path_index].y + Dy8[dirs[which]];
        }

        for( i = cut[which];  i > 0;  --i )
        {
            pixels[x][y].inside = FALSE;

            if( i != 1 )
                find_path_transform_neighbour( x_size, y_size, pixels,
                                               x, y, i-1, &x, &y );
        }
    }
}

private  void  find_path_transform_neighbour( x_size, y_size, pixels, x, y,
                                              path_transform, x_next, y_next )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            x, y;
    int            path_transform;
    int            *x_next, *y_next;
{
    int      dir, nx, ny;
    Boolean  found;

    found = FALSE;

    for_less( dir, 0, N_4_NEIGHBOURS )
    {
        nx = x + Dx4[dir];
        ny = y + Dy4[dir];

        if( nx >= 0 && nx < x_size &&
            ny >= 0 && ny < y_size && 
            pixels[nx][ny].inside  &&
            !pixels[nx][ny].on_path &&
            pixels[nx][ny].path_dist_transform == path_transform )
        {
            *x_next = nx;
            *y_next = ny;
            found = TRUE;
            break;
        }
    }

    if( !found )
    {
        (void) printf( "Error:  find_path_transform_neighbour\n" );
        (void) abort();
    }
}

private  void  expand_region_of_interest( x_size, y_size, pixels,
                                          label_of_interest )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            label_of_interest;
{
    Status                         status;
    int                            x, y, nx, ny, dir;
    voxel_struct                   insert, entry;
    QUEUE_STRUCT( voxel_struct )   queue;

    INITIALIZE_QUEUE( queue );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].inside &&
                pixels[x][y].label == label_of_interest )
            {
                insert.x = x;
                insert.y = y;
                INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
                pixels[x][y].queued = TRUE;
            }
            else
            {
                pixels[x][y].queued = FALSE;
            }
        }
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;

        for_less( dir, 0, N_8_NEIGHBOURS )
        {
            nx = x + Dx8[dir];
            ny = y + Dy8[dir];

            if( nx >= 0 && nx < x_size &&
                ny >= 0 && ny < y_size && 
                pixels[nx][ny].inside  &&
                !pixels[nx][ny].queued && 
                pixels[nx][ny].label == INVALID_LABEL )
            {
                insert.x = nx;
                insert.y = ny;
                INSERT_IN_QUEUE( status, queue, voxel_struct, insert );
                pixels[nx][ny].queued = TRUE;
                pixels[nx][ny].label = label_of_interest;
            }
        }
    }

    DELETE_QUEUE( status, queue );
}
