#ifndef  DEF_SLICE
#define  DEF_SLICE

/** \file slice.h
 * \brief Definitions for slice view window (3-axis volume view).
 *
 * \copyright
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
*/

#include  <volume_io.h>
#include  <atlas.h>
#include  <display_types.h>

#define N_SLICE_VIEWS 4     /**< Total number of slice view panels. */

#define OBLIQUE_VIEW_INDEX (N_SLICE_VIEWS-1) /**< Default oblique view. */

/**
 * Indices associated with each of the slice readout ("probe") text elements.
 */
typedef enum {
  VOLUME_INDEX,                 /**< Current volume. */
  X_VOXEL_PROBE_INDEX,          /**< Voxel X coordinate.  */
  Y_VOXEL_PROBE_INDEX,          /**< Voxel Y coordinate. */
  Z_VOXEL_PROBE_INDEX,          /**< Voxel Z coordinate. */
  X_WORLD_PROBE_INDEX,          /**< World X coordinate. */
  Y_WORLD_PROBE_INDEX,          /**< World Y coordinate. */
  Z_WORLD_PROBE_INDEX,          /**< World Z coordinate. */
  VOXEL_PROBE_INDEX,            /**< Current voxel raw value. */
  VAL_PROBE_INDEX,              /**< Current voxel real value. */
  LABEL_PROBE_INDEX,            /**< Current voxel label. */
  RATIO_PROBE_INDEX,            /**< Volume ratio. */
  DISTANCE_PROBE_INDEX,         /**< Distance to cursor. */
  N_READOUT_MODELS              /**< Total number of elements.  */
} Slice_readout_indices;

/**
 * Information corresponding to the current label painting, or segmenting,
 * operations.
 */
typedef  struct segmenting_struct
{
    /** Minimum painting threshold. If current voxel's real value is 
     * less than this threshold, it will not be painted. If min_threshold
     * is greater than max_threshold, these values are ignored.
     */
    VIO_Real        min_threshold;

    /** Maximum painting threshold. If current voxel's real value is 
     * greater than this threshold, it will not be painted. If min_threshold
     * is greater than max_threshold, these values are ignored.
     */
    VIO_Real        max_threshold;

    Neighbour_types connectivity;

    /** Number of start positions allocated by the fast painting operation.
     */
    int             n_starts_alloced;
    int             *y_starts;

    /** Start X position of the ongoing brush stroke.
     */
    int             x_mouse_start;

    /** Start Y position of the ongoing brush stroke.
     */
    int             y_mouse_start;

    /** Controls the translation of the mouse position to the 
     * actual slice position. Probably never needs to be set to
     * anything except 1.0.
     */
    VIO_Real        mouse_scale_factor;
  
    /** True if updates should be made in the current slice only, if 
     * possible.
     */
    VIO_BOOL        fast_updating_allowed;

     /** True if cursor position is continuously updated as the paintbrush
      * is moved.
      */
    VIO_BOOL        cursor_follows_paintbrush;
} segmenting_struct;

typedef struct
{
    VIO_Real             top_offset;
    VIO_Real             bottom_offset;
    VIO_Real             left_offset;
    VIO_Real             bar_width;
    VIO_Real             tick_width;
    int              desired_n_intervals;
} colour_bar_struct;

typedef  struct
{
    VIO_BOOL   crop_visible;
    int       axis_being_moved[2];
    int       limit_being_moved[2];
    int       view_index;
    VIO_Real  start_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real  limits[2][VIO_MAX_DIMENSIONS];
    VIO_STR    filename;
} crop_struct;

#define N_SPARSE_HASH 19009
#define N_SPARSE_COORD 4

typedef struct sparse_array_entry {
  struct sparse_array_entry *link;
  int coord[N_SPARSE_COORD];
  int value;
} sparse_array_entry_t;

typedef struct sparse_array {
  int n_entries;
  int n_dimensions;
  sparse_array_entry_t *table[N_SPARSE_HASH];
} sparse_array_t;

typedef struct
{
    int            n_undo;
    sparse_array_t *prior_labels;
} volume_undo_struct;

typedef enum { UPDATE_SLICE, UPDATE_LABELS, UPDATE_BOTH } Update_types;

typedef  struct
{
    VIO_STR                filename;
    VIO_Volume             volume;
    VIO_Volume             labels;
    VIO_General_transform  original_transform;
    VIO_STR                labels_filename;
    int                    n_labels;
    int                    offset;
    VIO_Colour             *colour_table;
    VIO_Colour             *label_colour_table;
    VIO_Real               label_colour_opacity;
    colour_coding_struct   colour_coding;
    VIO_BOOL               display_labels;
    VIO_Real               opacity;
    VIO_Real               current_voxel[VIO_MAX_DIMENSIONS];
    volume_undo_struct     undo;

    struct volume_view_struct
    {
        VIO_BOOL               visibility;
        int                    n_pixels_alloced;
        int                    n_label_pixels_alloced;
        VIO_Real               x_axis[VIO_MAX_DIMENSIONS];
        VIO_Real               y_axis[VIO_MAX_DIMENSIONS];
        VIO_Real               x_trans, y_trans;
        VIO_Real               x_scaling, y_scaling;
        VIO_BOOL               update_flag;
        VIO_BOOL               update_labels_flag;
        VIO_Filter_types       filter_type;
        VIO_Real               filter_width;
        int                    n_pixels_redraw;
        VIO_BOOL               update_in_progress[2];
        int                    x_min_update[2];
        int                    x_max_update[2];
        int                    y_min_update[2];
        int                    y_max_update[2];
        int                    edge_index[2];
    }  views[N_SLICE_VIEWS];
} loaded_volume_struct;

typedef  struct
{
    int           prev_viewport_x_size;
    int           prev_viewport_y_size;
    int           used_viewport_x_size;
    int           used_viewport_y_size;
    VIO_BOOL      update_cursor_flag;
    VIO_BOOL      update_text_flag;
    VIO_BOOL      update_cross_section_flag;
    VIO_BOOL      update_crop_flag;
    VIO_BOOL      update_atlas_flag;
    VIO_BOOL      update_composite_flag;
    VIO_BOOL      update_outline_flag; /* TRUE if outline needs updating. */

    int           n_atlas_pixels_alloced;
    int           n_composite_pixels_alloced;

    VIO_BOOL      use_sub_region;

    VIO_BOOL      sub_region_specified;
    int           x_min;
    int           x_max;
    int           y_min;
    int           y_max;
    VIO_BOOL      prev_sub_region_specified;
    int           prev_x_min;
    int           prev_x_max;
    int           prev_y_min;
    int           prev_y_max;
} slice_view_struct;

typedef struct
{
    object_struct *lines;
    int           n_points_alloced;
    int           n_indices_alloced;
    int           n_end_indices_alloced;
} outline_struct;

typedef  struct
{
    int                    n_volumes;
    loaded_volume_struct   *volumes;
    int                    current_volume_index;

    VIO_BOOL               toggle_undo_feature;

    VIO_BOOL               crop_labels_on_output_flag;
    VIO_BOOL               share_labels_flag;
    colour_bar_struct      colour_bar;

    VIO_BOOL               ratio_enabled;
    int			   ratio_volume_numerator;
    int                    ratio_volume_denominator;

    VIO_BOOL               using_transparency;
    int                    degrees_continuity;

    slice_view_struct      slice_views[N_SLICE_VIEWS];
    void                   *render_storage;

    VIO_Real               x_split, y_split;

    segmenting_struct      segmenting;
    atlas_struct           atlas;
    crop_struct            crop;

    VIO_Real               x_brush_radius, y_brush_radius, z_brush_radius;
    int                    current_paint_label;
    int                    current_erase_label;
    int                    painting_volume_index;
    int                    painting_view_index;
    object_struct          *brush_outline;

    lines_struct           unscaled_histogram_lines;
    object_struct          *histogram_object;

    VIO_BOOL               cursor_visibility;

    int                    cross_section_index;
    VIO_BOOL               cross_section_visibility;
    VIO_BOOL               cross_section_vector_present;
    VIO_Real               cross_section_vector[VIO_MAX_DIMENSIONS];

    VIO_BOOL               update_slice_dividers_flag;
    VIO_BOOL               update_colour_bar_flag;
    VIO_BOOL               update_probe_flag;

    VIO_BOOL               viewport_update_flags[N_MODELS][2];

    VIO_BOOL               incremental_update_allowed;
    VIO_Real               allowable_slice_update_time;
    VIO_Real               total_slice_update_time1;
    VIO_Real               total_slice_update_time2;
    int                    current_update_volume;
    int                    current_update_view;

    VIO_Real               volume_rotation_step;
    VIO_Real               volume_translation_step;
    VIO_Real               volume_scale_step;

    outline_struct         outlines[N_SLICE_VIEWS];
    
} slice_window_struct;


#endif
