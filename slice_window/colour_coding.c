
#include  <def_display.h>

public  void  initialize_slice_colour_coding(
    display_struct    *slice_window )
{
    initialize_colour_coding( &slice_window->slice.colour_coding,
                              GRAY_SCALE, Colour_below, Colour_above,
                              0.0, 1.0 );

    initialize_colour_bar( slice_window );

    slice_window->slice.label_colour_ratio = Label_colour_display_ratio;
    slice_window->slice.fast_lookup_present = FALSE;
}

public  void  set_colour_coding_for_new_volume(
    display_struct    *slice_window )
{
    Real             min_value, max_value;
    int              label, num_entries;
    Volume           volume;

    (void) get_slice_window_volume( slice_window, &volume );

    slice_window->slice.fast_lookup_present = FALSE;

    for_less( label, 0, NUM_LABELS )
    {
        slice_window->slice.fast_lookup[label] = (Colour *) 0;
        slice_window->slice.label_colours_used[label] = FALSE;
    }

    add_new_label( slice_window, ACTIVE_BIT, WHITE );

    add_new_label( slice_window, ACTIVE_BIT | LABEL_BIT, Labeled_voxel_colour);

    add_new_label( slice_window, LABEL_BIT, Inactive_and_labeled_voxel_colour);

    add_new_label( slice_window, 0, Inactive_voxel_colour );

    get_volume_voxel_range( volume, &min_value, &max_value );
    num_entries = (int) max_value - (int) min_value + 1;

    slice_window->slice.fast_lookup_present =
                  (num_entries <= Max_fast_colour_lookup);

    if( slice_window->slice.fast_lookup_present )
    {
        create_fast_lookup( slice_window, 0 );
        create_fast_lookup( slice_window, ACTIVE_BIT );
        create_fast_lookup( slice_window, LABEL_BIT );
        create_fast_lookup( slice_window, ACTIVE_BIT | LABEL_BIT );
    }

    change_colour_coding_range( slice_window, min_value, max_value );

    rebuild_colour_bar( slice_window );
}

public  void  delete_slice_colour_coding(
    slice_window_struct   *slice )
{
    int      i;

    if( slice->fast_lookup_present )
    {
        for_less( i, 0, NUM_LABELS )
        {
            if( slice->fast_lookup[i] != (Colour *) 0 )
                FREE( slice->fast_lookup[i] );
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

public  Colour  get_slice_colour_coding(
    display_struct    *slice_window,
    int               value,
    int               label )
{
    Colour           col, label_col, mult, scaled_col;

    col = get_colour_code( &slice_window->slice.colour_coding, (Real) value );

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

public  void  create_fast_lookup(
    display_struct    *slice_window,
    int               label )
{
    Real             min_value, max_value;

    get_volume_voxel_range( slice_window->slice.volume, &min_value, &max_value);

    ALLOC( slice_window->slice.fast_lookup[label], 
           (int) max_value - (int) min_value + 1 );
}

public  void   add_new_label(
    display_struct    *slice_window,
    int               label,
    Colour            colour )
{
    slice_window->slice.label_colours[label] = colour;
    slice_window->slice.label_colours_used[label] = TRUE;

    if( slice_window->slice.fast_lookup_present )
    {
        if( slice_window->slice.fast_lookup[label] == (Colour *) 0 )
            create_fast_lookup( slice_window, label );

        rebuild_fast_lookup_for_label( slice_window, label );
    }
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

public  void  rebuild_fast_lookup_for_label(
    display_struct    *slice_window,
    int               label )
{
    int              val;
    Colour           colour;
    Real             min_value, max_value;

    get_volume_voxel_range( slice_window->slice.volume, &min_value, &max_value);

    for_inclusive( val, (int) min_value, (int) max_value )
    {
        colour = get_slice_colour_coding( slice_window, val, label );

        slice_window->slice.fast_lookup[label][val-(int) min_value] = colour;
    }
}

public  void  rebuild_fast_lookup(
    display_struct    *slice_window )
{
    int              label;

    if( slice_window->slice.fast_lookup_present )
    {
        for_less( label, 0, NUM_LABELS )
        {
            if( slice_window->slice.fast_lookup[label] != (Colour *) 0 )
                rebuild_fast_lookup_for_label( slice_window, label );
        }
    }
}

public  void  colour_coding_has_changed(
    display_struct    *display )
{
    display_struct    *slice_window;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window != (display_struct  *) 0 )
    {
        rebuild_fast_lookup( slice_window );

        rebuild_colour_bar( slice_window );
        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }
}
