
#include  <def_graphics.h>

public  void  advance_current_object( graphics )
    graphics_struct   *graphics;
{
    int             object_index;
    model_struct    *model;

    if( !current_object_is_top_level(graphics) )
    {
        object_index = TOP_OF_STACK( graphics->three_d.current_object ).
                                          object_index;
        model = TOP_OF_STACK(graphics->three_d.current_object).
                                          model_object->ptr.model;

        if( model->n_objects > 0 )
        {
            TOP_OF_STACK( graphics->three_d.current_object ).object_index =
                 (object_index + 1) % model->n_objects;
        }
    }
}

public  void  retreat_current_object( graphics )
    graphics_struct   *graphics;
{
    int            object_index;
    model_struct   *model;

    if( !current_object_is_top_level(graphics) )
    {
        object_index = TOP_OF_STACK( graphics->three_d.current_object ).
                                   object_index;
        model = TOP_OF_STACK(graphics->three_d.current_object).
                                   model_object->ptr.model;

        if( model->n_objects > 0 )
        {
            TOP_OF_STACK( graphics->three_d.current_object ).object_index =
                 (object_index - 1 + model->n_objects) % model->n_objects;
        }
    }
}

public  object_struct  *get_current_model_object( graphics )
    graphics_struct   *graphics;
{
    object_struct   *model_object;

    if( current_object_is_top_level(graphics) )
    {
        model_object = graphics->models[THREED_MODEL];
    }
    else
    {
        model_object = TOP_OF_STACK(graphics->three_d.current_object).
                                            model_object;
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
    int  index;

    if( current_object_is_top_level(graphics) )
    {
        index = -1;
    }
    else
    {
        index = TOP_OF_STACK( graphics->three_d.current_object ).object_index;
    }

    return( index );
}

public  void  set_current_object_index( graphics, index )
    graphics_struct   *graphics;
    int               index;
{
    if( !current_object_is_top_level(graphics) )
    {
        if( index == 0 ||
            (index >= 0 &&
             index < TOP_OF_STACK( graphics->three_d.current_object ).
                            model_object->ptr.model->n_objects) )
        {
            TOP_OF_STACK( graphics->three_d.current_object ).object_index =
                                                             index;
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
    int             object_index;
    Boolean         current_object_exists;
    model_struct    *model;

    if( current_object_is_top_level(graphics) )
    {
        *current_object = graphics->models[THREED_MODEL];
        current_object_exists = TRUE;
    }
    else
    {
        object_index = TOP_OF_STACK( graphics->three_d.current_object ).
                                   object_index;
        model = TOP_OF_STACK(graphics->three_d.current_object).
                                   model_object->ptr.model;

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
    INITIALIZE_STACK( *current_object );

    return( OK );
}

public  Status  terminate_current_object( current_object )
    selection_struct   *current_object;
{
    Status  status;

    DELETE_STACK( status, *current_object );

    return( status );
}

public  Status  push_current_object( graphics )
    graphics_struct   *graphics;
{
    Status            status;
    selection_entry   entry;
    object_struct     *current_object;

    status = OK;

    if( get_current_object( graphics, &current_object ) &&
        current_object->object_type == MODEL )
    {
        entry.object_index = 0;
        entry.model_object = current_object;

        PUSH_STACK( status, graphics->three_d.current_object, entry );
    }

    return( status );
}

public  Boolean  current_object_is_top_level( graphics )
    graphics_struct   *graphics;
{
    return( IS_STACK_EMPTY(graphics->three_d.current_object) );
}

public  Status  pop_current_object( graphics )
    graphics_struct   *graphics;
{
    selection_entry   entry;

    if( !current_object_is_top_level(graphics) )
    {
        POP_STACK( graphics->three_d.current_object, entry );
    }

#ifdef lint
    if( entry.object_index )   {}
#endif

    return( OK );
}
