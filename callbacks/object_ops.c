
#include  <def_graphics.h>
#include  <def_files.h>
#include  <def_string.h>

public  DEF_MENU_FUNCTION( reverse_normals )   /* ARGSUSED */
{
    object_struct   *current_object;
    Boolean         get_current_object();
    Status          status;
    Status          reverse_object_normals();
    void            set_update_required();

    status = OK;

    if( get_current_object( graphics, &current_object ) )
    {
        status = reverse_object_normals( current_object );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(reverse_normals )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( advance_visible )     /* ARGSUSED */
{
    object_struct    *current_object;
    Boolean          get_current_object();
    void             advance_current_object();
    void             graphics_models_have_changed();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = OFF;

        advance_current_object( graphics );

        if( get_current_object( graphics, &current_object ) )
            current_object->visibility = ON;

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(advance_visible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_visible )     /* ARGSUSED */
{
    object_struct    *current_object;
    Boolean          get_current_object();
    void             retreat_current_object();
    void             graphics_models_have_changed();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = OFF;

        retreat_current_object( graphics );

        if( get_current_object( graphics, &current_object ) )
            current_object->visibility = ON;

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_visible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_all_invisible )     /* ARGSUSED */
{
    Status           status;
    object_struct    *object, *current_object;
    Boolean          get_current_object();
    void             graphics_models_have_changed();
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();

    status = OK;

    if( get_current_object( graphics, &current_object ) )
    {
        status = initialize_object_traverse( &object_traverse, 1,
                                             &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
               object->visibility = FALSE;

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(make_all_invisible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_all_visible )     /* ARGSUSED */
{
    Status           status;
    object_struct    *current_object, *object;
    Boolean          get_current_object();
    void             graphics_models_have_changed();
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();

    if( get_current_object( graphics, &current_object ) )
    {
        status = initialize_object_traverse( &object_traverse, 1,
                                             &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
            object->visibility = TRUE;

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(make_all_visible )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( advance_selected )     /* ARGSUSED */
{
    void    advance_current_object();
    void    rebuild_selected_list();

    advance_current_object( graphics );

    rebuild_selected_list( graphics, menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(advance_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_selected )     /* ARGSUSED */
{
    void    retreat_current_object();
    void    rebuild_selected_list();

    retreat_current_object( graphics );

    rebuild_selected_list( graphics, menu_window );

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( descend_selected )     /* ARGSUSED */
{
    Status    status;
    Status    push_current_object();
    void      rebuild_selected_list();

    status = push_current_object( graphics );

    rebuild_selected_list( graphics, menu_window );

    return( status );
}

public  DEF_MENU_UPDATE(descend_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( ascend_selected )     /* ARGSUSED */
{
    Status    status;
    Status    pop_current_object();
    void      rebuild_selected_list();

    status = pop_current_object( graphics );

    rebuild_selected_list( graphics, menu_window );

    return( status );
}

public  DEF_MENU_UPDATE(ascend_selected )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_object_visibility )     /* ARGSUSED */
{
    object_struct    *current_object;
    Boolean          get_current_object();
    void             graphics_models_have_changed();

    if( get_current_object( graphics, &current_object ) )
    {
        current_object->visibility = !current_object->visibility;

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_object_visibility )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( create_model )     /* ARGSUSED */
{
    Status           status;
    Status           create_model_after_current();
    void             graphics_models_have_changed();

    status = create_model_after_current( graphics );

    graphics_models_have_changed( graphics );

    return( status );
}

public  DEF_MENU_UPDATE(create_model )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( change_model_name )     /* ARGSUSED */
{
    object_struct    *current_object;
    Boolean          get_current_object();
    Status           status;
    String           name;
    void             rebuild_selected_list();

    status = OK;

    if( get_current_object( graphics, &current_object ) &&
        current_object->object_type == MODEL )
    {
        PRINT( "Enter the new model name: " );

        if( input_string( stdin, name, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            (void) strcpy( current_object->ptr.model->filename, name );
        }

        (void) input_newline( stdin );

        rebuild_selected_list( graphics, menu_window );
    }

    return( status );
}

public  DEF_MENU_UPDATE(change_model_name )     /* ARGSUSED */
{
    return( OK );
}

private  Boolean  remove_current_object_from_hierarchy( graphics, object )
    graphics_struct  *graphics;
    object_struct    **object;
{
    Boolean          removed;
    int              obj_index;
    model_struct     *current_model;
    model_struct     *get_current_model();
    Boolean          get_current_object();
    Boolean          is_a_marker;
    void             regenerate_voxel_marker_labels();
    Status           status;
    void             graphics_models_have_changed();
    void             set_current_object_index();
    Status           remove_object_from_model();

    if( !current_object_is_top_level( graphics ) &&
        get_current_object( graphics, object ) )
    {
        is_a_marker = ((*object)->object_type == MARKER);

        obj_index = get_current_object_index( graphics );

        current_model = get_current_model( graphics );

        status = remove_object_from_model( current_model, obj_index );

        if( status == OK )
        {
            if( current_model->n_objects == 0 )
                obj_index = 0;
            else if( obj_index >= current_model->n_objects )
                obj_index = current_model->n_objects - 1;

            set_current_object_index( graphics, obj_index );
        }

        graphics_models_have_changed( graphics );

        if( is_a_marker )
            regenerate_voxel_marker_labels( graphics );

        removed = TRUE;
    }
    else
        removed = FALSE;

    return( removed );
}

public  DEF_MENU_FUNCTION( delete_current_object )     /* ARGSUSED */
{
    Status           status;
    object_struct    *object;
    Status           delete_object();

    status = OK;

    if( remove_current_object_from_hierarchy( graphics, &object ) )
        status = delete_object( object );

    return( status );
}

public  DEF_MENU_UPDATE(delete_current_object )     /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_current_object_colour )   /* ARGSUSED */
{
    Status          status;
    Status          convert_string_to_colour();
    object_struct   *current_object;
    Boolean         get_current_object();
    void            set_object_colour();
    void            set_update_required();
    void            rebuild_selected_list();
    Colour          col;
    String          line;

    status = OK;

    if( get_current_object( graphics, &current_object ) )
    {
        PRINT( "Enter colour name or r g b:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
            status = convert_string_to_colour( line, &col );

        if( status == OK )
        {
            set_object_colour( current_object, &col );

            if( current_object->object_type == MARKER )
                regenerate_voxel_marker_labels( graphics );

            set_update_required( graphics, NORMAL_PLANES );
            rebuild_selected_list( graphics, menu_window );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_current_object_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_current_object_surfprop )   /* ARGSUSED */
{
    Status          status;
    Status          convert_string_to_colour();
    object_struct   *current_object;
    Boolean         get_current_object();
    void            set_object_surfprop();
    void            set_update_required();
    void            rebuild_selected_list();
    Surfprop        spr;

    status = OK;

    if( get_current_object( graphics, &current_object ) )
    {
        PRINT( "Enter ambient, diffuse, specular, shininess, opacity:" );

        if( input_real( stdin, &Surfprop_a(spr) ) == OK &&
            input_real( stdin, &Surfprop_d(spr) ) == OK &&
            input_real( stdin, &Surfprop_s(spr) ) == OK &&
            input_real( stdin, &Surfprop_se(spr) ) == OK &&
            input_real( stdin, &Surfprop_t(spr) ) == OK )
        {
            set_object_surfprop( current_object, &spr );

            set_update_required( graphics, NORMAL_PLANES );
            rebuild_selected_list( graphics, menu_window );
        }

        (void) input_newline( stdin );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_current_object_surfprop )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( cut_object )   /* ARGSUSED */
{
    Status          status;
    object_struct   *object;
    model_struct    *cut_model;
    Status          add_object_to_model();
    model_struct    *get_graphics_model();

    status = OK;

    if( remove_current_object_from_hierarchy( graphics, &object ) )
    {
        cut_model = get_graphics_model( graphics, CUT_BUFFER_MODEL );

        status = add_object_to_model( cut_model, object );
    }

    return( status );
}

public  DEF_MENU_UPDATE(cut_object )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paste_object )   /* ARGSUSED */
{
    Status          status;
    Status          add_object_to_model();
    Status          remove_object_from_model();
    int             n_objects;
    object_struct   *object;
    model_struct    *cut_model, *current_model;
    model_struct    *get_current_model();
    model_struct    *get_graphics_model();
    void            graphics_models_have_changed();
    void            rebuild_selected_list();

    status = OK;

    cut_model = get_graphics_model( graphics, CUT_BUFFER_MODEL );
    current_model = get_current_model( graphics );
    n_objects = cut_model->n_objects;

    while( cut_model->n_objects > 0 )
    {
        object = cut_model->object_list[0];

        if( status == OK )
            status = add_object_to_model( current_model, object );

        if( status == OK )
            status = remove_object_from_model( cut_model, 0 );
    }

    if( n_objects > 0 )
    {
        graphics_models_have_changed( graphics );
        rebuild_selected_list( graphics, menu_window );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paste_object )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( mark_vertices )   /* ARGSUSED */
{
    Status         status;
    object_struct  *object;
    int            i, n_points;
    Point          *points;
    Status         create_marker_at_position();

    status = OK;

    if( get_current_object( graphics, &object ) &&
        object->object_type == LINES )
    {
        n_points = object->ptr.lines->n_points;
        points = object->ptr.lines->points;

        for_less( i, 0, n_points )
        {
            if( status == OK )
                status = create_marker_at_position( graphics, &points[i] );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(mark_vertices )   /* ARGSUSED */
{
    return( OK );
}
