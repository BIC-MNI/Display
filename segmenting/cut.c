
#include  <def_connect.h>
#include  <def_alloc.h>
#include  <def_queue.h>

#define  DECREASING      0
#define  FREE_RANGING    1
#define  INCREASING      2

#define  N_4_NEIGHBOURS    4

static   int   Dx4[N_4_NEIGHBOURS] = { 1, 0, -1,  0 };
static   int   Dy4[N_4_NEIGHBOURS] = { 0, 1,  0, -1 };

#define  N_8_NEIGHBOURS    8

static   int   Dx8[N_8_NEIGHBOURS] = {  1,  1,  0, -1, -1, -1,  0,  1 };
static   int   Dy8[N_8_NEIGHBOURS] = {  0,  1,  1,  1,  0, -1, -1, -1 };

#define  INFINITY          10000
#define  INVALID_DISTANCE     -1 

static    void          create_distance_transform();
static    Status        add_a_cut();
static    void          expand_region_of_interest();
static    Boolean       find_other_label_connected();
static    Status        find_path();
static    void          shrink_boundary_to_path();
static    Status        find_pixel_to_cut();
static    Status        cut_at_pixel();
static    Status        find_min_cut();
static    Status        find_two_cut_neighbours();
static    int           get_dir();
static    Status        find_path_transform_neighbour();

public  Status  label_components( x_size, y_size, pixels, label_of_interest )
    int             x_size, y_size;
    pixel_struct    **pixels;
    int             label_of_interest;
{
    Status        status;
    int           x, y;
    int           cut_num, min_cut;
    void          perform_cut();

    status = OK;

    cut_num = 0;

    create_distance_transform( x_size, y_size, pixels );

    min_cut = get_minimum_cut( x_size, y_size, pixels, label_of_interest );

(void) printf( "Cut %d\n", min_cut );

    perform_cut( x_size, y_size, pixels, label_of_interest, min_cut );

#ifdef OLD
    if( min_cut > 0 )
    {
    }

    while( find_other_label_connected( x_size, y_size, pixels,
                                       label_of_interest,&x,&y) &&
           status == OK )
    {
        status = add_a_cut( x_size, y_size, pixels, x, y );

        create_distance_transform( x_size, y_size, pixels );

        ++cut_num;
        (void) printf( "Cut %d\n", cut_num );
    }

    if( status == OK )
        expand_region_of_interest( x_size, y_size, pixels, label_of_interest );
#endif

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
                INSERT_IN_QUEUE( status, queue, insert );
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

        for_less( dir, 0, N_8_NEIGHBOURS )
        {
            nx = x + Dx8[dir];
            ny = y + Dy8[dir];

            if( nx >= 0 && nx < x_size &&
                ny >= 0 && ny < y_size && 
                pixels[nx][ny].inside  &&
                !pixels[nx][ny].queued )
            {
                insert.x = nx;
                insert.y = ny;
                INSERT_IN_QUEUE( status, queue, insert );
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

    for_less( dir, 0, N_8_NEIGHBOURS )
    {
        nx = x + Dx8[dir];
        ny = y + Dy8[dir];

        if( nx < 0 || nx >= x_size ||
            ny < 0 || ny >= y_size ||
            !pixels[nx][ny].inside )
        {
            return( TRUE );
        }
    }

    return( FALSE );
}

private  int  get_minimum_cut( x_size, y_size, pixels, label_of_interest )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            label_of_interest;
{
    int                            x, y, cut, min_cut;
    void                           expand_region();

    expand_region( x_size, y_size, pixels, label_of_interest, FALSE, -1 );

    min_cut = 0;

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].label != INVALID_LABEL &&
                pixels[x][y].label != label_of_interest &&
                pixels[x][y].cutoff >= 0 )
            {
                cut = pixels[x][y].cutoff;

                if( cut > min_cut )
                    min_cut = cut;
            }
        }
    }

    return( min_cut );
}

private  void  expand_region( x_size, y_size, pixels, label_of_interest,
                              other_label_flag, global_cutoff )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            label_of_interest;
    Boolean        other_label_flag;
    int            global_cutoff;
{
    int                            x, y, nx, ny;
    int                            dir, cutoff, class;
    Status                         status;
    voxel_struct                   insert, entry;
    QUEUE_STRUCT( voxel_struct )   queue;
    void                           get_neighbours_influence_cut();

    INITIALIZE_QUEUE( queue );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].label != INVALID_LABEL &&
                ((!other_label_flag &&
                  pixels[x][y].label == label_of_interest) ||
                 (other_label_flag &&
                  pixels[x][y].label != label_of_interest)
                ) )
            {
                pixels[x][y].cutoff = pixels[x][y].dist_transform;
                pixels[x][y].dist_from_region = INCREASING;
                pixels[x][y].queued = TRUE;
                insert.x = x;
                insert.y = y;
                INSERT_IN_QUEUE( status, queue, insert );
            }
            else
            {
                pixels[x][y].cutoff = 0;
                pixels[x][y].dist_from_region = FREE_RANGING;
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

        for_less( dir, 0, N_8_NEIGHBOURS )
        {
            nx = x + Dx8[dir];
            ny = y + Dy8[dir];

            if( nx >= 0 && nx < x_size &&
                ny >= 0 && ny < y_size &&
                pixels[nx][ny].inside )
            {
                get_neighbours_influence_cut( pixels[nx][ny].dist_transform,
                                              &pixels[x][y], &cutoff, &class );
                if( cutoff_is_better( global_cutoff, cutoff, class,
                                      pixels[nx][ny].cutoff,
                                      pixels[nx][ny].dist_from_region ) )
                {
                    pixels[nx][ny].cutoff = cutoff;
                    pixels[nx][ny].dist_from_region = class;

                    if( !pixels[nx][ny].queued &&
                        (pixels[nx][ny].label == INVALID_LABEL ||
                         ((!other_label_flag &&
                           pixels[nx][ny].label == label_of_interest) ||
                          (other_label_flag &&
                           pixels[nx][ny].label != label_of_interest)
                         )
                        ) )
                    {
                        pixels[nx][ny].queued = TRUE;
                        insert.x = nx;
                        insert.y = ny;
                        INSERT_IN_QUEUE( status, queue, insert );
                    }
                }
            }
        }
    }

    DELETE_QUEUE( status, queue );
}

private  void  get_neighbours_influence_cut( this_pixel_dist_transform,
                                             neighbour_pixel, cutoff, class )
    int            this_pixel_dist_transform;
    pixel_struct   *neighbour_pixel;
    int            *cutoff;
    int            *class;
{
    switch( neighbour_pixel->dist_from_region )
    {
    case FREE_RANGING:
        if( this_pixel_dist_transform < neighbour_pixel->cutoff )
        {
            *cutoff = neighbour_pixel->cutoff;
            *class = DECREASING;
        }
        else
        {
            *cutoff = neighbour_pixel->cutoff;
            *class = FREE_RANGING;
        }
        break;

    case INCREASING:
        if( this_pixel_dist_transform > neighbour_pixel->dist_transform )
        {
            *cutoff = this_pixel_dist_transform;
            *class = INCREASING;
        }
        else if( this_pixel_dist_transform < neighbour_pixel->dist_transform )
        {
            *cutoff = neighbour_pixel->dist_transform;
            *class = DECREASING;
        }
        else
        {
            *cutoff = this_pixel_dist_transform;
            *class = FREE_RANGING;
        }
        break;

    case DECREASING:
        if( this_pixel_dist_transform < neighbour_pixel->dist_transform )
        {
            *cutoff = neighbour_pixel->cutoff;
            *class = DECREASING;
        }
        else
        {
            *cutoff = neighbour_pixel->dist_transform;
            *class = FREE_RANGING;
        }
        break;
    }
}

private  Boolean  cutoff_is_better( global_cutoff,
                                    cutoff1, class1, cutoff2, class2 )
    int   global_cutoff;
    int   cutoff1;
    int   class1;
    int   cutoff2;
    int   class2;
{
    Boolean  first_is_better;

    if( class1 == FREE_RANGING && cutoff1 <= global_cutoff )
        return( FALSE );

    if( cutoff1 > cutoff2 )
        first_is_better = TRUE;
    else if( cutoff1 < cutoff2 )
        first_is_better = FALSE;
    else if( class1 > class2 )
        first_is_better = TRUE;
    else
        first_is_better = FALSE;

    return( first_is_better );
}

private  void  perform_cut( x_size, y_size, pixels, label_of_interest,
                            global_cutoff )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            label_of_interest;
    int            global_cutoff;
{
    int                            x, y;
    void                           expand_region();

    expand_region( x_size, y_size, pixels, label_of_interest, TRUE,
                   global_cutoff );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].inside && pixels[x][y].cutoff == 0 )
                pixels[x][y].label = label_of_interest;
        }
    }
}

#ifdef OLD
private  Boolean  find_other_label_connected( x_size, y_size, pixels,
                                          label_of_interest, x_label, y_label )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            label_of_interest;
    int            *x_label;
    int            *y_label;
{
    int                            x, y, nx, ny, dist;
    int                            dir;
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
                INSERT_IN_QUEUE( status, queue, insert );
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

        for_less( dir, 0, N_8_NEIGHBOURS )
        {
            nx = x + Dx8[dir];
            ny = y + Dy8[dir];

            if( nx >= 0 && nx < x_size &&
                ny >= 0 && ny < y_size &&
                !pixels[nx][ny].queued &&
                pixels[nx][ny].label != label_of_interest &&
                pixels[nx][ny].inside )
            {
                pixels[nx][ny].dist_from_region =
                         pixels[x][y].dist_from_region + 1;
                pixels[nx][ny].queued = TRUE;

                if( pixels[nx][ny].label == INVALID_LABEL )
                {
                    insert.x = nx;
                    insert.y = ny;
                    INSERT_IN_QUEUE( status, queue, insert );
                }
            }
        }
    }

    DELETE_QUEUE( status, queue );

    dist = 0;

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].label != INVALID_LABEL &&
                pixels[x][y].label != label_of_interest &&
                pixels[x][y].dist_from_region > dist )
            {
                dist = pixels[x][y].dist_from_region;
                *x_label = x;
                *y_label = y;
            }
        }
    }

    return( dist > 1 );
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

private   Status   add_a_cut( x_size, y_size, pixels, x, y )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            x, y;
{
    Status         status;
    int            i, j, path_length, path_index;
    voxel_struct   *path;

    path_length = pixels[x][y].dist_from_region + 1;

    ALLOC( status, path, path_length );

    if( status == OK )
        status = find_path( x_size, y_size, pixels, x, y, path );

    if( status == OK )
    {
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

        status = find_pixel_to_cut( x_size, y_size, pixels, path_length, path,
                                    &path_index );
    }

    if( status == OK )
        status = cut_at_pixel( x_size, y_size, pixels, path_length, path,
                               path_index );

    if( status == OK )
        FREE( status, path )

    return( status );
}

private   Status   find_path( x_size, y_size, pixels, x, y, path )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            x;
    int            y;
    voxel_struct   path[];
{
    int   nx, ny, dir, length;
    Boolean  found;
    Status   status;

    status = OK;

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
             status = ERROR;
             break;
        }

#ifdef DEBUG
pixels[x][y].label = 9;
#endif

        path[length].x = x;
        path[length].y = y;
    }

    return( status );
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
                INSERT_IN_QUEUE( status, queue, insert );
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
                INSERT_IN_QUEUE( status, queue, insert );
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

private  Status  find_pixel_to_cut( x_size, y_size, pixels, path_length, path,
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
    Status   status;

    status = OK;

#define TAKE_GLOBAL_MIN
#ifdef  TAKE_GLOBAL_MIN
    start = path_length-1;
    end = 0;
#else
    prev_cut = 0;

    for( start = path_length-1;  start >= 0;  --start )
    {
        status = find_min_cut( x_size, y_size, pixels, path_length, path,
                               start, &cut );

        if( cut < prev_cut || status != OK )
            break;

        prev_cut = cut;
    }

    if( status == OK )
    {
        prev_cut = 0;

        for( end = 0;  end < path_length;  ++end )
        {
            status = find_min_cut( x_size, y_size, pixels, path_length, path,
                                   end, &cut );

            if( cut < prev_cut )
                break;

            prev_cut = cut;
        }

        if( start < end )
        {
            start = (start + end) / 2;
            end = start;
        }
    }
#endif

    if( status == OK )
    {
        min_cut = 0;

        for( i = start;  i >= end;  --i )
        {
            status = find_min_cut( x_size, y_size, pixels, path_length, path,
                                   i, &cut );

            if( (i == start) || cut < min_cut )
            {
                min_cut = cut;
                *path_index = i;
            }
        }
    }

    return( status );
}

private  Status  find_min_cut( x_size, y_size, pixels, path_length, path, i,
                            min_cut )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            path_length;
    voxel_struct   path[];
    int            i;
    int            *min_cut;
{
    int     dir1, dir2, cut1, cut2;
    Status  status;

    status = find_two_cut_neighbours( x_size, y_size, pixels, path_length,
                      path, i, &dir1, &dir2, &cut1, &cut2 );

    *min_cut = cut1 + cut2 + 1;

    return( status );
}

private  Status  find_two_cut_neighbours( x_size, y_size, pixels,
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
    int      x, y, dir, start_dir, end_dir, prev_dir, next_dir;
    int      nx, ny;
    Status   status;

    status = OK;

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
        status = ERROR;
    }

    if( status == OK )
    {
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
            status = ERROR;
        }
    }

    return( status );
}

private  int  get_dir( dx, dy )
    int   dx, dy;
{
    int   d;

    for_less( d, 0, N_8_NEIGHBOURS )
    {
        if( dx == Dx8[d] && dy == Dy8[d] )
            break;
    }

    if( d == N_8_NEIGHBOURS )
    {
        (void) printf( "get_dir error\n" );
        d = 987654321;
        abort();
    }

    return( d );
}

private  Status  cut_at_pixel( x_size, y_size, pixels, path_length, path,
                             path_index )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            path_length;
    voxel_struct   path[];
    int            path_index;
{
    int      i, x, y, which;
    int      dirs[2], cut[2];
    Status   status;

    status = find_two_cut_neighbours( x_size, y_size, pixels, path_length, path,
                             path_index, &dirs[0], &dirs[1], &cut[0], &cut[1] );

    if( status == OK )
    {
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
                    status = find_path_transform_neighbour( x_size, y_size,
                                     pixels, x, y, i-1, &x, &y );

                if( status != OK )  break;
            }
            if( status != OK )  break;
        }
    }

    return( status );
}

private  Status  find_path_transform_neighbour( x_size, y_size, pixels, x, y,
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
    Status   status;

    status = OK;

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
        status = ERROR;
    }

    return( status );
}

private  void  expand_region_of_interest( x_size, y_size, pixels,
                                          label_of_interest, global_cutoff )
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
                INSERT_IN_QUEUE( status, queue, insert );
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
                INSERT_IN_QUEUE( status, queue, insert );
                pixels[nx][ny].queued = TRUE;
                pixels[nx][ny].label = label_of_interest;
            }
        }
    }

    DELETE_QUEUE( status, queue );
}
#endif
