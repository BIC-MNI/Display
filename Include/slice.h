
#ifndef  DEF_SLICE
#define  DEF_SLICE

#include  <def_mni.h>
#include  <def_atlas.h>

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

typedef  enum  { FOUR_NEIGHBOURS, EIGHT_NEIGHBOURS } Neighbour_types;

typedef  struct
{
    int               n_labels;
    label_struct      *labels;
    int               min_threshold;
    int               max_threshold;
    Neighbour_types   connectivity;
} segmenting_struct;

typedef struct
{
    Real             top_offset;
    Real             bottom_offset;
    Real             left_offset;
    Real             bar_width;
    Real             tick_width;
    int              desired_n_intervals;
} colour_bar_struct;

#define  N_AUXILIARY_VOXEL_BYTES   1
#define  LABEL_BIT                 128
#define  ACTIVE_BIT                64
#define  LOWER_AUXILIARY_BITS      63

#define  NUM_LABELS   (1 << (8*N_AUXILIARY_VOXEL_BYTES) )

typedef  struct
{
    Volume                 original_volume;
    Volume                 volume;

    Boolean                fast_lookup_present;
    Colour                 *fast_lookup[NUM_LABELS];
    Real                   label_colour_ratio;
    Boolean                label_colours_used[NUM_LABELS];
    Colour                 label_colours[NUM_LABELS];
    colour_coding_struct   colour_coding;
    colour_bar_struct      colour_bar;

    int                    x_split, y_split;

    int                    slice_index[N_DIMENSIONS];
    Boolean                slice_locked[N_DIMENSIONS];
    slice_view_struct      slice_views[3];
    int                    next_to_update;

    int                    *temporary_indices;
    int                    temporary_indices_alloced;

    segmenting_struct      segmenting;
    atlas_struct           atlas;

    Real                   x_brush_radius, y_brush_radius, z_brush_radius;
    int                    current_paint_label;

} slice_window_struct;


#endif
