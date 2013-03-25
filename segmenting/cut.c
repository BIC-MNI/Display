#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

private   void   create_distance_transform(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels );
private  VIO_BOOL  is_border_pixel(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            x,
    int            y );
private  int  get_minimum_cut(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            label_of_interest );
private  void  expand_region(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            label_of_interest,
    VIO_BOOL        other_label_flag,
    int            global_cutoff );
private  void  get_neighbours_influence_cut(
    int            this_pixel_dist_transform,
    pixel_struct   *neighbour_pixel,
    int            *cutoff,
    int            *class );
private  VIO_BOOL  cutoff_is_better(
    int   global_cutoff,
    int   cutoff1,
    int   class1,
    int   cutoff2,
    int   class2 );
private  void  perform_cut(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            label_of_interest,
    int            global_cutoff );
private  void  expand_label_of_interest(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            label_of_interest );

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


public  void  label_components(
    int             x_size,
    int             y_size,
    pixel_struct    **pixels,
    int             label_of_interest )
{
    int           min_cut;

    create_distance_transform( x_size, y_size, pixels );

    min_cut = get_minimum_cut( x_size, y_size, pixels, label_of_interest );

    print( "Cut %d\n", min_cut );

    if( min_cut > 0 )
    {
        perform_cut( x_size, y_size, pixels, label_of_interest, min_cut );

        expand_label_of_interest( x_size, y_size, pixels, label_of_interest );
    }
    else
        print( "Algorithm unsuccessful\n" );
}

typedef  struct
{
    int  x, y;
} voxxx_struct;

private   void   create_distance_transform(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels )
{
    int                            x, y, nx, ny, dist, dir;
    voxxx_struct                   insert, entry;
    QUEUE_STRUCT( voxxx_struct )   queue;

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
                INSERT_IN_QUEUE( queue, insert );
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
                INSERT_IN_QUEUE( queue, insert );
                pixels[nx][ny].queued = TRUE;
                pixels[nx][ny].dist_transform = dist;
            }
        }
    }

    DELETE_QUEUE( queue );
}

private  VIO_BOOL  is_border_pixel(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            x,
    int            y )
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

private  int  get_minimum_cut(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            label_of_interest )
{
    int                            x, y, cut, min_cut;

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

private  void  expand_region(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            label_of_interest,
    VIO_BOOL        other_label_flag,
    int            global_cutoff )
{
    int                            x, y, nx, ny;
    int                            dir, cutoff, class;
    voxxx_struct                   insert, entry;
    QUEUE_STRUCT( voxxx_struct )   queue;

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
                INSERT_IN_QUEUE( queue, insert );
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
                        INSERT_IN_QUEUE( queue, insert );
                    }
                }
            }
        }
    }

    DELETE_QUEUE( queue );
}

private  void  get_neighbours_influence_cut(
    int            this_pixel_dist_transform,
    pixel_struct   *neighbour_pixel,
    int            *cutoff,
    int            *class )
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

private  VIO_BOOL  cutoff_is_better(
    int   global_cutoff,
    int   cutoff1,
    int   class1,
    int   cutoff2,
    int   class2 )
{
    VIO_BOOL  first_is_better;

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

private  void  perform_cut(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            label_of_interest,
    int            global_cutoff )
{
    expand_region( x_size, y_size, pixels, label_of_interest, TRUE,
                   global_cutoff );
}

private  void  expand_label_of_interest(
    int            x_size,
    int            y_size,
    pixel_struct   **pixels,
    int            label_of_interest )
{
    int                            x, y, nx, ny;
    int                            dir;
    voxxx_struct                   insert, entry;
    QUEUE_STRUCT( voxxx_struct )   queue;

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
                INSERT_IN_QUEUE( queue, insert );
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
                INSERT_IN_QUEUE( queue, insert );
            }
        }
    }

    DELETE_QUEUE( queue );
}
