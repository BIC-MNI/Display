
#include  <def_display.h>

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

    for_less( label, 0, NUM_LABELS )
    {
        slice_window->slice.colour_tables[label] = (Colour *) 0;
        slice_window->slice.label_colours_used[label] = FALSE;
    }
}

public  void  set_colour_coding_for_new_volume(
    display_struct    *slice_window )
{
    Real             low_limit, high_limit;
    Real             min_voxel, max_voxel;
    Real             min_value, max_value;
    int              label;
    Volume           volume;
    static Colour    default_colours[] = { RED, GREEN, BLUE, CYAN, MAGENTA,
                                           YELLOW };

    (void) get_slice_window_volume( slice_window, &volume );

    for_less( label, 0, NUM_LABELS )
    {
        slice_window->slice.colour_tables[label] = (Colour *) 0;
        slice_window->slice.label_colours_used[label] = FALSE;
    }

    add_new_label( slice_window, ACTIVE_BIT, WHITE );
    add_new_label( slice_window, ACTIVE_BIT | LABEL_BIT, Labeled_voxel_colour);
    add_new_label( slice_window, LABEL_BIT, Inactive_and_labeled_voxel_colour);
    add_new_label( slice_window, 0, Inactive_voxel_colour );

    for_less( label, 0, SIZEOF_STATIC_ARRAY(default_colours) )
    {
        add_new_label( slice_window, ACTIVE_BIT | (label+1),
                       default_colours[label] );
    }

    get_volume_voxel_range( volume, &min_voxel, &max_voxel );

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

    for_less( i, 0, NUM_LABELS )
    {
        if( slice->colour_tables[i] != (Colour *) 0 )
            FREE( slice->colour_tables[i] );
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

public  Colour  apply_label_colour(
    display_struct    *slice_window,
    Colour            col,
    int               label )
{
    Colour           label_col, mult, scaled_col;

    if( label != ACTIVE_BIT )
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

private  int  get_signed_subtractor(
    Volume   volume )
{
    int  subtractor;

    switch( volume->data_type )
    {
    case SIGNED_BYTE:     subtractor = (1 << 8);     break;
    case SIGNED_SHORT:    subtractor = (1 << 16);    break;
    default:              subtractor = 0;           break;
    } 

    return( subtractor );
}

public  void  create_colour_tables(
    display_struct    *slice_window,
    int               label )
{
    int         n_entries;
    Real        min_voxel, max_voxel;

    get_volume_voxel_range( slice_window->slice.volume, &min_voxel, &max_voxel);

    if( min_voxel < 0 )
        n_entries = get_signed_subtractor( slice_window->slice.volume );
    else
        n_entries = (int) max_voxel - (int) min_voxel + 1;

    ALLOC( slice_window->slice.colour_tables[label], n_entries );
}

public  void   add_new_label(
    display_struct    *slice_window,
    int               label,
    Colour            colour )
{
    slice_window->slice.label_colours[label] = colour;
    slice_window->slice.label_colours_used[label] = TRUE;

    if( slice_window->slice.colour_tables[label] == (Colour *) 0 )
        create_colour_tables( slice_window, label );

    rebuild_colour_table_for_label( slice_window, label );
}

public  int  lookup_label_colour(
    display_struct    *slice_window,
    Colour            colour )
{
    Boolean   found_colour, found_empty;
    int       i, first_empty, label;

    found_colour = FALSE;
    found_empty = FALSE;

    for_inclusive( i, 1, LOWER_AUXILIARY_BITS )
    {
        label = i | ACTIVE_BIT;

        if( slice_window->slice.label_colours_used[label] )
        {
            if( equal_colours( slice_window->slice.label_colours[label],
                               colour ) )
            {
                found_colour = TRUE;
                break;
            }
        }
        else if( !found_empty )
        {
            found_empty = TRUE;
            first_empty = label;
        }
    }

    if( !found_colour )
    {
        if( found_empty )
        {
            label = first_empty;
            (void) add_new_label( slice_window, label, colour );
        }
        else
            label = (ACTIVE_BIT | LABEL_BIT);
    }

    return( label );
}

private  Colour  get_slice_colour_coding(
    display_struct    *slice_window,
    Real              value,
    int               label )
{
    Colour           col;

    col = get_colour_code( &slice_window->slice.colour_coding, value );

    col = apply_label_colour( slice_window, col, label );

    return( col );
}

private  void  set_colour_table_entry(
    Colour   table[],
    int      min_voxel,
    int      subtractor,
    int      voxel,
    Colour   colour )
{
    if( min_voxel < 0 )
    {
        if( voxel < 0 )
            table[subtractor + voxel] = colour;
        else
            table[voxel] = colour;
    }
    else
        table[voxel-min_voxel] = colour;
}

public  void  rebuild_colour_table_for_label(
    display_struct    *slice_window,
    int               label )
{
    int              voxel, subtractor;
    Real             value;
    Colour           colour;
    Real             min_voxel, max_voxel;

    get_volume_voxel_range( get_volume(slice_window), &min_voxel, &max_voxel );

    subtractor = get_signed_subtractor( get_volume(slice_window) );

    for_inclusive( voxel, (int) min_voxel, (int) max_voxel )
    {
        value = CONVERT_VOXEL_TO_VALUE( get_volume(slice_window), voxel );
        colour = get_slice_colour_coding( slice_window, value, label );

        set_colour_table_entry( slice_window->slice.colour_tables[label],
                                (int) min_voxel, subtractor, voxel, colour );
    }
}

public  void  rebuild_colour_tables(
    display_struct    *slice_window )
{
    int              label;

    for_less( label, 0, NUM_LABELS )
    {
        if( slice_window->slice.label_colours_used[label] &&
            slice_window->slice.colour_tables[label] == (Colour *) 0 )
        {
            create_colour_tables( slice_window, label );
        }

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
        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }
}
