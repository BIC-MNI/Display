
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
    int           min_cut;
    void          perform_cut();
    void          expand_label_of_interest();

    status = OK;

    create_distance_transform( x_size, y_size, pixels );

    min_cut = get_minimum_cut( x_size, y_size, pixels, label_of_interest );

(void) printf( "Cut %d\n", min_cut );

    if( min_cut > 0 )
    {
        perform_cut( x_size, y_size, pixels, label_of_interest, min_cut );

        expand_label_of_interest( x_size, y_size, pixels, label_of_interest );
    }
    else
        (void) printf( "Algorithm unsuccessful\n" );

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
    void                           expand_region();

    expand_region( x_size, y_size, pixels, label_of_interest, TRUE,
                   global_cutoff );
}

private  void  expand_label_of_interest( x_size, y_size, pixels,
                                         label_of_interest )
    int            x_size;
    int            y_size;
    pixel_struct   **pixels;
    int            label_of_interest;
{
    int                            x, y, nx, ny;
    int                            dir;
    Status                         status;
    voxel_struct                   insert, entry;
    QUEUE_STRUCT( voxel_struct )   queue;
    void                           get_neighbours_influence_cut();

    INITIALIZE_QUEUE( queue );

    for_less( x, 0, x_size )
    {
        for_less( y, 0, y_size )
        {
            if( pixels[x][y].label == label_of_interest ) 
            {
                pixels[x][y].queued = TRUE;
                insert.x = x;
                insert.y = y;
                INSERT_IN_QUEUE( status, queue, insert );
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
                pixels[nx][ny].inside &&
                pixels[nx][ny].cutoff == 0 &&
                !pixels[nx][ny].queued )
            {
                pixels[nx][ny].label = label_of_interest;
                pixels[nx][ny].queued = TRUE;
                insert.x = nx;
                insert.y = ny;
                INSERT_IN_QUEUE( status, queue, insert );
            }
        }
    }

    DELETE_QUEUE( status, queue );
}
