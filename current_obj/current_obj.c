
#include  <def_graphics.h>

public  void  advance_current_object( graphics )
    graphics_struct   *graphics;
{
    int            object_index;
    model_struct   *model;

    if( !current_object_is_top_level( graphics ) )
    {
        object_index = TOP_OF_STACK( graphics->current_object ).object_index;
        model = TOP_OF_STACK( graphics->current_object ).model;

        if( model->n_objects > 0 )
        {
            TOP_OF_STACK( graphics->current_object ).object_index =
                 (object_index + 1) % model->n_objects;
        }
    }
}

public  void  retreat_current_object( graphics )
    graphics_struct   *graphics;
{
    int            object_index;
    model_struct   *model;

    if( !current_object_is_top_level( graphics ) )
    {
        object_index = TOP_OF_STACK( graphics->current_object ).object_index;
        model = TOP_OF_STACK( graphics->current_object ).model;

        if( model->n_objects > 0 )
        {
            TOP_OF_STACK( graphics->current_object ).object_index =
                 (object_index - 1 + model->n_objects) % model->n_objects;
        }
    }
}

public  model_struct  *get_current_model( graphics )
    graphics_struct   *graphics;
{
    model_struct   *model;

    if( current_object_is_top_level( graphics ) )
    {
        model = &graphics->models[THREED_MODEL];
    }
    else
    {
        model = TOP_OF_STACK( graphics->current_object ).model;
    }

    return( model );
}

public  int  get_current_object_index( graphics )
    graphics_struct   *graphics;
{
    int  index;

    if( current_object_is_top_level( graphics ) )
    {
        index = -1;
    }
    else
    {
        index = TOP_OF_STACK( graphics->current_object ).object_index;
    }

    return( index );
}

public  int  get_current_objects( graphics, object_list )
    graphics_struct   *graphics;
    object_struct     *object_list[];
{
    int            n_objects;
    int            object_index;
    model_struct   *model;

    if( current_object_is_top_level( graphics ) )
    {
        n_objects = graphics->models[THREED_MODEL].n_objects;
        *object_list = graphics->models[THREED_MODEL].object_list;
    }
    else
    {
        object_index = TOP_OF_STACK( graphics->current_object ).object_index;
        model = TOP_OF_STACK( graphics->current_object ).model;

        if( object_index >= 0 && object_index < model->n_objects )
        {
            n_objects = 1;
            *object_list = &model->object_list[object_index];
        }
        else
        {
            n_objects = 0;
            *object_list = (object_struct *) 0;
        }
    }

    return( n_objects );
}

public  Status  push_current_object( graphics )
    graphics_struct   *graphics;
{
    Status            status;
    selection_entry   entry;
    int               n_objects;
    object_struct     *object_list;

    status = OK;

    if( current_object_is_top_level( graphics ) )
    {
        entry.object_index = 0;
        entry.model = &graphics->models[THREED_MODEL];

        PUSH_STACK( status, graphics->current_object, selection_entry, entry );
    }
    else
    {
        n_objects = get_current_objects( graphics, &object_list );

        if( n_objects == 1 && object_list[0].object_type == MODEL )
        {
            entry.object_index = 0;
            entry.model = object_list[0].ptr.model;

            PUSH_STACK( status, graphics->current_object, selection_entry,
                        entry );
        }
    }

    return( status );
}

public  Boolean  current_object_is_top_level( graphics )
    graphics_struct   *graphics;
{
    return( IS_STACK_EMPTY(graphics->current_object) );
}

public  Status  pop_current_object( graphics )
    graphics_struct   *graphics;
{
    Status            status;
    selection_entry   entry;

    if( !IS_STACK_EMPTY(graphics->current_object) )
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
