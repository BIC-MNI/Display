
#include  <def_stdio.h>
#include  <def_string.h>
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_alloc.h>

private  graphics_struct  **windows = (graphics_struct **) 0;
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

private  Status  delete_graphics_list()
{
    Status    status;

    if( windows != (graphics_struct **) 0 )
    {
        FREE1( status, windows );
    }

    return( status );
}

public  Status  initialize_graphics()
{
    Status   status;
    Status   G_initialize();

    status = G_initialize();

    return( status );
}

public  Status  terminate_graphics()
{
    Status            status;
    Status            G_terminate();
    Status            delete_graphics_window();
    graphics_struct   **graphics_windows;

    status = OK;

    while( get_list_of_windows( &graphics_windows ) > 0 )
    {
        if( status == OK )
        {
            status = delete_graphics_window( graphics_windows[0] );
        }
    }

    if( status == OK )
    {
        status = delete_graphics_list();
    }

    if( status == OK )
    {
        status = G_terminate();
    }

    return( status );
}

public  Status  create_graphics_window( window_type, graphics,
                                        title, width, height )
    window_types      window_type;
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
        status = G_create_window( title, width, height, 
                                  &Initial_background_colour,
                                  &(*graphics)->window );
    }

    if( status == OK )
    {
        (*graphics)->window_type = window_type;
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

public  Status  create_model_after_current( graphics )
    graphics_struct   *graphics;
{
    Status         status;
    Status         create_object();
    Status         add_object_to_model();
    model_struct   *model;
    model_struct   *get_current_model();
    object_struct  *new_model;

    model = get_current_model( graphics );

    status = create_object( &new_model, MODEL );

    if( status == OK )
    {
        (void) strcpy( new_model->ptr.model->filename, "Created" );
    }

    if( status == OK )
    {
        status = add_object_to_model( model, new_model );
    }

    return( status );
}

private  Status  initialize_graphics_window( graphics )
    graphics_struct   *graphics;
{
    int            i;
    void           initialize_action_table();
    void           initialize_window_events();
    void           initialize_mouse_events();
    void           initialize_render();
    void           make_identity_transform();
    void           initialize_objects();
    void           initialize_menu_actions();
    Status         initialize_slice_window();
    Status         initialize_three_d_window();
    Status         status;
    Status         create_object();
    view_types     view_type;
    model_struct   *model;
    model_struct   *get_graphics_model();

    graphics->associated[THREE_D_WINDOW] = (graphics_struct *) 0;
    graphics->associated[MENU_WINDOW] = (graphics_struct *) 0;
    graphics->associated[SLICE_WINDOW] = (graphics_struct *) 0;

    initialize_action_table( &graphics->action_table );

    initialize_mouse_events( graphics );
    initialize_window_events( graphics );
    initialize_menu_actions( graphics );

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

            switch( graphics->window_type )
            {
            case THREE_D_WINDOW:   view_type = MODEL_VIEW;   break;
            case MENU_WINDOW:      view_type = PIXEL_VIEW;   break;
            case SLICE_WINDOW:     view_type = PIXEL_VIEW;   break;
            }

            model->view_type = view_type;
            model->n_objects = 0;
            (void) strcpy( model->filename, "Top Level" );

            initialize_render( &model->render );
            make_identity_transform( &model->transform );
        }
    }

    if( status == OK && graphics->window_type == THREE_D_WINDOW )
    {
        status = initialize_three_d_window( graphics );
    }

    if( status == OK && graphics->window_type == SLICE_WINDOW )
    {
        status = initialize_slice_window( graphics );
    }

    if( status == OK )
    {
        graphics->frame_number = 0;
        graphics->update_required = FALSE;
        graphics->update_interrupted.last_was_interrupted = FALSE;
        graphics->update_interrupted.size_of_interrupted = Size_of_interrupted;
        graphics->update_interrupted.interval_of_check = Interval_of_check;
    }

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
    Status   delete_three_d();
    Status   delete_menu();
    Status   delete_slice_window();

    status = OK;

    for_less( i, 0, N_MODELS )
    {
        if( status == OK )
        {
            status = delete_object( graphics->models[i] );
        }
    }

    if( status == OK && graphics->window_type == THREE_D_WINDOW )
    {
        status = delete_three_d( graphics );
    }

    if( status == OK && graphics->window_type == MENU_WINDOW )
    {
        status = delete_menu( &graphics->menu );
    }

    if( status == OK && graphics->window_type == SLICE_WINDOW )
    {
        status = delete_slice_window( &graphics->slice );
    }

    return( status );
}

public  void  update_view( graphics )
    graphics_struct  *graphics;
{
    void   G_define_3D_view();

    G_define_3D_view( &graphics->window, &graphics->three_d.view );
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

    initialize_view( &graphics->three_d.view,
                     line_of_sight, horizontal );
    set_model_scale( &graphics->three_d.view,
                     Initial_x_scale, Initial_y_scale, Initial_z_scale );
    adjust_view_for_aspect( &graphics->three_d.view,
                            &graphics->window );
    fit_view_to_domain( &graphics->three_d.view,
                        &graphics->three_d.min_limit,
                        &graphics->three_d.max_limit );
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
    Status           initialize_cursor();

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
        rebuild_selected_list( graphics, graphics->associated[MENU_WINDOW] );
    }

    if( status == OK )
    {
        if( !get_range_of_object( graphics->models[THREED_MODEL], FALSE,
                                  &graphics->three_d.min_limit,
                                  &graphics->three_d.max_limit ) )
        {
            fill_Point( graphics->three_d.min_limit, 0.0, 0.0, 0.0 );
            fill_Point( graphics->three_d.max_limit, 1.0, 1.0, 1.0 );
            PRINT( "No objects range.\n" );
        }

        ADD_POINTS( graphics->three_d.centre_of_objects,
                    graphics->three_d.min_limit,
                    graphics->three_d.max_limit );
        SCALE_POINT( graphics->three_d.centre_of_objects,
                     graphics->three_d.centre_of_objects,
                     0.5 );

        status = initialize_cursor( graphics );
    }

    graphics->update_required = TRUE;

    return( status );
}

public  Real  size_of_domain( graphics )
    graphics_struct   *graphics;
{
    Vector   diff;

    SUB_POINTS( diff, graphics->three_d.max_limit,
                           graphics->three_d.min_limit );

    return( MAGNITUDE(diff) );
}
