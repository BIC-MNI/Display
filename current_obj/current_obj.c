/**
 * \file current_obj.c
 * \brief Functions to manipulate the object list.
 *
 * The object list contains the graphical objects (surfaces, lines,
 * etc.) that are displayed in the 3D window. The loaded objects are
 * contained within a special top-level model
 * (display->models[THREED_MODEL]) that is created at run time. In
 * terms of the graphical object library in bicpl/bicgl, models are
 * graphical objects that contain other graphical objects, including
 * models.
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
 * Selects the next object in the currently active model. Wraps
 * back to the first object in the model if it advances past the end
 * of the model's object list.
 *
 * \param display The display_struct of the 3D window.
 */
void
advance_current_object( display_struct *display )
{
  if (find_index_in_hierarchy( display, 
                               display->three_d.current_object + 1 ) != NULL )
  {
    display->three_d.current_object++;
  }
}

/**
 * Selects the previous object in the object hierarchy.

 * \param display The display_struct of the 3D window.
 */
void
retreat_current_object( display_struct *display )
{
  if (display->three_d.current_object > 0)
  {
    display->three_d.current_object--;
  }
}

/**
 * Returns the currently selected model as a generic object_struct.
 * \param display The display_struct of the 3D window.
 * \returns An object_struct corresponding to the current model.
 */
object_struct *
get_current_model_object( display_struct *display )
{
  object_struct *object_ptr;

  if (get_current_object( display, &object_ptr ) &&
      get_object_type( object_ptr ) == MODEL )
  {
    return object_ptr;
  }
  return find_containing_model( display, display->three_d.current_object );
}

/**
 * Returns the currently selected model as a model_struct.
 * \param display The display_struct of the 3D window.
 * \returns An model_struct corresponding to the current model.
 */
model_struct *
get_current_model( display_struct *display )
{
    return get_model_ptr( get_current_model_object( display ) );
}

/**
 * Returns the index of the currently selected object.
 * \param display The display_struct of the 3D window.
 * \returns An integer representing the position of the selected
 * entry within the object list.
 */
int  
get_current_object_index( display_struct *display )
{
    return( display->three_d.current_object );
}

/**
 * Makes a given object the current object. Will recursively search 
 * through the entire model list and its substructure in order to find
 * the object.
 *
 * \param display The display_struct of the 3D window.
 * \param sought_object A pointer to the object_struct to select.
 */
void
set_current_object( display_struct *display, object_struct *sought_object )
{
  int index = find_object_in_hierarchy( display, sought_object );
  if (index >= 0)
  {
    display->three_d.current_object = index;
  }
}

/**
 * Returns the index of the model in the top-level object list.
 * \param display A pointer to the display_struct of the 3D view.
 * \param object_ptr A pointer to the object to find.
 * \returns An index greater than or equal to zero if found, -1 if
 * not found.
 */
int
get_object_index( display_struct *display, object_struct *object_ptr )
{
  return find_object_in_hierarchy( display, object_ptr );
}

/**
 * Set the current object to be the one at the given index within the
 * currently active model. Does nothing if the index is outside the
 * range (0, n_objects-1), with the exception that an index of zero is
 * always legal.
 *
 * \param display The display_struct of the 3D window.
 * \param index The number of the object to select.
 */
void
set_current_object_index( display_struct *display, int index )
{
  while (index >= 0)
  {
    if (find_index_in_hierarchy( display, index ) != NULL)
    {
      display->three_d.current_object = index;
      break;
    }
    index--;
  }
}

/**
 * Get the currently selected object, if any.
 *
 * \param display The display_struct of the 3D window.
 * \param current_object A pointer to a pointer to an object_struct.
 * \returns TRUE if the current object is valid.
 */
VIO_BOOL
get_current_object(display_struct *display, object_struct  **current_object )
{
  *current_object = find_index_in_hierarchy( display,
                                             display->three_d.current_object );
  return( *current_object != NULL );
}

/**
 * Initializes the structure that represents the currently selected
 * object.
 * \param display The display_struct for the 3D window.
 */
void
initialize_current_object( display_struct *display )
{
    display->three_d.current_object = 0;
}

/**
 * Check whether or not the selected object has the requested type.
 * \param display The display_struct of the 3D window.
 * \param object_type The type of object (e.g. POLYGONS, LINES, PIXELS).
 * \returns TRUE if the selected object is of type \c object_type.
 */
VIO_BOOL
current_object_is_this_type(display_struct *display, Object_types object_type)
{
    object_struct *object_ptr;

    return( get_current_object( display, &object_ptr ) &&
            get_object_type(object_ptr) == object_type );
}

/**
 * Returns true if the currently selected object is found.
 * \param display The display_struct of the 3D window.
 * \returns TRUE if there is a valid selected object.
 */
VIO_BOOL
current_object_exists( display_struct *display )
{
    object_struct *object_ptr;

    return( get_current_object( display, &object_ptr ) );
}

/**
 * \brief Removes the current object from the marker list, and from any 
 * containing model. 
 *
 * Does not delete the object, but rather returns a pointer to the object
 * so that it can be saved or moved elsewhere in the hierarchy.
 *
 * \param display The 3D window object.
 * \param object A pointer to and object pointer, where the deleted object
 * will be returned.
 * \returns True if the object was found and returned.
 */
VIO_BOOL  remove_current_object_from_hierarchy(
    display_struct   *display,
    object_struct    **object )
{
    int              obj_index;
    object_struct    *parent_ptr;

    if( !get_current_object( display, object ) )
    {
        return FALSE;
    }

    obj_index = get_current_object_index( display );

    parent_ptr = find_containing_model( display, obj_index );

    remove_object_from_model( get_model_ptr( parent_ptr ), *object );

    set_current_object_index( display, obj_index );

    return TRUE;
}

