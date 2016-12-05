/**
 * \file input_files.c
 * \brief Function to load graphics or volume files.
 *
 * \copyright
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>
#include <float.h>

/**
 * Get the determinant of the 3x3 transform (i.e. the direction
 * cosines).
 *
 * This is useful, for example, in checking whether or not
 * we have a rigid body transform (the determinant of a rigid
 * transform is either 1.0 or -1.0).

 * \param volume The volume whose determinant we want.
 * \returns The determinant of the 3x3 matrix formed by the direction cosines.
 */
VIO_Real get_volume_transform_determinant( VIO_Volume volume )
{
     VIO_Real m[VIO_N_DIMENSIONS][VIO_N_DIMENSIONS];
     int i;

     for_less( i, 0, VIO_N_DIMENSIONS )
         get_volume_direction_cosine( volume, i, m[i] );

     return (+ m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[2][1] * m[1][2]
             - m[1][0] * m[0][1] * m[2][2] + m[1][0] * m[2][1] * m[0][2]
             + m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[1][1] * m[0][2] );
}

/**
 * Check whether this volume's voxel to world transform specifies a
 * rigid body transform.
 * \param volume The volume whose transform we want to check.
 * \returns TRUE if the transform is a rigid body transform.
 */
VIO_BOOL is_volume_transform_rigid( VIO_Volume volume )
{
    VIO_Real det = get_volume_transform_determinant( volume );
    return (fabs( fabs( det ) - 1.0 ) < 1e-6 );
}

#if GIFTI_FOUND
#include "gifti_io.h"

static VIO_Status
read_gifti_surface( const char *filename, model_struct *model )
{
  gifti_image *gii_ptr = gifti_read_image( filename, 1 );
  if ( gii_ptr == NULL )
  {
    print_error("Unknown error reading GIFTI file.\n");
    return VIO_ERROR;
  }
  /* Check that all of the properties of the GIFTI file are
   * set correctly for a surface.
   */
  else if ( gii_ptr->numDA != 2 ||
            gii_ptr->darray[0]->intent != NIFTI_INTENT_POINTSET ||
            gii_ptr->darray[0]->datatype != NIFTI_TYPE_FLOAT32 ||
            gii_ptr->darray[0]->num_dim != 2 ||
            gii_ptr->darray[0]->dims[1] != 3 ||
            gii_ptr->darray[1]->intent != NIFTI_INTENT_TRIANGLE ||
            gii_ptr->darray[1]->datatype != NIFTI_TYPE_INT32 ||
            gii_ptr->darray[1]->num_dim != 2 ||
            gii_ptr->darray[1]->dims[1] != 3 )
  {
    print_error("This file does not look like a GIFTI surface.\n");
    gifti_free_image( gii_ptr );
    return VIO_ERROR;
  }
  else
  {
    object_struct   *object_ptr = create_object( POLYGONS );
    polygons_struct *polygons_ptr = get_polygons_ptr( object_ptr );
    float           *float32_ptr;
    int32_t         *int32_ptr;
    VIO_Transform   xfm;
    int             rh1, rh2;
    giiCoordSystem  *gii_coord_ptr;
    int             n_indices;
    int             x_ind, y_ind, z_ind;
    int             x_dir, y_dir, z_dir;
    int             i, j;

    /* Interpret the GIFTI_orientation parameter, which controls
     * translation of the GIFTI vertices into Display's RAS space.
     */
    for_less ( i, 0, VIO_N_DIMENSIONS )
    {
      switch ( GIFTI_orientation[i] )
      {
      case 'R': case 'r':
        x_ind = i;
        x_dir = 1;
        break;
      case 'L': case 'l':
        x_ind = i;
        x_dir = -1;
        break;
      case 'A': case 'a':
        y_ind = i;
        y_dir = 1;
        break;
      case 'P': case 'p':
        y_ind = i;
        y_dir = -1;
        break;
      case 'S': case 's':
        z_ind = i;
        z_dir = 1;
        break;
      case 'I': case 'i':
        z_ind = i;
        z_dir = -1;
        break;
      default:
        print_error("ERROR in GIFTI_orientation.\n");
        gifti_free_image( gii_ptr );
        return VIO_ERROR;
      }
    }

    make_identity_transform( &xfm );
    initialize_polygons( polygons_ptr, WHITE, NULL );

    /* See if there is a transform defined for the surface points,
     * and apply it if present.
     */
    if (gii_ptr->darray[0]->coordsys != NULL &&
        (gii_coord_ptr = gii_ptr->darray[0]->coordsys[0]) != NULL)
    {
      /* Information about the "meaning" of the transform is
         located in these two files.
         gii_ptr->darray[0]->coordsys[0]->dataspace,
         gii_ptr->darray[0]->coordsys[0]->xformspace);
         For now we just apply the transform blindly.
      */
      for (i = 0; i < 4; i++)
      {
        for (j = 0; j < 4; j++)
        {
          Transform_elem(xfm, i, j) = gii_coord_ptr->xform[i][j];
        }
      }
    }
    else
    {
      /* This is just an informational message. It could be removed.
       */
      print("No GIFTI spatial transform found.\n");
    }

    /* Calculate right-handedness flag pre-adjustment. */
    rh1 = is_transform_right_handed( &xfm );
    /* Swap the orientations of the transform rows if needed. */
    for ( j = 0; j < 4; j++ )
    {
      Transform_elem( xfm, 0, j ) *= x_dir;
      Transform_elem( xfm, 1, j ) *= y_dir;
      Transform_elem( xfm, 2, j ) *= z_dir;
    }

    /* Calculate right-handedness flag post-adjustment. */
    rh2 = is_transform_right_handed( &xfm );

    /* Copy the vertices from the GIFTI object.
     */
    polygons_ptr->n_points = gii_ptr->darray[0]->dims[0];
    ALLOC( polygons_ptr->points, polygons_ptr->n_points );
    float32_ptr = (float *) gii_ptr->darray[0]->data;
    for ( i = j = 0; i < polygons_ptr->n_points; i++, j += 3 )
    {
      VIO_Real x = float32_ptr[j + x_ind];
      VIO_Real y = float32_ptr[j + y_ind];
      VIO_Real z = float32_ptr[j + z_ind];
      transform_point( &xfm, x, y, z, &x, &y, &z );
      fill_Point( polygons_ptr->points[i], x, y, z );
    }
    /* Now copy the indices and the end indices. We assume that
     * the surface is triangular.
     */
    polygons_ptr->n_items = gii_ptr->darray[1]->dims[0];
    n_indices = polygons_ptr->n_items * 3;
    ALLOC( polygons_ptr->indices, n_indices );
    ALLOC( polygons_ptr->end_indices, polygons_ptr->n_items );
    int32_ptr = (int32_t *) gii_ptr->darray[1]->data;
    for ( i = 0; i < n_indices; i++ )
    {
      polygons_ptr->indices[i] = int32_ptr[i];
    }
    for ( i = 0, j = 3; i < polygons_ptr->n_items; i++, j += 3 )
    {
      polygons_ptr->end_indices[i] = j;
    }
    ALLOC( polygons_ptr->normals, polygons_ptr->n_points );
    compute_polygon_normals( polygons_ptr );
    /* If the handedness changed because of our reorientation,
     * try to reverse the direction of the normals. This seems
     * to be the right thing to do in many, but not all, cases.
     */
    if ( rh1 != rh2 )
    {
      reverse_vectors( polygons_ptr->n_points, polygons_ptr->normals );
    }
    add_object_to_model( model, object_ptr );
    gifti_free_image( gii_ptr );
  }
  return VIO_OK;
}
#endif /* GIFTI_FOUND */

/**
 * Load either a graphical object (MNI .obj format, e.g.) or a volume file
 * (MINC voxel data, e.g.).
 * \param display A pointer to the display_struct of the 3D view window.
 * \param filename The name of the file to open.
 * \param is_label_file A boolean that indicates whether the volume file
 * should be treated as containing anatomical labels for an existing
 * volume.
 * \returns VIO_OK on success.
 */
  VIO_Status  load_graphics_file(
    display_struct   *display,
    VIO_STR           filename,
    VIO_BOOL          is_label_file )
{
  return load_graphics_file_with_colour( display, filename, is_label_file,
                                         WHITE );
}

/**
 * \brief Convert BrainSuite coordinates to world space.
 * 
 * This is needed because BrainSuite files surface and fiber track files 
 * use a "sort-of" voxel coordinate for their xyz values, but they use 
 * NIfTI-1 for volumetric data. We need to transform the points into 
 * our voxel space, then into world space.
 * 
 * \param volume The loaded volume from which we get the transform.
 * \param n_points The number of points to fix.
 * \param points The array of points that need fixing.
 */
static void
fix_brainsuite_points(VIO_Volume volume, int n_points, VIO_Point points[])
{
  VIO_Real steps[VIO_MAX_DIMENSIONS];
  int sizes[VIO_MAX_DIMENSIONS];
  int i, j;

  get_volume_sizes( volume, sizes );
  get_volume_separations( volume, steps );

  for (i = 0; i < n_points; i++)
  {
    VIO_Real voxel[VIO_MAX_DIMENSIONS];
    VIO_Real wx, wy, wz;

    voxel[VIO_X] = Point_x(points[i]);
    voxel[VIO_Y] = Point_y(points[i]);
    voxel[VIO_Z] = Point_z(points[i]);

    for (j = 0; j < VIO_N_DIMENSIONS; j++)
    {
      if (steps[j] < 0)
      {
        voxel[j] = (sizes[j] - voxel[j] / fabs(steps[j]));
      }
      else
      {
        voxel[j] /= steps[j];
      }
    }
    convert_voxel_to_world( volume, voxel, &wx, &wy, &wz );
    fill_Point( points[i], wx, wy, wz );
  }
}

/**
 * Load either a graphical object (MNI .obj format, e.g.) or a volume file
 * (MINC voxel data, e.g.).
 * \param display A pointer to the display_struct of the 3D view window.
 * \param filename The name of the file to open.
 * \param is_label_file A boolean that indicates whether the volume file
 * should be treated as containing anatomical labels for an existing
 * volume.
 * \param preferred_colour Specifies a colour to be applied to the object
 * (if it is a polygons object with a single colour).
 * \returns VIO_OK on success.
 */
VIO_Status
load_graphics_file_with_colour(
    display_struct   *display,
    VIO_STR           filename,
    VIO_BOOL          is_label_file,
    VIO_Colour        preferred_colour)
{
    VIO_Status               status;
    object_struct            *object;
    model_struct             *model;
    int                      n_items, sizes[VIO_MAX_DIMENSIONS];
    VIO_Volume               volume_read_in;
    object_struct            *current_object;
    object_traverse_struct   object_traverse;
    char                     volume_description[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_BOOL                 volume_present;
    display_struct           *slice_window;

    object = create_object( MODEL );

    print( "Input %s\n", filename );

    model = get_model_ptr( object );
    initialize_display_model( model );
    initialize_3D_model_info( model );

    replace_string( &model->filename, create_string(filename) );

    volume_present = FALSE;

    status = VIO_OK;

    if( filename_extension_matches(filename,"mnc") ||
        filename_extension_matches(filename,"mni") ||
        filename_extension_matches(filename,"nil") ||
        filename_extension_matches(filename,"iff") ||
        filename_extension_matches(filename,"mgh") ||
        filename_extension_matches(filename,"mgz") ||
        filename_extension_matches(filename,"nrrd") ||
        filename_extension_matches(filename,"nhdr") ||
        filename_extension_matches(filename,"nii") ||
        filename_extension_matches(filename,"hdr") ||
        filename_extension_matches(filename,"fre") )
    {
        if( !is_label_file )
        {
            status = input_volume_file( filename, &volume_read_in );

            if( status == VIO_OK )
                volume_present = TRUE;
        }
        else
        {
            if( get_n_volumes(display) == 0 )
            {
                print( "No volume to load labels for.\n" );
                status = VIO_ERROR;
            }
            else
            {
                status = input_label_volume_file( display, filename );
                if (Tags_from_label)
                {
                input_tag_objects_label(display,
                  &model->n_objects, &model->objects);
                }
            }
        }
    }
#if GIFTI_FOUND
    else if( filename_extension_matches( filename, "gii" ) )
    {
      status = read_gifti_surface( filename, model );
    }
#endif /* GIFTI_FOUND */
    else if( filename_extension_matches(filename,
                                        get_default_colour_map_suffix()) &&
             get_n_volumes(display) > 0 &&
             get_slice_window( display, &slice_window ) )
    {
        status = load_label_colour_map( slice_window, filename );
    }
    else if( filename_extension_matches(filename,
                    get_default_transform_file_suffix()) )
    {
        transform_current_volume_from_file( display, filename );
        status = VIO_OK;
    }
    else
    {
        if( is_label_file &&
            (filename_extension_matches( filename,
                                         get_default_tag_file_suffix() ) ||
             filename_extension_matches( filename,
                                         get_default_landmark_file_suffix()) ) )
        {
            status = input_tag_label_file( display, filename );
        }
        else
        {
            status = input_objects_any_format( get_volume(display), filename,
                                     display->three_d.default_marker_colour,
                                     display->three_d.default_marker_size,
                                     display->three_d.default_marker_type,
                                     &model->n_objects,
                                     &model->objects); //VF:

            /* Hacks to deal with non-world coordinates in foreign object
             * files.
             */
            if (filename_extension_matches( filename, "dft") ||
                filename_extension_matches( filename, "dfc") ||
                filename_extension_matches( filename, "dfs"))
            {
                VIO_Volume volume = get_volume(display);
                if ( volume != NULL ) 
                {
                  int i;

                  for (i = 0; i < model->n_objects; i++)
                  {
                    int n_points;
                    VIO_Point *points;
                    n_points = get_object_points(model->objects[i], &points);
                    fix_brainsuite_points(volume, n_points, points);
                  }
                }
            }
        }
    }

    if( status == VIO_OK )
    {
        print( "Objects input.\n" );

        initialize_object_traverse( &object_traverse, FALSE, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( !Visibility_on_input )
                set_object_visibility( current_object, FALSE );
        }
    }

    if( status == VIO_OK )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( current_object != object &&
                current_object->object_type == MODEL )
            {
                initialize_3D_model_info( get_model_ptr(current_object) );
            }
            else if( current_object->object_type == POLYGONS )
            {
                polygons_struct   *polygons;

                polygons = get_polygons_ptr( current_object );

                if ( polygons->colour_flag == ONE_COLOUR )
                {
                    polygons->colours[0] = preferred_colour;
                }

                n_items = polygons->n_items;

                if( Polygon_bintree_threshold >= 0 &&
                    n_items > Polygon_bintree_threshold )
                {
                    create_polygons_bintree( polygons,
                              VIO_ROUND( (VIO_Real) n_items * Bintree_size_factor ) );
                }

                if( Compute_neighbours_on_input )
                    check_polygons_neighbours_computed( polygons );
            }
            else if ( current_object->object_type == LINES )
            {
                lines_struct *lines_ptr = get_lines_ptr( current_object );
                if (Lines_bintree_threshold >= 0 &&
                    (lines_ptr->n_items > Lines_bintree_threshold ||
                     lines_ptr->n_points > Lines_bintree_threshold))
                {
                    create_lines_bintree( lines_ptr,
                                          VIO_ROUND( lines_ptr->n_items *
                                                     Bintree_size_factor ) );
                }
            }
            else if( current_object->object_type == QUADMESH )
            {
                quadmesh_struct   *quadmesh;

                quadmesh = get_quadmesh_ptr( current_object );

                if ( quadmesh->colour_flag == ONE_COLOUR )
                {
                    quadmesh->colours[0] = preferred_colour;
                }
            }
        }
    }

    if( status == VIO_OK && model->n_objects > 0 )
    {
        add_object_to_current_model( display, object );

        rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
    }
    else
        delete_object( object );

    if( status == VIO_OK && volume_present )
    {
        get_volume_sizes( volume_read_in, sizes );

        (void) sprintf( volume_description, "%s : X:%d Y:%d Z:%d",
                        filename, sizes[VIO_X], sizes[VIO_Y], sizes[VIO_Z] );
        if (get_volume_n_dimensions(volume_read_in) > VIO_N_DIMENSIONS)
        {
          sprintf(volume_description + strlen(volume_description),
                  " T:%d", sizes[VIO_T]);
          if (get_volume_n_dimensions( volume_read_in ) == VIO_MAX_DIMENSIONS)
          {
            sprintf(volume_description + strlen(volume_description),
                    " V:%d", sizes[VIO_V]);
          }
        }

        if ( !is_volume_transform_rigid( volume_read_in ) )
        {
          char *message = {
            "WARNING: Volume transform is not rigid. This may cause distortion or\n"
            "         other odd effects if this volume is superimposed on another volume.\n" };
          print( message );
        }

        add_slice_window_volume( display, filename, volume_description,
                                 volume_read_in );
    }

    return( status );
}
