include $(SRC_DIRECTORY)/Graphics/Makefile.include
include $(SRC_DIRECTORY)/David/Makefile.include

OPT = -g $(SPECIAL_OPT)

LDFLAGS =

INCLUDE = -IInclude $(DLIB_INCLUDE) $(GRAPHICS_INCLUDE)

PROTOTYPE_FILE = Include/display_prototypes.h

main/main.o: Include/global_variables.h
main/main.u: Include/global_variables.h

DISPLAY = Display

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
           deform/deform.c \
           callbacks/atlas.c \
           callbacks/call_globals.c \
           callbacks/colour_coding.c \
           callbacks/deform.c \
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
           markers/segment.c \
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

LINT_LIBS = $(DLIB_LINT_LIBS) $(GRAPHICS_LINT_LIBS)

display_lint = $(display_src:.c=.ln)

lint_display: $(PROTOTYPE_FILE) $(display_lint)
	@$(LINT) $(LINTFLAGS) $(display_lint) $(LINT_LIBS)

$(DISPLAY).irisgl: $(PROTOTYPE_FILE) $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(IRISGL_GRAPHICS_LIBS)

$(DISPLAY)-O3.irisgl: $(PROTOTYPE_FILE) $(display_obj:.o=.u)
	$(CC) $(LDFLAGS) -O3 $(display_obj:.o=.u) -o $@ $(DLIB_LIBS-O3) \
                          $(IRISGL_GRAPHICS_LIBS-O3)

$(DISPLAY).opengl: $(PROTOTYPE_FILE) $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(OPENGL_GRAPHICS_LIBS)

$(DISPLAY)-O3.opengl: $(PROTOTYPE_FILE) $(display_obj:.o=.u)
	$(CC) $(LDFLAGS) -O3 $(display_obj:.o=.u) -o $@ $(DLIB_LIBS-O3) \
                          $(OPENGL_GRAPHICS_LIBS-O3)

$(DISPLAY).mesa: $(PROTOTYPE_FILE) $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(MESA_GRAPHICS_LIBS)

$(DISPLAY)-O3.mesa: $(PROTOTYPE_FILE) $(display_obj:.o=.u)
	$(CC) $(LDFLAGS) -O3 $(display_obj:.o=.u) -o $@ $(DLIB_LIBS-O3) \
                          $(MESA_GRAPHICS_LIBS-O3)

list_all_objects:
	@echo $(display_obj) $(display_src:.c=.ln)

$(DISPLAY).irisgl.pixie:  $(DISPLAY).irisgl
	@pixie $(DISPLAY).irisgl -o $@

prof:
	prof -quit 100 -pixie -h -p Display.irisgl >&! profiling/heavy

#-----------------

$(PROTOTYPE_FILE): $(display_src)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(display_src)

clean_all: clean
	\rm -rf $(DISPLAY).irisgl $(DISPLAY).opengl $(DISPLAY).mesa

update: $(DISPLAY).irisgl
	strip $(DISPLAY).irisgl
	\mv ~david/public_bin/Display ~david/public_bin/Display.old
	\mv $(DISPLAY).irisgl ~david/public_bin/Display
	\cp Display.menu ~david/public_bin
	\rm -rf ~david/public_bin/Display.old

update-O3: $(DISPLAY)-O3.irisgl
	strip $(DISPLAY)-O3.irisgl
	\mv ~david/public_bin/Display ~david/public_bin/Display.old
	\mv $(DISPLAY)-O3.irisgl ~david/public_bin/Display
	\cp Display.menu ~david/public_bin
	\rm -rf ~david/public_bin/Display.old

update-O3.mesa: $(DISPLAY)-O3.mesa
	strip $(DISPLAY)-O3.mesa
	\mv ~david/public_bin/Display.mesa ~david/public_bin/Display.mesa.old
	\mv $(DISPLAY)-O3.mesa ~david/public_bin/Display.mesa
	\cp Display.menu ~david/public_bin
	\rm -rf ~david/public_bin/Display.mesa.old
