include $(SRC_DIRECTORY)/Graphics/Makefile.include
include $(SRC_DIRECTORY)/David/Makefile.include

OPT = -O $(SPECIAL_OPT)

INCLUDE = -IInclude $(DLIB_INCLUDE) $(GRAPHICS_INCLUDE)

PROTOTYPE_FILE = Include/display_prototypes.h

main/main.o: Include/global_variables.h

DISPLAY = Display

display_obj = \
           main/main.o \
           main/display.o \
           main/event_loop.o \
           main/graphics.o \
           main/three_d.o \
           main/transforms.o \
           atlas/atlas.o \
           input_files/input_files.o \
           input_files/volume_file.o \
           deform/deform.o \
           callbacks/atlas.o \
           callbacks/call_globals.o \
           callbacks/colour_coding.o \
           callbacks/deform.o \
           callbacks/file.o \
           callbacks/line_ops.o \
           callbacks/object_ops.o \
           callbacks/marker_ops.o \
           callbacks/polygon_ops.o \
           callbacks/quit.o \
           callbacks/regions.o \
           callbacks/render_ops.o \
           callbacks/segmenting.o \
           callbacks/surf_segmenting.o \
           callbacks/surface_curves.o \
           callbacks/surface_extract.o \
           callbacks/view_ops.o \
           callbacks/volume_ops.o \
           current_obj/current_obj.o \
           edit_surface/connected.o \
           edit_surface/edit.o \
           edit_surface/segment.o \
           images/images.o \
           markers/markers.o \
           markers/segment.o \
           surface_extraction/boundary_extraction.o \
           surface_extraction/data_structs.o \
           surface_extraction/init_surface.o \
           surface_extraction/extract.o \
           surface_extraction/surface.o \
           surface_extraction/surface_events.o \
           voxel_scan/scan_objects.o \
           events/change_markers.o \
           events/clip_plane.o \
           events/film_loop.o \
           events/magnify.o \
           events/mouse.o \
           events/mouse_trans.o \
           events/pick_object.o \
           events/pick_view.o \
           events/rotate_slice.o \
           events/spaceball.o \
           events/virt_sb.o \
           events/window_man.o \
           events/utilities.o \
           immediate_mode/draw_immed.o \
           intersect/ray_polygons.o \
           intersect/plane_polygons.o \
           cursor/cursor.o \
           cursor/cursor_icon.o \
           menu/build_menu.o \
           menu/cursor_pos.o \
           menu/menu.o \
           menu/input_menu.o \
           menu/selected.o \
           menu/text.o \
           cursor_contours/contours.o \
           segmenting/cut_neighbours.o \
           segmenting/painting.o \
           segmenting/segmenting.o \
           segmenting/segment_polygons.o \
           slice_window/colour_bar.o \
           slice_window/colour_coding.o \
           slice_window/crop.o \
           slice_window/draw_slice.o \
           slice_window/histogram.o \
           slice_window/pick_angle.o \
           slice_window/quadmesh.o \
           slice_window/slice.o \
           slice_window/slice_3d.o \
           slice_window/slice_events.o \
           slice_window/undo.o \
           slice_window/view.o \
           surface_curves/closest_line.o \
           surface_curves/events.o \
           surface_curves/edge_distance.o \
           tubes/convert_lines.o \
           structures/action_table.o \
           structures/fit_view.o \
           structures/lights.o \
           structures/render.o \
           structures/view.o \
           structures/window.o

LINT_LIBS = $(DLIB_LINT_LIBS) $(GRAPHICS_LINT_LIBS)

display_lint = $(display_obj:.o=.ln)

lint_display: $(PROTOTYPE_FILE) $(display_lint)
	@$(LINT) $(LINTFLAGS) $(display_lint) $(LINT_LIBS)

$(DISPLAY).irisgl: $(PROTOTYPE_FILE) $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(IRISGL_GRAPHICS_LIBS)

$(DISPLAY).opengl: $(PROTOTYPE_FILE) $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(OPENGL_GRAPHICS_LIBS)

$(DISPLAY).mesa: $(PROTOTYPE_FILE) $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(MESA_GRAPHICS_LIBS)

list_all_objects:
	@echo $(display_obj) $(display_obj:.o=.ln)

$(DISPLAY).irisgl.pixie:  $(DISPLAY).irisgl
	@pixie $(DISPLAY).irisgl -o $@

prof:
	prof -quit 200 -pixie Display.irisgl -p >&! profiling/procedures
	prof -quit 200 -pixie Display.irisgl -h >&! profiling/heavy
	prof -pixie Display.irisgl >&! profiling/gprof

#-----------------

$(PROTOTYPE_FILE): $(display_obj:.o=.c) Include/*.h
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(display_obj:.o=.c)

clean_all: clean
	\rm -rf $(DISPLAY).irisgl $(DISPLAY).opengl $(DISPLAY).mesa

update: Display.irisgl
	strip Display.irisgl
	\mv ~david/public_bin/Display ~david/public_bin/Display.old
	\mv Display.irisgl ~david/public_bin/Display
	\cp Display.menu ~david/public_bin
	\rm -rf ~david/public_bin/Display.old
