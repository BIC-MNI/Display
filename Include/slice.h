
#ifndef  DEF_SLICE
#define  DEF_SLICE

#include  <def_bitlist.h>
#include  <def_objects.h>
#include  <def_graphics_types.h>

typedef  struct
{
    int       axis_map[N_DIMENSIONS];
    Boolean   axis_flip[N_DIMENSIONS];
    int       x_offset, y_offset;
    Real      x_scale, y_scale;
    Boolean   update_flag;
} slice_view_struct;

typedef  struct
{
    int   voxel_indices[3];
    int   id;
} label_struct;

typedef  struct
{
    int            n_labels;
    label_struct   *labels;
    int            min_threshold;
    int            max_threshold;
} segmenting_struct;


typedef  struct
{
    volume_struct          *volume;

    Boolean                fast_lookup_present;
    Pixel_colour           *fast_lookup;
    colour_coding_struct   colour_coding;

    int                    x_split, y_split;

    int                    slice_index[N_DIMENSIONS];
    Boolean                slice_locked[N_DIMENSIONS];
    slice_view_struct      slice_views[3];

    int                    *temporary_indices;
    int                    temporary_indices_alloced;

    segmenting_struct      segmenting;

} slice_window_struct;


#endif
