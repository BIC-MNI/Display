
#include  <def_graphics.h>

private  selection_entry  *get_current_entry( graphics )
    graphics_struct  *graphics;
{
    return( &graphics->three_d.current_object.stack
                        [graphics->three_d.current_object.current_level-1] );
}

public  void  advance_current_object( graphics )
    graphics_struct   *graphics;
{
    int               object_index;
    selection_entry   *entry;
    model_struct      *model;

    if( !current_object_is_top_level(graphics) )
    {
        entry = get_current_entry( graphics );

        object_index = entry->object_index;
        model = entry->model_object->ptr.model;

        if( model->n_objects > 0 )
        {
            entry->object_index = (object_index + 1) % model->n_objects;
        }
    }
}

public  void  retreat_current_object( graphics )
    graphics_struct   *graphics;
{
    int               object_index;
    selection_entry   *entry;
    model_struct      *model;

    if( !current_object_is_top_level(graphics) )
    {
        entry = get_current_entry( graphics );

        object_index = entry->object_index;
        model = entry->model_object->ptr.model;

        if( model->n_objects > 0 )
        {
            entry->object_index = (object_index - 1 + model->n_objects) %
                                  model->n_objects;
        }
    }
}

public  object_struct  *get_current_model_object( graphics )
    graphics_struct   *graphics;
{
    object_struct     *model_object;
    selection_entry   *entry;

    if( current_object_is_top_level(graphics) )
    {
        model_object = graphics->models[THREED_MODEL];
    }
    else
    {
        entry = get_current_entry( graphics );
        model_object = entry->model_object;
    }

    return( model_object );
}

public  model_struct  *get_current_model( graphics )
    graphics_struct   *graphics;
{
    object_struct  *model_object;
    object_struct  *get_current_model_object();
    model_struct   *model;

    model_object = get_current_model_object( graphics );

    model = model_object->ptr.model;

    return( model );
}

public  int  get_current_object_index( graphics )
    graphics_struct   *graphics;
{
    int               index;
    selection_entry   *entry;

    if( current_object_is_top_level(graphics) )
        index = -1;
    else
    {
        entry = get_current_entry( graphics );
        index = entry->object_index;
    }

    return( index );
}

public  void  set_current_object_index( graphics, index )
    graphics_struct   *graphics;
    int               index;
{
    selection_entry   *entry;

    if( !current_object_is_top_level(graphics) )
    {
        entry = get_current_entry( graphics );

        if( index == 0 ||
            (index >= 0 &&
             index < entry->model_object->ptr.model->n_objects) )
        {
            entry->object_index = index;
        }
        else
        {
            HANDLE_INTERNAL_ERROR( "set current object index" );
        }
    }
}

public  Boolean  get_current_object( graphics, current_object )
    graphics_struct   *graphics;
    object_struct     **current_object;
{
    int               object_index;
    Boolean           current_object_exists;
    model_struct      *model;
    selection_entry   *entry;

    if( current_object_is_top_level(graphics) )
    {
        *current_object = graphics->models[THREED_MODEL];
        current_object_exists = TRUE;
    }
    else
    {
        entry = get_current_entry( graphics );
        object_index = entry->object_index;
        model = entry->model_object->ptr.model;

        if( object_index >= 0 && object_index < model->n_objects )
        {
            *current_object = model->object_list[object_index];
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

public  Status  initialize_current_object( current_object )
    selection_struct   *current_object;
{
    current_object->n_levels_alloced = 0;
    current_object->max_levels = 0;
    current_object->current_level = 0;

    return( OK );
}

public  Status  terminate_current_object( current_object )
    selection_struct   *current_object;
{
    Status  status;

    status = OK;

    if( current_object->n_levels_alloced > 0 )
        FREE( status, current_object->stack );

    return( status );
}

public  Status  push_current_object( graphics )
    graphics_struct   *graphics;
{
    Status            status;
    Boolean           previously_here;
    selection_entry   entry, *entry_ptr;
    selection_struct  *selection;
    object_struct     *current_object;

    status = OK;

    if( get_current_object( graphics, &current_object ) &&
        current_object->object_type == MODEL )
    {
        previously_here = FALSE;

        selection = &graphics->three_d.current_object;

        if( selection->current_level < selection->max_levels )
        {
            entry_ptr = &selection->stack[selection->current_level];
            if( entry_ptr->model_object == current_object &&
                entry_ptr->object_index >= 0 &&
                entry_ptr->object_index < current_object->ptr.model->n_objects )
            {
                previously_here = TRUE;
                ++selection->current_level;
            }
        }

        if( !previously_here )
        {
            entry.object_index = 0;
            entry.model_object = current_object;

            ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status, selection->n_levels_alloced,
                 selection->current_level, selection->stack, entry,
                 DEFAULT_CHUNK_SIZE )

            if( selection->current_level > selection->max_levels )
                selection->max_levels = selection->current_level;
        }
    }

    return( status );
}

public  Boolean  current_object_is_top_level( graphics )
    graphics_struct   *graphics;
{
    return( graphics->three_d.current_object.current_level == 0 );
}

public  Status  pop_current_object( graphics )
    graphics_struct   *graphics;
{
    if( !current_object_is_top_level(graphics) )
    {
        --graphics->three_d.current_object.current_level;
    }

    return( OK );
}
