include $(SRC_DIRECTORY)/Graphics/Makefile.include
include $(SRC_DIRECTORY)/David/Makefile.include

OPT = $(OPT_O)

LDFLAGS =

INCLUDE = -IInclude $(DLIB_INCLUDE) $(GRAPHICS_INCLUDE)

PROTOTYPE_FILE = Include/display_prototypes.h

main/$(ARCH_DIR)/main.o main/$(ARCH_DIR)/main.u: Include/global_variables.h
menu/$(ARCH_DIR)/menu.u menu/$(ARCH_DIR)/menu.o: menu/Display.menu.include

display_src = \
           main/$(ARCH_DIR)/main.c \
           main/$(ARCH_DIR)/display.c \
           main/$(ARCH_DIR)/event_loop.c \
           main/$(ARCH_DIR)/graphics.c \
           main/$(ARCH_DIR)/three_d.c \
           main/$(ARCH_DIR)/transforms.c \
           atlas/$(ARCH_DIR)/atlas.c \
           input_files/$(ARCH_DIR)/input_files.c \
           input_files/$(ARCH_DIR)/volume_file.c \
           deform/$(ARCH_DIR)/deform.c \
           callbacks/$(ARCH_DIR)/atlas.c \
           callbacks/$(ARCH_DIR)/call_globals.c \
           callbacks/$(ARCH_DIR)/colour_coding.c \
           callbacks/$(ARCH_DIR)/deform.c \
           callbacks/$(ARCH_DIR)/file.c \
           callbacks/$(ARCH_DIR)/line_ops.c \
           callbacks/$(ARCH_DIR)/object_ops.c \
           callbacks/$(ARCH_DIR)/marker_ops.c \
           callbacks/$(ARCH_DIR)/polygon_ops.c \
           callbacks/$(ARCH_DIR)/quit.c \
           callbacks/$(ARCH_DIR)/regions.c \
           callbacks/$(ARCH_DIR)/render_ops.c \
           callbacks/$(ARCH_DIR)/segmenting.c \
           callbacks/$(ARCH_DIR)/surf_segmenting.c \
           callbacks/$(ARCH_DIR)/surface_curves.c \
           callbacks/$(ARCH_DIR)/surface_extract.c \
           callbacks/$(ARCH_DIR)/view_ops.c \
           callbacks/$(ARCH_DIR)/volume_ops.c \
           callbacks/$(ARCH_DIR)/volume_transform_ops.c \
           current_obj/$(ARCH_DIR)/current_obj.c \
           edit_surface/$(ARCH_DIR)/connected.c \
           edit_surface/$(ARCH_DIR)/edit.c \
           edit_surface/$(ARCH_DIR)/segment.c \
           images/$(ARCH_DIR)/images.c \
           markers/$(ARCH_DIR)/markers.c \
           surface_extraction/$(ARCH_DIR)/boundary_extraction.c \
           surface_extraction/$(ARCH_DIR)/data_structs.c \
           surface_extraction/$(ARCH_DIR)/init_surface.c \
           surface_extraction/$(ARCH_DIR)/extract.c \
           surface_extraction/$(ARCH_DIR)/surface.c \
           surface_extraction/$(ARCH_DIR)/surface_events.c \
           voxel_scan/$(ARCH_DIR)/scan_objects.c \
           events/$(ARCH_DIR)/change_markers.c \
           events/$(ARCH_DIR)/clip_plane.c \
           events/$(ARCH_DIR)/film_loop.c \
           events/$(ARCH_DIR)/magnify.c \
           events/$(ARCH_DIR)/mouse.c \
           events/$(ARCH_DIR)/mouse_trans.c \
           events/$(ARCH_DIR)/pick_object.c \
           events/$(ARCH_DIR)/pick_view.c \
           events/$(ARCH_DIR)/rotate_slice.c \
           events/$(ARCH_DIR)/spaceball.c \
           events/$(ARCH_DIR)/virt_sb.c \
           events/$(ARCH_DIR)/window_man.c \
           events/$(ARCH_DIR)/utilities.c \
           immediate_mode/$(ARCH_DIR)/draw_immed.c \
           intersect/$(ARCH_DIR)/ray_polygons.c \
           intersect/$(ARCH_DIR)/plane_polygons.c \
           cursor/$(ARCH_DIR)/cursor.c \
           cursor/$(ARCH_DIR)/cursor_icon.c \
           menu/$(ARCH_DIR)/build_menu.c \
           menu/$(ARCH_DIR)/cursor_pos.c \
           menu/$(ARCH_DIR)/menu.c \
           menu/$(ARCH_DIR)/input_menu.c \
           menu/$(ARCH_DIR)/selected.c \
           menu/$(ARCH_DIR)/text.c \
           cursor_contours/$(ARCH_DIR)/contours.c \
           segmenting/$(ARCH_DIR)/cut_neighbours.c \
           segmenting/$(ARCH_DIR)/painting.c \
           segmenting/$(ARCH_DIR)/segmenting.c \
           segmenting/$(ARCH_DIR)/segment_polygons.c \
           slice_window/$(ARCH_DIR)/colour_bar.c \
           slice_window/$(ARCH_DIR)/colour_coding.c \
           slice_window/$(ARCH_DIR)/crop.c \
           slice_window/$(ARCH_DIR)/draw_slice.c \
           slice_window/$(ARCH_DIR)/histogram.c \
           slice_window/$(ARCH_DIR)/pick_angle.c \
           slice_window/$(ARCH_DIR)/quadmesh.c \
           slice_window/$(ARCH_DIR)/slice.c \
           slice_window/$(ARCH_DIR)/slice_3d.c \
           slice_window/$(ARCH_DIR)/slice_events.c \
           slice_window/$(ARCH_DIR)/undo.c \
           slice_window/$(ARCH_DIR)/view.c \
           surface_curves/$(ARCH_DIR)/closest_line.c \
           surface_curves/$(ARCH_DIR)/events.c \
           surface_curves/$(ARCH_DIR)/edge_distance.c \
           tubes/$(ARCH_DIR)/convert_lines.c \
           structures/$(ARCH_DIR)/action_table.c \
           structures/$(ARCH_DIR)/fit_view.c \
           structures/$(ARCH_DIR)/lights.c \
           structures/$(ARCH_DIR)/render.c \
           structures/$(ARCH_DIR)/view.c \
           structures/$(ARCH_DIR)/window.c

$(display_src) menu/$(ARCH_DIR)/Display.menu.include:
	@$(MAKE_SOURCE_LINKS)

menu/$(ARCH_DIR)/menu.o menu/$(ARCH_DIR)/menu.u: \
         menu/$(ARCH_DIR)/Display.menu.include

display_obj = $(display_src:.c=.o)

LINT_LIBS = $(DLIB_LINT_LIBS) $(GRAPHICS_LINT_LIBS)

display_lint = $(display_src:.c=.ln)

lint_display: $(PROTOTYPE_FILE) $(display_lint)
	@$(LINT) $(LINTFLAGS) $(display_lint) $(LINT_LIBS)

Display.irisgl: $(PROTOTYPE_FILE) $(display_obj) Display.menu
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(IRISGL_GRAPHICS_LIBS)

Display-O3.irisgl: $(PROTOTYPE_FILE) $(display_obj:.o=.u) Display.menu
	$(CC) $(LDFLAGS) -O3 $(display_obj:.o=.u) -o $@ $(DLIB_LIBS-O3) \
                          $(IRISGL_GRAPHICS_LIBS-O3)

Display.opengl: $(PROTOTYPE_FILE) $(display_obj) Display.menu
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(OPENGL_GRAPHICS_LIBS)

Display-O3.opengl: $(PROTOTYPE_FILE) $(display_obj:.o=.u) Display.menu
	$(CC) $(LDFLAGS) -O3 $(display_obj:.o=.u) -o $@ $(DLIB_LIBS-O3) \
                          $(OPENGL_GRAPHICS_LIBS-O3)

Display.mesa: $(PROTOTYPE_FILE) $(display_obj) Display.menu
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(DLIB_LIBS) \
                          $(MESA_GRAPHICS_LIBS)

Display-O3.mesa: $(PROTOTYPE_FILE) $(display_obj:.o=.u) Display.menu
	$(CC) $(LDFLAGS) -O3 $(display_obj:.o=.u) -o $@ $(DLIB_LIBS-O3) \
                          $(MESA_GRAPHICS_LIBS-O3)

list_all_objects:
	@echo $(display_obj) $(display_src:.c=.ln)

Display.irisgl.pixie:  Display.irisgl
	@pixie Display.irisgl -o $@

prof:
	prof -E MI_convert_type -E memcpy -quit 100 -pixie -h -p Display.irisgl >&! profiling/heavy

#-----------------

$(PROTOTYPE_FILE): $(display_src)
	@$(MAKE_PROTOTYPES) $@ $(display_src)

clean_all: clean
	\rm -rf Display.irisgl Display.opengl Display.mesa
	\rm -rf Display-O3.irisgl Display-O3.opengl Display-O3.mesa

update: Display.irisgl Display.menu
	strip Display.irisgl
	\mv ~david/public_bin/Display ~david/public_bin/Display.old
	\mv Display.irisgl ~david/public_bin/Display
	\cp Display.menu ~david/public_bin
#	\rm -rf ~david/public_bin/Display.old

update-O3: Display-O3.irisgl Display-O3.mesa Display-O3.opengl Display.menu
	strip Display-O3.irisgl Display-O3.mesa Display-O3.opengl
	\mv ~david/public_bin/Display ~david/public_bin/Display.old
	\mv Display-O3.irisgl ~david/public_bin/Display
	\mv Display-O3.mesa ~david/public_bin/Display.mesa
	\mv Display-O3.opengl ~david/public_bin/Display.opengl
	\cp Display.menu ~david/public_bin
	\rm  ~david/public_bin/Display.old

update-O3.mesa: Display-O3.mesa Display.menu
	strip Display-O3.mesa
	\mv ~david/public_bin/Display.mesa ~david/public_bin/Display.mesa.old
	\mv Display-O3.mesa ~david/public_bin/Display.mesa
	\cp Display.menu ~david/public_bin
#	\rm -rf ~david/public_bin/Display.mesa.old

Display.menu: menu/Display.menu.include
	sed -e 's/\\//g' -e 's/\"//'    < menu/Display.menu.include    > $@
