/**
 * \file poly_formats.c
 * \brief Functions to write polygonal objects in alternate formats.
 *
 * Note that these functions have been intentionally kept simple - we
 * want to write files that can be readily imported into tools such as
 * Blender, but we do _not_ care about handling every conceivable
 * case.
 *
 * One important omission is standardizing the coordinate space where
 * possible. It is not clear what the standard coordinate space is for
 * most of these formats. X3D probably defines something along these
 * lines, but I don't implement it here (yet).
 */

#include "config.h"
#include <display.h>

#if GIFTI_FOUND
#include <time.h>
#include "gifti_io.h"
#endif /* GIFTI_FOUND */
/**
 * Write a Wavefront .obj file in ASCII format, given a memory representation
 * of an MNI polygonal surface.
 *
 * This function will write a SINGLE polygonal surface object, with
 * no colour information. These files do not support vertex or face colouring.
 *
 * References:
 * https://en.wikipedia.org/wiki/Wavefront_.obj_file
 *
 * \param filename The name of the file to create.
 * \param object_ptr A pointer to a single polygonal object.
 * \returns VIO_OK if the operation succeeded.
 */
VIO_Status
output_wavefront_obj(VIO_STR filename, object_struct *object_ptr)
{
    polygons_struct *polygons_ptr = get_polygons_ptr( object_ptr );
    FILE *fp = fopen(filename, "w");
    int i = 0, j = 0;

    if ( fp == NULL || polygons_ptr == NULL )
    {
        return VIO_ERROR;
    }

    fprintf( fp, "# Created by %s %s\n", PROJECT_NAME, PROJECT_VERSION );
    for (i = 0; i < polygons_ptr->n_points; i++)
    {
        fprintf( fp, "v %f %f %f\n",
                 Point_x(polygons_ptr->points[i]),
                 Point_y(polygons_ptr->points[i]),
                 Point_z(polygons_ptr->points[i]) );
    }
    if ( polygons_ptr->normals != NULL )
    {
        for (i = 0; i < polygons_ptr->n_points; i++)
        {
            fprintf( fp, "vn %f %f %f\n",
                     Vector_x(polygons_ptr->normals[i]),
                     Vector_y(polygons_ptr->normals[i]),
                     Vector_z(polygons_ptr->normals[i]) );
        }
    }

    for (j = i = 0; i < polygons_ptr->n_items; i++)
    {
        int k = polygons_ptr->end_indices[i];
        fprintf( fp, "f " );
        for ( ; j < k; j++)
        {
            int n = polygons_ptr->indices[j] + 1;
            fprintf( fp, "%d//%d", n, n );
            if (j < k - 1)
                fprintf( fp, " " );
        }
        fprintf( fp, "\n");
    }
    fclose( fp );
    return VIO_OK;
}

/**
 * Write a Stanford .ply file in ASCII format, given a memory representation
 * of an MNI polygonal surface.
 *
 * This function will write a SINGLE polygonal surface object, with either
 * a no colour information, or with per-vertex colours. It isn't trivial for
 * the Stanford format to represent a per-face colour.
 *
 * References:
 * https://en.wikipedia.org/wiki/PLY_(file_format)
 * http://paulbourke.net/dataformats/ply/
 *
 * \param filename The name of the file to create.
 * \param object_ptr A pointer to a single polygonal object.
 * \returns VIO_OK if the operation succeeded.
 */
VIO_Status
output_stanford_ply(VIO_STR filename, object_struct *object_ptr)
{
    polygons_struct *polygons_ptr = get_polygons_ptr( object_ptr );
    FILE *fp = fopen(filename, "w");
    int i = 0, j = 0;

    if ( fp == NULL || polygons_ptr == NULL )
    {
        return VIO_ERROR;
    }

    fprintf( fp, "ply\n");
    fprintf( fp, "format ascii 1.0\n");
    fprintf( fp, "comment Created by %s %s\n", PROJECT_NAME, PROJECT_VERSION );
    fprintf( fp, "element vertex %d\n", polygons_ptr->n_points );
    fprintf( fp, "property float x\n" );
    fprintf( fp, "property float y\n" );
    fprintf( fp, "property float z\n" );
    if (polygons_ptr->colour_flag == PER_VERTEX_COLOURS)
    {
      fprintf( fp, "property uchar red\n" );
      fprintf( fp, "property uchar green\n" );
      fprintf( fp, "property uchar blue\n" );
    }
    fprintf( fp, "element face %d\n", polygons_ptr->n_items );
    fprintf( fp, "property list uchar int vertex_index\n" );
    fprintf( fp, "end_header\n" );
    if (polygons_ptr->colour_flag == PER_VERTEX_COLOURS)
    {
        for (i = 0; i < polygons_ptr->n_points; i++)
        {
            fprintf(fp, "%f %f %f %d %d %d\n",
                    Point_x(polygons_ptr->points[i]),
                    Point_y(polygons_ptr->points[i]),
                    Point_z(polygons_ptr->points[i]),
                    get_Colour_r(polygons_ptr->colours[i]),
                    get_Colour_g(polygons_ptr->colours[i]),
                    get_Colour_b(polygons_ptr->colours[i]));
        }
    }
    else
    {
        for (i = 0; i < polygons_ptr->n_points; i++)
        {
            fprintf(fp, "%f %f %f\n",
                    Point_x(polygons_ptr->points[i]),
                    Point_y(polygons_ptr->points[i]),
                    Point_z(polygons_ptr->points[i]));
        }
    }


    for (j = i = 0; i < polygons_ptr->n_items; i++)
    {
        int k = polygons_ptr->end_indices[i];
        fprintf( fp, "%d ", k - j );
        for ( ; j < k; j++)
        {
            fprintf( fp, "%d", polygons_ptr->indices[j] );
            if (j < k - 1)
                fprintf( fp, " " );
        }
        fprintf( fp, "\n");
    }
    fclose( fp );
    return VIO_OK;
}

/**
 * Write an X3D format file, given a memory representation
 * of an MNI polygonal surface.
 *
 * As with most XML formats, reading the actual specification makes my
 * head hurt. I relied on reverse-engineering to get this compatible
 * with Blender's import function.
 *
 * References:
 * https://en.wikipedia.org/wiki/X3D
 *
 * \param filename The name of the file to create.
 * \param object_ptr A pointer to a single polygonal object.
 * \returns VIO_OK if the operation succeeded.
 */
VIO_Status
output_x3d(VIO_STR filename, object_struct *object_ptr)
{
    polygons_struct *polygons_ptr = get_polygons_ptr( object_ptr );
    FILE *fp = fopen( filename, "w" );
    int i = 0, j = 0;

    if ( fp == NULL || polygons_ptr == NULL )
    {
        return VIO_ERROR;
    }

    fprintf( fp, "<?xml version='1.0' encoding='UTF-8'?>\n" );
    fprintf( fp, "<!DOCTYPE X3D PUBLIC 'ISO//Web3D//DTD X3D 3.0//EN' 'http://www.web3d.org/specifications/x3d-3.0.dtd'>\n");
    fprintf( fp, "<!-- Created by %s %s -->\n", PROJECT_NAME, PROJECT_VERSION );
    fprintf( fp, "<X3D><Scene><Transform><Shape>\n" );

    if (polygons_ptr->colour_flag == PER_VERTEX_COLOURS)
    {
        fprintf( fp, "<IndexedFaceSet colorPerVertex='true' ");
    }
    else
    {
        fprintf( fp, "<IndexedFaceSet colorPerVertex='false' ");
    }
    fprintf( fp, "coordIndex='");
    for (j = i = 0; i < polygons_ptr->n_items; i++)
    {
        int k = polygons_ptr->end_indices[i];
        for ( ; j < k; j++)
        {
            fprintf( fp, "%d ", polygons_ptr->indices[j] );
        }
        fprintf( fp, "-1");
        if (i < polygons_ptr->n_items - 1)
            fprintf( fp, " " );
    }
    if (polygons_ptr->colour_flag == PER_VERTEX_COLOURS)
    {
        fprintf( fp, "' colorIndex='" );
        for ( i = 0; i < polygons_ptr->n_points; i++ )
        {
            fprintf( fp, "%d", i );
            if (i < polygons_ptr->n_points - 1)
                fprintf( fp, " " );
        }
    }
    fprintf( fp, "'>\n" );

    fprintf( fp, "<Coordinate point='" );
    for (i = 0; i < polygons_ptr->n_points; i++)
    {
        fprintf(fp, "%f %f %f",
                Point_x(polygons_ptr->points[i]),
                Point_y(polygons_ptr->points[i]),
                Point_z(polygons_ptr->points[i]));
        if ( i < polygons_ptr->n_points - 1 )
            fprintf( fp, " " );
    }
    fprintf( fp, "'/>\n");

    if (polygons_ptr->colour_flag == PER_VERTEX_COLOURS )
    {
        fprintf( fp, "<Color color='" );
        for (i = 0; i < polygons_ptr->n_points; i++)
        {
            fprintf(fp, "%f %f %f",
                    get_Colour_r_0_1(polygons_ptr->colours[i]),
                    get_Colour_g_0_1(polygons_ptr->colours[i]),
                    get_Colour_b_0_1(polygons_ptr->colours[i]) );

            if ( i < polygons_ptr->n_points - 1 )
              fprintf( fp, " " );
        }
        fprintf( fp, "'/>");
    }
    fprintf( fp, "</IndexedFaceSet>\n");
    fprintf( fp, "</Shape></Transform></Scene></X3D>\n");
    fclose( fp );
    return VIO_OK;
}

#define INDICES_PER_TRIANGLE 3
#define NUM_GIFTI_DARRAYS 2

/**
 * Check that this is in fact a triangular mesh. Give up and
 * indicate failure if not.
 *
 * TODO: Move this to bicpl?
 *
 * \param polygons_ptr A pointer to a polygons_struct to check.
 * \returns TRUE if the polygons_struct is a triangular mesh.
 */
static VIO_BOOL
is_triangular_polygon( polygons_struct *polygons_ptr )
{
    int i;
    int n = 0;

    for_less ( i, 0, polygons_ptr->n_items )
    {
        if ( polygons_ptr->end_indices[i] - n != INDICES_PER_TRIANGLE )
        {
            return FALSE;
        }
        n = polygons_ptr->end_indices[i];
    }
    return TRUE;
}

/**
 * Write a GIFTI format file for a surface. The GIfTI format allows for
 * the creation of a wide range of possible structures, but we are only
 * interested in the "Surface" file, which represents a triangular mesh.
 * This type of file does not represent colours or vertex values, although
 * other GIFTI file types can separately represent those things.
 *
 * \param filename The name of the file to create.
 * \param object_ptr A pointer to a single polygonal object.
 * \returns VIO_OK if the operation succeeded.
 */
VIO_Status
output_gifti( VIO_STR filename, object_struct *object_ptr )
{
#if GIFTI_FOUND
    polygons_struct *polygons_ptr = get_polygons_ptr( object_ptr );
    gifti_image *gii_ptr;
    int i;
    float *point_data;
    int *index_data;
    time_t now;
    char time_str[VIO_EXTREMELY_LARGE_STRING_SIZE];
    int dalist[1];

    if ( !is_triangular_polygon( polygons_ptr ) )
      return VIO_ERROR;

    /* Create the skeleton of the GIFTI image. This does not fill in any
     * of the details. I prefer to do that myself given the current
     * version of the API.
     */
    gii_ptr = gifti_create_image( 0, 0, 0, 0, NULL, 0 );
    if ( gii_ptr == NULL )
    {
        return VIO_ERROR;
    }

    /* Add some potentially useful metadata to the file.
     */
    if ( gifti_add_to_meta( &gii_ptr->meta, "MNI-Display-Version",
                            PACKAGE_VERSION, 0 ) )
    {
        return VIO_ERROR;
    }

    now = time( NULL );
    strftime( time_str, sizeof(time_str) - 1, "%F %T", localtime( &now ) );
    if ( gifti_add_to_meta( &gii_ptr->meta, "Date", time_str, 0 ) )
    {
        return VIO_ERROR;
    }

    /* Now start setting up the data arrays that will represent the
     * actual surface data. The surface file format requires two
     * data arrays, one for the points and another for the triangle
     * indices. I think they have to be in the right order.
     */
    if ( gifti_add_empty_darray( gii_ptr, NUM_GIFTI_DARRAYS ) )
    {
        return VIO_ERROR;
    }

    for_less ( i, 0, NUM_GIFTI_DARRAYS )
    {
        char *intent_str;
        char *type_str;

        dalist[0] = i;          /* To selectively set attributes. */

        gifti_set_DA_defaults( gii_ptr->darray[i] );

        if ( i == 0 )
        {
            /* Fill in the data array information for the point set.
             * We have to create a 2-dimensional array containing all
             * points. The first dimension gives the number of points,
             * the second gives the number of dimensions per point (e.g. 3).
             */
            intent_str = gifti_intent_to_string( NIFTI_INTENT_POINTSET );
            type_str = gifti_datatype2str( NIFTI_TYPE_FLOAT32 );
            gii_ptr->darray[i]->num_dim = 2;
            gii_ptr->darray[i]->dims[0] = polygons_ptr->n_points;
            gii_ptr->darray[i]->dims[1] = VIO_N_DIMENSIONS;
        }
        else
        {
            /* Fill in the data array information for the indices.
             * We have to create a 2-dimensional array containing all
             * triangles. The first dimension gives the number of triangles,
             * the second gives the number of indices per triangle (e.g. 3).
             */
            intent_str = gifti_intent_to_string( NIFTI_INTENT_TRIANGLE );
            type_str = gifti_datatype2str( NIFTI_TYPE_INT32 );
            gii_ptr->darray[i]->num_dim = 2;
            gii_ptr->darray[i]->dims[0] = polygons_ptr->n_items;
            gii_ptr->darray[i]->dims[1] = INDICES_PER_TRIANGLE;
        }
        gifti_set_atr_in_DAs( gii_ptr, "Intent", intent_str, dalist, 1 );
        gifti_set_atr_in_DAs( gii_ptr, "DataType", type_str, dalist, 1 );
    }

    /* Now we need to update the number of bytes per item in the
     * structure.
     */
    if ( gifti_update_nbyper( gii_ptr ) )
    {
        return VIO_ERROR;
    }

    /* And then update the number of values in the structure.
     */
    for_less( i, 0, NUM_GIFTI_DARRAYS )
      gii_ptr->darray[i]->nvals = gifti_darray_nvals( gii_ptr->darray[i] );

    /* Now allocate the data we will need to store the actual data points.
     */
    if ( gifti_alloc_DA_data( gii_ptr, NULL, 0 ) )
    {
        return VIO_ERROR;
    }

    /* Copy the points to the first data array.
     */
    point_data = (float *) gii_ptr->darray[0]->data;
    for_less( i, 0, polygons_ptr->n_points )
    {
        int j = i * VIO_N_DIMENSIONS;
        point_data[j + 0] = Point_x( polygons_ptr->points[i] );
        point_data[j + 1] = Point_y( polygons_ptr->points[i] );
        point_data[j + 2] = Point_z( polygons_ptr->points[i] );
    }

    /* Copy the indices to the second data array.
     */
    index_data = (int *) gii_ptr->darray[1]->data;
    for_less( i, 0, polygons_ptr->n_items * INDICES_PER_TRIANGLE )
    {
        index_data[i] = polygons_ptr->indices[i];
    }

    /* Actually write the file.
     */
    gifti_write_image( gii_ptr, filename, 1 );

    /* Finally free the memory we allocated.
     */
    gifti_free_image( gii_ptr );

    return VIO_OK;
#else

    print("This program was not compiled with GIFTI support.\n");
    return VIO_ERROR;

#endif /* GIFTI_FOUND */
}
