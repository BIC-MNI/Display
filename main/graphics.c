
#include  <def_stdio.h>
#include  <def_string.h>
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_alloc.h>

private  graphics_struct  **windows;
private  int              n_windows = 0;

public  int  get_list_of_windows( graphics )
    graphics_struct  ***graphics;
{
    *graphics = windows;

    return( n_windows );
}

public  graphics_struct  *lookup_window( window_id )
    Window_id   window_id;
{
    int              i;
    graphics_struct  *graphics;

    graphics = (graphics_struct *) 0;

    for_less( i, 0, n_windows )
    {
        if( windows[i]->window.window_id == window_id )
        {
            graphics = windows[i];
            break;
        }
    }

    return( graphics );
}

private  Status  get_new_graphics( graphics )
    graphics_struct   **graphics;
{
    Status   status;

    CHECK_ALLOC1( status, windows, n_windows, n_windows+1, graphics_struct *,
                  DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        CALLOC1( status, windows[n_windows], 1, graphics_struct );
    }

    if( status == OK )
    {
        *graphics = windows[n_windows];
        ++n_windows;
    }

    return( status );
}

private  Status  free_graphics( graphics )
    graphics_struct   *graphics;
{
    int      ind, i;
    Status   status;

    status = ERROR;

    for_less( ind, 0, n_windows )
    {
        if( windows[ind] == graphics )
        {
            status = OK;
            break;
        }
    }

    if( status == OK )
    {
        for_less( i, ind, n_windows-1 )
        {
            windows[i] = windows[i+1];
        }

        --n_windows;

        FREE1( status, graphics );
    }

    return( status );
}

public  Status  create_graphics_window( graphics, title, width, height )
    graphics_struct   **graphics;
    char              title[];
    int               width, height;
{
    Status   status;
    Status   initialize_graphics_window();
    Status   get_new_graphics();
    Status   G_create_window();

    status = get_new_graphics( graphics );

    if( status == OK )
    {
        status = G_create_window( title, width, height, &(*graphics)->window );
    }

    if( status == OK )
    {
        status = initialize_graphics_window( *graphics );
    }
    else
    {
        PRINT_ERROR( "Cannot open window.\n" );
    }

    return( status );
}

public  model_struct  *get_graphics_model( graphics, model_index )
    graphics_struct   *graphics;
    int               model_index;
{
    return( graphics->models[model_index]->ptr.model );
}

private  Status  initialize_graphics_window( graphics )
    graphics_struct   *graphics;
{
    static         Vector    line_of_sight = { 0.0, 0.0, -1.0 };
    static         Vector    horizontal = { 1.0, 0.0, 0.0 };
    int            i;
    void           initialize_view();
    void           adjust_view_for_aspect();
    void           G_define_view();
    void           initialize_lights();
    void           G_define_light();
    void           G_set_light_state();
    void           initialize_action_table();
    void           initialize_virtual_spaceball();
    void           initialize_window_events();
    void           initialize_mouse_events();
    void           initialize_render();
    void           initialize_objects();
    Status         status;
    Status         create_object();
    Status         initialize_current_object();
    model_struct   *model;
    model_struct   *get_graphics_model();

    initialize_view( &graphics->view, &line_of_sight, &horizontal );
    adjust_view_for_aspect( &graphics->view, &graphics->window );
    G_define_view( &graphics->window, &graphics->view );

    initialize_lights( graphics->lights );

    graphics->point_position.line_active = FALSE;

    G_define_light( &graphics->window, &graphics->lights[0], 0 );
    G_define_light( &graphics->window, &graphics->lights[1], 1 );

    G_set_light_state( &graphics->window, 0, graphics->lights[0].state );
    G_set_light_state( &graphics->window, 1, graphics->lights[1].state );

    initialize_action_table( &graphics->action_table );

    initialize_mouse_events( graphics );
    initialize_virtual_spaceball( graphics );
    initialize_window_events( graphics );

    status = OK;

    for_less( i, 0, N_MODELS )
    {
        if( status == OK )
        {
            status = create_object( &graphics->models[i], MODEL );
        }

        if( status == OK )
        {
            model = get_graphics_model( graphics, i );

            model->n_objects = 0;
            (void) strcpy( model->filename, "Top Level" );

            initialize_render( &model->render );
        }
    }

    if( status == OK )
    {
        status = initialize_current_object( graphics );
    }

    graphics->frame_number = 0;
    graphics->update_required = FALSE;
    graphics->update_interrupted.last_was_interrupted = FALSE;

    return( status );
}

private  void  display_frame_info( graphics, frame_number, update_time )
    graphics_struct   *graphics;
    int               frame_number;
    Real              update_time;
{
    void          G_set_view_type();
    void          G_draw_text();
    text_struct   frame_text;
    String        frame_time_str;
    void          format_time();
    model_struct  *model;
    model_struct  *get_graphics_model();

    (void) sprintf( frame_text.text, "%d: ", frame_number );

    format_time( frame_time_str, "%g %s", update_time );

    (void) strcat( frame_text.text, frame_time_str );

    fill_Point( frame_text.origin, Frame_info_x, Frame_info_y, 0.0 );
    fill_Colour( frame_text.colour, 1.0, 1.0, 1.0 );

    G_set_view_type( &graphics->window, PIXEL_VIEW );

    model = get_graphics_model( graphics, THREED_MODEL );

    G_draw_text( &graphics->window, &frame_text, &model->render );
}

public  void  update_graphics( graphics, interrupt )
    graphics_struct              *graphics;
    update_interrupted_struct    *interrupt;
{
    int           i;
    void          G_update_window();
    void          G_append_to_last_update();
    void          display_objects();
    void          display_frame_info();
    void          format_time();
    Real          start, end;
    Real          current_realtime_seconds();

    if( interrupt->last_was_interrupted )
    {
        G_append_to_last_update( &graphics->window );
    }

    start = current_realtime_seconds();

    interrupt->interrupt_at = start + Maximum_display_time;

    interrupt->current_interrupted = FALSE;

    for_less( i, 0, N_MODELS )
    {
        display_objects( &graphics->window, graphics->models[i],
                         interrupt );

        if( interrupt->current_interrupted )
        {
            break;
        }
    }

    interrupt->last_was_interrupted = interrupt->current_interrupted;

    end = current_realtime_seconds();

    ++graphics->frame_number;

    if( !interrupt->current_interrupted && Display_frame_info )
    {
        display_frame_info( graphics, graphics->frame_number, end - start );
    }

    G_update_window( &graphics->window );

    graphics->update_required = FALSE;
}

public  Status  delete_graphics_window( graphics )
    graphics_struct   *graphics;
{
    Status   status;
    Status   G_delete_window();
    Status   free_graphics();
    Status   terminate_graphics_window();

    status = G_delete_window( &graphics->window );

    if( status == OK )
    {
        status = terminate_graphics_window( graphics );
    }

    if( status == OK )
    {
        status = free_graphics( graphics );
    }

    return( status );
}

private  Status  terminate_graphics_window( graphics )
    graphics_struct   *graphics;
{
    int      i;
    Status   status;
    Status   delete_object();
    Status   delete_menu();
    Status   terminate_current_object();

    status = OK;

    for_less( i, 0, N_MODELS )
    {
        if( status == OK )
        {
            status = delete_object( graphics->models[i] );
        }
    }

    if( status == OK )
    {
        status = terminate_current_object( graphics );
    }

    if( status == OK && graphics == graphics->menu_window )
    {
        status = delete_menu( &graphics->menu );
    }

    return( status );
}

public  void  update_view( graphics )
    graphics_struct  *graphics;
{
    void   G_define_view();

    G_define_view( &graphics->window, &graphics->view );
}

public  void  reset_view_parameters( graphics, line_of_sight, horizontal )
    graphics_struct  *graphics;
    Vector           *line_of_sight;
    Vector           *horizontal;
{
    void   adjust_view_for_aspect();
    void   initialize_view();
    void   set_model_scale();
    void   fit_view_to_domain();

    initialize_view( &graphics->view, line_of_sight, horizontal );
    set_model_scale( &graphics->view,
                     Initial_x_scale, Initial_y_scale, Initial_z_scale );
    adjust_view_for_aspect( &graphics->view, &graphics->window );
    fit_view_to_domain( &graphics->view, &graphics->min_limit,
                        &graphics->max_limit );
}

public  void  transform_model( graphics, transform )
    graphics_struct   *graphics;
    Transform         *transform;
{
    void  concat_transforms();

    concat_transforms( &graphics->view.modeling_transform,
                       &graphics->view.modeling_transform,
                       transform );
}

public  Status  load_graphics_file( graphics, filename )
    graphics_struct  *graphics;
    char             filename[];
{
    Status           status;
    Status           input_graphics_file();
    Status           create_object();
    Status           push_current_object();
    Status           add_object_to_model();
    object_struct    *object;
    model_struct     *model;
    model_struct     *get_current_model();
    Boolean          get_range_of_object();
    void             rebuild_selected_list();
    void             set_current_object_index();

    status = create_object( &object, MODEL );

    if( status == OK )
    {
        PRINT( "Inputting objects.\n" );

        model = object->ptr.model;

        (void) strcpy( model->filename, filename );

        status = input_graphics_file( filename,
                                      &model->n_objects,
                                      &model->object_list );

        PRINT( "Objects input.\n" );
    }

    if( status == OK )
    {
        if( !Visibility_on_input )
        {
            BEGIN_TRAVERSE_OBJECT( status, object );
                OBJECT->visibility = OFF;
            END_TRAVERSE_OBJECT
        }
    }

    if( status == OK )
    {
        model = get_current_model( graphics );

        status = add_object_to_model( model, object );

        if( current_object_is_top_level(graphics) )
        {
            if( model->n_objects == 1 )               /* first object */
            {
                status = push_current_object( graphics );
            }
        }
        else
        {
            set_current_object_index( graphics, model->n_objects-1 );
        }
    }

    if( status == OK )
    {
        rebuild_selected_list( graphics, graphics->menu_window );
    }

    if( status == OK )
    {
        if( !get_range_of_object( graphics->models[THREED_MODEL], FALSE,
                                  &graphics->min_limit, &graphics->max_limit ) )
        {
            fill_Point( graphics->min_limit, 0.0, 0.0, 0.0 );
            fill_Point( graphics->max_limit, 1.0, 1.0, 1.0 );
            PRINT( "No objects range.\n" );
        }

        ADD_POINTS( graphics->centre_of_objects, graphics->min_limit,
                    graphics->max_limit );
        SCALE_POINT( graphics->centre_of_objects,
                     graphics->centre_of_objects,
                     0.5 );
    }

    graphics->update_required = TRUE;

    return( status );
}
