#include  <mni.h>

typedef  enum  { DECREASING, SAME, INCREASING } Classes;

#define  USER_SET_BIT    128

private  Volume  make_distance_transform(
    Volume    volume,
    Real      min_threshold,
    Real      max_threshold )
{
    int      x, y, z, nx, ny, nz, sizes[MAX_DIMENSIONS];
    int      dx, dy, dz;
    Real     value, voxel, dist, min_neighbour;
    BOOLEAN  changed;
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
                    voxel = 0.0;
                else
                    voxel = 1.0;

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
                    min_neighbour = 0.0;
                    for_inclusive( dx, -1, 1 )
                    {
                        nx = x + dx;
                        for_inclusive( dy, -1, 1 )
                        {
                            ny = y + dy;
                            for_inclusive( dz, -1, 1 )
                            {
                                nz = z + dz;
                                if( (dx != 0 || dy != 0 || dz != 0) &&
                                    nx >= 0 && nx < sizes[X] &&
                                    ny >= 0 && ny < sizes[Y] &&
                                    nz >= 0 && nz < sizes[Z] )
                                {
                                    GET_VOXEL_3D( dist, distance, nx, ny, nz );
                                    if( dist < min_neighbour )
                                        min_neighbour = dist;
                                }
                            }
                        }
                    }

                    GET_VOXEL_3D( dist, distance, x, y, z );

                    if( dist != min_neighbour + 1.0 )
                        changed = TRUE;

                    SET_VOXEL_3D( new_distance, x, y, z, min_neighbour + 1.0 );
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

public  void  initialize_segmenting_3d(
    Volume    volume,
    Volume    label_volume,
    Real      min_threshold,
    Real      max_threshold,
    Volume    *distance_transform,
    Volume    *cuts )
{
    int      x, y, z, sizes[MAX_DIMENSIONS];

    *distance_transform = make_distance_transform( volume,
                                                   min_threshold,
                                                   max_threshold );

    *cuts = copy_volume_definition( label_volume, NC_BYTE, FALSE, 0.0, 255.0 );

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                SET_VOXEL_3D( *cuts, x, y, z, 0 );
            }
        }
    }
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

public  BOOLEAN  expand_labels_3d(
    Volume    label_volume,
    Volume    distance_transform,
    Volume    cuts )
{
    int      x, y, z, nx, ny, nz, sizes[MAX_DIMENSIONS];
    int      dx, dy, dz;
    int      label, dist, cut;
    int      neigh_dist, neigh_cut;
    int      best_label, neigh_label, best_cut;
    int      new_cut;
    BOOLEAN  changed, better;
    Classes  class, new_class, neigh_class, best_class;
    Volume   new_cuts, new_labels;

    new_cuts = copy_volume_definition( label_volume,
                                       NC_BYTE, FALSE, 0.0, 255.0 );
    new_labels = copy_volume_definition( label_volume,
                                         NC_BYTE, FALSE, 0.0, 255.0 );

    get_volume_sizes( label_volume, sizes );

    changed = FALSE;

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                GET_VOXEL_3D( label, label_volume, x, y, z );
                if( (label & USER_SET_BIT) != 0 )
                    label -= USER_SET_BIT;
                GET_VOXEL_3D( dist, distance_transform, x, y, z );
                GET_VOXEL_3D( cut, cuts, x, y, z );
                cut = get_cut_class( cut, &class );

                best_label = label;
                best_cut = cut;
                best_class = class;

                for_inclusive( dx, -1, 1 )
                {
                    nx = x + dx;
                    for_inclusive( dy, -1, 1 )
                    {
                        ny = y + dy;
                        for_inclusive( dz, -1, 1 )
                        {
                            nz = z + dz;
                            if( (dx != 0 || dy != 0 || dz != 0) &&
                                nx >= 0 && nx < sizes[X] &&
                                ny >= 0 && ny < sizes[Y] &&
                                nz >= 0 && nz < sizes[Z] )
                            {
                                GET_VOXEL_3D( neigh_label, label_volume,
                                              nx, ny, nz );
                                if( (neigh_label & USER_SET_BIT) != 0 )
                                    neigh_label -= USER_SET_BIT;
                                GET_VOXEL_3D( neigh_dist, distance_transform,
                                              nx, ny, nz );
                                GET_VOXEL_3D( neigh_cut, cuts,
                                              nx, ny, nz );
                                neigh_cut = get_cut_class( neigh_cut,
                                                           &neigh_class );
                                if( must_change_cut( cut, dist, neigh_cut,
                                                     neigh_dist,
                                                     &new_cut, &new_class ))
                                {
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
                    }
                }

                if( best_label != label ||
                    best_cut != cut ||
                    best_class != class )
                {
                    changed = TRUE;
                }

                SET_VOXEL_3D( new_labels, x, y, z, best_label | USER_SET_BIT );
                cut = create_cut_class( best_cut, best_class );
                SET_VOXEL_3D( new_cuts, x, y, z, cut );
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
