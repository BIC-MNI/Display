AUTOMAKE_OPTIONS = check-news
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = INSTALL.display \
	Hardware/supports_gl.c Include/atlas.h Include/display.h \
	Include/display_prototypes.h Include/display_types.h \
	Include/global_variables.h Include/main.h Include/menu.h \
	Include/slice.h Include/surface_extract.h \
	epm-header.in \
	$(m4_files)

SUBDIRS = Graphics \
	markers \
	menu \
	segmenting \
	atlas \
	callbacks \
	current_obj \
	cursor \
	cursor_contours \
	edit_surface \
	events \
	images \
	immediate_mode \
	input_files \
	intersect \
	main \
	slice_window \
	structures \
	surface_curves \
	surface_extraction \
	tubes \
	voxel_scan \
	Documentation

m4_files = \
	m4/mni_REQUIRE_LIB.m4 \
	m4/mni_REQUIRE_MNILIBS.m4 \
	m4/mni_REQUIRE_OPENINVENTOR.m4 \
	m4/mni_cxx_have_koenig_lookup.m4 \
	m4/smr_CGAL_MAKEFILE.m4 \
	m4/smr_OPTIONAL_LIB.m4 \
	m4/smr_REQUIRED_LIB.m4 \
	m4/smr_WITH_BUILD_PATH.m4

PROTOTYPE_FILE = Include/display_prototypes.h


bin_PROGRAMS = Display

# Adding a dummy source file to register is a work-around for a bug in 
# automake, which does not (in version 1.4) allow linking together
# only objects in subdirectories.

Display_SOURCES = dummy.c

dummy.c:
	touch $@

CLEANFILES = dummy.c Display.menu

Display_LDFLAGS = $(X_LIBS)
Display_LDADD = $(display_obj) \
	Graphics/libbicgl.a $(GL_LIBS) $(GLU_LIBS) $(GLUT_LIBS)
if USE_SEAL
Display_LDADD += -lbicseal
endif
Display_LDADD += $(MNI_LIBS) $(malloc_LIBS)

Display_DEPENDENCIES = Display.menu $(display_obj)



display_src = \
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
           tubes/convert_lines.c \
           structures/action_table.c \
           structures/fit_view.c \
           structures/lights.c \
           structures/render.c \
           structures/view.c \
           structures/window.c

display_obj = $(display_src:.c=.o)

if USE_SEAL
display_obj += callbacks/seal.o
endif



#$(PROTOTYPE_FILE): $(display_src)
#	@$(MAKE_PROTOTYPES) $@ $(display_src)

Display.menu: menu/Display.menu.include
	sed -e 's/\\//g' -e 's/\"//'    \
		< $(top_builddir)/menu/Display.menu.include    > $@

