
#include  <display.h>

private  selection_entry  *get_current_entry(
    display_struct   *display )
{
    return( &display->three_d.current_object.stack
                        [display->three_d.current_object.current_level-1] );
}

public  void  advance_current_object(
    display_struct    *display )
{
    int               object_index;
    selection_entry   *entry;
    model_struct      *model;

    if( !current_object_is_top_level(display) )
    {
        entry = get_current_entry( display );

        object_index = entry->object_index;
        model = get_model_ptr( entry->model_object );

        if( model->n_objects > 0 )
        {
            entry->object_index = (object_index + 1) % model->n_objects;
        }
    }
}

public  void  retreat_current_object(
    display_struct    *display )
{
    int               object_index;
    selection_entry   *entry;
    model_struct      *model;

    if( !current_object_is_top_level(display) )
    {
        entry = get_current_entry( display );

        object_index = entry->object_index;
        model = get_model_ptr( entry->model_object );

        if( model->n_objects > 0 )
        {
            entry->object_index = (object_index - 1 + model->n_objects) %
                                  model->n_objects;
        }
    }
}

public  object_struct  *get_current_model_object(
    display_struct    *display )
{
    object_struct     *model_object;
    selection_entry   *entry;

    if( current_object_is_top_level(display) )
    {
        model_object = display->models[THREED_MODEL];
    }
    else
    {
        entry = get_current_entry( display );
        model_object = entry->model_object;
    }

    return( model_object );
}

public  model_struct  *get_current_model(
    display_struct    *display )
{
    object_struct  *model_object;
    model_struct   *model;

    model_object = get_current_model_object( display );

    model = get_model_ptr( model_object );

    return( model );
}

public  int  get_current_object_index(
    display_struct    *display )
{
    int               index;
    selection_entry   *entry;

    if( current_object_is_top_level(display) )
        index = -1;
    else
    {
        entry = get_current_entry( display );
        index = entry->object_index;
    }

    return( index );
}

public  void  set_current_object(
    display_struct    *display,
    object_struct     *object )
{
    BOOLEAN            found, done;
    selection_struct   *current_selection;
    object_struct      *current_object;

    current_selection = &display->three_d.current_object;

    current_selection->current_level = 1;
    current_selection->max_levels = 1;
    current_selection->stack[0].object_index = 0;

    found = FALSE;
    done = FALSE;

    while( !found && !done )
    {
        if( get_current_object( display, &current_object ) &&
            current_object == object )
        {
            found = TRUE;
        }
        else if( current_object->object_type == MODEL &&
                 get_model_ptr(current_object)->n_objects > 0 )
        {
            push_current_object( display );
            current_selection->stack[current_selection->current_level-1].
                    object_index = 0;
        }
        else
        {
            while( current_selection->current_level > 1 &&
                   current_selection->stack[current_selection->current_level-1].
                    object_index >=
                   get_model_ptr( current_selection->stack
                                  [current_selection->current_level-1].
                                  model_object )->n_objects-1 )
            {
                pop_current_object( display );
            }

            if( current_selection->current_level == 1 &&
                current_selection->stack[current_selection->current_level-1].
                    object_index >=
                 get_model_ptr(current_selection->stack[0].model_object)->
                                                                  n_objects-1 )
            {
                done = TRUE;
            }
            else
            {
                ++current_selection->stack[current_selection->current_level-1].
                    object_index;
            }
        }
    }
}

public  void  set_current_object_index(
    display_struct    *display,
    int               index )
{
    selection_entry   *entry;

    if( !current_object_is_top_level(display) )
    {
        entry = get_current_entry( display );

        if( index == 0 ||
            (index >= 0 &&
             index < get_model_ptr(entry->model_object)->n_objects) )
        {
            entry->object_index = index;
        }
        else
        {
            HANDLE_INTERNAL_ERROR( "set current object index" );
        }
    }
}

public  BOOLEAN  get_current_object(
    display_struct    *display,
    object_struct     **current_object )
{
    int               object_index;
    BOOLEAN           current_object_exists;
    model_struct      *model;
    selection_entry   *entry;

    if( current_object_is_top_level(display) )
    {
        *current_object = display->models[THREED_MODEL];
        current_object_exists = TRUE;
    }
    else
    {
        entry = get_current_entry( display );
        object_index = entry->object_index;
        model = get_model_ptr( entry->model_object );

        if( object_index >= 0 && object_index < model->n_objects )
        {
            *current_object = model->objects[object_index];
            current_object_exists = TRUE;
        }
        else
        {
            *current_object = (object_struct *) 0;
            current_object_exists = FALSE;
        }
    }

    return( current_object_exists );
}

public  void  initialize_current_object(
    display_struct    *display )
{
    display->three_d.current_object.n_levels_alloced = 0;
    display->three_d.current_object.max_levels = 0;
    display->three_d.current_object.current_level = 0;

    /* start off below top level */

    push_current_object( display );
}

public  void  terminate_current_object(
    selection_struct   *current_object )
{
    if( current_object->n_levels_alloced > 0 )
        FREE( current_object->stack );
}

public  void  push_current_object(
    display_struct    *display )
{
    BOOLEAN           previously_here;
    selection_entry   entry, *entry_ptr;
    selection_struct  *selection;
    object_struct     *current_object;

    if( get_current_object( display, &current_object ) &&
        current_object->object_type == MODEL )
    {
        previously_here = FALSE;

        selection = &display->three_d.current_object;

        if( selection->current_level < selection->max_levels )
        {
            entry_ptr = &selection->stack[selection->current_level];
            if( entry_ptr->model_object == current_object &&
                entry_ptr->object_index >= 0 &&
                entry_ptr->object_index <
                                      get_model_ptr(current_object)->n_objects )
            {
                previously_here = TRUE;
                ++selection->current_level;
            }
        }

        if( !previously_here )
        {
            entry.object_index = 0;
            entry.model_object = current_object;

            ADD_ELEMENT_TO_ARRAY_WITH_SIZE( selection->stack,
                 selection->n_levels_alloced, selection->current_level, entry,
                 DEFAULT_CHUNK_SIZE )

            if( selection->current_level > selection->max_levels )
                selection->max_levels = selection->current_level;
        }
    }
}

public  BOOLEAN  current_object_is_top_level(
    display_struct    *display )
{
    return( display->three_d.current_object.current_level == 0 );
}

public  void  pop_current_object(
    display_struct    *display )
{
    /* don't allow popping up to the top level */

    if( display->three_d.current_object.current_level > 1 )
        --display->three_d.current_object.current_level;
}
