
#ifndef  DEF_SLICE
#define  DEF_SLICE

#include  <def_bitlist.h>
#include  <def_objects.h>
#include  <def_graphics_types.h>

typedef  struct
{
    Boolean   update_flag;
    int       slice_index;
    int       x_offset, y_offset;
    Real      x_scale, y_scale;

} slice_view_struct;


typedef  struct
{
    volume_struct          *volume;
    colour_coding_struct   colour_coding;

    int                    x_split, y_split;

    slice_view_struct      slice_views[3];

    int                    *temporary_indices;
    int                    temporary_indices_alloced;

} slice_window_struct;


#endif