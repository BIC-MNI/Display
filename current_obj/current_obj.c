
#include  <def_graphics.h>

public  Status  push_current_object( current_object, object_index, model )
    selection_struct   *current_object;
    int                object_index;
    model_struct       *model;
{
    Status            status;
    selection_entry   entry;

    entry.object_index = object_index;
    entry.model = model;

    PUSH_STACK( status, *current_object, selection_entry, entry );

    return( status );
}

public  Boolean  current_object_is_top_level( current_object )
    selection_struct   *current_object;
{
    return( N_ENTRIES_IN_STACK(*current_object) == 1 );
}

public  Status  pop_current_object( current_object, object_index, model )
    selection_struct   *current_object;
    int                *object_index;
    model_struct       **model;
{
    Status            status;
    selection_entry   entry;

    if( N_ENTRIES_IN_STACK(*current_object) > 1 )
    {
        POP_STACK( status, *current_object, entry );

        if( status == OK )
        {
            *object_index = entry.object_index;
            *model = entry.model;
        }
    }
    else
    {
        PRINT_ERROR( "pop current object" );
        status = ERROR;
    }

    return( status );
}
