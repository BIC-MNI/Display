include ../C_dev/Makefile.include

OPT = $(OPT_O)

OPT_g = -g
OPT_O = -O

INCLUDE = -IInclude $(C_UTILS_INCLUDE) -Igraphics_lib

LIBS = -lgl_s -lfm_s -lm
LIBS_PORTIA = -lgl -lm

PROG_UTILS = \
             prog_utils/alloc.o \
             prog_utils/alloc_check.o \
             prog_utils/arguments.o \
             prog_utils/files.o \
             prog_utils/progress.o \
             prog_utils/random.o \
             prog_utils/string.o \
             prog_utils/time.o

DATA_STRUCTURES = \
                  data_structures/bitlist.o \
                  data_structures/hash_table.o \
                  data_structures/skiplist.o

UTILITIES = \
            utilities/bintree.o \
            utilities/build_bintree.o \
            utilities/colours.o \
            utilities/geometry.o \
            utilities/globals.o \
            utilities/graphics_io.o \
            utilities/intersect.o \
            utilities/lines.o \
            utilities/line_circle.o \
            utilities/least_squares.o \
            utilities/marching_cubes.o \
            utilities/marching_no_holes.o \
            utilities/mr_io.o \
            utilities/neighbours.o \
            utilities/numerical.o \
            utilities/object_io.o \
            utilities/objects.o \
            utilities/path_surface.o \
            utilities/pixels.o \
            utilities/points.o \
            utilities/polygon_extract.o \
            utilities/polygon_sphere.o \
            utilities/polygons.o \
            utilities/quadmesh.o \
            utilities/random_order.o \
            utilities/resample.o \
            utilities/rgb_lookup.o \
            utilities/search_bintree.o \
            utilities/smoothing.o \
            utilities/stdio_decomp.o \
            utilities/subdivide_lines.o \
            utilities/subdivide_polygons.o \
            utilities/talairach.o \
            utilities/transforms.o \
            utilities/volume.o

graphics_obj = graphics_lib/GL_graphics.o \
               structures/action_table.o \
               structures/colour_coding.o \
               structures/event_struct.o \
               structures/fit_view.o \
               structures/lights.o \
               structures/render.o \
               structures/view.o \
               structures/window.o

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
           input_files/landmark_file.o \
           $(graphics_obj) \
           callbacks/atlas.o \
           callbacks/call_globals.o \
           callbacks/colour_coding.o \
           callbacks/file.o \
           callbacks/fit_surface.o \
           callbacks/line_ops.o \
           callbacks/object_ops.o \
           callbacks/marker_ops.o \
           callbacks/polygon_ops.o \
           callbacks/quit.o \
           callbacks/render_ops.o \
           callbacks/segmenting.o \
           callbacks/surf_segmenting.o \
           callbacks/surface_curves.o \
           callbacks/surface_extract.o \
           callbacks/view_ops.o \
           callbacks/volume_ops.o \
           colour_coding/colour_coding.o \
           current_obj/current_obj.o \
           deform/create_sphere.o \
           deform/deform_polygons.o \
           deform/deform_line.o \
           deform/find_in_direction.o \
           deform/search_utils.o \
           edit_surface/connected.o \
           edit_surface/edit.o \
           edit_surface/segment.o \
           markers/markers.o \
           markers/segment.o \
           surface_extraction/activity.o \
           surface_extraction/boundary_extraction.o \
           surface_extraction/data_structs.o \
           surface_extraction/init_surface.o \
           surface_extraction/extract.o \
           surface_extraction/surface.o \
           surface_extraction/surface_events.o \
           surface_fitting/create_model.o \
           surface_fitting/downhill_simplex.o \
           surface_fitting/evaluate.o \
           surface_fitting/evaluate_graphics.o \
           surface_fitting/minimization.o \
           surface_fitting/one_parm_minimization.o \
           surface_fitting/scan_to_voxels.o \
           surface_fitting/scan_polygons.o \
           surface_fitting/surface_fitting.o \
           surface_rep/spline.o \
           surface_rep/surface_reps.o \
           surface_rep/superquadric.o \
           events/clip_plane.o \
           events/film_loop.o \
           events/magnify.o \
           events/mouse.o \
           events/mouse_trans.o \
           events/pick_object.o \
           events/pick_view.o \
           events/slice_transforms.o \
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
           segmenting/connect.o \
           segmenting/cut.o \
           segmenting/expand_3d.o \
           segmenting/fill_3d.o \
           segmenting/segmenting.o \
           segmenting/segment_polygons.o \
           slice_window/colour_bar.o \
           slice_window/draw_slice.o \
           slice_window/render_markers.o \
           slice_window/slice.o \
           slice_window/slice_events.o \
           surface_curves/events.o \
           surface_curves/edge_distance.o \
           tubes/convert_lines.o \
           tubes/generate_tube.o \
           $(UTILITIES) \
           $(DATA_STRUCTURES) \
           $(PROG_UTILS)

objects_g.o: objects.c
	$(CC) $(INCLUDE) -g -DDEBUG -c objects.c -o $@

objects_O.o: objects.c
	$(CC) $(INCLUDE) -O -c objects.c -o $@

display_lint = $(display_obj:.o=.ln)

utilities/globals.o:  Include/def_globals.h

globals.ln:  Include/def_globals.h

test_obj = test.o \
           prog_utils/time.o \
           $(graphics_obj)

test_lint = $(test_obj:.o=.ln)

display: $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(LIBS)

display.portia: $(display_obj)
	$(CC) $(LDFLAGS) $(display_obj) -o $@ $(LIBS_PORTIA)

display.pixie:  $(display_obj)
	if( -e display ) rm display
	if( -e display.Addrs ) rm display.Addrs
	if( -e display.Counts ) rm display.Counts
	make display LIBS="-lfm -lgl -lX11 -lm"
	@\rm -f display.Counts
	@pixie display -o $@

prof:
	prof -pixie display -proc >&! profiling/procedures
	prof -pixie display -heavy >&! profiling/heavy

lint_display: $(display_lint)
	@echo "Global lint started"
	@$(LINT) -u $(LINTFLAGS) $(display_lint) | filter_lint

test: $(test_obj)
	$(CC) $(CFLAGS) $(test_obj) -o $@ $(LIBS)

lint_test: $(test_lint)
	$(LINT) -u $(LINTFLAGS) $(test_lint)

# ---------------------------------------

g_obj = \
                graphics.o \
                prog_utils/alloc.o \
                utilities/colours.o \
                prog_utils/files.o \
                utilities/points.o \
                utilities/object_io.o \
                prog_utils/time.o \
                utilities/transforms.o \
                prog_utils/random.o \
                utilities/random_order.o \
                utilities/stdio_decomp.o \
                structures/view.o \
                graphics_lib/GL_graphics.o

g_lint = $(g_obj:.o=.ln)

graphics: $(g_obj)
	$(CC) $(CFLAGS) $(g_obj) -o $@ $(LIBS)

lint_graphics: $(g_lint)
	$(LINT) -u $(LINTFLAGS) $(g_lint)

# --------------------------------------

bintree_obj = utilities/test_bintree.o \
              utilities/search_bintree.o \
              utilities/bintree.o \
              prog_utils/alloc.o \
              prog_utils/time.o \
              utilities/intersect.o \
              prog_utils/files.o \
              utilities/points.o \
              prog_utils/progress.o \
              prog_utils/random.o \
              utilities/geometry.o \
              intersect/intersect.o \
              utilities/build_bintree.o

bintree_ln = $(bintree_obj:.o=.ln)

test_bintree: $(bintree_obj)
	$(CC) $(CFLAGS) $(bintree_obj) -o $@ $(LIBS)


lint_bintree: $(bintree_ln)
	$(LINT) -u $(LINTFLAGS) $(bintree_ln)

# -------

timing_obj = timing.o \
             prog_utils/alloc.o \
             prog_utils/alloc_check.o \
             prog_utils/files.o \
             utilities/stdio_decomp.o \
             prog_utils/time.o

timing_ln = $(timing_obj:.o=.ln)

timing: $(timing_obj)
	$(CC) -O $(INCLUDE) $(timing_obj) -o $@ $(LIBS)


lint_timing: $(timing_ln)
	$(LINT) -u $(LINTFLAGS) $(timing_ln)

# -------

test_gl_obj = test_gl.o \
              utilities/stdio_decomp.o \
              prog_utils/time.o \
              utilities/object_io.o \
              utilities/points.o \
              prog_utils/alloc.o \
              prog_utils/alloc_check.o \
              prog_utils/arguments.o \
              prog_utils/files.o \
              prog_utils/string.o

test_gl_ln = $(test_gl_obj:.o=.ln)

test_gl: $(test_gl_obj)
	$(CC) $(INCLUDE) $(test_gl_obj) -o $@ $(LIBS)


lint_test_gl: $(test_gl_ln)
	$(LINT) -u $(LINTFLAGS) $(test_gl_ln)

# -------

fix_obj = reassemble.c \
                 prog_utils/alloc.c \
                 utilities/bintree.c \
                 utilities/build_bintree.c \
                 prog_utils/files.c \
                 utilities/object_io.c \
                 utilities/points.c \
                 utilities/polygons.c \
                 prog_utils/progress.c \
                 prog_utils/time.c \
                 utilities/colours.c

reassemble_ln = $(reassemble_obj)

reassemble: $(reassemble_obj)
	$(CC) $(INCLUDE) $(reassemble_obj) -o $@ $(LIBS)


lint_reassemble: $(reassemble_ln)
	$(LINT) -u $(LINTFLAGS) $(reassemble_ln)

# -------

add_lines_obj = add_lines.c \
                 prog_utils/alloc.o \
                 prog_utils/alloc_check.o \
                 prog_utils/files.o \
                 prog_utils/progress.o \
                 prog_utils/time.o \
                 utilities/object_io.o \
                 utilities/lines.o \
                 utilities/colours.o \
                 utilities/points.o \
                 utilities/stdio_decomp.o

add_lines_ln = $(add_lines_obj:.o=.ln)

add_lines: $(add_lines_obj)
	$(CC) -g $(INCLUDE) $(add_lines_obj) -o $@ $(LIBS)


lint_add_lines: $(add_lines_ln)
	$(LINT) -u $(LINTFLAGS) $(add_lines_ln)

# -------

tube_obj = test_tube.c \
                 prog_utils/alloc.o \
                 prog_utils/files.o \
                 utilities/geometry.o \
                 utilities/object_io.o \
                 utilities/points.o \
                 tubes/generate_tube.o \
                 utilities/colours.o \
                 utilities/stdio_decomp.o

tube_ln = $(tube_obj:.o=.ln)

test_tube: $(tube_obj)
	$(CC) -g $(INCLUDE) $(tube_obj) -o $@ $(LIBS)


lint_tube: $(tube_ln)
	$(LINT) -u $(LINTFLAGS) $(tube_ln)

# -------

buffer_obj = test_buffers.c \
             utilities/stdio_decomp.o

buffer_ln = $(buffer_obj:.o=.ln)

test_buffers: $(buffer_obj)
	$(CC) -g $(INCLUDE) $(buffer_obj) -o $@ $(LIBS)


lint_buffer: $(buffer_ln)
	$(LINT) -u $(LINTFLAGS) $(buffer_ln)

# -------

glhistory_obj = glhistory.c

glhistory_ln = $(glhistory_obj:.o=.ln)

glhistory: $(glhistory_obj)
	$(CC) -g $(glhistory_obj) -o $@ $(LIBS)


lint_glhistory: $(glhistory_ln)
	$(LINT) -u $(LINTFLAGS) $(glhistory_ln)

# -------

lamps_obj = lamps.c

lamps_ln = $(lamps_obj:.o=.ln)

lamps: $(lamps_obj)
	$(CC) -g $(lamps_obj) -o $@ $(LIBS)


lint_lamps: $(lamps_ln)
	$(LINT) -u $(LINTFLAGS) $(lamps_ln)

# -------

overlay_obj = overlay.o \
              prog_utils/alloc.o \
              prog_utils/alloc_check.o \
              utilities/stdio_decomp.o \
              prog_utils/time.o

overlay_ln = $(overlay_obj:.o=.ln)

overlay: $(overlay_obj)
	$(CC) -g $(INCLUDE) $(overlay_obj) -o $@ $(LIBS)


lint_overlay: $(overlay_ln)
	$(LINT) -u $(LINTFLAGS) $(overlay_ln)


# -------

film_loop_obj = film_loop.o \
                prog_utils/alloc.o \
                prog_utils/alloc_check.o \
                prog_utils/files.o \
                prog_utils/random.o \
                prog_utils/time.o \
                graphics_lib/GL_graphics.o \
                utilities/colours.o \
                utilities/object_io.o \
                utilities/random_order.o \
                utilities/transforms.o \
                utilities/stdio_decomp.o

film_loop_ln = $(film_loop_obj:.o=.ln)

film_loop: $(film_loop_obj)
	$(CC) $(INCLUDE) $(film_loop_obj) -o $@ $(LIBS)


lint_film_loop: $(film_loop_ln)
	$(LINT) -u $(LINTFLAGS) $(film_loop_ln)


# -------

display_pixels_obj = display_pixels.o \
                utilities/tiff.o \
                prog_utils/alloc.o \
                prog_utils/alloc_check.o \
                prog_utils/files.o \
                prog_utils/random.o \
                prog_utils/string.o \
                prog_utils/time.o \
                graphics_lib/GL_graphics.o \
                utilities/colours.o \
                utilities/object_io.o \
                utilities/least_squares.o \
                utilities/random_order.o \
                utilities/pixels.o \
                utilities/stdio_decomp.o \
                utilities/transforms.o

display_pixels_ln = $(display_pixels_obj:.o=.ln)

display_pixels: $(display_pixels_obj)
	$(CC) $(INCLUDE) $(display_pixels_obj) -o $@ $(LIBS)


lint_display_pixels: $(display_pixels_ln)
	$(LINT) -u $(LINTFLAGS) $(display_pixels_ln)


# -------

pick_points_obj = pick_points.o \
                utilities/tiff.o \
                prog_utils/alloc.o \
                prog_utils/alloc_check.o \
                prog_utils/files.o \
                prog_utils/random.o \
                prog_utils/string.o \
                prog_utils/time.o \
                graphics_lib/GL_graphics.o \
                utilities/colours.o \
                utilities/object_io.o \
                utilities/least_squares.o \
                utilities/pixels.o \
                utilities/random_order.o \
                utilities/stdio_decomp.o \
                utilities/transforms.o

pick_points_ln = $(pick_points_obj:.o=.ln)

pick_points: $(pick_points_obj)
	$(CC) $(INCLUDE) $(pick_points_obj) -o $@ $(LIBS)


lint_pick_points: $(pick_points_ln)
	$(LINT) -u $(LINTFLAGS) $(pick_points_ln)


# -------

resample_pixels_obj = resample_pixels.o \
                utilities/tiff.o \
                prog_utils/alloc.o \
                prog_utils/alloc_check.o \
                prog_utils/files.o \
                prog_utils/random.o \
                prog_utils/string.o \
                prog_utils/time.o \
                utilities/clip.o \
                utilities/colours.o \
                utilities/geometry.o \
                utilities/object_io.o \
                utilities/least_squares.o \
                utilities/pixels.o \
                utilities/points.o \
                utilities/random_order.o \
                utilities/resample_pixels.o \
                utilities/talairach.o \
                utilities/transforms.o \
                utilities/stdio_decomp.o

resample_pixels_ln = $(resample_pixels_obj:.o=.ln)

resample_pixels: $(resample_pixels_obj)
	$(CC) $(INCLUDE) $(resample_pixels_obj) -o $@ $(LIBS)


lint_resample_pixels: $(resample_pixels_ln)
	$(LINT) -u $(LINTFLAGS) $(resample_pixels_ln) | filter_lint


# -------

create_atlas_slice_obj = create_atlas_slice.o \
                utilities/tiff.o \
                prog_utils/alloc.o \
                prog_utils/alloc_check.o \
                prog_utils/files.o \
                prog_utils/random.o \
                prog_utils/string.o \
                prog_utils/time.o \
                utilities/clip.o \
                utilities/colours.o \
                utilities/geometry.o \
                utilities/object_io.o \
                utilities/least_squares.o \
                utilities/pixels.o \
                utilities/points.o \
                utilities/random_order.o \
                utilities/resample_pixels.o \
                utilities/talairach.o \
                utilities/transforms.o \
                utilities/stdio_decomp.o

create_atlas_slice_ln = $(create_atlas_slice_obj:.o=.ln)

create_atlas_slice: $(create_atlas_slice_obj)
	$(CC) $(INCLUDE) $(create_atlas_slice_obj) -o $@ $(LIBS)


lint_create_atlas_slice: $(create_atlas_slice_ln)
	$(LINT) -u $(LINTFLAGS) $(create_atlas_slice_ln) | filter_lint


# -------

convert_tiff_to_pixels_obj = convert_tiff_to_pixels.o \
                utilities/tiff.o \
                prog_utils/alloc.o \
                prog_utils/alloc_check.o \
                prog_utils/files.o \
                prog_utils/random.o \
                prog_utils/string.o \
                prog_utils/time.o \
                utilities/clip.o \
                utilities/colours.o \
                utilities/geometry.o \
                utilities/object_io.o \
                utilities/pixels.o \
                utilities/points.o \
                utilities/random_order.o \
                utilities/talairach.o \
                utilities/stdio_decomp.o

convert_tiff_to_pixels_ln = $(convert_tiff_to_pixels_obj:.o=.ln)

convert_tiff_to_pixels: $(convert_tiff_to_pixels_obj)
	$(CC) $(INCLUDE) $(convert_tiff_to_pixels_obj) -o $@ $(LIBS)


lint_convert_tiff_to_pixels: $(convert_tiff_to_pixels_ln)
	$(LINT) -u $(LINTFLAGS) $(convert_tiff_to_pixels_ln) | filter_lint


# -------

convert_to_8_bit_obj = convert_to_8_bit.o \
                prog_utils/alloc.o \
                prog_utils/alloc_check.o \
                prog_utils/files.o \
                prog_utils/random.o \
                prog_utils/string.o \
                prog_utils/time.o \
                utilities/clip.o \
                utilities/colours.o \
                utilities/geometry.o \
                utilities/object_io.o \
                utilities/pixels.o \
                utilities/points.o \
                utilities/random_order.o \
                utilities/talairach.o \
                utilities/stdio_decomp.o

convert_to_8_bit_ln = $(convert_to_8_bit_obj:.o=.ln)

convert_to_8_bit: $(convert_to_8_bit_obj)
	$(CC) $(INCLUDE) $(convert_to_8_bit_obj) -o $@ $(LIBS)


lint_convert_to_8_bit: $(convert_to_8_bit_ln)
	$(LINT) -u $(LINTFLAGS) $(convert_to_8_bit_ln) | filter_lint

# -------

test_graphics_obj = test_graphics.o \
        graphics_lib/GL_graphics.o \
        utilities/random_order.o \
        utilities/rgb_lookup.o \
        utilities/stdio_decomp.o \
        utilities/transforms.o \
        prog_utils/alloc.o \
        prog_utils/alloc_check.o \
        prog_utils/random.o \
        prog_utils/time.o

test_graphics_ln = $(test_graphics_obj:.o=.ln)

test_graphics: $(test_graphics_obj)
	$(CC) $(INCLUDE) $(test_graphics_obj) -o $@ $(LIBS)

lint_test_graphics: $(test_graphics_ln)
	$(LINT) -u $(LINTFLAGS) $(test_graphics_ln) | filter_lint
