#include  <def_objects.h>

#define  MAX_TEMP_STORAGE  100

public  void  scan_polygons_to_voxels( polygons, volume, label, max_distance )
    polygons_struct     *polygons;
    volume_struct       *volume;
    int                 label;
    Real                max_distance;
{
    int        i, poly, size;
    Point      vertices[MAX_TEMP_STORAGE];
    void       recursive_polygon_scan();

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );
        if( size > MAX_TEMP_STORAGE )
            size = MAX_TEMP_STORAGE;

        for_less( i, 0, size )
        {
            vertices[i] = polygons->points[
              polygons->indices[POINT_INDEX(polygons->end_indices,poly,i)]];
        }

        if( size == 3 || size == 4 )
            recursive_polygon_scan( size, vertices, volume, label,
                                    max_distance );
    }
}

private  void  recursive_polygon_scan( size, vertices, volume, label,
                                       max_distance )
    int                 size;
    Point               vertices[];
    volume_struct       *volume;
    int                 label;
    Real                max_distance;
{
    Point            midpoints[4], min_point, max_point, centre;
    Point            sub_points[4];
    int              edge;
    void             set_voxel_label_flag();
    void             get_points_centroid();
    void             get_range_points();
    void             convert_point_to_voxel();

    get_range_points( size, vertices, &min_point, &max_point );

    if( Point_x(max_point)-Point_x(min_point) < max_distance &&
        Point_y(max_point)-Point_y(min_point) < max_distance &&
        Point_z(max_point)-Point_z(min_point) < max_distance )
    {
        get_points_centroid( size, vertices, &centre );
        convert_point_to_voxel( volume, Point_x(centre), Point_y(centre),
                                Point_z(centre), 
                                &Point_x(centre), &Point_y(centre), 
                                &Point_z(centre) );

        if( voxel_is_within_volume( volume, Point_x(centre),
                                    Point_y(centre), Point_z(centre) ) )
        {
            set_voxel_label_flag( volume,
                                  ROUND( Point_x(centre) ),
                                  ROUND( Point_y(centre) ),
                                  ROUND( Point_z(centre) ), TRUE );
        }
        return;
    }

    for_less( edge, 0, size )
    {
        INTERPOLATE_POINTS( midpoints[edge], vertices[edge],
                            vertices[(edge+1)%size], 0.5 );
    }

    if( size == 3 )
    {
        sub_points[0] = vertices[0];
        sub_points[1] = midpoints[0];
        sub_points[2] = midpoints[2];
        recursive_polygon_scan( size, sub_points, volume, label, max_distance );

        sub_points[0] = midpoints[0];
        sub_points[1] = vertices[1];
        sub_points[2] = midpoints[1];
        recursive_polygon_scan( size, sub_points, volume, label, max_distance );

        sub_points[0] = midpoints[2];
        sub_points[1] = midpoints[1];
        sub_points[2] = vertices[2];
        recursive_polygon_scan( size, sub_points, volume, label, max_distance );

        sub_points[0] = midpoints[0];
        sub_points[1] = midpoints[1];
        sub_points[2] = midpoints[2];
        recursive_polygon_scan( size, sub_points, volume, label, max_distance );
    }
    else
    {
        get_points_centroid( size, vertices, &centre );

        sub_points[0] = vertices[0];
        sub_points[1] = midpoints[0];
        sub_points[2] = centre;
        sub_points[3] = midpoints[3];
        recursive_polygon_scan( size, sub_points, volume, label, max_distance );

        sub_points[0] = midpoints[0];
        sub_points[1] = vertices[1];
        sub_points[2] = midpoints[1];
        sub_points[3] = centre;
        recursive_polygon_scan( size, sub_points, volume, label, max_distance );

        sub_points[0] = midpoints[3];
        sub_points[1] = centre;
        sub_points[2] = midpoints[2];
        sub_points[3] = vertices[3];
        recursive_polygon_scan( size, sub_points, volume, label, max_distance );

        sub_points[0] = centre;
        sub_points[1] = midpoints[1];
        sub_points[2] = vertices[2];
        sub_points[3] = midpoints[2];
        recursive_polygon_scan( size, sub_points, volume, label, max_distance );
    }
}
