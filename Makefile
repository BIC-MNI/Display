include ../Graphics/Makefile.include

OPT = $(OPT_O) -Wf,-XNd11000

OPT_g = -g
OPT_O = -O

IMAGE_DIR = /usr/people/4Dgifts/iristools
IMAGE_LIB = ../RGB_files/librgb_files.a $(IMAGE_DIR)/libimage/libimage.a

INCLUDE = -IInclude -I../Modules/Include $(GRAPHICS_INCLUDE) \
          $(C_UTILS_INCLUDE) \
          -I../RGB_files

PROTOTYPE_FILE = Include/display_prototypes.h

main/main.o: Include/global_variables.h

DISPLAY = display

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
           voxel_scan/scan_markers.o \
           voxel_scan/scan_objects.o \
           voxel_scan/scan_polygons.o \
           voxel_scan/scan_lines.o \
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
           menu/menu.o \
           menu/input_menu.o \
           menu/selected.o \
           menu/text.o \
           cursor_contours/contours.o \
           segmenting/cut_neighbours.o \
           segmenting/expand_3d.o \
           segmenting/fill_3d.o \
           segmenting/labels.o \
           segmenting/regions.o \
           segmenting/segmenting.o \
           segmenting/segment_polygons.o \
           slice_window/colour_bar.o \
           slice_window/colour_coding.o \
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

MODULE_LIBS = -L../Modules \
              -ldeform -lgeometry -lnumerical -lmarching \
              -lsurface -ldata_structures -L/usr/lib


display_lint = $(display_obj:.o=.ln)

lint_display: $(PROTOTYPE_FILE) $(display_lint)
	@echo "Global lint started"
	@$(LINT) -u $(LINTFLAGS) $(display_lint) $(MODULE_LIBS) $(GRAPHICS_LINT_LIBS) | filter_lint

$(DISPLAY): $(PROTOTYPE_FILE) $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(IMAGE_LIB) \
              $(MODULE_LIBS) $(GRAPHICS_LIBS)

$(DISPLAY)_2d: $(PROTOTYPE_FILE) $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(MODULE_LIBS) $(GRAPHICS_LIBS_2D)

list_all_objects:
	@echo $(display_obj) $(display_obj:.o=.ln)

display.pixie:  $(DISPLAY)
	if( -e display.Addrs ) rm display.Addrs
	if( -e display.Counts ) rm display.Counts
	@\rm -f display.Counts
	@pixie display -o $@

prof:
	prof -pixie display -proc >&! profiling/procedures
	prof -pixie display -heavy >&! profiling/heavy

#-----------------

$(PROTOTYPE_FILE): $(display_obj:.o=.c) Include/*.h
	@echo "#ifndef  DEF_DISPLAY_PROTOTYPES"              >  $@
	@echo "#define  DEF_DISPLAY_PROTOTYPES"              >> $@
	@extract_functions -public $(display_obj:.o=.c)          >> $@
	@echo "#endif"                                       >> $@

