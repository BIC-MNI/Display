
#include  <display.h>

public  void  initialize_segmenting(
    segmenting_struct  *segmenting )
{
    segmenting->min_threshold = 0.0;
    segmenting->max_threshold = -1.0;
    segmenting->connectivity = (Neighbour_types) Segmenting_connectivity;
}

public  void  clear_all_labels(
    display_struct    *display )
{
    display_struct    *slice_window;

    if( get_slice_window( display, &slice_window ) )
        set_all_volume_label_data( get_label_volume(slice_window), 0 );
}

public  void  set_labels_on_slice(
    Volume         label_volume,
    int            axis_index,
    int            position,
    int            label )
{
    int     voxel[N_DIMENSIONS], sizes[N_DIMENSIONS], a1, a2;

    get_volume_sizes( label_volume, sizes );

    voxel[axis_index] = position;

    a1 = (axis_index + 1) % N_DIMENSIONS;
    a2 = (axis_index + 2) % N_DIMENSIONS;

    for_less( voxel[a1], 0, sizes[a1] )
    {
        for_less( voxel[a2], 0, sizes[a2] )
        {
            set_volume_label_data( label_volume, voxel, label );
        }
    }
}

typedef struct
{
    int  x, y;
} slice_position;

public  void  set_connected_voxels_labels(
    Volume            volume,
    Volume            label_volume,
    int               axis_index,
    int               position[],
    Real              min_threshold,
    Real              max_threshold,
    int               label_min_threshold,
    int               label_max_threshold,
    Neighbour_types   connectivity,
    int               label )
{
    int                             voxel[N_DIMENSIONS], sizes[N_DIMENSIONS];
    int                             a1, a2, x, y;
    int                             dir, n_dirs, *dx, *dy;
    slice_position                  entry;
    QUEUE_STRUCT( slice_position )  queue;

    n_dirs = get_neighbour_directions( connectivity, &dx, &dy );

    get_volume_sizes( volume, sizes );

    voxel[0] = position[0];
    voxel[1] = position[1];
    voxel[2] = position[2];

    a1 = (axis_index + 1) % N_DIMENSIONS;
    a2 = (axis_index + 2) % N_DIMENSIONS;

    INITIALIZE_QUEUE( queue );

    if( should_change_this_one( volume, label_volume, voxel,
                                min_threshold, max_threshold, 
                                label_min_threshold, label_max_threshold,
                                label) )
    {
        set_volume_label_data( label_volume, voxel, label );
        entry.x = voxel[a1];
        entry.y = voxel[a2];
        INSERT_IN_QUEUE( queue, entry );
    }

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;

        for_less( dir, 0, n_dirs )
        {
            voxel[a1] = x + dx[dir];
            voxel[a2] = y + dy[dir];

            if( voxel[a1] >= 0 && voxel[a1] < sizes[a1] &&
                voxel[a2] >= 0 && voxel[a2] < sizes[a2] &&
                should_change_this_one( volume, label_volume, voxel,
                                     min_threshold, max_threshold,
                                     label_min_threshold, label_max_threshold,
                                     label))
            {
                set_volume_label_data( label_volume, voxel, label);
                entry.x = voxel[a1];
                entry.y = voxel[a2];
                INSERT_IN_QUEUE( queue, entry );
            }
        }
    }

    DELETE_QUEUE( queue );
}

public  BOOLEAN  should_change_this_one(
    Volume          volume,
    Volume          label_volume,
    int             voxel[],
    Real            min_threshold,
    Real            max_threshold,
    int             label_min_threshold,
    int             label_max_threshold,
    int             desired_label )
{
    int      label;
    Real     value;

    label = get_volume_label_data( label_volume, voxel );

    if( label == desired_label )
        return( FALSE );

    if( label_min_threshold <= label_max_threshold &&
        (label < label_min_threshold || label > label_max_threshold) )
        return( FALSE );

    if( min_threshold > max_threshold )
        return( TRUE );

    GET_VALUE_3D( value, volume, voxel[X], voxel[Y], voxel[Z] );

    return( min_threshold <= value && value <= max_threshold );
}

private   int   Dx4[4] = { 1, 0, -1,  0 };
private   int   Dy4[4] = { 0, 1,  0, -1 };

private   int   Dx8[8] = {  1,  1,  0, -1, -1, -1,  0,  1 };
private   int   Dy8[8] = {  0,  1,  1,  1,  0, -1, -1, -1 };


public  int  get_neighbour_directions(
    Neighbour_types   connectivity,
    int               *dx[],
    int               *dy[] )
{
    int   n_dirs;

    switch( connectivity )
    {
    case  FOUR_NEIGHBOURS:
        *dx = Dx4;
        *dy = Dy4;
        n_dirs = 4;
        break;

    case  EIGHT_NEIGHBOURS:
        *dx = Dx8;
        *dy = Dy8;
        n_dirs = 8;
        break;
    }

    return( n_dirs );
}

private   int   Dx6[6] = { 1, 0, 0, -1,  0,  0 };
private   int   Dy6[6] = { 0, 1, 0,  0, -1,  0 };
private   int   Dz6[6] = { 0, 0, 1,  0,  0, -1 };

private   int   Dx26[26] = { 1, 0, 0, -1,  0,  0 };
private   int   Dy26[26] = { 0, 1, 0,  0, -1,  0 };
private   int   Dz26[26] = { 0, 0, 1,  0,  0, -1 };

private  void  create_3D_neighbours()
{
    int   x, y, z, n;

    n = 0;
    for_inclusive( x, -1, 1 )
    for_inclusive( y, -1, 1 )
    for_inclusive( z, -1, 1 )
    {
        if( x != 0 || y != 0 || z != 0 )
        {
            Dx26[n] = x;
            Dy26[n] = y;
            Dz26[n] = z;
            ++n;
        }
    }

    if( n != 26 )
    {
        HANDLE_INTERNAL_ERROR( "create_3D_neighbours" );
    }
}

public  int  get_3D_neighbour_directions(
    Neighbour_types   connectivity,
    int               *dx[],
    int               *dy[],
    int               *dz[] )
{
    static  BOOLEAN  first = TRUE;
    int              n_dirs;

    if( first )
    {
        first = FALSE;
        create_3D_neighbours();
    }

    switch( connectivity )
    {
    case  FOUR_NEIGHBOURS:
        *dx = Dx6;
        *dy = Dy6;
        *dz = Dz6;
        n_dirs = 6;
        break;

    case  EIGHT_NEIGHBOURS:
        *dx = Dx26;
        *dy = Dy26;
        *dz = Dy26;
        n_dirs = 26;
        break;
    }

    return( n_dirs );
}
