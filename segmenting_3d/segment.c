#include  <mni.h>

typedef  enum  { SAME, INCREASING, NUM_CLASSES } Classes;

#define  USER_SET_BIT    128

#define  FACE_NEIGHBOURS

#undef   EDGE_NEIGHBOURS
#define  EDGE_NEIGHBOURS

#undef   CORNER_NEIGHBOURS
#define  CORNER_NEIGHBOURS

private  int  neighbour_deltas[][N_DIMENSIONS] = {

#ifdef  FACE_NEIGHBOURS
      {  1,  0,  0 },
      { -1,  0,  0 },
      {  0,  1,  0 },
      {  0, -1,  0 },
      {  0,  0,  1 },
      {  0,  0, -1 }
#endif

#ifdef  EDGE_NEIGHBOURS
      , {  1,  1,  0 },
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
      {  0, -1, -1 }
#endif

#ifdef  CORNER_NEIGHBOURS
      , {  1,  1,  1 },
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

private  void   create_to_do_list(
    int                 min_range[],
    int                 max_range[],
    bitlist_3d_struct   *to_do )
{
    create_bitlist_3d( max_range[X] - min_range[X] + 1,
                       max_range[Y] - min_range[Y] + 1,
                       max_range[Z] - min_range[Z] + 1, to_do );
}

private  void   clear_to_do_list(
    bitlist_3d_struct   *to_do )
{
    zero_bitlist_3d( to_do );
}

private  void   set_to_do_list(
    int                 min_range[],
    bitlist_3d_struct   *to_do,
    int                 x,
    int                 y,
    int                 z )
{
    set_bitlist_bit_3d( to_do,
                        x - min_range[X],
                        y - min_range[Y],
                        z - min_range[Z], TRUE );
}

private  void   set_to_do_list_neighbours_no_check(
    int                 min_range[],
    bitlist_3d_struct   *to_do,
    int                 x,
    int                 y,
    int                 z )
{
    int   i, nx, ny, nz;

    FOR_LOOP_NEIGHBOURS( i, x, y, z, nx, ny, nz )
        set_to_do_list( min_range, to_do, nx, ny, nz );
    }
}

private  void   set_to_do_list_neighbours(
    int                 min_range[],
    int                 max_range[],
    bitlist_3d_struct   *to_do,
    int                 x,
    int                 y,
    int                 z )
{
    int   i, nx, ny, nz;

    FOR_LOOP_NEIGHBOURS( i, x, y, z, nx, ny, nz )
        if( min_range[X] <= nx && nx <= max_range[X] &&
            min_range[Y] <= ny && ny <= max_range[Y] &&
            min_range[Z] <= nz && nz <= max_range[Z] )
        {
            set_to_do_list( min_range, to_do, nx, ny, nz );
        }
    }
}

private  BOOLEAN   must_do_voxel(
    int                 min_range[],
    bitlist_3d_struct   *to_do,
    int                 x,
    int                 y,
    int                 z )
{
    return( get_bitlist_bit_3d( to_do,
                                x - min_range[X],
                                y - min_range[Y],
                                z - min_range[Z] ) );
}

private  void   delete_to_do_list(
    bitlist_3d_struct   *to_do )
{
    delete_bitlist_3d( to_do );
}

#define  INVALID_DIST   255

private  unsigned char  ***make_distance_transform(
    Volume    volume,
    int       n_dimensions,
    int       axis,
    int       min_range[],
    int       max_range[],
    Real      min_threshold,
    Real      max_threshold )
{
    int                 i, x, y, z, nx, ny, nz, *neigh[MAX_DIMENSIONS];
    int                 iteration;
    Real                value;
    int                 this_dist, dist, new_value;
    BOOLEAN             changed, found;
    BOOLEAN             x_okay, y_okay, away_from_boundary;
    unsigned char       ***distance[2];
    int                 which;
    bitlist_3d_struct   to_do[2];

    neigh[X] = &nx;
    neigh[Y] = &ny;
    neigh[Z] = &nz;

    ALLOC3D( distance[0], max_range[X]+1, max_range[Y]+1, max_range[Z]+1 );
    ALLOC3D( distance[1], max_range[X]+1, max_range[Y]+1, max_range[Z]+1 );

    create_to_do_list( min_range, max_range, &to_do[0] );
    create_to_do_list( min_range, max_range, &to_do[1] );

    which = 0;

    for_inclusive( x, min_range[X], max_range[X] )
    {
        for_inclusive( y, min_range[Y], max_range[Y] )
        {
            for_inclusive( z, min_range[Z], max_range[Z] )
            {
                GET_VALUE_3D( value, volume, x, y, z );
                if( min_threshold <= value && value <= max_threshold )
                {
                    set_to_do_list( min_range, &to_do[which], x, y, z );
                    distance[which][x][y][z] = INVALID_DIST;
                }
                else
                    distance[which][x][y][z] = 0;
            }
        }
    }

    iteration = 0;
    do
    {
        clear_to_do_list( &to_do[1-which] );
        ++iteration;
        print( "Distance Transform Iteration: %d\n", iteration );
        changed = FALSE;
        for_inclusive( x, min_range[X], max_range[X] )
        {
            x_okay = x > min_range[X] && x < max_range[X];
            for_inclusive( y, min_range[Y], max_range[Y] )
            {
                y_okay = x_okay && y > min_range[Y] && y < max_range[Y];
                for_inclusive( z, min_range[Z], max_range[Z] )
                {
                    this_dist = distance[which][x][y][z];
                    if( this_dist != INVALID_DIST ||
                        !must_do_voxel( min_range, &to_do[which], x, y, z ) )
                    {
                        distance[1-which][x][y][z] = this_dist;
                        continue;
                    }

                    away_from_boundary = y_okay &&
                                         z > min_range[Z] && z < max_range[Z];

                    found = FALSE;
                    FOR_LOOP_NEIGHBOURS( i, x, y, z, nx, ny, nz )
                        if( n_dimensions == 2 &&
                            (*(neigh[axis]) < min_range[axis] ||
                             *(neigh[axis]) > max_range[axis]) )
                            continue;

                        if( !away_from_boundary &&
                            (nx < min_range[X] || nx > max_range[X] ||
                             ny < min_range[Y] || ny > max_range[Y] ||
                             nz < min_range[Z] || nz > max_range[Z]) )
                        {
                            dist = INVALID_DIST;
                        }
                        else
                            dist =distance[which][nx][ny][nz];

                        if( dist == iteration-1 )
                        {
                            found = TRUE;
                            break;
                        }
                    }

                    if( found )
                    {
                        new_value = iteration;
                        changed = TRUE;
                        if( away_from_boundary )
                            set_to_do_list_neighbours_no_check( min_range,
                                                    &to_do[1-which], x, y, z );
                        else
                            set_to_do_list_neighbours( min_range, max_range,
                                                  &to_do[1-which], x, y, z );
                    }
                    else
                        new_value = this_dist;

                    distance[1-which][x][y][z] = new_value;
                }
            }
        }

        which = 1 - which;
    }
    while( changed );

    delete_to_do_list( &to_do[0] );
    delete_to_do_list( &to_do[1] );

    FREE3D( distance[1-which] );

    return( distance[which] );
}

private  int  get_cut_class(
    int       cut,
    Classes   *class )
{
    *class = (Classes) (cut % NUM_CLASSES);
    return( cut / NUM_CLASSES );
}

private  int  create_cut_class(
    int       cut,
    Classes   class )
{
    return( NUM_CLASSES * cut + (int) class );
}

private  void  get_voxel_range(
    Volume    volume,
    int       n_dimensions,
    int       voxel_pos,
    int       axis,
    int       min_range[],
    int       max_range[] )
{
    int  c, sizes[MAX_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    for_less( c, 0, get_volume_n_dimensions(volume) )
    {
        min_range[c] = 0;
        max_range[c] = sizes[c]-1;
    }

    if( n_dimensions == 2 )
    {
        min_range[axis] = voxel_pos;
        max_range[axis] = voxel_pos;
    }
}

public  void  initialize_segmenting_3d(
    Volume             volume,
    Volume             label_volume,
    int                n_dimensions,
    int                voxel_pos,
    int                axis,
    Real               min_threshold,
    Real               max_threshold,
    unsigned char      ****distance_transform,
    unsigned char      ****cuts,
    bitlist_3d_struct  *to_do )
{
    int      label, dist, cut, x, y, z;
    int      min_range[MAX_DIMENSIONS], max_range[MAX_DIMENSIONS];

    get_voxel_range( volume, n_dimensions, voxel_pos, axis,
                     min_range, max_range );

    *distance_transform = make_distance_transform( volume, n_dimensions,
                                                   axis, min_range, max_range,
                                                   min_threshold,
                                                   max_threshold );

    ALLOC3D( *cuts, max_range[X]+1, max_range[Y]+1, max_range[Z]+1 );

    create_to_do_list( min_range, max_range, to_do );

    for_inclusive( x, min_range[X], max_range[X] )
    {
        for_inclusive( y, min_range[Y], max_range[Y] )
        {
            for_inclusive( z, min_range[Z], max_range[Z] )
            {
                GET_VOXEL_3D( label, label_volume, x, y, z );
                if( label & USER_SET_BIT )
                {
                    label = 0;
                    SET_VOXEL_3D( label_volume, x, y, z, label );
                }

                dist = (*distance_transform)[x][y][z];

                if( dist == 0 )
                    cut = 0;
                else
                {
                    if( label != 0 )
                    {
                        cut = create_cut_class( dist, INCREASING );
                        set_to_do_list_neighbours( min_range, max_range,
                                                   to_do, x, y, z );
                    }
                    else
                        cut = create_cut_class( 0, INCREASING );
                }

                (*cuts)[x][y][z] = cut;
            }
        }
    }
}

private  void  propagate_neighbour_cut(
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
            *new_cut = MIN( neigh_cut, bound_dist );
            *new_class = SAME;
        }
        else
        {
            *new_cut = neigh_cut;
            *new_class = SAME;
        }
        break;

    case  SAME:
        if( bound_dist >= neigh_bound_dist )
        {
            *new_cut = MIN( neigh_cut, neigh_bound_dist );
            *new_class = SAME;
        }
        else
        {
            *new_cut = neigh_cut;
            *new_class = SAME;
        }
        break;

    default:
        HANDLE_INTERNAL_ERROR( "propagate_cuts" );
    }
}

public  BOOLEAN  expand_labels_3d(
    Volume             label_volume,
    unsigned char      ***distance_transform,
    unsigned char      ***cuts,
    bitlist_3d_struct  *to_do,
    int                n_dimensions,
    int                voxel_pos,
    int                axis )
{
    int                i, x, y, z, nx, ny, nz, x_size, y_size;
    int                label, dist, cut;
    int                neigh_dist, neigh_cut;
    int                best_label, neigh_label, best_cut;
    int                new_cut;
    BOOLEAN            x_okay, y_okay, away_from_boundary;
    BOOLEAN            changed, better, user_set_it;
    Classes            class, new_class, neigh_class, best_class;
    unsigned char      ***new_cuts, ***new_labels;
    int                min_range[MAX_DIMENSIONS], max_range[MAX_DIMENSIONS];
    progress_struct    progress;
    bitlist_3d_struct  next_to_do;

    get_voxel_range( label_volume, n_dimensions, voxel_pos, axis,
                     min_range, max_range );

    ALLOC3D( new_cuts, max_range[X]+1, max_range[Y]+1, max_range[Z]+1 );
    ALLOC3D( new_labels, max_range[X]+1, max_range[Y]+1, max_range[Z]+1 );

    create_to_do_list( min_range, max_range, &next_to_do );

    changed = FALSE;

    x_size = max_range[X] - min_range[X] + 1;
    y_size = max_range[Y] - min_range[Y] + 1;
    initialize_progress_report( &progress, FALSE, x_size * y_size,
                                "Expanding Labels" );

    for_inclusive( x, min_range[X], max_range[X] )
    {
        x_okay = x > min_range[X] && x < max_range[X];
        for_inclusive( y, min_range[Y], max_range[Y] )
        {
            y_okay = x_okay && y > min_range[Y] && y < max_range[Y];
            for_inclusive( z, min_range[Z], max_range[Z] )
            {
                away_from_boundary = y_okay &&
                                     z > min_range[Z] && z < max_range[Z];

                GET_VOXEL_3D( label, label_volume, x, y, z );
                user_set_it = FALSE;
                if( (label & USER_SET_BIT) != 0 )
                    label -= USER_SET_BIT;
                else if( label != 0 )
                    user_set_it = TRUE;
                dist = distance_transform[x][y][z];
                cut = get_cut_class( cuts[x][y][z], &class );

                best_label = label;
                best_cut = cut;
                best_class = class;

                if( dist != 0 && !user_set_it &&
                    must_do_voxel( min_range, to_do, x, y, z ) )
                {
                    FOR_LOOP_NEIGHBOURS( i, x, y, z, nx, ny, nz )
                        if( away_from_boundary ||
                            nx >= min_range[X] && nx <= max_range[X] &&
                            ny >= min_range[Y] && ny <= max_range[Y] &&
                            nz >= min_range[Z] && nz <= max_range[Z] )
                        {
                            neigh_dist = distance_transform[nx][ny][nz];

                            if( neigh_dist == 0 )
                                continue;

                            GET_VOXEL_3D( neigh_label, label_volume,
                                          nx, ny, nz );

                            if( neigh_label == 0 )
                                continue;

                            if( (neigh_label & USER_SET_BIT) != 0 )
                                neigh_label -= USER_SET_BIT;

                            neigh_cut = get_cut_class( cuts[nx][ny][nz],
                                                       &neigh_class );
                            propagate_neighbour_cut( dist,
                                                     neigh_cut,
                                                     neigh_class,
                                                     neigh_dist,
                                                     &new_cut,
                                                     &new_class );

                            if( new_class < best_class )
                                better = TRUE;
                            else if( new_class == best_class &&
                                     (new_cut > best_cut ||
                                      new_cut == best_cut &&
                                      neigh_label < best_label) )
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
                }

                if( best_label != label ||
                    best_cut != cut ||
                    best_class != class )
                {
                    new_labels[x][y][z] = best_label | USER_SET_BIT;
                    changed = TRUE;
                    if( away_from_boundary )
                        set_to_do_list_neighbours_no_check(
                                                   min_range,
                                                   &next_to_do, x, y, z );
                    else
                        set_to_do_list_neighbours( min_range, max_range,
                                                   &next_to_do, x, y, z );
                }
                else
                {
                    if( !user_set_it && best_label != 0 )
                        best_label |= USER_SET_BIT;
                    new_labels[x][y][z] = best_label;
                }

                cut = create_cut_class( best_cut, best_class );
                new_cuts[x][y][z] = cut;
            }
            update_progress_report( &progress, (x - min_range[X]) * y_size +
                                                y - min_range[Y] + 1 );
        }
    }

    terminate_progress_report( &progress );

    if( changed )
    {
        for_inclusive( x, min_range[X], max_range[X] )
        {
            for_inclusive( y, min_range[Y], max_range[Y] )
            {
                for_inclusive( z, min_range[Z], max_range[Z] )
                {
                    label = new_labels[x][y][z];
                    SET_VOXEL_3D( label_volume, x, y, z, label );
                    cuts[x][y][z] = new_cuts[x][y][z];
                }
            }
        }
    }

    FREE3D( new_cuts );
    FREE3D( new_labels );

    delete_to_do_list( to_do );
    *to_do = next_to_do;

    return( changed );
}
