
#include  <display.h>

#define    MAX_LABEL_COLOUR_TABLE_SIZE    2000000

private  void  rebuild_colour_table(
    display_struct    *slice_window );

public  void  initialize_slice_colour_coding(
    display_struct    *slice_window )
{
    initialize_colour_coding( &slice_window->slice.colour_coding,
                              (Colour_coding_types) Initial_colour_coding_type,
                              Colour_below, Colour_above,
                              0.0, 1.0 );

    initialize_colour_bar( slice_window );

    slice_window->slice.label_colour_opacity = Label_colour_opacity;
    slice_window->slice.n_labels = Initial_num_labels;

    slice_window->slice.offset = 0;
    slice_window->slice.colour_table = (Colour *) 0;
    slice_window->slice.label_colour_table = (Colour *) 0;
    slice_window->slice.labels = (Volume) NULL;
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

public  BOOLEAN  label_volume_exists(
    display_struct   *display )
{
    Volume   label;

    label = get_label_volume( display );

    return( label != NULL && label->data != NULL );
}

public  BOOLEAN  get_label_visibility(
    display_struct    *slice_window,
    int               view_index )
{
    return( get_slice_visibility( slice_window, view_index ) &&
            slice_window->slice.display_labels &&
            label_volume_exists( slice_window ) );
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

private  void  delete_slice_labels(
    slice_window_struct   *slice )
{
    if( slice->labels == (Volume) NULL )
        return;

    delete_volume( slice->labels );
    slice->labels = NULL;

    FREE( slice->label_colour_table );
}

public  void  delete_slice_colour_coding(
    slice_window_struct   *slice )
{
    Colour   *ptr;

    if( slice->labels == (Volume) NULL )
        return;

    delete_slice_labels( slice );

    ptr = slice->colour_table;

    if( ptr != NULL )
    {
        ptr += (int) slice->offset;
        FREE( ptr );
    }
}

private  void  realloc_label_colour_table(
    display_struct    *slice_window )
{
    int              ind;
    int              label, n_labels;
    static Colour    default_colours[] = { RED, GREEN, BLUE,
                                           CYAN, MAGENTA, YELLOW,
                                           BLUE_VIOLET, DEEP_PINK,
                                           GREEN_YELLOW, LIGHT_SEA_GREEN,
                                           MEDIUM_TURQUOISE, PURPLE };

    n_labels = get_num_labels( slice_window );
    ALLOC( slice_window->slice.label_colour_table, n_labels );

    for_less( label, 1, n_labels )
    {
        ind = (label - 1) % SIZEOF_STATIC_ARRAY(default_colours);
        set_colour_of_label( slice_window, label, default_colours[ind] );
    }

    set_colour_of_label( slice_window, 0, make_rgba_Colour(0,0,0,0) );
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
    set_volume_voxel_range( slice->labels, 0.0, (Real) slice->n_labels-1.0 );
    realloc_label_colour_table( slice_window );
}

public  void  set_slice_window_number_labels(
    display_struct    *slice_window,
    int               n_labels )
{
    delete_slice_labels( &slice_window->slice );

    slice_window->slice.n_labels = n_labels;

    create_colour_coding( slice_window );
}

private  void  alloc_colour_table(
    display_struct    *slice_window )
{
    Real        min_voxel, max_voxel;
    Colour      *ptr;

    if( is_an_rgb_volume(get_volume(slice_window)) )
    {
        slice_window->slice.colour_table = NULL;
        return;
    }

    get_volume_voxel_range( slice_window->slice.volume,
                            &min_voxel, &max_voxel );

    ALLOC( ptr, (int) max_voxel - (int) min_voxel + 1 );

    slice_window->slice.offset = (int) min_voxel;
    slice_window->slice.colour_table = ptr - (int) min_voxel;
}

public  void  set_colour_coding_for_new_volume(
    display_struct    *slice_window )
{
    Real             low_limit, high_limit;
    Real             min_value, max_value;
    Volume           volume;

    delete_slice_colour_coding( &slice_window->slice );

    (void) get_slice_window_volume( slice_window, &volume );

    alloc_colour_table( slice_window );
    rebuild_colour_table( slice_window );
    create_colour_coding( slice_window );

    get_volume_real_range( volume, &min_value, &max_value );

    low_limit = min_value + Initial_low_limit_position *
                (max_value - min_value);
    high_limit = min_value + Initial_high_limit_position *
                 (max_value - min_value);

    change_colour_coding_range( slice_window, low_limit, high_limit );
}

private  Colour  apply_label_colour(
    display_struct    *slice_window,
    Colour            col,
    int               label )
{
    Real      r1, g1, b1, a1, r2, g2, b2, a2;
    Real      r, g, b, a;
    Colour    label_col;

    if( label != 0 )
    {
        label_col = slice_window->slice.label_colour_table[label];

        r1 = get_Colour_r_0_1(col);
        g1 = get_Colour_g_0_1(col);
        b1 = get_Colour_b_0_1(col);
        a1 = get_Colour_a_0_1(col);

        r2 = get_Colour_r_0_1(label_col);
        g2 = get_Colour_g_0_1(label_col);
        b2 = get_Colour_b_0_1(label_col);
        a2 = get_Colour_a_0_1(label_col);

        r = a1 * r1 * (1.0 - a2) + a2 * r2;
        g = a1 * g1 * (1.0 - a2) + a2 * g2;
        b = a1 * b1 * (1.0 - a2) + a2 * b2;
        a = a1 * (1.0 - a2) + a2 * a2;

        col = make_rgba_Colour_0_1( r, g, b, a );
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

private  void  rebuild_colour_table(
    display_struct    *slice_window )
{
    int              voxel;
    Real             value;
    Colour           colour;
    Real             min_voxel, max_voxel;

    if( is_an_rgb_volume(get_volume(slice_window)) )
        return;

    get_volume_voxel_range( get_volume(slice_window), &min_voxel, &max_voxel );

    for_inclusive( voxel, (int) min_voxel, (int) max_voxel )
    {
        value = CONVERT_VOXEL_TO_VALUE( get_volume(slice_window), voxel );
        colour = get_colour_code( &slice_window->slice.colour_coding, value );

        slice_window->slice.colour_table[voxel] = colour;
    }
}

public  void   set_colour_of_label(
    display_struct    *slice_window,
    int               label,
    Colour            colour )
{
    Real  r, g, b;

    if( get_Colour_a(colour) == 255 )
    {
        r = get_Colour_r_0_1( colour );
        g = get_Colour_g_0_1( colour );
        b = get_Colour_b_0_1( colour );

        colour = make_rgba_Colour_0_1( r, g, b,
                                     slice_window->slice.label_colour_opacity );
    }

    slice_window->slice.label_colour_table[label] = colour;
}

public  Colour   get_colour_of_label(
    display_struct    *slice_window,
    int               label )
{
    return( slice_window->slice.label_colour_table[label] );
}

public  void   set_label_opacity(
    display_struct   *slice_window,
    Real             opacity )
{
    int    i;
    Real   r, g, b;

    slice_window->slice.label_colour_opacity = opacity;

    for_less( i, 1, get_num_labels(slice_window) )
    {
        r = get_Colour_r_0_1( slice_window->slice.label_colour_table[i] );
        g = get_Colour_g_0_1( slice_window->slice.label_colour_table[i] );
        b = get_Colour_b_0_1( slice_window->slice.label_colour_table[i] );
        slice_window->slice.label_colour_table[i] = make_rgba_Colour_0_1(
                      r, g, b, opacity );
    }

    colour_coding_has_changed( slice_window, UPDATE_LABELS );
}

public  void  colour_coding_has_changed(
    display_struct    *display,
    Update_types      type )
{
    display_struct    *slice_window;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window != (display_struct  *) 0 )
    {
        if( type == UPDATE_SLICE || type == UPDATE_BOTH )
        {
            rebuild_colour_table( slice_window );

            rebuild_colour_bar( slice_window );
        }

        set_slice_window_all_update( slice_window, type );
    }
}

public  void  change_colour_coding_range(
    display_struct    *slice_window,
    Real              min_value,
    Real              max_value )
{
    set_colour_coding_min_max( &slice_window->slice.colour_coding,
                               min_value, max_value );

    colour_coding_has_changed( slice_window, UPDATE_SLICE );
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
    Colour   colour;

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
        if( is_an_rgb_volume( volume ) )
        {
            convert_real_to_int_voxel( get_volume_n_dimensions(volume),
                                       voxel, int_voxel );
            if( int_voxel_is_within_volume( volume, int_voxel ) )
            {
                GET_VOXEL_3D( colour, volume, int_voxel[0], int_voxel[1],
                              int_voxel[2] );
            }
            else
                colour = BLACK;
        }
        else
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
        }

        if( slice_window->slice.display_labels && label_volume->data != NULL )
        {
            convert_world_to_voxel( volume,
                                    Point_x(points[i]),
                                    Point_y(points[i]),
                                    Point_z(points[i]), voxel );
            convert_real_to_int_voxel( get_volume_n_dimensions(volume),
                                       voxel, int_voxel );

            label = get_volume_label_data( label_volume, int_voxel );

            if( is_an_rgb_volume( volume ) )
                colour = apply_label_colour( slice_window, colour, label );
        }
        else
            label = 0;

        if( is_an_rgb_volume( volume ) )
            (*colours)[i] = colour;
        else
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
