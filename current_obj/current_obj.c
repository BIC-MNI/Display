
#include  <def_graphics.h>

public  void  advance_current_object( graphics )
    graphics_struct   *graphics;
{
    int            object_index;
    model_struct   *model;

    object_index = TOP_OF_STACK( graphics->current_object ).object_index;
    model = TOP_OF_STACK( graphics->current_object ).model;

    if( model->n_objects > 0 )
    {
        TOP_OF_STACK( graphics->current_object ).object_index =
             (object_index + 1) % model->n_objects;
    }
}

public  void  retreat_current_object( graphics )
    graphics_struct   *graphics;
{
    int            object_index;
    model_struct   *model;

    object_index = TOP_OF_STACK( graphics->current_object ).object_index;
    model = TOP_OF_STACK( graphics->current_object ).model;

    if( model->n_objects > 0 )
    {
        TOP_OF_STACK( graphics->current_object ).object_index =
             (object_index - 1 + model->n_objects) % model->n_objects;
    }
}

public  model_struct  *get_current_model( graphics )
    graphics_struct   *graphics;
{
    model_struct   *model;

    model = TOP_OF_STACK( graphics->current_object ).model;

    return( model );
}

public  int  get_current_object_index( graphics )
    graphics_struct   *graphics;
{
    int  index;

    index = TOP_OF_STACK( graphics->current_object ).object_index;

    return( index );
}

public  object_struct  *get_current_object( graphics )
    graphics_struct   *graphics;
{
    int             object_index;
    model_struct    *model;
    object_struct   *object;

    object_index = TOP_OF_STACK( graphics->current_object ).object_index;
    model = TOP_OF_STACK( graphics->current_object ).model;

    if( object_index >= 0 && object_index < model->n_objects )
    {
        object = model->object_list[object_index];
    }
    else
    {
        object = (object_struct *) 0;
    }

    return( object );
}

public  Status  initialize_current_object( graphics )
    graphics_struct   *graphics;
{
    Status            status;
    selection_entry   entry;
    model_struct      *get_graphics_model();

    entry.object_index = 0;
    entry.model = get_graphics_model( graphics, THREED_MODEL );

    PUSH_STACK( status, graphics->current_object, selection_entry, entry );

    return( status );
}

public  Status  push_current_object( graphics )
    graphics_struct   *graphics;
{
    Status            status;
    selection_entry   entry;
    object_struct     *current_object;

    status = OK;

    current_object = get_current_object( graphics );

    if( current_object != (object_struct *) 0 &&
        current_object->object_type == MODEL )
    {
        entry.object_index = 0;
        entry.model = current_object->ptr.model;

        PUSH_STACK( status, graphics->current_object, selection_entry,
                    entry );
    }

    return( status );
}

private  Boolean  current_object_is_top_level( graphics )
    graphics_struct   *graphics;
{
    return( N_ENTRIES_IN_STACK(graphics->current_object) == 1 );
}

public  Status  pop_current_object( graphics )
    graphics_struct   *graphics;
{
    Status            status;
    selection_entry   entry;

    if( N_ENTRIES_IN_STACK(graphics->current_object) > 1 )
    {
        POP_STACK( graphics->current_object, entry );

        status = OK;
    }
    else
    {
        PRINT_ERROR( "pop current object" );
        status = ERROR;
    }

    return( status );
}
