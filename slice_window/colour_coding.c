
#include  <display.h>

#define    MAX_LABEL_COLOUR_TABLE_SIZE    2000000

public  void  initialize_slice_colour_coding(
    display_struct    *slice_window )
{
    int   label;

    initialize_colour_coding( &slice_window->slice.colour_coding,
                              (Colour_coding_types) Initial_colour_coding_type,
                              Colour_below, Colour_above,
                              0.0, 1.0 );

    initialize_colour_bar( slice_window );

    slice_window->slice.label_colour_ratio = Label_colour_display_ratio;
    slice_window->slice.n_labels = NUM_LABELS;

    for_less( label, 0, NUM_LABELS )
        slice_window->slice.colour_tables[label] = (Colour *) 0;
}

public  int  get_num_labels(
    display_struct   *slice_window )
{
    if( slice_window == NULL )
        return( NUM_LABELS );
    else
        return( slice_window->slice.n_labels );
}

public  void  set_colour_coding_for_new_volume(
    display_struct    *slice_window )
{
    Real             low_limit, high_limit;
    Real             min_voxel, max_voxel;
    Real             min_value, max_value;
    int              label, ind, n_labels;
    Volume           volume;
    static Colour    default_colours[] = { RED, GREEN, BLUE,
                                           CYAN, MAGENTA, YELLOW,
                                           BLUE_VIOLET, DEEP_PINK,
                                           GREEN_YELLOW, LIGHT_SEA_GREEN,
                                           MEDIUM_TURQUOISE, PURPLE };

    (void) get_slice_window_volume( slice_window, &volume );

    for_less( label, 0, NUM_LABELS )
        slice_window->slice.colour_tables[label] = (Colour *) 0;

    get_volume_voxel_range( volume, &min_voxel, &max_voxel );

    n_labels = MAX_LABEL_COLOUR_TABLE_SIZE /
               ((int) max_voxel - (int) min_voxel + 1);

    if( n_labels < 1 )
        n_labels = 1;
    else if( n_labels > NUM_LABELS )
        n_labels = NUM_LABELS;

    slice_window->slice.n_labels = n_labels;

    set_colour_of_label( slice_window, 0, WHITE );

    for_less( label, 1, n_labels )
    {
        ind = (label - 1) % SIZEOF_STATIC_ARRAY(default_colours);
        set_colour_of_label( slice_window, label, default_colours[ind] );
    }

    for_less( label, n_labels, NUM_LABELS )
    {
        slice_window->slice.colour_tables[label] = 
             slice_window->slice.colour_tables[0];
    }

    get_volume_real_range( volume, &min_value, &max_value );

    rebuild_colour_tables( slice_window );

    low_limit = min_value + Initial_low_limit_position *
                (max_value - min_value);
    high_limit = min_value + Initial_high_limit_position *
                 (max_value - min_value);
    change_colour_coding_range( slice_window, low_limit, high_limit );

    rebuild_colour_bar( slice_window );
}

public  void  delete_slice_colour_coding(
    slice_window_struct   *slice )
{
    int      i;
    Real     min_voxel, max_voxel;
    Colour   *ptr;

    for_less( i, 0, slice->n_labels )
    {
        if( slice->colour_tables[i] != (Colour *) 0 )
        {
            get_volume_voxel_range( slice->volume, &min_voxel, &max_voxel );

            ptr = slice->colour_tables[i];
            ptr += (int) min_voxel;
            FREE( ptr );
        }
    }
}

public  void  change_colour_coding_range(
    display_struct    *slice_window,
    Real              min_value,
    Real              max_value )
{
    set_colour_coding_min_max( &slice_window->slice.colour_coding,
                               min_value, max_value );

    colour_coding_has_changed( slice_window );
}

private  void  create_colour_table_for_label(
    display_struct    *slice_window,
    int               label )
{
    Real        min_voxel, max_voxel;
    Colour      *ptr;

    get_volume_voxel_range( slice_window->slice.volume,
                            &min_voxel, &max_voxel );

    ALLOC( ptr, (int) max_voxel - (int) min_voxel + 1 );

    slice_window->slice.colour_tables[label] = ptr - (int) min_voxel;
}

public  void   set_colour_of_label(
    display_struct    *slice_window,
    int               label,
    Colour            colour )
{
    slice_window->slice.label_colours[label] = colour;

    if( slice_window->slice.colour_tables[label] == (Colour *) 0 )
        create_colour_table_for_label( slice_window, label );

    rebuild_colour_table_for_label( slice_window, label );
}

private  Colour  apply_label_colour(
    display_struct    *slice_window,
    Colour            col,
    int               label )
{
    Colour           label_col, mult, scaled_col;

    if( label != 0 )
    {
        label_col = slice_window->slice.label_colours[label];
        MULT_COLOURS( mult, label_col, col );
        mult = SCALE_COLOUR( mult, 1.0-slice_window->slice.label_colour_ratio);
        scaled_col = SCALE_COLOUR( label_col,
                                   slice_window->slice.label_colour_ratio);
        ADD_COLOURS( col, mult, scaled_col );
    }

    return( col );
}

private  Colour  get_slice_colour_coding(
    display_struct    *slice_window,
    Real              value,
    int               label )
{
    Colour           col;

    col = get_colour_code( &slice_window->slice.colour_coding, value );

    if( label > 0 )
        col = apply_label_colour( slice_window, col, label );

    return( col );
}

public  void  rebuild_colour_table_for_label(
    display_struct    *slice_window,
    int               label )
{
    int              voxel;
    Real             value;
    Colour           colour;
    Real             min_voxel, max_voxel;

    if( label >= get_num_labels(slice_window) )
        return;

    get_volume_voxel_range( get_volume(slice_window), &min_voxel, &max_voxel );

    for_inclusive( voxel, (int) min_voxel, (int) max_voxel )
    {
        if( label == 0 )
        {
            value = CONVERT_VOXEL_TO_VALUE( get_volume(slice_window), voxel );
            colour = get_slice_colour_coding( slice_window, value, label );
        }
        else
            colour = apply_label_colour( slice_window,
                           slice_window->slice.colour_tables[0][voxel], label );

        slice_window->slice.colour_tables[label][voxel] = colour;
    }
}

public  void  rebuild_colour_tables(
    display_struct    *slice_window )
{
    int              label;

    for_less( label, 0, get_num_labels(slice_window) )
    {
        if( slice_window->slice.colour_tables[label] != (Colour *) 0 )
            rebuild_colour_table_for_label( slice_window, label );
    }
}

public  void  colour_coding_has_changed(
    display_struct    *display )
{
    display_struct    *slice_window;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window != (display_struct  *) 0 )
    {
        rebuild_colour_tables( slice_window );

        rebuild_colour_bar( slice_window );
        set_slice_window_all_update( slice_window );
    }
}

private  void  colour_code_points(
    display_struct        *slice_window,
    int                   continuity,
    Colour_flags          *colour_flag,
    Colour                *colours[],
    int                   n_points,
    Point                 points[] )
{
    int      i, int_voxel[MAX_DIMENSIONS], label;
    Real     val, voxel[MAX_DIMENSIONS];
    Volume   volume, label_volume;

    if( *colour_flag != PER_VERTEX_COLOURS )
    {
        if( n_points > 0 )
        {
            REALLOC( *colours, n_points );
        }
        else
        {
            FREE( *colours );
        }
        *colour_flag = PER_VERTEX_COLOURS;
    }

    volume = get_volume( slice_window );
    label_volume = get_label_volume( slice_window );

    for_less( i, 0, n_points )
    {
        evaluate_3D_volume_in_world( volume,
                                     Point_x(points[i]),
                                     Point_y(points[i]),
                                     Point_z(points[i]), continuity,
                                     &val, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL );

        if( slice_window->slice.display_labels && label_volume->data != NULL )
        {
            convert_world_to_voxel( volume,
                                    Point_x(points[i]),
                                    Point_y(points[i]),
                                    Point_z(points[i]), voxel );
            convert_real_to_int_voxel( get_volume_n_dimensions(volume),
                                       voxel, int_voxel );

            label = get_volume_label_data( label_volume, int_voxel );
        }
        else
            label = 0;

        (*colours)[i] = get_slice_colour_coding( slice_window, val, label );
    }
}

private  void  colour_code_object_points(
    display_struct         *slice_window,
    int                    continuity,
    object_struct          *object )
{
    polygons_struct         *polygons;
    quadmesh_struct         *quadmesh;
    lines_struct            *lines;

    switch( object->object_type )
    {
    case POLYGONS:
        polygons = get_polygons_ptr( object );
        colour_code_points( slice_window, continuity,
                            &polygons->colour_flag, &polygons->colours,
                            polygons->n_points, polygons->points );
        break;

    case QUADMESH:
        quadmesh = get_quadmesh_ptr( object );
        colour_code_points( slice_window, continuity,
                            &quadmesh->colour_flag, &quadmesh->colours,
                            quadmesh->m * quadmesh->n,
                            quadmesh->points );
        break;

    case LINES:
        lines = get_lines_ptr( object );
        colour_code_points( slice_window, continuity,
                            &lines->colour_flag, &lines->colours,
                            lines->n_points, lines->points );
        break;
    }
}
public  void  colour_code_an_object(
    display_struct   *display,
    object_struct    *object )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window) )
    {
        colour_code_object_points( slice_window, Volume_continuity, object );
    }
}
