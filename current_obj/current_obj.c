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
 * The position at each level is represented as a selection_entry,
 * which contains a pointer to the active model and the index of the
 * selected object within the active model. We can descend further
 * into the hierarchy if the selected object is itself a model. A
 * single selection_struct is used to represent the current location
 * within the object hierarchy.  The selection_struct contains a stack
 * which is a list of selection_entry objects recording the active model
 * and object indices at each level of the hierarchy.
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
 * Gets the selection_entry at the top of the selection_struct stack.
 * \param display The display_struct of the 3D window.  
 * \returns A pointer to the selection_entry representing the current
 * position in the object hierarchy.
 */
static selection_entry *
get_current_entry(display_struct *display)
{
    selection_struct *selection_ptr = &display->three_d.current_object;

    if (selection_ptr->current_level <= 0)
    {
        HANDLE_INTERNAL_ERROR("Object stack is empty!");
        return NULL;
    }
    
    return( &selection_ptr->stack[selection_ptr->current_level - 1] );
}

/**
 * Selects the next object in the currently active model. Wraps
 * back to the first object in the model if it advances past the end
 * of the model's object list.
 *
 * \param display The display_struct of the 3D window.
 */
void
advance_current_object(display_struct *display)
{
    if( !current_object_is_top_level(display) )
    {
        selection_entry *entry_ptr = get_current_entry( display );
        model_struct    *model_ptr = get_model_ptr( entry_ptr->model_object );

        if (++entry_ptr->object_index >= model_ptr->n_objects)
        {
            entry_ptr->object_index = 0;
        }
    }
}

/**
 * Selects the previous object in the currently active model. Wraps back
 * to the last object in the list if it tries to go past the beginning.
 * \param display The display_struct of the 3D window.
 */
void
retreat_current_object(display_struct *display)
{
    if( !current_object_is_top_level(display) )
    {
        selection_entry *entry_ptr = get_current_entry( display );
        model_struct    *model_ptr = get_model_ptr( entry_ptr->model_object );

        if (--entry_ptr->object_index < 0)
        {
            entry_ptr->object_index = model_ptr->n_objects - 1;
        }
    }
}

/**
 * Returns the currently selected model as a generic object_struct.
 * \param display The display_struct of the 3D window.
 * \returns An object_struct corresponding to the current model.
 */
object_struct *
get_current_model_object(display_struct *display)
{
    object_struct     *model_object;

    if( current_object_is_top_level(display) )
    {
        model_object = display->models[THREED_MODEL];
    }
    else
    {
        model_object = get_current_entry( display )->model_object;
    }

    return( model_object );
}

/**
 * Returns the currently selected model as a model_struct.
 * \param display The display_struct of the 3D window.
 * \returns An model_struct corresponding to the current model.
 */
model_struct *
get_current_model(display_struct *display)
{
    return get_model_ptr( get_current_model_object( display ) );
}

/**
 * Returns the index of the currently selected object.
 * \param display The display_struct of the 3D window.
 * \returns An integer representing the position of the selected
 * entry within the currently active model.
 */
int  
get_current_object_index(display_struct *display)
{
    int index;

    if( current_object_is_top_level(display) )
        index = -1;
    else
        index = get_current_entry( display )->object_index;
    return( index );
}

/**
 * Check whether the object index is within the bounds of the current
 * model.
 * \param entry_ptr The selection_entry to check.
 * \returns TRUE if the object_index field of the selection_entry is
 * still within the bounds of the model's size.
 */
static VIO_BOOL
index_within_bounds(selection_entry *entry_ptr)
{
  model_struct *model_ptr = get_model_ptr(entry_ptr->model_object);
  return (entry_ptr->object_index >= 0 && 
          entry_ptr->object_index < model_ptr->n_objects);
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
set_current_object(display_struct *display, object_struct *sought_object)
{
    VIO_BOOL           done;
    selection_struct   *selection_ptr;
    object_struct      *current_object;
    selection_entry    *entry_ptr;

    /* Get a pointer to the current selection_struct and initialize it
     * appropriately. This assumes that the stack has been allocated
     * correctly.
     */
    selection_ptr = &display->three_d.current_object;

    if (selection_ptr->n_levels_alloced <= 0)
    {
        HANDLE_INTERNAL_ERROR("Stack is not initialized.");
        return;
    }

    /* Start the selection at the beginning.
     */
    selection_ptr->current_level = 1;
    selection_ptr->max_levels = 1;
    selection_ptr->stack[0].object_index = 0;

    done = FALSE;

    while( !done )
    {
        /* See if the currently selected object corresponds to the
         * thing we are looking for. If so, we are done.
         */
        if( get_current_object( display, &current_object ) &&
            current_object == sought_object )
        {
            done = TRUE;
        }
        /* Otherwise, see if the current object is a non-empty model.
         * If so, we're going to descend into it.
         */
        else if( get_object_type(current_object) == MODEL &&
                 get_model_ptr(current_object)->n_objects > 0 )
        {
            push_current_object( display );
            entry_ptr = get_current_entry( display );
            entry_ptr->object_index = 0;
        }
        else
        {
            /* Advance to the next object in the model.
             */
            entry_ptr = get_current_entry( display );
            ++entry_ptr->object_index;

            /* We may have reached the end of this model's objects.
             * If so, we want to pop back up to the previous level, 
             * or possible declare the search a failure.
             */
            while (!done && !index_within_bounds(entry_ptr))
            {
                if ( selection_ptr->current_level <= 1 )
                {
                    /* If we can't pop the stack, we failed to
                     * find the requested object and we're done.
                     */
                    done = TRUE;
                }
                else
                {
                    /* Otherwise pop the stack.
                     */
                    pop_current_object( display );

                    /* Advance to the next object in the model.
                     */
                    entry_ptr = get_current_entry( display );
                    ++entry_ptr->object_index;
                }
            }
        }
    }
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
set_current_object_index(display_struct *display, int index )
{
    if( !current_object_is_top_level(display) )
    {
        model_struct *model_ptr = get_current_model( display );

        if( index == 0 || (index >= 0 && index < model_ptr->n_objects) )
        {
            get_current_entry(display)->object_index = index;
        }
        else
        {
            HANDLE_INTERNAL_ERROR( "set current object index" );
        }
    }
}

/**
 * Get the currently selected object.
 * \param display The display_struct of the 3D window.
 * \param current_object A pointer to a pointer to an object_struct.
 */
VIO_BOOL
get_current_object(display_struct *display, object_struct  **current_object )
{
    if( current_object_is_top_level(display) )
    {
        *current_object = display->models[THREED_MODEL];
    }
    else
    {
        selection_entry *entry_ptr = get_current_entry( display );

        if( index_within_bounds(entry_ptr) )
        {
            model_struct *model_ptr = get_model_ptr( entry_ptr->model_object );
            *current_object = model_ptr->objects[entry_ptr->object_index];
        }
        else
        {
            *current_object = NULL;
        }
    }

    return( *current_object != NULL );
}

/**
 * Initializes the selection_struct that represents the currently selected
 * object.
 * \param display The display_struct for the 3D window.
 */
void
initialize_current_object(display_struct *display)
{
    display->three_d.current_object.n_levels_alloced = 0;
    display->three_d.current_object.max_levels = 0;
    display->three_d.current_object.current_level = 0;

    /* start off below top level */

    push_current_object( display );
}

/**
 * Frees the stack in the selection_struct.
 * \param current_object The selection_struct associated with the 3D window.
 */
void
terminate_current_object(selection_struct *current_object)
{
    if( current_object->n_levels_alloced > 0 )
        FREE( current_object->stack );
}

/**
 * Pushes the selection_struct to the next level, if the current
 * object is a model. If we are pushing into the same model again, we
 * also maintain the current object index within that model, otherwise
 * we reset the object index to zero.
 *
 * \param display The display_struct for the 3D window.
 */
void
push_current_object(display_struct *display)
{
    selection_struct  *selection_ptr;
    object_struct     *object_ptr;
    selection_entry   entry;

    /* If the current selection is not an model, give up.
     */
    if ( !get_current_object( display, &object_ptr ) ||
         get_object_type(object_ptr) != MODEL )
    {
        return;
    }

    selection_ptr = &display->three_d.current_object;

    if( selection_ptr->current_level < selection_ptr->max_levels )
    {
        /* See if the next level is still valid.
         */
        selection_entry *entry_ptr = &selection_ptr->stack[selection_ptr->current_level];
        if( entry_ptr->model_object == object_ptr &&
            index_within_bounds( entry_ptr ) )
        {
            /* Just descend into the next level, retaining the object
             * index.
             */
            ++selection_ptr->current_level;
            return;
        }
    }

    /* The next level has changed, so need to add a level to
     * the stack.
     */
    entry.object_index = 0;
    entry.model_object = object_ptr;

    /*
     * This macro will add the entry to the stack at the position
     * selection_ptr->current_level, and increments the current_level. 
     * If needed, the array will grow and selection_ptr->n_levels_alloced
     * will be increased as well.
     */
    ADD_ELEMENT_TO_ARRAY_WITH_SIZE( selection_ptr->stack,
                                    selection_ptr->n_levels_alloced,
                                    selection_ptr->current_level,
                                    entry,
                                    DEFAULT_CHUNK_SIZE )

    /*
     * Finally update the max_levels value to reflect the maximum
     * stack depth.
     */
    if( selection_ptr->current_level > selection_ptr->max_levels )
        selection_ptr->max_levels = selection_ptr->current_level;
}

/**
 * Returns true if the selection stack is empty (i.e. the selection is 
 * at the single top level model). In practice, this should always
 * return FALSE, as we push into the top-level model
 * during initialization, and never pop above the top-level model.
 *
 * \param display The display_struct of the 3D window.
 * \returns TRUE if the current location is the automatically created 
 * top-level model.
 */
VIO_BOOL
current_object_is_top_level(display_struct *display)
{
    return( display->three_d.current_object.current_level == 0 );
}

/**
 * Pop the object stack up a level. The values at the previous level
 * are left undisturbed, in case we push back down into this same
 * object again. We never pop all the way back to the top-level 
 * model.
 *
 * \param display The display_struct of the 3D window.
 */
void
pop_current_object(display_struct *display )
{
    /* Don't allow popping up to the top level.
     */
    if( display->three_d.current_object.current_level > 1 )
        --display->three_d.current_object.current_level;
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
current_object_exists(display_struct *display)
{
    object_struct *object_ptr;

    return( get_current_object( display, &object_ptr ) );
}
