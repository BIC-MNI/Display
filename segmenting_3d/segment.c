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
    int                 max_range[],
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

private  Volume  make_distance_transform(
    Volume    volume,
    int       n_dimensions,
    int       axis,
    int       min_range[],
    int       max_range[],
    Real      min_threshold,
    Real      max_threshold )
{
    int                 i, x, y, z, neigh[MAX_DIMENSIONS];
    int                 iteration;
    Real                value;
    int                 this_dist, dist, min_neighbour, new_value;
    BOOLEAN             changed, first;
    Volume              distance[2];
    int                 which;
    bitlist_3d_struct   to_do[2];

    distance[0] = copy_volume_definition( volume, NC_BYTE, FALSE, 0.0, 255.0 );
    distance[1] = copy_volume_definition( volume, NC_BYTE, FALSE, 0.0, 255.0 );

    create_to_do_list( min_range, max_range, &to_do[0] );
    create_to_do_list( min_range, max_range, &to_do[1] );

    which = 0;

    for_inclusive( x, min_range[X], max_range[X] )
    {
        for_inclusive( y, min_range[Y], max_range[Y] )
        {
            for_inclusive( z, min_range[Z], max_range[Z] )
            {
                SET_VOXEL_3D( distance[which], x, y, z, 0.0 );
                set_to_do_list( min_range, &to_do[which], x, y, z );
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
            for_inclusive( y, min_range[Y], max_range[Y] )
            {
                for_inclusive( z, min_range[Z], max_range[Z] )
                {
                    GET_VOXEL_3D( this_dist, distance[which], x, y, z );
                    if( !must_do_voxel( min_range, max_range, &to_do[which],
                                        x, y, z ) )
                    {
                        SET_VOXEL_3D( distance[1-which], x, y, z, this_dist );
                        continue;
                    }

                    GET_VALUE_3D( value, volume, x, y, z );
                    if( value < min_threshold || value > max_threshold ||
                        this_dist != 0 )
                    {
                        SET_VOXEL_3D( distance[1-which], x, y, z, this_dist );
                        continue;
                    }

                    first = TRUE;
                    min_neighbour = 0;
                    FOR_LOOP_NEIGHBOURS( i, x, y, z,
                                         neigh[X], neigh[Y], neigh[Z] )
                        if( n_dimensions == 2 &&
                            (neigh[axis] < min_range[axis] ||
                             neigh[axis] > max_range[axis]) )
                            continue;

                        if( neigh[X] < min_range[X] ||
                            neigh[X] > max_range[X] ||
                            neigh[Y] < min_range[Y] ||
                            neigh[Y] > max_range[Y] ||
                            neigh[Z] < min_range[Z] ||
                            neigh[Z] > max_range[Z] )
                        {
                            min_neighbour = 0;
                            first = FALSE;
                            break;
                        }
                        else
                        {
                            GET_VALUE_3D( value, volume,
                                          neigh[X], neigh[Y], neigh[Z] );
                            GET_VOXEL_3D( dist, distance[which],
                                          neigh[X], neigh[Y], neigh[Z] );
                            if( dist == 0 && min_threshold <= value &&
                                value <= max_threshold )
                                continue;
                            if( first || dist < min_neighbour )
                            {
                                min_neighbour = dist;
                                first = FALSE;
                                if( dist == 0 )
                                    break;
                            }
                        }
                    }

                    if( !first && this_dist != min_neighbour + 1 )
                    {
                        new_value = min_neighbour + 1;
                        changed = TRUE;
                        set_to_do_list_neighbours( min_range, max_range,
                                                   &to_do[1-which], x, y, z );
                    }
                    else
                        new_value = this_dist;

                    SET_VOXEL_3D( distance[1-which], x, y, z, new_value );
                }
            }
        }

        which = 1 - which;
    }
    while( changed );

    delete_to_do_list( &to_do[0] );
    delete_to_do_list( &to_do[1] );

    delete_volume( distance[1-which] );

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
    Volume             *distance_transform,
    Volume             *cuts,
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

    *cuts = copy_volume_definition( label_volume, NC_BYTE, FALSE, 0.0, 255.0 );

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

                GET_VOXEL_3D( dist, *distance_transform, x, y, z );

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
    Volume             distance_transform,
    Volume             cuts,
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
    BOOLEAN            changed, better, user_set_it;
    Classes            class, new_class, neigh_class, best_class;
    Volume             new_cuts, new_labels;
    int                min_range[MAX_DIMENSIONS], max_range[MAX_DIMENSIONS];
    progress_struct    progress;
    bitlist_3d_struct  next_to_do;

    new_cuts = copy_volume_definition( label_volume,
                                       NC_BYTE, FALSE, 0.0, 255.0 );
    new_labels = copy_volume_definition( label_volume,
                                         NC_BYTE, FALSE, 0.0, 255.0 );

    get_voxel_range( label_volume, n_dimensions, voxel_pos, axis,
                     min_range, max_range );

    create_to_do_list( min_range, max_range, &next_to_do );

    changed = FALSE;

    x_size = max_range[X] - min_range[X] + 1;
    y_size = max_range[Y] - min_range[Y] + 1;
    initialize_progress_report( &progress, FALSE, x_size * y_size,
                                "Expanding Labels" );

    for_inclusive( x, min_range[X], max_range[X] )
    {
        for_inclusive( y, min_range[Y], max_range[Y] )
        {
            for_inclusive( z, min_range[Z], max_range[Z] )
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

                if( must_do_voxel( min_range, max_range, to_do, x, y, z ) &&
                    dist != 0 && !user_set_it )
                {
                    FOR_LOOP_NEIGHBOURS( i, x, y, z, nx, ny, nz )
                        if( nx >= min_range[X] && nx <= max_range[X] &&
                            ny >= min_range[Y] && ny <= max_range[Y] &&
                            nz >= min_range[Z] && nz <= max_range[Z] )
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
                }

                if( best_label != label ||
                    best_cut != cut ||
                    best_class != class )
                {
                    SET_VOXEL_3D( new_labels, x, y, z,
                                  best_label | USER_SET_BIT );
                    changed = TRUE;
                    set_to_do_list_neighbours( min_range, max_range,
                                               &next_to_do, x, y, z );
                }
                else
                {
                    if( !user_set_it && best_label != 0 )
                        best_label |= USER_SET_BIT;
                    SET_VOXEL_3D( new_labels, x, y, z, best_label );
                }

                cut = create_cut_class( best_cut, best_class );
                SET_VOXEL_3D( new_cuts, x, y, z, cut );
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

    delete_to_do_list( to_do );
    *to_do = next_to_do;

    return( changed );
}
