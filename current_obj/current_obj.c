
#include  <def_graphics.h>

public  void  advance_current_object( graphics )
    graphics_struct   *graphics;
{
    int            object_index;
    model_struct   *model;
    void           rebuild_selected_list();

    if( !current_object_is_top_level(graphics) )
    {
        object_index = TOP_OF_STACK( graphics->current_object ).object_index;
        model = TOP_OF_STACK( graphics->current_object ).model;

        if( model->n_objects > 0 )
        {
            TOP_OF_STACK( graphics->current_object ).object_index =
                 (object_index + 1) % model->n_objects;
        }

        rebuild_selected_list( graphics, graphics->menu_window );
    }
}

public  void  retreat_current_object( graphics )
    graphics_struct   *graphics;
{
    int            object_index;
    model_struct   *model;
    void           rebuild_selected_list();

    if( !current_object_is_top_level(graphics) )
    {
        object_index = TOP_OF_STACK( graphics->current_object ).object_index;
        model = TOP_OF_STACK( graphics->current_object ).model;

        if( model->n_objects > 0 )
        {
            TOP_OF_STACK( graphics->current_object ).object_index =
                 (object_index - 1 + model->n_objects) % model->n_objects;
        }

        rebuild_selected_list( graphics, graphics->menu_window );
    }
}

public  model_struct  *get_current_model( graphics )
    graphics_struct   *graphics;
{
    model_struct   *model;

    if( current_object_is_top_level(graphics) )
    {
        model = graphics->models[THREED_MODEL]->ptr.model;
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

    if( current_object_is_top_level(graphics) )
    {
        index = -1;
    }
    else
    {
        index = TOP_OF_STACK( graphics->current_object ).object_index;
    }

    return( index );
}

public  object_struct  *get_current_object( graphics )
    graphics_struct   *graphics;
{
    int             object_index;
    model_struct    *model;
    object_struct   *object;

    if( current_object_is_top_level(graphics) )
    {
        object = graphics->models[THREED_MODEL];
    }
    else
    {
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
    }

    return( object );
}

public  Status  initialize_current_object( graphics )
    graphics_struct   *graphics;
{
    model_struct      *get_graphics_model();

    INITIALIZE_STACK( graphics->current_object );

    return( OK );
}

public  Status  push_current_object( graphics )
    graphics_struct   *graphics;
{
    Status            status;
    selection_entry   entry;
    object_struct     *current_object;
    void              rebuild_selected_list();

    status = OK;

    current_object = get_current_object( graphics );

    if( current_object != (object_struct *) 0 &&
        current_object->object_type == MODEL )
    {
        entry.object_index = 0;
        entry.model = current_object->ptr.model;

        PUSH_STACK( status, graphics->current_object, selection_entry,
                    entry );

        rebuild_selected_list( graphics, graphics->menu_window );
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
    selection_entry   entry;
    void           rebuild_selected_list();

    if( !current_object_is_top_level(graphics) )
    {
        POP_STACK( graphics->current_object, entry );

        rebuild_selected_list( graphics, graphics->menu_window );
    }

    return( OK );
}
