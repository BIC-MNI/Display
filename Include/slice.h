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

/**
 * Total number of slice view panels, one for each spatial axis plus
 * one more for the arbitrary (oblique) view.
 */
#define N_SLICE_VIEWS 4

/** Define the default arbitrary (oblique) view. */
#define OBLIQUE_VIEW_INDEX (N_SLICE_VIEWS-1)

/** number of measurement lines supported. */
#define N_MEASUREMENTS 3

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
 * Indices of object associated with each of the slice views. Note
 * that this ordering is very dependent on the order of initialization
 * in initialize_slice_models().
 */
typedef  enum  {
  ATLAS_SLICE_INDEX,            /**< Pixels object for atlas.  */
  COMPOSITE_SLICE_INDEX,        /**< Pixels of the volume voxels. */
  CROSS_SECTION_INDEX,          /**< Lines indicating arbitrary plane. */
  CROP_BOX_INDEX,               /**< Polygons defining the crop box. */
  CURSOR_INDEX1,                /**< Part of the slice cursor. */
  CURSOR_INDEX2,                /**< Part of the slice cursor. */
  RULERS,                       /**< The axis ruler models. */
  TEXT_INDEX,                   /**< The text axis position label. */
  FOV_INDEX,                    /**< The field-of-view text. */
} Slice_model_indices;

/**
 * Indices associated with the \c FULL_WINDOW_MODEL in the slice window.
 */
typedef  enum  { 
  DIVIDER_INDEX                 /**< Slice divider lines. */
} Full_window_indices;

/**
 * \brief Describes a brush.
 */
typedef struct brush
{
    /** Defines the X, Y, and Z radius for a brush. */
    VIO_Real radius[VIO_N_DIMENSIONS];
} brush_struct;

/** Number of brushes supported. Allows support for secondary brush. */
#define N_BRUSHES 2

/**
 * \brief Information about the current label painting, or segmenting,
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

    /** The current setting of the connectivity parameter for painting
     * operations. Legal values are either FOUR_NEIGHBOURS or EIGHT_NEIGHBOURS.
     */
    Neighbour_types connectivity;

    /** Number of start positions allocated by the fast painting operation.
     */
    int             n_starts_alloced;

    /** Dynamically allocated array of row start positions used by the fast
     * painting operation.
     */
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

    /** The index of the currently active brush. Must be greater than and 
     * less than N_BRUSHES.
     */
    int             brush_index;

    /** Sizes of the various different brushes.
     */
    brush_struct    brush[N_BRUSHES];
} segmenting_struct;

/**
 * \brief Structure that represents the layout of the colour bar.
 */
typedef struct colour_bar
{
    VIO_Real         top_offset; /**< Offset from top of viewport. */
    VIO_Real         bottom_offset; /**< Offset from bottom of viewport. */
    VIO_Real         left_offset; /**< Offset from left side of viewport. */
    VIO_Real         bar_width; /**< Width of colour bar in pixels. */
    VIO_Real         tick_width; /**< Width of tick marks in pixels. */
    int              desired_n_intervals; /**< Requested number of ticks. */
} colour_bar_struct;

/**
 * \brief Structure that represents a volume cropping operation in progress.
 */
typedef struct crop
{
    /** True if the crop box should be visible. */
    VIO_BOOL  crop_visible;

    /** Column and row axis indices for view that is being adjusted. */
    int       axis_being_moved[2];

    /**
     * Describes which limit (0=low, 1=high) is being adjusted for the
     * column and row.
     */
    int       limit_being_moved[2];

    /** View index where box is being adjusted. */
    int       view_index;

    /** Start voxel position of the box selection. */
    VIO_Real  start_voxel[VIO_MAX_DIMENSIONS];

    /** Current limits (0=low, 1=high) of the crop box */
    VIO_Real  limits[2][VIO_MAX_DIMENSIONS];

    /** The _input_ file name for cropping operations. */
    VIO_STR   filename;
} crop_struct;

/**
 * Sets the number of hash buckets. As generally known from the hash
 * table lore, this value should be a prime. It was chosen to be fairly
 * large without causing an empty sparse array to take up more than
 * a few hundred K bytes.
 */
#define N_SPARSE_HASH 19009

/**
 * Sets the maximum number of coordinates. In our application this could
 * probably be limited to 3.
 */
#define N_SPARSE_COORD 4

/**
 * \brief Represents a single entry in a sparse array.
 *
 * These are arranged in a linked list in an open hashing strategy.
 */
typedef struct sparse_array_entry
{
  /** A link to the next item in the hash bucket list. */
  struct sparse_array_entry *link;

  /** The array of coordinates for this entry. */
  int coord[N_SPARSE_COORD];

  /**
   * The actual value stored at this entry. We assume an integer
   * because we know we are storing voxel labels.
   */
  int value;
} sparse_array_entry_t;

/**
 * \brief Represents a sparse array, in a somewhat efficient manner.
 *
 * There are probably better ways to do this, especially given that
 * our brush strokes tend to be confined to very limited geometries.
 */
typedef struct sparse_array
{
  /** The total number of values in the array. */
  int n_entries;

  /** The number of dimensions used, from 1 to N_SPARSE_COORD */
  int n_dimensions;

  /** The array of hash buckets */
  sparse_array_entry_t *table[N_SPARSE_HASH];

} sparse_array_t;

/**
 * \brief Represents an undo-able event by recording all of the coordinates
 * and values affected by an event.
 */
typedef struct volume_undo
{
    /** number of undo items in the queue */
    int            n_undo;

    /** array of Undo_list_length undo records. */
    sparse_array_t *prior_labels;
} volume_undo_struct;

/**
 * This type is used by functions that attempt to optmize repainting
 * by only redrawing the volume slice or labels, if possible.
 */
typedef enum { UPDATE_SLICE, UPDATE_LABELS, UPDATE_BOTH } Update_types;

/**
 * \brief Represents the volume-specific, per-view state information.
 */
typedef struct volume_view
{
    /** TRUE if the volume is visible in this view. */
    VIO_BOOL               visibility;
    /** Number of pixels allocated for the volume voxel values. */
    int                    n_pixels_alloced;
    /** Number of pixels allocated for the label values. */
    int                    n_label_pixels_alloced;
    /** Column axis vector for this view. */
    VIO_Real               x_axis[VIO_MAX_DIMENSIONS];
    /** Row axis vector for this view. */
    VIO_Real               y_axis[VIO_MAX_DIMENSIONS];
    /** Column translation. */
    VIO_Real               x_trans;
    /** Row translation. */
    VIO_Real               y_trans;
    /** Column zoom factor. */
    VIO_Real               x_scaling;
    /** Row zoom factor. */
    VIO_Real               y_scaling;
    /** TRUE if the volume values should be rendered and redisplayed. */
    VIO_BOOL               update_flag;
    /** TRUE if the volume labels should be rendered and redisplayed. */
    VIO_BOOL               update_labels_flag;
    /** Filter type for pixel rendering, one of NEAREST_NEIGHBOUR,
     * LINEAR_INTERPOLATION, BOX_FILTER, TRIANGLE_FILTER, or GAUSSIAN_FILTER.
     * Used only for the voxels; labels always use NEAREST_NEIGHBOUR.
     */
    VIO_Filter_types       filter_type;
    /** Width parameter for the pixel rendering filter. */
    VIO_Real               filter_width;
    /**
     * Number of pixels to redraw per rendering, if incremental rendering is
     * enabled.
     */
    int                    n_pixels_redraw;
    /**
     * Per-volume (0=data, 1=label) flags to keep track of whether an
     * update is in progress.
     */
    VIO_BOOL               update_in_progress[2];
    /**
     * Per-volume (0=data, 1=label) minimum update column.
     */
    int                    x_min_update[2];
    /**
     * Per-volume (0=data, 1=label) maximum update column.
     */
    int                    x_max_update[2];
    /**
     * Per-volume (0=data, 1=label) minimum update row.
     */
    int                    y_min_update[2];
    /**
     * Per-volume (0=data, 1=label) maximum update row.
     */
    int                    y_max_update[2];
    /**
     * Per-volume (0=data, 1=label) storage of state information (??).
     */
    int                    edge_index[2];
}  volume_view_struct;

/** Shorthand for a pointer to a volume_view_struct */
typedef volume_view_struct *volume_view_ptr;

/**
 * \brief Represents a single loaded volume in the program.
 */
typedef struct loaded_volume
{
    /**
     * The filename associated with this volume.
     */
    VIO_STR                filename;

    /**
     * The pointer to the volume_io structure used to access the volume
     * data and current coordinate transform.
     */
    VIO_Volume             volume;

    /**
     * The pointer to the volume_io structure used to access the label
     * volume associated with this data volume.
     */
    VIO_Volume             labels;

    /**
     * Saves the original transform of the volume, so that it can be
     * restored.
     */
    VIO_General_transform  original_transform;

    /**
     * The filename associated with the labels loaded with this volume, if
     * any. A value of NULL indicates that the labels are not associated
     * with a file.
     */
    VIO_STR                labels_filename;

    /** The total number of labels that can be currently used with this
     * volume.
     */
    int                    n_labels;

    /**
     * The offset added to the colour_table pointer to compensate for a
     * non-zero minimum value.
     */
    int                    colour_offset;

    /**
     * Array used to map voxel values to precomputed colour coding values
     * efficiently. The pointer stored here has been incremented by
     * the colour_offset value, so that no correction need be performed
     * during value lookups.
     */
    VIO_Colour             *colour_table;

    /**
     * Colour coding structure used for volume voxel (_not_ label) data.
     * This is used to generate the values in colour_table.
     */
    colour_coding_struct   colour_coding;

    /**
     * Array used to map label values to colour values. Because label
     * values are integers, this table is a straightforward map.
     */
    VIO_Colour             *label_colour_table;

    /**
     * The opacity to apply to label colours, a real value on [0,1].
     */
    VIO_Real               label_colour_opacity;

    /**
     * Flag to determine whether labels are currently displayed.
     */
    VIO_BOOL               display_labels;

    /**
     * The opacity to apply to voxel colours, a real value on [0,1].
     */
    VIO_Real               opacity;

    /**
     * The cursor location in this volume, in local voxel coordinates.
     */
    VIO_Real               current_voxel[VIO_MAX_DIMENSIONS];

    /**
     * Undo information for this volume.
     */
    volume_undo_struct     undo;

    /**
     * Label stack used by this volume. This is only used if
     * Tags_from_label is true.
     */
    struct stack_list      **label_stack;

    /**
     * Count of of labels of each value. This is only used if
     * Tags_from_label is true.
     */
    unsigned int           *label_count;

    /**
     * View-specific parameters for this volume.
     */
    volume_view_struct views[N_SLICE_VIEWS];
} loaded_volume_struct;

/**
 * \brief Structure representing non-volume-specific information about a view.
 *
 * Used primarily to guide redrawing.
 */
typedef  struct slice_view
{
    /** Previous viewport width. */
    int           prev_viewport_x_size;

    /** Previous viewport height. */
    int           prev_viewport_y_size;

    /** Current viewport width. */
    int           used_viewport_x_size;

    /** Current viewport height. */
    int           used_viewport_y_size;

    /** TRUE if the cursor needs to be redrawn. */
    VIO_BOOL      update_cursor_flag;

    /** TRUE if the slice view position text needs to be redrawn. */
    VIO_BOOL      update_text_flag;

    /** TRUE if the cross-section needs to be redrawn. */
    VIO_BOOL      update_cross_section_flag;

    /** TRUE if the volume crop box needs to be redrawn. */
    VIO_BOOL      update_crop_flag;

    /** TRUE if the atlas needs to be redrawn. */
    VIO_BOOL      update_atlas_flag;

    /**
     * TRUE if the composite image (volumes plus labels) needs to be
     * redrawn. Only used if transparency hardware is not enabled.
     */
    VIO_BOOL      update_composite_flag;

    /**
     * TRUE if the outline of the 3D objects on the slice views needs
     * to be redrawn.
     */
    VIO_BOOL      update_outline_flag;

    /** The number of pixels allocated for the atlas display, if any. */
    int           n_atlas_pixels_alloced;

    /**
     * The number of pixels allocated for the composite image, if we are
     * not using OpenGL transparency.
     */
    int           n_composite_pixels_alloced;

    /** TRUE if we are just repainting a subregion. */
    VIO_BOOL      use_sub_region;

    /** TRUE if a current sub-region has been specified. */
    VIO_BOOL      sub_region_specified;

    /** The minimum x-coordinate of the sub-region. */
    int           x_min;

    /** The maximum x-coordinate of the sub-region. */
    int           x_max;

    /** The minimum y-coordinate of the sub-region. */
    int           y_min;

    /** The maximum y-coordinate of the sub-region. */
    int           y_max;

    /** True if sub-region was specified on the previous redraw. */
    VIO_BOOL      prev_sub_region_specified;
    /** The minimum x-coordinate of the previous sub-region. */
    int           prev_x_min;
    /** The maximum x-coordinate of the previous sub-region. */
    int           prev_x_max;
    /** The minimum y-coordinate of the previous sub-region. */
    int           prev_y_min;
    /** The maximum y-coordinate of the previous sub-region. */
    int           prev_y_max;
} slice_view_struct;

/**
 * \brief Represents the 3D object outlines displayed on the slice view.
 */
typedef struct outline
{
    /** The object outlines are represented as a lines_object. */
    object_struct *lines;

    /** The number of points allocated in the lines_object. */
    int           n_points_alloced;

    /** The number of indices allocated in the lines_object. */
    int           n_indices_alloced;

    /** The number of end indices allocated in the lines_object. */
    int           n_end_indices_alloced;
} outline_struct;

/**
 * \brief The massive structure that represents the state associated
 * with the slice view window.
 */
typedef struct slice_window
{
    /** Total number of loaded volumes. */
    int                    n_volumes;

    /** The array of loaded volumes. */
    loaded_volume_struct   *volumes;

    /** The index of the current volume, [0,n_volumes-1] */
    int                    current_volume_index;

    /** TRUE if undo is enabled for voxel painting operations. */
    VIO_BOOL               toggle_undo_feature;

    /** TRUE if label volumes should be cropped when written. */
    VIO_BOOL               crop_labels_on_output_flag;

    /**
     * TRUE if label volumes should be shared between volumes with
     * identical voxel-to-world transformations.
     */
    VIO_BOOL               share_labels_flag;

    /** Defines the layout of the colour bar widget. */
    colour_bar_struct      colour_bar;

    /** TRUE if the volume voxel ratio should be displayed. */
    VIO_BOOL               ratio_enabled;

    /**
     * If ratio_enabled is TRUE, this defines the volume index of the
     * numerator volume.
     */
    int			   ratio_volume_numerator;

    /**
     * If ratio_enabled is TRUE, this defines the volume index of the
     * denominator volume.
     */
    int                    ratio_volume_denominator;

    /**
     * TRUE if compositing is being handled by OpenGL.
     */
    VIO_BOOL               using_transparency;

    /**
     * Specifies the type of interpolation used in volume evaluation.
     * - -1 : nearest neighbor
     * - 0 : trilinear
     * - 2 : tricubic
     */
    int                    degrees_continuity;

    /** Per-view repainting information. */
    slice_view_struct      slice_views[N_SLICE_VIEWS];

    /** Optional opaque storage for use by the rendering code. */
    void                   *render_storage;

    /** Fractional position of the slice view divider along the column axis.
     */
    VIO_Real               x_split;

    /** Fractional position of the slice view divider along the row axis.
     */
    VIO_Real               y_split;

    /** State information and parameters for voxel painting. */
    segmenting_struct      segmenting;

    /** State information and parameters for atlas display. */
    atlas_struct           atlas;

    /** State information and parameters for volume cropping. */
    crop_struct            crop;

    /** Current radius of the brush in the X direction. */
    VIO_Real               x_brush_radius;
    /** Current radius of the brush in the Y direction. */
    VIO_Real               y_brush_radius;
    /** Current radius of the brush in the Z direction. */
    VIO_Real               z_brush_radius;
    /** Current paint label. */
    int                    current_paint_label;
    /** Current erase label (typically left at zero). */
    int                    current_erase_label;
    /** Volume index of current paint operation. */
    int                    painting_volume_index;
    /** View index of current paint operation. */
    int                    painting_view_index;
    /** Object of type LINES used to draw the brush outline, if enabled. */
    object_struct          *brush_outline;

    /** The representation of the histogram on the slice window, unscaled. */
    lines_struct           unscaled_histogram_lines;

    /** The scaled lines object representing the histogram. */
    object_struct          *histogram_object;

    /** TRUE if the volume cursor should be visible. */
    VIO_BOOL               cursor_visibility;

    /** The view index used for the oblique (arbitrary) view plane. */
    int                    cross_section_index;

    /** TRUE if the oblique view plane should be drawn in the slice
     * view panels.
     */
    VIO_BOOL               cross_section_visibility;

    /** TRUE if a cross section anchor has been selected. */
    VIO_BOOL               cross_section_vector_present;

    /** Cross section anchor vector. */
    VIO_Real               cross_section_vector[VIO_MAX_DIMENSIONS];

    /**
     * TRUE if the slice dividers between the four view panels need to
     * be redrawn (because they are being dragged, e.g.).
     */
    VIO_BOOL               update_slice_dividers_flag;

    /** TRUE if the colour bar widget needs to be redrawn. */
    VIO_BOOL               update_colour_bar_flag;

    /**
     * TRUE if the "probe" text giving detailed information about the
     * volume at the current mouse position should be updated.
     */
    VIO_BOOL               update_probe_flag;

    /** Individual redraw flags for every model, per bitplane. */
    VIO_BOOL               viewport_update_flags[N_MODELS][2];

    /**
     * TRUE if incremental update is enabled. Incremental update means we
     * only redisplay a portion of the slice view on each repaint operation.
     */
    VIO_BOOL               incremental_update_allowed;

    /**
     * This parameter controls whether the incremental redisplay code will
     * attempt to adapt the n_pixels_redraw value.
     */
    VIO_Real               allowable_slice_update_time;

    /**
     * Amount of time to allow incremental redisplay to run if a viewport
     * _has_ changed.
     */
    VIO_Real               total_slice_update_time1;

    /**
     * Amount of time to allow incremental redisplay to run if a viewport
     * _has not_ changed.
     */
    VIO_Real               total_slice_update_time2;

    /** State variable used to track volumes if incremental update is on. */
    int                    current_update_volume;

    /** State variable used to track views if incremental update is on. */
    int                    current_update_view;

    /**
     * Degrees by which to rotate the volume for each step requested in the
     * volume transform commands. See rotate_current_volume().
     */
    VIO_Real               volume_rotation_step;

    /**
     * Units by which to translate the volume for each step requested in the
     * volume transform commands. See translate_current_volume().
     */
    VIO_Real               volume_translation_step;

    /**
     * Fraction by which to scale the volume for each step requested in the
     * volume transform commands. See scale_current_volume().
     */
    VIO_Real               volume_scale_step;

    /**
     * State information related to the display of 3D object outlines
     * in the slice view window.
     */
    outline_struct         outlines[N_SLICE_VIEWS];

    /**
     * Lines object for the measurement (ctrl+left click) feature.
     */
    object_struct          *measure_line[N_MEASUREMENTS];

    /**
     * Text object for the measurement (ctrl+left click) feature.
     */
    object_struct          *measure_text[N_MEASUREMENTS];

    /**
     * View index where measurement is taking place.
     */
    int                    measure_view;

    /**
     * Origin of measurement operation in world coordinates.
     */
    VIO_Point             measure_origin[N_MEASUREMENTS];

    /**
     * End of measurement operation in world coordinates.
     */
    VIO_Point             measure_end[N_MEASUREMENTS];

    /**
     * Current measure number.
     */
    int                   measure_number;

    /**
     * Intensity plot axis.
     */
    int                   intensity_plot_axis;

} slice_window_struct;


#endif
