/**
 * \file undo.c
 * \brief Functions to implement the "undo" command for voxel labeling.
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
 * Initialize a sparse array object. This is a very simple implementation 
 * of a sparse matrix used by the undo code.
 *
 * \param array_ptr The sparse array object to initialize.
 * \param n_dimensions The number of dimensions in this particular array.
 */
static void
sparse_array_initialize(sparse_array_t *array_ptr, int n_dimensions)
{
    int i;
    array_ptr->n_entries = 0;
    array_ptr->n_dimensions = n_dimensions;
    for (i = 0; i < N_SPARSE_HASH; i++)
    {
        array_ptr->table[i] = NULL;
    }
}

/**
 * Hash a set of coordinates into a single integer value.
 * \param array_ptr The sparse array object.
 * \param coord The coordinate array to hash.
 * \returns A single integer hash value.
 */
int sparse_array_hash(sparse_array_t *array_ptr, const int *coord)
{
    int i, h = coord[0];

    for (i = 1; i < array_ptr->n_dimensions; i++)
    {
        h *= coord[i];
    }
    return h % N_SPARSE_HASH;
}

/**
 * The type of function pointers in the sparse_array_apply() function. This 
 * function will be called with the coordinates and value of every item in
 * the array.
 *
 * \param n_dim The number of dimensions in the sparse array.
 * \param coord The coordinates of the item.
 * \param value The value of the item.
 * \param data A possibly useful opaque data pointer.
 */
typedef void (*sparse_apply_func_t)(int n_dim, const int *coord, int value, void *data);

/**
 * Apply a function to every element in a sparse array.
 *
 * \param array_ptr The sparse array object.
 * \param data An argument to the function we intend to apply.
 * \param func The function pointer to call for each element.
 */
void
sparse_array_apply(sparse_array_t *array_ptr, void *data, 
                   sparse_apply_func_t func)
{
    int i;
    sparse_array_entry_t *entry_ptr;
    int n_occupied = 0;
    int n;
    int max_list = 0;

    for (i = 0; i < N_SPARSE_HASH; i++)
    {
        if (array_ptr->table[i] != NULL)
            n_occupied++;
        n = 0;
        for (entry_ptr = array_ptr->table[i]; entry_ptr != NULL; entry_ptr = entry_ptr->link)
        {
            (*func)(array_ptr->n_dimensions, entry_ptr->coord, entry_ptr->value, data);
            n++;
        }
        if (n > max_list)
            max_list = n;
    }
}

/**
 * Find an entry in the the given sparse array that corresponds to the
 * given coordinate.
 * 
 * \param array_ptr The sparse array object.
 * \param coord The coordinates to look up.
 * \returns The sparse array entry, or NULL if it is not found.
 */
static sparse_array_entry_t *
sparse_array_find(sparse_array_t *array_ptr, const int *coord)
{
    int i;
    int h = sparse_array_hash(array_ptr, coord);
    sparse_array_entry_t *entry_ptr;
    for (entry_ptr = array_ptr->table[h]; entry_ptr != NULL;
         entry_ptr = entry_ptr->link)
    {
        if (entry_ptr->coord[0] == coord[0])
        {
            for (i = 1; i < array_ptr->n_dimensions; i++)
            {
                if (entry_ptr->coord[i] != coord[i])
                {
                    break;
                }
            }
            if (i == array_ptr->n_dimensions)
            {
                return entry_ptr;
            }
        }
    }
    return NULL;
}

/**
 * Insert a new value into a sparse array.
 *
 * \param array_ptr The sparse array object.
 * \param coord The coordinates of the new value.
 * \param value The value to save.
 */
void
sparse_array_insert(sparse_array_t *array_ptr, const int *coord, int value)
{
    sparse_array_entry_t *entry_ptr = sparse_array_find(array_ptr, coord);
    if (entry_ptr == NULL)
    {
        int i;
        int h = sparse_array_hash(array_ptr, coord);
        entry_ptr = malloc(sizeof(sparse_array_entry_t));
        if (entry_ptr == NULL)
        {
            return;
        }
        for (i = 0; i < array_ptr->n_dimensions; i++)
        {
            entry_ptr->coord[i] = coord[i];
        }
        entry_ptr->link = array_ptr->table[h];
        array_ptr->table[h] = entry_ptr;
        array_ptr->n_entries++;
    }
    entry_ptr->value = value;
}

/**
 * Free the resources associated with a sparse array object.
 * 
 * \param array_ptr The sparse array object.
 */
void
sparse_array_free(sparse_array_t *array_ptr)
{
    int i;
    for (i = 0; i < N_SPARSE_HASH; i++)
    {
        sparse_array_entry_t *entry_ptr;
        while ((entry_ptr = array_ptr->table[i]) != NULL)
        {
            array_ptr->table[i] = entry_ptr->link;
            free(entry_ptr);
        }
    }
    array_ptr->n_entries = 0;
}

/**
 * Initialize the volume_undo_struct. 
 *
 * Allocates Undo_list_length entries in the prior_labels field, and
 * sets the n_undo field to zero.
 *
 * \param undo The structure to initialize.
 */
void
initialize_slice_undo(volume_undo_struct  *undo)
{
    int i;

    ALLOC(undo->prior_labels, Undo_list_length);

    undo->n_undo = 0;
    for (i = 0; i < Undo_list_length; i++)
    {
        sparse_array_initialize(&undo->prior_labels[i], VIO_N_DIMENSIONS);
    }
}

/**
 * Complete recording a brush stroke for later undo operations.
 *
 * \param slice_window The display_struct for the slice window.
 * \param volume_index The index of the current volume.
 */
void
undo_finish(display_struct *slice_window, int volume_index)
{
    volume_undo_struct *undo_ptr = &slice_window->slice.volumes[volume_index].undo;
    int n_undo = undo_ptr->n_undo + 1;
    if (slice_window->slice.toggle_undo_feature)
    {
        if (n_undo >= Undo_list_length)
            n_undo = 0;
        undo_ptr->n_undo = n_undo;
    }
}

/**
 * Start recording a brush stroke for later undo operations.
 *
 * \param slice_window The display_struct for the slice window.
 * \param volume_index The index of the current volume.
 */
void
undo_start(display_struct *slice_window, int volume_index)
{
    volume_undo_struct *undo_ptr = &slice_window->slice.volumes[volume_index].undo;
    int n_undo = undo_ptr->n_undo;

    if (slice_window->slice.toggle_undo_feature)
    {
        if (undo_ptr->prior_labels[n_undo].n_entries > 0)
        {
            sparse_array_free(&undo_ptr->prior_labels[n_undo]);
        }
        sparse_array_initialize(&undo_ptr->prior_labels[n_undo], 
                                VIO_N_DIMENSIONS);
    }
}

/**
 * Save undo information for the current brushstroke.
 *
 * Saves the coordinate and old label as a brushstroke is performed.
 *
 * \param slice_window The display_struct of the slice window.
 * \param volume_index The index of the current volume.
 * \param coord The voxel coordinates of the changing label.
 * \param label The old label value.
 */
void
undo_save(display_struct *slice_window,
          int volume_index, const int coord[], int label)
{
    volume_undo_struct *undo_ptr = &slice_window->slice.volumes[volume_index].undo;
    int n_undo = undo_ptr->n_undo;
    if (slice_window->slice.toggle_undo_feature)
    {
        sparse_array_insert(&undo_ptr->prior_labels[n_undo], coord, label);
    }
}

/**
 * Static function to reset a voxel label to its previous value.
 *
 * \param n_dimensions The number of dimensions in the array.
 * \param coord The coordinates of the element.
 * \param value The value to which we should set the element.
 * \param data An alias for the slice window's display_struct.
 */
static void
apply_entry(int n_dimensions, const int *coord, int value, void *data)
{
    display_struct *slice_window = (display_struct *) data;
    int volume_index = get_current_volume_index( slice_window );

    set_voxel_label( slice_window, volume_index,
                     coord[VIO_X], coord[VIO_Y], coord[VIO_Z],
                     value);
}

/**
 * Determine whether there is valid undo information stored for
 * the current volume.
 *
 * \param slice_window The display_struct of the slice window.
 * \param volume_index The current volume index.
 * \returns TRUE if valid undo information is stored for volume_index.
 */
static VIO_BOOL
undo_present(display_struct *slice_window, int volume_index)
{
  volume_undo_struct *undo_ptr = &slice_window->slice.volumes[volume_index].undo;
  if (!slice_window->slice.toggle_undo_feature)
  {
    return FALSE;
  }
  int n_undo = undo_ptr->n_undo - 1;
  if (n_undo < 0)
    n_undo = Undo_list_length - 1;

  return undo_ptr->prior_labels[n_undo].n_entries > 0;
}

/**
 * Revert the most recent brush stroke on the volume.
 *
 * \param slice_window The display_struct for the volume.
 * \param volume_index The index of the current volume.
 * \returns The volume index (for historical reasons).
 */
static int
undo_apply(display_struct *slice_window, int volume_index)
{
  volume_undo_struct *undo_ptr = &slice_window->slice.volumes[volume_index].undo;
  int n_undo = undo_ptr->n_undo - 1;
  if (n_undo < 0)
    n_undo = Undo_list_length - 1;
  undo_ptr->n_undo = n_undo;

  sparse_array_apply(&undo_ptr->prior_labels[n_undo],
                     slice_window,
                     apply_entry);

  sparse_array_free(&undo_ptr->prior_labels[n_undo]);

  sparse_array_initialize(&undo_ptr->prior_labels[n_undo], 
                          VIO_N_DIMENSIONS);

  return volume_index;
}

/**
 * If a global change has occured in the program, the currently
 * saved undo information may be invalid. We signal that possibility
 * here.
 *
 * \param slice_window The display_struct for the volume.
 * \param volume_index The index of the current volume, or -1 if we
 * should perform the operation for all volumes.
 */
void
delete_slice_undo(display_struct *slice_window, int volume_index )
{
    int i_vol_start, i_vol_end;
    int i, j;

    if (volume_index < 0)
    {
        i_vol_start = 0;
        i_vol_end = get_n_volumes( slice_window );
    }
    else
    {
        i_vol_start = volume_index;
        i_vol_end = volume_index + 1;
    }
    for_less (i, i_vol_start, i_vol_end)
    {
        volume_undo_struct *undo_ptr = &slice_window->slice.volumes[i].undo;
        for_less (j, 0, Undo_list_length)
        {
            sparse_array_free(&undo_ptr->prior_labels[j]);
        }
        undo_ptr->n_undo = 0;
    }
}

/**
 * Determine whether there is valid undo information available for
 * the currently selected volume.
 *
 * \param display Any display_struct.
 * \returns TRUE if there is valid undo information for the current volume.
 */
VIO_BOOL
slice_labels_to_undo(display_struct *display)
{
    display_struct *slice_window;
    int            volume_index;
    return( get_slice_window( display, &slice_window ) &&
            slice_window->slice.toggle_undo_feature &&
            (volume_index = get_current_volume_index(slice_window)) >= 0 &&
            undo_present(slice_window, volume_index));
}

/**
 * Performs an undo operation if possible.
 *
 * \param display Any display_struct.
 * \returns The volume index of the currently selected volume, if 
 * an undo operation actually occurred. Returns -1 if no undo operation
 * was possible.
 */
int
undo_slice_labels_if_any(display_struct *display)
{
    int             volume_index;
    display_struct  *slice_window;

    if ( get_slice_window( display, &slice_window ) &&
         slice_window->slice.toggle_undo_feature)
    {
        volume_index = get_current_volume_index( slice_window );
        volume_index = undo_apply(slice_window, volume_index );
    }
    else
    {
        print( "Nothing to undo.\n" );
        volume_index = -1;
    }

    return( volume_index );
}
