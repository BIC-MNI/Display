
#ifndef  DEF_SLICE
#define  DEF_SLICE

#include  <mni.h>
#include  <atlas.h>

#define   N_SLICE_VIEWS   4

#define   OBLIQUE_VIEW_INDEX    (N_SLICE_VIEWS-1)

typedef  struct
{
    Real          x_axis[N_DIMENSIONS];
    Real          y_axis[N_DIMENSIONS];
    Real          x_trans, y_trans;
    Real          x_scaling, y_scaling;
    int           prev_viewport_x_size;
    int           prev_viewport_y_size;
    int           used_viewport_x_size;
    int           used_viewport_y_size;
    BOOLEAN       update_flag;
    Filter_types  filter_type;
    Real          filter_width;
} slice_view_struct;

typedef  struct
{
    int   voxel_indices[N_DIMENSIONS];
    int   id;
} label_struct;

typedef  enum  { FOUR_NEIGHBOURS, EIGHT_NEIGHBOURS } Neighbour_types;

typedef  struct
{
    int               n_labels;
    label_struct      *labels;
    Real              min_threshold;
    Real              max_threshold;
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

#define  NUM_LABELS   256

typedef  struct
{
    Volume                 original_volume;
    Volume                 original_labels;

    Volume                 volume;
    Volume                 labels;

    Colour                 *colour_tables[NUM_LABELS];
    Real                   label_colour_ratio;
    BOOLEAN                label_colours_used[NUM_LABELS];
    Colour                 label_colours[NUM_LABELS];
    colour_coding_struct   colour_coding;
    colour_bar_struct      colour_bar;
    BOOLEAN                display_labels;

    Real                   x_split, y_split;

    Real                   current_voxel[N_DIMENSIONS];
    slice_view_struct      slice_views[N_SLICE_VIEWS];
    int                    next_to_update;

    segmenting_struct      segmenting;
    atlas_struct           atlas;

    Real                   x_brush_radius, y_brush_radius, z_brush_radius;
    int                    current_paint_label;
    object_struct          *brush_outline;

} slice_window_struct;


#endif
