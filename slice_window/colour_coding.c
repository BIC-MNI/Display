
#include  <display.h>

#define    MAX_LABEL_COLOUR_TABLE_SIZE    2000000

private  void  rebuild_colour_tables(
    display_struct    *slice_window );

public  void  initialize_slice_colour_coding(
    display_struct    *slice_window )
{
    initialize_colour_coding( &slice_window->slice.colour_coding,
                              (Colour_coding_types) Initial_colour_coding_type,
                              Colour_below, Colour_above,
                              0.0, 1.0 );

    initialize_colour_bar( slice_window );

    slice_window->slice.label_colour_ratio = Label_colour_display_ratio;
    slice_window->slice.n_labels = Initial_num_labels;
    slice_window->slice.n_distinct_colour_tables = 0;

    slice_window->slice.colour_tables = (Colour **) 0;
    slice_window->slice.label_colours = (Colour *) 0;
    slice_window->slice.labels = (Volume) NULL;
    slice_window->slice.offset = 0;
}

public  Volume  get_label_volume(
    display_struct   *display )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        return( slice_window->slice.labels );
    else
        return( (Volume) NULL );
}

public  int  get_num_labels(
    display_struct   *display )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) == NULL )
        return( Initial_num_labels );
    else
        return( slice_window->slice.n_labels );
}

public  void  delete_slice_colour_coding(
    slice_window_struct   *slice )
{
    int      i;
    Colour   *ptr;

    if( slice->n_distinct_colour_tables == 0 )
        return;

    delete_volume( slice->labels );
    slice->labels = NULL;

    for_less( i, 0, slice->n_distinct_colour_tables )
    {
        ptr = slice->colour_tables[i];
        ptr += (int) slice->offset;
        FREE( ptr );
    }

    FREE( slice->colour_tables );
    FREE( slice->label_colours );
}

private  void  alloc_colour_table_for_label(
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

private  void  realloc_colour_maps(
    display_struct    *slice_window )
{
    int              i, ind, n_indiv_tables, n_voxel_values;
    Real             min_voxel, max_voxel;
    int              label, n_labels;
    Volume           volume;
    static Colour    default_colours[] = { RED, GREEN, BLUE,
                                           CYAN, MAGENTA, YELLOW,
                                           BLUE_VIOLET, DEEP_PINK,
                                           GREEN_YELLOW, LIGHT_SEA_GREEN,
                                           MEDIUM_TURQUOISE, PURPLE };

    (void) get_slice_window_volume( slice_window, &volume );

    n_labels = get_num_labels( slice_window );

    ALLOC( slice_window->slice.label_colours, n_labels );
    ALLOC( slice_window->slice.colour_tables, n_labels );

    get_volume_voxel_range( volume, &min_voxel, &max_voxel );
    n_voxel_values = (int) max_voxel - (int) min_voxel + 1;

    n_indiv_tables = MAX_LABEL_COLOUR_TABLE_SIZE / n_voxel_values;

    if( n_indiv_tables < 1 )
        n_indiv_tables = 1;
    else if( n_indiv_tables > n_labels )
        n_indiv_tables = n_labels;

    slice_window->slice.n_distinct_colour_tables = n_indiv_tables;

    for_less( i, 0, n_labels )
    {
        if( i < n_indiv_tables )
            alloc_colour_table_for_label( slice_window, i );
        else
            slice_window->slice.colour_tables[i] = 
                         slice_window->slice.colour_tables[i % n_indiv_tables];
    }

    set_colour_of_label( slice_window, 0, WHITE );

    for_less( label, 1, n_indiv_tables )
    {
        ind = (label - 1) % SIZEOF_STATIC_ARRAY(default_colours);
        set_colour_of_label( slice_window, label, default_colours[ind] );
    }

    rebuild_colour_tables( slice_window );
}

private  void  create_colour_coding(
    display_struct    *slice_window )
{
    nc_type               type;
    slice_window_struct   *slice;

    slice = &slice_window->slice;

    if( slice->n_labels <= (1 << 8) )
        type = NC_BYTE;
    else if( slice->n_labels <= (1 << 16) )
        type = NC_SHORT;
    else
        type = NC_LONG;

    slice->labels = create_label_volume( get_volume(slice_window), type );
    slice->offset = (int) get_volume_voxel_min( get_volume( slice_window ) );

    realloc_colour_maps( slice_window );
}

public  void  set_slice_window_number_labels(
    display_struct    *slice_window,
    int               n_labels )
{
    delete_slice_colour_coding( &slice_window->slice );

    slice_window->slice.n_labels = n_labels;

    create_colour_coding( slice_window );
}

public  void  set_colour_coding_for_new_volume(
    display_struct    *slice_window )
{
    Real             low_limit, high_limit;
    Real             min_value, max_value;
    Volume           volume;

    delete_slice_colour_coding( &slice_window->slice );

    (void) get_slice_window_volume( slice_window, &volume );

    create_colour_coding( slice_window );

    get_volume_real_range( volume, &min_value, &max_value );

    low_limit = min_value + Initial_low_limit_position *
                (max_value - min_value);
    high_limit = min_value + Initial_high_limit_position *
                 (max_value - min_value);

    change_colour_coding_range( slice_window, low_limit, high_limit );

    rebuild_colour_bar( slice_window );
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

private  void  rebuild_colour_table_for_label(
    display_struct    *slice_window,
    int               label )
{
    int              voxel, used_label;
    Real             value;
    Colour           colour;
    Real             min_voxel, max_voxel;

    if( label >= get_num_labels(slice_window) )
        return;

    used_label = label % slice_window->slice.n_distinct_colour_tables;

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

        slice_window->slice.colour_tables[used_label][voxel] = colour;
    }
}

private  void  rebuild_colour_tables(
    display_struct    *slice_window )
{
    int              label;

    for_less( label, 0, slice_window->slice.n_distinct_colour_tables )
        rebuild_colour_table_for_label( slice_window, label );
}

public  void   set_colour_of_label(
    display_struct    *slice_window,
    int               label,
    Colour            colour )
{
    if( label >= 0 && label < slice_window->slice.n_labels )
    {
        slice_window->slice.label_colours[label] = colour;

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

public  void  change_colour_coding_range(
    display_struct    *slice_window,
    Real              min_value,
    Real              max_value )
{
    set_colour_coding_min_max( &slice_window->slice.colour_coding,
                               min_value, max_value );

    colour_coding_has_changed( slice_window );
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
    Colour                  *colours;
    Colour_flags            colour_flag;
    polygons_struct         *polygons;
    quadmesh_struct         *quadmesh;
    lines_struct            *lines;
    marker_struct           *marker;

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

    case MARKER:
        marker = get_marker_ptr( object );
        colour_flag = PER_VERTEX_COLOURS;
        colours = &marker->colour;
        colour_code_points( slice_window, continuity,
                            &colour_flag, &colours, 1, &marker->position );
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
