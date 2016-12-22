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

#include <display.h>
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
    print_error( "Unknown error reading GIFTI file.\n" );
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
    print_error( "This file does not look like a GIFTI surface.\n" );
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
        print_error( "ERROR in GIFTI_orientation.\n" );
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
      print( "No GIFTI spatial transform found.\n" );
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
 * Examine a filename's extension to determine whether it is
 * a volume file, such as MINC or NIfTI-1.
 *
 * \param filename The filename to check.
 * \returns TRUE if the filename appears to be a volume.
 */
static VIO_BOOL is_volume_filename( const VIO_STR filename )
{
  static char *volume_extensions[] = {
    /* standard volume types we support. */
    "hdr", "mnc", "mgh", "mgz", "nii", "nhdr", "nrrd",
    /* obsolete volume types?? */
    "fre", "iff", "mni", "nil",
    NULL
  };

  int i;

  for (i = 0; volume_extensions[i] != NULL; i++)
  {
    if (filename_extension_matches( filename, volume_extensions[i] ) )
      return TRUE;
  }
  return FALSE;
}

static void
format_slice_window_title( VIO_Volume volume,
                           const VIO_STR filename,
                           char title[],
                           int title_max_length )
{
  int sizes[VIO_MAX_DIMENSIONS];
  int n_dimensions = get_volume_n_dimensions( volume );
  int title_cur_length = 0;

  get_volume_sizes( volume, sizes );

  title_cur_length = snprintf( title, title_max_length, "%s : X:%d Y:%d Z:%d",
                               filename,
                               sizes[VIO_X], sizes[VIO_Y], sizes[VIO_Z] );

  if ( n_dimensions > VIO_N_DIMENSIONS)
  {
    title_cur_length += snprintf( title + title_cur_length,
                                  title_max_length - title_cur_length,
                                  " T:%d", sizes[VIO_T] );
  }
  if ( n_dimensions == VIO_MAX_DIMENSIONS )
  {
    title_cur_length += snprintf( title + title_cur_length,
                                  title_max_length - title_cur_length,
                                  " V:%d", sizes[VIO_V] );
  }
}

/**
 * Handle postprocessing of newly loaded polygons.
 *
 * \param object_ptr Pointer to a lines object.
 */
static void
finish_loading_polygons( object_struct *object_ptr,
                         VIO_Colour preferred_colour,
                         int index )
{
  polygons_struct *polygons_ptr = get_polygons_ptr( object_ptr );
  if ( polygons_ptr->colour_flag == ONE_COLOUR )
  {
    if ( preferred_colour == TRANSPARENT )
    {
      if ( polygons_ptr->colours[0] == WHITE )
      {
        polygons_ptr->colours[0] = get_automatic_colour( index );
      }
    }
    else
    {
      polygons_ptr->colours[0] = preferred_colour;
    }
  }
  else
  {
    if ( preferred_colour != TRANSPARENT )
    {
      set_object_colour( object_ptr, preferred_colour );
    }
  }

  if( Polygon_bintree_threshold >= 0 &&
      ( polygons_ptr->n_points > Polygon_bintree_threshold ||
        polygons_ptr->n_items > Polygon_bintree_threshold ) )
  {
    create_polygons_bintree( polygons_ptr, VIO_ROUND( polygons_ptr->n_items *
                                                      Bintree_size_factor ) );
  }

  if( Compute_neighbours_on_input )
  {
    check_polygons_neighbours_computed( polygons_ptr );
  }
}

/**
 * Handle postprocessing of newly loaded polygons.
 *
 * \param object_ptr Pointer to a polygons object.
 * \param preferred_colour The colour to use if the surface is solid white,
 * or TRANSPARENT if we should choose a colour automatically.
 * \param index The ordinal number of this surface.
 */
static void
finish_loading_lines( object_struct *object_ptr )
{
  lines_struct *lines_ptr = get_lines_ptr( object_ptr );
  if (Lines_bintree_threshold >= 0 &&
      ( lines_ptr->n_items > Lines_bintree_threshold ||
        lines_ptr->n_points > Lines_bintree_threshold ) )
  {
    create_lines_bintree( lines_ptr, VIO_ROUND( lines_ptr->n_items *
                                                Bintree_size_factor ) );
  }
}

/**
 * Handle postprocessing of a newly loaded quadmesh.
 *
 * \param object_ptr Pointer to a quadmesh object.
 * \param preferred_colour The colour to use if the surface is solid white,
 * or TRANSPARENT if we should choose a colour automatically.
 * \param index The ordinal number of this surface.
 */
static void
finish_loading_quadmesh( object_struct *object_ptr,
                         VIO_Colour preferred_colour,
                         int index )
{
  quadmesh_struct *quadmesh_ptr = get_quadmesh_ptr( object_ptr );
  if ( quadmesh_ptr->colour_flag == ONE_COLOUR )
  {
    if ( preferred_colour == TRANSPARENT )
    {
      if ( quadmesh_ptr->colours[0] == WHITE )
      {
        quadmesh_ptr->colours[0] = get_automatic_colour( index );
      }
    }
    else
    {
      quadmesh_ptr->colours[0] = preferred_colour;
    }
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
  display_struct *display,
  VIO_STR        filename,
  VIO_BOOL       is_label_file,
  VIO_Colour     preferred_colour )
{
  VIO_Status             status;
  object_struct          *new_object_ptr = create_object( MODEL );
  model_struct           *new_model_ptr;
  VIO_Volume             volume_read_in = NULL;
  object_struct          *current_object;
  object_traverse_struct object_traverse;
  display_struct         *slice_window;

  print( "Input %s\n", filename );

  new_model_ptr = get_model_ptr( new_object_ptr );
  initialize_display_model( new_model_ptr );
  initialize_3D_model_info( new_model_ptr );

  replace_string( &new_model_ptr->filename, create_string( filename ) );

  status = VIO_OK;

  if( is_volume_filename( filename ) )
  {
    if( !is_label_file )
    {
      status = input_volume_file( filename, &volume_read_in );

      if( status != VIO_OK )
        volume_read_in = NULL;
    }
    else
    {
      if( get_n_volumes( display ) == 0 )
      {
        print( "No volume to load labels for.\n" );
        status = VIO_ERROR;
      }
      else
      {
        status = input_label_volume_file( display, filename );
        if ( Tags_from_label )
        {
          input_tag_objects_label( display, &new_model_ptr->n_objects,
                                   &new_model_ptr->objects );
        }
      }
    }
  }
#if GIFTI_FOUND
  else if( filename_extension_matches( filename, "gii" ) )
  {
    status = read_gifti_surface( filename, new_model_ptr );
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
      VIO_Volume volume = get_volume( display );
      status = input_objects_any_format( volume, filename,
                                         display->three_d.default_marker_colour,
                                         display->three_d.default_marker_size,
                                         display->three_d.default_marker_type,
                                         &new_model_ptr->n_objects,
                                         &new_model_ptr->objects);

      /* Hacks to deal with non-world coordinates in foreign object
       * files.
       */
      if (filename_extension_matches( filename, "dft") ||
          filename_extension_matches( filename, "dfc") ||
          filename_extension_matches( filename, "dfs"))
      {
        if ( volume != NULL )
        {
          int i;
          for_less (i, 0, new_model_ptr->n_objects )
          {
            int       n_points;
            VIO_Point *points;
            n_points = get_object_points( new_model_ptr->objects[i], &points );
            fix_brainsuite_points( volume, n_points, points );
          }
        }
      }
    }
  }

  if( status == VIO_OK )
  {
    static int n_surfaces = 0;
    print( "Objects input.\n" );

    if ( !Visibility_on_input )
    {
      initialize_object_traverse( &object_traverse, FALSE, 1, &new_object_ptr );

      while( get_next_object_traverse( &object_traverse, &current_object ) )
      {
        set_object_visibility( current_object, FALSE );
      }
    }

    initialize_object_traverse( &object_traverse, FALSE, 1, &new_object_ptr );
    while( get_next_object_traverse( &object_traverse, &current_object ) )
    {
      if( current_object != new_object_ptr &&
          current_object->object_type == MODEL )
      {
        initialize_3D_model_info( get_model_ptr( current_object ) );
      }
      else if( current_object->object_type == POLYGONS )
      {
        finish_loading_polygons( current_object, preferred_colour,
                                 n_surfaces++ );
      }
      else if ( current_object->object_type == LINES )
      {
        finish_loading_lines( current_object );
      }
      else if( current_object->object_type == QUADMESH )
      {
        finish_loading_quadmesh( current_object, preferred_colour,
                                 n_surfaces++ );
      }
    }
  }

  if( status == VIO_OK && new_model_ptr->n_objects > 0 )
  {
    add_object_to_main_model( display, new_object_ptr, Update_3d_view_on_load );
    rebuild_selected_list( display, get_display_by_type( MARKER_WINDOW ) );
  }
  else
  {
    delete_object( new_object_ptr );
  }

  if( status == VIO_OK && volume_read_in != NULL )
  {
    char title[VIO_EXTREMELY_LARGE_STRING_SIZE];

    format_slice_window_title( volume_read_in, filename, title,
                               sizeof( title ) );

    if ( !is_volume_transform_rigid( volume_read_in ) )
    {
      char *message = {
        "WARNING: Volume transform is not rigid. This may cause distortion or\n"
        "         other odd effects if this volume is superimposed on another volume.\n"
      };
      print( message );
    }

    add_slice_window_volume( display, filename, title, volume_read_in );
  }

  return( status );
}
