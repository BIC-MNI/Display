/**
 * \file slice_window/label_tags.c
 * \brief Automatically generate tags from labels.
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

/**
 * Verify that the current label counts match up with reality.
 *
 * \param slice_window A pointer to the slice view window.
 * \param volume_index The index of the current volume.
 */
void
check_label_counts( display_struct *slice_window, int volume_index )
{
  int n_labels = get_num_labels( slice_window, volume_index );
  int i;
  int x, y, z;
  int sizes[VIO_MAX_DIMENSIONS];
  int *new_counts;
  VIO_Volume volume = get_nth_label_volume( slice_window, volume_index );

  get_volume_sizes( volume, sizes );

  ALLOC( new_counts, n_labels );
  for (i = 0; i < n_labels; i++)
    new_counts[i] = 0;

  for (x = 0; x < sizes[VIO_X]; x++)
    for (y = 0; y < sizes[VIO_Y]; y++)
      for (z = 0; z < sizes[VIO_Z]; z++)
      {
        int label = get_3D_volume_label_data( volume, x, y, z );
        if (label != 0)
          new_counts[label]++;
      }
        
  for ( i = 0; i < n_labels; i++ )
  {
    int n = slice_window->slice.volumes[volume_index].label_count[i];
    int m = new_counts[i];
    if (n || m)
    {
      printf("%4d: %6d %6d %s\n", i, n, m, n!=m ? "***" : "");
    }
  }
  FREE(new_counts);
}

/**
 * Helper function for find_nearest_label. 
 *
 * \param volume The volume to search.
 * \param i The index of the x-coordinate for this plane.
 * \param j The index of the y-coordinate for this plane.
 * \param k The index of the z-coordinate for this plane.
 * \param label The label we are looking for.
 * \param r The search radius.
 * \param sizes The volume's sizes for each dimension.
 * \param start The start position along each dimension.
 * \param voxel_real The returned position of the first voxel.
 * \returns TRUE if a voxel matching 'label' was found.
 */
static VIO_BOOL
find_loop(VIO_Volume volume, int i, int j, int k, int label, int r,
          const int sizes[], const int start[], VIO_Real voxel_real[])
{
  int x, y, z;
  int lo[VIO_MAX_DIMENSIONS];
  int hi[VIO_MAX_DIMENSIONS];
  
  lo[j] = MAX( 0, start[j] - r );
  hi[j] = MIN( sizes[j], start[j] + r );
  lo[k] = MAX( 0, start[k] - r );
  hi[k] = MIN( sizes[k], start[k] + r );

  lo[i] = MIN( sizes[i] - 1, start[i] + r );
  hi[i] = lo[i] + 1;
  
  for (x = lo[VIO_X]; x < hi[VIO_X]; x++)
  {
    for (y = lo[VIO_Y]; y < hi[VIO_Y]; y++)
    {
      for (z = lo[VIO_Z]; z < hi[VIO_Z]; z++ )
      {
        if (get_3D_volume_label_data( volume, x, y, z ) == label )
        {
          voxel_real[VIO_X] = x;
          voxel_real[VIO_Y] = y;
          voxel_real[VIO_Z] = z;
          return TRUE;
        }
      }
    }
  }
    
  lo[i] = MAX( 0, start[i] - r );
  hi[i] = lo[i] + 1;
  for (x = lo[VIO_X]; x < hi[VIO_X]; x++)
  {
    for (y = lo[VIO_Y]; y < hi[VIO_Y]; y++)
    {
      for (z = lo[VIO_Z]; z < hi[VIO_Z]; z++ )
      {
        if (get_3D_volume_label_data( volume, x, y, z ) == label )
        {
          voxel_real[VIO_X] = x;
          voxel_real[VIO_Y] = y;
          voxel_real[VIO_Z] = z;
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**
 * Helper function to search an ever-widening cube around the current
 * position, attempting to find the "closest" point with the given
 * label.
 *
 * \param volume The (label) volume in which to search.
 * \param s_x The voxel x-coordinate of the starting position.
 * \param s_y The voxel y-coordinate of the starting position.
 * \param s_z The voxel z-coordinate of the starting position.
 * \param label The label value to search for.
 * \param voxel_real The location of the closest point that has
 * the value 'label.'
 * \returns TRUE if successful, FALSE if no such voxel is found.
 */
static VIO_BOOL
find_nearest_label( VIO_Volume volume, int s_x, int s_y, int s_z, int label,
                    VIO_Real voxel_real[] )
{
  int r = 1;                    /* search "radius". */
  int sizes[VIO_MAX_DIMENSIONS];
  int start[VIO_MAX_DIMENSIONS];

  start[VIO_X] = s_x;
  start[VIO_Y] = s_y;
  start[VIO_Z] = s_z;

  get_volume_sizes( volume, sizes );
  
  while ( s_x + r < sizes[VIO_X] || s_x - r >= 0 ||
          s_y + r < sizes[VIO_Y] || s_y - r >= 0 ||
          s_z + r < sizes[VIO_Z] || s_z - r >= 0 )
  {
    if (find_loop( volume, 0, 1, 2, label, r, sizes, start, voxel_real ))
      return TRUE;

    if (find_loop( volume, 1, 0, 2, label, r, sizes, start, voxel_real ))
      return TRUE;
    
    if (find_loop( volume, 2, 0, 1, label, r, sizes, start, voxel_real ))
      return TRUE;

    r++;
  }
  return FALSE;
}

/**
 * Create a new label tag at voxel position x, y, z.
 * \param slice_window A pointer to the slice view window.
 * \param volume_index The index of the current volume.
 * \param x The x coordinate for the new tag.
 * \param y The y coordinate for the new tag.
 * \param z The z coordinate for the new tag.
 * \param label The label value of the new tag.
 * \returns A pointer to the new tag object.
 */
object_struct *
create_a_label_tag( display_struct *slice_window,
                    int volume_index,
                    int x,
                    int y,
                    int z,
                    int label )
{
  display_struct *three_d_window = get_display_by_type( THREE_D_WINDOW );
  object_struct  *object = create_object( MARKER );
  marker_struct  *marker = get_marker_ptr( object );
  VIO_Real       voxel_real[VIO_MAX_DIMENSIONS];
  VIO_Real       w_x, w_y, w_z;
  
  voxel_real[VIO_X] = x;
  voxel_real[VIO_Y] = y;
  voxel_real[VIO_Z] = z;
  convert_voxel_to_world( get_nth_label_volume( slice_window, volume_index ),
                          voxel_real, &w_x, &w_y, &w_z );
  fill_Point( marker->position, w_x, w_y, w_z );
  marker->label = create_string( "" );
  marker->structure_id = label;
  marker->patient_id = -1;
  marker->size = three_d_window->three_d.default_marker_size;
  marker->colour = get_colour_of_label( slice_window, volume_index, label );
  marker->type = three_d_window->three_d.default_marker_type;
  return object;
}

/**
 * If we are auto-generating tags from the labels, this function will
 * update the tags list to reflect the new labels. It is therefore called
 * after each label operation.
 * \param slice_window A pointer to the display_struct for the slice window.
 * \param volume_index The desired volume index.
 * \param x The voxel x coordinate
 * \param y The voxel y coordinate
 * \param z The voxel z coordinate
 * \param label The newly applied label.
 */
void
update_label_tag( display_struct *slice_window, int volume_index,
                  int x, int y, int z, int label )
{
  VIO_Volume           volume;
  object_struct        *object_ptr;
  marker_struct        *marker_ptr;
  model_struct         *model_ptr;
  display_struct       *three_d_window;
  VIO_Real             w_x, w_y, w_z;
  VIO_Real             voxel_real[VIO_MAX_DIMENSIONS];
  int                  value;
  int                  n_labels;
  loaded_volume_struct *volume_ptr;
  VIO_BOOL             need_update = FALSE;
  
  three_d_window = get_display_by_type( THREE_D_WINDOW );
  volume = get_nth_label_volume( slice_window, volume_index );
  n_labels = get_num_labels( slice_window, volume_index );
  volume_ptr = &slice_window->slice.volumes[volume_index];

  model_ptr = get_current_model( three_d_window );

  if ( label > 0 && label < n_labels ) /* add a voxel to a label region */
  {
    if ( volume_ptr->label_count[label]++ == 0 )
    {
      object_ptr = create_a_label_tag( slice_window, volume_index,
                                       x, y, z, label );
      volume_ptr->label_tags[label] = object_ptr;
      add_object_to_list( &model_ptr->n_objects, &model_ptr->objects,
                          object_ptr );
      need_update = TRUE;
    }
  }

  /* Get the current value of this voxel, if any. If we
   * are about to change the value, we want to update the
   * state of the old label appropriately.
   */
  value = get_3D_volume_label_data( volume, x, y, z );
  if ( value != label && value > 0 && value < n_labels )
  {
    if ( volume_ptr->label_count[value] > 0 )
    {
      volume_ptr->label_count[value]--;
    }

    /* No more labels of this value, so delete the tag.
     */
    if ( volume_ptr->label_count[value] == 0 )
    {
      object_ptr = volume_ptr->label_tags[value];
      volume_ptr->label_tags[value] = NULL;

      if ( remove_given_object_from_hierarchy( three_d_window, object_ptr ) )
      {
        delete_object( object_ptr );
        need_update = TRUE;
      }
      else
      {
        print_error("Unexpected problem removing label tag from list.\n");
      }
    }
    else
    {
      marker_ptr = get_marker_ptr( volume_ptr->label_tags[value] );
      if ( marker_ptr->structure_id == value )
      {
        convert_world_to_voxel( volume,
                                Point_x( marker_ptr->position ),
                                Point_y( marker_ptr->position ),
                                Point_z( marker_ptr->position ),
                                voxel_real );

        if ((int)voxel_real[VIO_X] == x &&
            (int)voxel_real[VIO_Y] == y &&
            (int)voxel_real[VIO_Z] == z)
        {
          find_nearest_label( volume, x, y, z, value, voxel_real );
          convert_voxel_to_world( volume, voxel_real, &w_x, &w_y, &w_z );
          fill_Point( marker_ptr->position, w_x, w_y, w_z );
          need_update = TRUE;
        }
      }
    }
  }
  if ( need_update )
  {
    graphics_models_have_changed( three_d_window );
    rebuild_selected_list( three_d_window,
                           get_display_by_type( MARKER_WINDOW ) );
  }
}

/**
 * This function sweeps through all of the labels for the current
 * volume and builds a marker object associated with each of the labels.
 *
 * As of now, the tag point is simply the first point encountered during
 * the sweep.
 *
 * \param display A pointer to any display_struct.
 * \param n_objects The number of objects created.
 * \param object_list The object list which will be filled in with the
 * created markers.
 * \returns VIO_OK on success.
 */

VIO_Status
input_tag_objects_label( display_struct *display,
                         int            *n_objects,
                         object_struct  **object_list[] )
{
  display_struct       *slice_window;
  int                  volume_index;
  VIO_Volume           volume;
  int                  sizes[VIO_MAX_DIMENSIONS];
  int                  x, y, z;
  int                  label;
  int                  n_labels;
  loaded_volume_struct *volume_ptr;
  object_struct        *object_ptr;
  VIO_Status           status = VIO_OK;

  *n_objects = 0;

  if ( !get_slice_window( display, &slice_window ) )
    return VIO_ERROR;

  if ( ( volume_index = get_current_volume_index( slice_window ) ) < 0 )
    return VIO_ERROR;

  volume = get_nth_label_volume( slice_window, volume_index );
  n_labels = get_num_labels( slice_window, volume_index );
  volume_ptr = &slice_window->slice.volumes[volume_index];
  
  get_volume_sizes( volume, sizes );

  for_less( x, 0, sizes[VIO_X] )
  {
    for_less( y, 0, sizes[VIO_Y] )
    {
      for_less( z, 0, sizes[VIO_Z] )
      {
        label = get_3D_volume_label_data( volume, x, y, z );
        if ( label > 0 && label < n_labels )
        {
          if ( volume_ptr->label_count[label]++ == 0 )
          {
            object_ptr = create_a_label_tag( slice_window, volume_index,
                                             x, y, z, label );
            volume_ptr->label_tags[label] = object_ptr;
            add_object_to_list( n_objects, object_list, object_ptr );
          }
        }
      }
    }
  }
  check_label_counts( slice_window, volume_index );
  return status;
}

