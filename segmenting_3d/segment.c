#include  <mni.h>

typedef  enum  { DECREASING, SAME, INCREASING } Classes;

#define  USER_SET_BIT    128

#define  FACE_NEIGHBOURS
#define  EDGE_NEIGHBOURS
#define  CORNER_NEIGHBOURS

private  int  neighbour_deltas[][N_DIMENSIONS] = {

#ifdef  FACE_NEIGHBOURS
      {  1,  0,  0 },
      { -1,  0,  0 },
      {  0,  1,  0 },
      {  0, -1,  0 },
      {  0,  0,  1 },
      {  0,  0, -1 },
#endif

#ifdef  EDGE_NEIGHBOURS
      {  1,  1,  0 },
      {  1, -1,  0 },
      { -1,  1,  0 },
      { -1, -1,  0 },
      {  1,  0,  1 },
      {  1,  0, -1 },
      { -1,  0,  1 },
      { -1,  0, -1 },
      {  0,  1,  1 },
      {  0,  1, -1 },
      {  0, -1,  1 },
      {  0, -1, -1 },
#endif

#ifdef  CORNER_NEIGHBOURS
      {  1,  1,  1 },
      {  1,  1, -1 },
      {  1, -1,  1 },
      {  1, -1, -1 },
      { -1,  1,  1 },
      { -1,  1, -1 },
      { -1, -1,  1 },
      { -1, -1, -1 }
#endif
                                           };

#define  FOR_LOOP_NEIGHBOURS( i, x, y, z, nx, ny, nz ) \
    for_less( i, 0, SIZEOF_STATIC_ARRAY( neighbour_deltas ) ) \
    { \
        (nx) = (x) + neighbour_deltas[i][X]; \
        (ny) = (y) + neighbour_deltas[i][Y]; \
        (nz) = (z) + neighbour_deltas[i][Z];

private  Volume  make_distance_transform(
    Volume    volume,
    Real      min_threshold,
    Real      max_threshold )
{
    int      i, x, y, z, nx, ny, nz, sizes[MAX_DIMENSIONS];
    Real     value;
    int      voxel, dist, min_neighbour, new_value;
    BOOLEAN  changed, first;
    Volume   distance, new_distance;

    distance = copy_volume_definition( volume, NC_BYTE, FALSE, 0.0, 255.0 );
    new_distance = copy_volume_definition( volume, NC_BYTE, FALSE, 0.0, 255.0 );
    get_volume_sizes( volume, sizes );

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                GET_VALUE_3D( value, volume, x, y, z );

                if( min_threshold <= value && value <= max_threshold )
                    voxel = 1.0;
                else
                    voxel = 0.0;

                SET_VOXEL_3D( distance, x, y, z, voxel );
            }
        }
    }

    do
    {
        changed = FALSE;
        for_less( x, 0, sizes[X] )
        {
            for_less( y, 0, sizes[Y] )
            {
                for_less( z, 0, sizes[Z] )
                {
                    min_neighbour = 0;
                    first = TRUE;
                    FOR_LOOP_NEIGHBOURS( i, x, y, z, nx, ny, nz )
#ifdef TWO_D
                        if( nz < 0 || nz >= sizes[Z] )
                            continue;
#endif

                        if( nx < 0 || nx >= sizes[X] ||
                            ny < 0 || ny >= sizes[Y] ||
                            nz < 0 || nz >= sizes[Z] )
                        {
                            min_neighbour = 0;
                            first = FALSE;
                        }
                        else
                        {
                            GET_VOXEL_3D( dist, distance, nx, ny, nz );
                            if( first || dist < min_neighbour )
                            {
                                min_neighbour = dist;
                                first = FALSE;
                            }
                        }
                    }

                    GET_VOXEL_3D( dist, distance, x, y, z );

                    if( dist != 0 && min_neighbour != 0 &&
                        dist != min_neighbour + 1 )
                    {
                        new_value = min_neighbour + 1;
                        changed = TRUE;
                    }
                    else
                        new_value = dist;

                    SET_VOXEL_3D( new_distance, x, y, z, new_value );
                }
            }
        }

        if( changed )
        {
            for_less( x, 0, sizes[X] )
            {
                for_less( y, 0, sizes[Y] )
                {
                    for_less( z, 0, sizes[Z] )
                    {
                        GET_VOXEL_3D( dist, new_distance, x, y, z );
                        SET_VOXEL_3D( distance, x, y, z, dist );
                    }
                }
            }
        }
    }
    while( changed );

    return( distance );
}

private  int  get_cut_class(
    int       cut,
    Classes   *class )
{
    *class = (Classes) (cut % 3);
    return( cut / 3 );
}

private  int  create_cut_class(
    int       cut,
    Classes   class )
{
    return( 3 * cut + (int) class );
}

public  void  initialize_segmenting_3d(
    Volume    volume,
    Volume    label_volume,
    Real      min_threshold,
    Real      max_threshold,
    Volume    *distance_transform,
    Volume    *cuts )
{
    int      label, dist, cut, x, y, z, sizes[MAX_DIMENSIONS];

    *distance_transform = make_distance_transform( volume,
                                                   min_threshold,
                                                   max_threshold );

    *cuts = copy_volume_definition( label_volume, NC_BYTE, FALSE, 0.0, 255.0 );

    get_volume_sizes( volume, sizes );

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                GET_VOXEL_3D( label, label_volume, x, y, z );
                if( label & USER_SET_BIT )
                {
                    label -= USER_SET_BIT;
                    SET_VOXEL_3D( label_volume, x, y, z, label );
                }

                GET_VOXEL_3D( dist, *distance_transform, x, y, z );

                if( dist == 0 )
                    cut = 0;
                else
                {
                    if( label != 0 )
                        cut = create_cut_class( dist, INCREASING );
                    else
                        cut = create_cut_class( 0, INCREASING );
                }

                SET_VOXEL_3D( *cuts, x, y, z, cut );
            }
        }
    }
}

private  void  propagate_neighbour_cut(
    int      cut,
    Classes  class,
    int      bound_dist,
    int      neigh_cut,
    Classes  neigh_class,
    int      neigh_bound_dist,
    int      *new_cut,
    Classes  *new_class )
{
    switch( neigh_class )
    {
    case  INCREASING:
        if( bound_dist > neigh_bound_dist )
        {
            *new_cut = bound_dist;
            *new_class = INCREASING;
        }
        else if( bound_dist == neigh_bound_dist )
        {
            *new_cut = bound_dist;
            *new_class = SAME;
        }
        else
        {
            *new_cut = neigh_bound_dist;
            *new_class = DECREASING;
        }
        break;

    case  SAME:
        if( bound_dist >= neigh_cut )
        {
            *new_cut = neigh_cut;
            *new_class = SAME;
        }
        else
        {
            *new_cut = neigh_cut;
            *new_class = DECREASING;
        }
        break;

    case DECREASING:
        if( bound_dist >= neigh_bound_dist )
        {
            *new_cut = neigh_bound_dist;
            *new_class = SAME;
        }
        else
        {
            *new_cut = neigh_cut;
            *new_class = DECREASING;
        }
    }
}

public  BOOLEAN  expand_labels_3d(
    Volume    label_volume,
    Volume    distance_transform,
    Volume    cuts )
{
    int      i, x, y, z, nx, ny, nz, sizes[MAX_DIMENSIONS];
    int      label, dist, cut;
    int      neigh_dist, neigh_cut;
    int      best_label, neigh_label, best_cut;
    int      new_cut;
    BOOLEAN  changed, better, user_set_it;
    Classes  class, new_class, neigh_class, best_class;
    Volume   new_cuts, new_labels;
    progress_struct  progress;

    new_cuts = copy_volume_definition( label_volume,
                                       NC_BYTE, FALSE, 0.0, 255.0 );
    new_labels = copy_volume_definition( label_volume,
                                         NC_BYTE, FALSE, 0.0, 255.0 );

    get_volume_sizes( label_volume, sizes );

    changed = FALSE;

    initialize_progress_report( &progress, FALSE, sizes[X] * sizes[Y],
                                "Expanding Labels" );

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                GET_VOXEL_3D( label, label_volume, x, y, z );
                user_set_it = FALSE;
                if( (label & USER_SET_BIT) != 0 )
                    label -= USER_SET_BIT;
                else if( label != 0 )
                    user_set_it = TRUE;
                GET_VOXEL_3D( dist, distance_transform, x, y, z );
                GET_VOXEL_3D( cut, cuts, x, y, z );
                cut = get_cut_class( cut, &class );

                best_label = label;
                best_cut = cut;
                best_class = class;

                if( dist == 0 || user_set_it )
                {
                    SET_VOXEL_3D( new_labels, x, y, z, label );
                    cut = create_cut_class( cut, class );
                    SET_VOXEL_3D( new_cuts, x, y, z, cut );
                }
                else
                {
                    FOR_LOOP_NEIGHBOURS( i, x, y, z, nx, ny, nz )
                        if( nx >= 0 && nx < sizes[X] &&
                            ny >= 0 && ny < sizes[Y] &&
                            nz >= 0 && nz < sizes[Z] )
                        {
                            GET_VOXEL_3D( neigh_dist,
                                          distance_transform,
                                          nx, ny, nz );

                            GET_VOXEL_3D( neigh_label, label_volume,
                                          nx, ny, nz );
                            if( (neigh_label & USER_SET_BIT) != 0 )
                                neigh_label -= USER_SET_BIT;

                            if( neigh_dist == 0 || neigh_label == 0 )
                                continue;

                            GET_VOXEL_3D( neigh_cut, cuts,
                                          nx, ny, nz );
                            neigh_cut = get_cut_class( neigh_cut,
                                                       &neigh_class );
                            propagate_neighbour_cut( cut, class, dist,
                                                     neigh_cut,
                                                     neigh_class,
                                                     neigh_dist,
                                                     &new_cut,
                                                     &new_class );

                            if( new_class < best_class )
                                better = TRUE;
                            else if( new_class == best_class &&
                                     new_cut > best_cut )
                                better = TRUE;
                            else if( new_class == best_class &&
                                     new_cut == best_cut &&
                                     neigh_label < best_label )
                                better = TRUE;
                            else
                                better = FALSE;

                            if( better )
                            {
                                best_label = neigh_label;
                                best_cut = new_cut;
                                best_class = new_class;
                            }
                        }
                    }

                    if( best_label != label ||
                        best_cut != cut ||
                        best_class != class )
                    {
                        changed = TRUE;
                    }

                    SET_VOXEL_3D( new_labels, x, y, z,
                                  best_label | USER_SET_BIT );
                    cut = create_cut_class( best_cut, best_class );
                    SET_VOXEL_3D( new_cuts, x, y, z, cut );
                }
            }
            update_progress_report( &progress, x * sizes[Y] + y + 1 );
        }
    }

    terminate_progress_report( &progress );

    if( changed )
    {
        for_less( x, 0, sizes[X] )
        {
            for_less( y, 0, sizes[Y] )
            {
                for_less( z, 0, sizes[Z] )
                {
                    GET_VOXEL_3D( label, new_labels, x, y, z );
                    SET_VOXEL_3D( label_volume, x, y, z, label );
                    GET_VOXEL_3D( cut, new_cuts, x, y, z );
                    SET_VOXEL_3D( cuts, x, y, z, cut );
                }
            }
        }
    }

    delete_volume( new_cuts );
    delete_volume( new_labels );

    return( changed );
}
