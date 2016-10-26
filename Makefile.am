AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS =

EXTRA_DIST = INSTALL.display \
	Hardware/supports_gl.c Include/atlas.h Include/display.h \
	Include/display_prototypes.h Include/display_types.h \
	Include/global_variables.h Include/main.h Include/menu.h \
	Include/slice.h Include/surface_extract.h \
	menu/Display.menu.include \
	menu/Display.menu.base \
	Graphics/G_graphics/draw_quadmesh.include.c \
	Graphics/G_graphics/draw_polygons.include.c

PROTOTYPE_FILE = Include/display_prototypes.h

bin_PROGRAMS = Display

CLEANFILES = Display.menu

Display_DEPENDENCIES = Display.menu $(display_obj)

AM_CPPFLAGS = \
	-I$(top_srcdir)/Include \
	-I$(top_srcdir)/Graphics/OpenGL_graphics/Include \
	-I$(top_srcdir)/Graphics/Include \
	-I$(top_srcdir)/Graphics/GLUT_windows/Include

noinst_HEADERS = \
	Include/atlas.h \
	Include/display.h \
	Include/display_prototypes.h \
	Include/display_types.h \
	Include/global_variables.h \
	Include/main.h \
	Include/menu.h \
	Include/slice.h \
	Include/stack.h \
	Include/surface_extract.h \
	Graphics/Include/graphics.h \
	Graphics/Include/GS_graphics.h \
	Graphics/Include/graphics_base.h \
	Graphics/Include/graphics_prototypes.h \
	Graphics/Include/graphics_struct.h \
	Graphics/Include/random_order.h \
	Graphics/OpenGL_graphics/Include/GS_graphics.h \
	Graphics/OpenGL_graphics/Include/opengl_graphics_prototypes.h \
	Graphics/GLUT_windows/Include/WS_graphics.h \
	Graphics/GLUT_windows/Include/glut_window_prototypes.h

Display_SOURCES = \
           main/main.c \
           main/display.c \
           main/event_loop.c \
           main/graphics.c \
           main/three_d.c \
           main/transforms.c \
           atlas/atlas.c \
           input_files/input_files.c \
           input_files/volume_file.c \
           callbacks/atlas.c \
           callbacks/call_globals.c \
           callbacks/colour_coding.c \
           callbacks/file.c \
           callbacks/line_ops.c \
           callbacks/object_ops.c \
           callbacks/marker_ops.c \
           callbacks/polygon_ops.c \
           callbacks/quit.c \
           callbacks/regions.c \
           callbacks/render_ops.c \
           callbacks/segmenting.c \
           callbacks/surf_segmenting.c \
           callbacks/surface_curves.c \
           callbacks/surface_extract.c \
           callbacks/view_ops.c \
           callbacks/volume_ops.c \
           callbacks/volume_transform_ops.c \
           current_obj/current_obj.c \
           edit_surface/connected.c \
           edit_surface/edit.c \
           edit_surface/segment.c \
           images/images.c \
           markers/markers.c \
           surface_extraction/boundary_extraction.c \
           surface_extraction/data_structs.c \
           surface_extraction/init_surface.c \
           surface_extraction/extract.c \
           surface_extraction/surface.c \
           surface_extraction/surface_events.c \
           voxel_scan/scan_objects.c \
           events/change_markers.c \
           events/clip_plane.c \
           events/film_loop.c \
           events/magnify.c \
           events/mouse.c \
           events/mouse_trans.c \
           events/pick_object.c \
           events/pick_view.c \
           events/rotate_slice.c \
           events/spaceball.c \
           events/virt_sb.c \
           events/window_man.c \
           events/utilities.c \
           immediate_mode/draw_immed.c \
           intersect/ray_polygons.c \
           intersect/plane_polygons.c \
           cursor/cursor.c \
           cursor/cursor_icon.c \
           menu/build_menu.c \
           menu/cursor_pos.c \
           menu/menu.c \
           menu/input_menu.c \
           menu/selected.c \
           menu/text.c \
           cursor_contours/contours.c \
           segmenting/cut_neighbours.c \
           segmenting/painting.c \
           segmenting/segmenting.c \
           segmenting/segment_polygons.c \
           slice_window/colour_bar.c \
           slice_window/colour_coding.c \
           slice_window/crop.c \
           slice_window/draw_slice.c \
           slice_window/histogram.c \
	slice_window/outline.c \
           slice_window/pick_angle.c \
           slice_window/quadmesh.c \
           slice_window/slice.c \
           slice_window/slice_3d.c \
           slice_window/slice_events.c \
           slice_window/undo.c \
           slice_window/view.c \
           surface_curves/closest_line.c \
           surface_curves/events.c \
           surface_curves/edge_distance.c \
	stack/stack.c \
           structures/action_table.c \
           structures/fit_view.c \
           structures/lights.c \
           structures/render.c \
           structures/view.c \
           structures/window.c \
           tubes/convert_lines.c \
	Graphics/G_graphics/draw.c \
	Graphics/G_graphics/draw_objects.c \
	Graphics/G_graphics/event_loop.c \
	Graphics/G_graphics/graphics_structs.c \
	Graphics/G_graphics/lights.c \
	Graphics/G_graphics/random_order.c \
	Graphics/G_graphics/render.c \
	Graphics/G_graphics/view.c \
	Graphics/G_graphics/windows.c \
	Graphics/GLUT_windows/glut_windows.c \
	Graphics/OpenGL_graphics/colour_def.c \
	Graphics/OpenGL_graphics/draw.c \
	Graphics/OpenGL_graphics/event_loop.c \
	Graphics/OpenGL_graphics/lights.c \
	Graphics/OpenGL_graphics/render.c \
	Graphics/OpenGL_graphics/view.c \
	Graphics/OpenGL_graphics/windows.c

display_obj = $(display_src:.c=.o)

if USE_SEAL
display_obj += callbacks/seal.o
endif

Display.menu: menu/Display.menu.include
	sed -e 's/\\//g' -e 's/\"//'    \
		< $(top_builddir)/menu/Display.menu.include    > $@

