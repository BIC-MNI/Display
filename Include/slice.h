
#ifndef  DEF_SLICE
#define  DEF_SLICE

#include  <volume_io.h>
#include  <atlas.h>
#include  <display_types.h>

#define   N_SLICE_VIEWS   4

#define   OBLIQUE_VIEW_INDEX    (N_SLICE_VIEWS-1)

typedef  struct
{
    Real              min_threshold;
    Real              max_threshold;
    Neighbour_types   connectivity;
    int               n_starts_alloced;
    int               *y_starts;
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

typedef  struct
{
    BOOLEAN   crop_visible;
    int       limit_being_moved;
    int       axis_being_moved;
    int       view_index;
    Real      limits[2][N_DIMENSIONS];
    STRING    filename;
} crop_struct;

typedef  struct
{
    int              volume_index;
    int              axis_index;
    int              slice_index;
    int              **saved_labels;
} slice_undo_struct;

typedef enum { UPDATE_SLICE, UPDATE_LABELS, UPDATE_BOTH } Update_types;

typedef  struct
{
    Volume                 volume;
    Volume                 labels;
    STRING                 labels_filename;
    int                    n_labels;
    int                    offset;
    Colour                 *colour_table;
    Colour                 *label_colour_table;
    Real                   label_colour_opacity;
    colour_coding_struct   colour_coding;
    BOOLEAN                display_labels;
    Real                   opacity;
    Real                   current_voxel[N_DIMENSIONS];

    struct
    {
        BOOLEAN                visibility;
        Real                   x_axis[N_DIMENSIONS];
        Real                   y_axis[N_DIMENSIONS];
        Real                   x_trans, y_trans;
        Real                   x_scaling, y_scaling;
        BOOLEAN                update_flag;
        BOOLEAN                update_labels_flag;
        Filter_types           filter_type;
        Real                   filter_width;
    }  views[N_SLICE_VIEWS];
} loaded_volume_struct;

typedef  struct
{
    int           prev_viewport_x_size;
    int           prev_viewport_y_size;
    int           used_viewport_x_size;
    int           used_viewport_y_size;
    BOOLEAN       update_cursor_flag;
    BOOLEAN       update_text_flag;
    BOOLEAN       update_cross_section_flag;
    BOOLEAN       update_crop_flag;
    BOOLEAN       update_atlas_flag;
    BOOLEAN       update_composite_flag;
} slice_view_struct;

typedef  struct
{
    int                    n_volumes;
    loaded_volume_struct   *volumes;
    int                    current_volume_index;

    BOOLEAN                share_labels_flag;
    colour_bar_struct      colour_bar;

    BOOLEAN                using_transparency;

    slice_view_struct      slice_views[N_SLICE_VIEWS];
    void                   *render_storage;

    Real                   x_split, y_split;

    segmenting_struct      segmenting;
    atlas_struct           atlas;
    crop_struct            crop;

    Real                   x_brush_radius, y_brush_radius, z_brush_radius;
    int                    current_paint_label;
    int                    painting_view_index;
    object_struct          *brush_outline;
    slice_undo_struct      undo;

    lines_struct           unscaled_histogram_lines;
    object_struct          *histogram_object;

    int                    cross_section_index;
    BOOLEAN                cross_section_visibility;
    BOOLEAN                cross_section_vector_present;
    Real                   cross_section_vector[MAX_DIMENSIONS];

    BOOLEAN                update_slice_dividers_flag;
    BOOLEAN                update_colour_bar_flag;
    BOOLEAN                update_probe_flag;

    BOOLEAN                viewport_update_flags[N_MODELS][2];

} slice_window_struct;


#endif
