
#include  <def_graphics.h>

typedef struct {
    Real           position;
    Colour         colour;
    Colour_spaces  interpolation_space;
} colour_point;

public  void  build_default_colour_map( colour_coding, min_colour, max_colour,
                                        interpolation_space )
    colour_coding_struct  *colour_coding;
    Colour                *min_colour;
    Colour                *max_colour;
    Colour_spaces         interpolation_space;
{
    void    rebuild_colour_coding();
    static  colour_point  desc[2];

    desc[0].position = 0.0;
    desc[0].colour = *min_colour;
    desc[0].interpolation_space = interpolation_space;

    desc[1].position = 1.0;
    desc[1].colour = *max_colour;
    desc[1].interpolation_space = interpolation_space;

    rebuild_colour_coding( colour_coding, 2, desc );
}

public  void  build_gray_scale_coding( colour_coding )
    colour_coding_struct  *colour_coding;
{
    void    rebuild_colour_coding();
    static  colour_point  desc[] = { {0.0, { BLACK_COL }, RGB_SPACE },
                                     {1.0, { WHITE_COL }, RGB_SPACE } };

    rebuild_colour_coding( colour_coding, SIZEOF_STATIC_ARRAY(desc), desc );
}

public  void  build_spectral_coding( colour_coding )
    colour_coding_struct  *colour_coding;
{
    void    rebuild_colour_coding();
    static  colour_point  desc[] = { {0.0,  { 0.0, 0.0, 1.0 }, HSL_SPACE },
                                     {0.9,  { 1.0, 0.0, 0.0 }, HSL_SPACE },
                                     {1.0,  { 1.0, 1.0, 1.0 }, RGB_SPACE } };

    rebuild_colour_coding( colour_coding, SIZEOF_STATIC_ARRAY(desc), desc );
}

public  void  build_hot_metal_coding( colour_coding )
    colour_coding_struct  *colour_coding;
{
    void    rebuild_colour_coding();
    static  colour_point  desc[] = { {0.0,  { 0.0, 0.0, 0.0 }, RGB_SPACE },
                                     {0.25, { 0.5, 0.0, 0.0 }, RGB_SPACE },
                                     {0.5,  { 1.0, 0.5, 0.0 }, RGB_SPACE },
                                     {0.75, { 1.0, 1.0, 0.5 }, RGB_SPACE },
                                     {1.0,  { 1.0, 1.0, 1.0 }, RGB_SPACE } };

    rebuild_colour_coding( colour_coding, SIZEOF_STATIC_ARRAY(desc), desc );
}

private  void  rebuild_colour_coding( colour_coding, n_points, points ) 
    colour_coding_struct  *colour_coding;
    int                   n_points;
    colour_point          points[];
{
    int    i, n_entries, first_entry, last_entry;
    void   interpolate_colours();

    n_entries = colour_coding->colour_table_size;

    for_less( i, 0, n_points-1 )
    {
        if( i == 0 )
            first_entry = 0;
        else
            first_entry = ROUND( points[i].position * (Real) n_entries );

        if( i < n_points-1 )
            last_entry = ROUND( points[i+1].position * (Real) n_entries );
        else
            last_entry = n_entries - 1;

        interpolate_colours( first_entry, last_entry,
                             colour_coding->colour_table,
                             &points[i].colour, &points[i+1].colour,
                             points[i].interpolation_space );
    }
}

private  void  interpolate_colours( first_entry, last_entry, table,
                                    first_colour, last_colour,
                                    interpolation_space )
    int            first_entry;
    int            last_entry;
    Colour         table[];
    Colour         *first_colour;
    Colour         *last_colour;
    Colour_spaces  interpolation_space;
{
    int   i;
    Real  ratio, r,g, b, r0, g0, b0, r1, g1, b1;
    void  hsl_to_rgb();
    void  rgb_to_hsl();

    r0 = Colour_r( *first_colour );
    g0 = Colour_g( *first_colour );
    b0 = Colour_b( *first_colour );

    r1 = Colour_r( *last_colour );
    g1 = Colour_g( *last_colour );
    b1 = Colour_b( *last_colour );

    if( interpolation_space == HSL_SPACE )
    {
        rgb_to_hsl( r0, g0, b0, &r0, &g0, &b0 );
        rgb_to_hsl( r1, g1, b1, &r1, &g1, &b1 );
    }

    for_inclusive( i, first_entry, last_entry )
    {
        ratio = (Real) (i - first_entry) /
                (Real) (last_entry - first_entry);

        r = r0 + (r1 - r0) * ratio;
        g = g0 + (g1 - g0) * ratio;
        b = b0 + (b1 - b0) * ratio;

        if( interpolation_space == HSL_SPACE )
        {
            hsl_to_rgb( r, g, b, &r, &g, &b );
        }

        fill_Colour( table[i], r, g, b );
    }
}

