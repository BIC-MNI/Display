include ../C_dev/Makefile.include

OPT = -O

INCLUDE = -IInclude -I$(C_UTILS_INCLUDE) -I/@/yorick/usr/include

#LIBS = -L/@/portia/usr/lib -lgl -lm
LIBS = -lgl -lm

graphics_obj = graphics_lib/GL_graphics.o \
               globals.o \
               structures/action_table.o \
               structures/colour_coding.o \
               structures/event_struct.o \
               structures/fit_view.o \
               structures/lights.o \
               structures/render.o \
               structures/view.o \
               structures/window.o \
               files.o \
               points.o \
               progress.o \
               transforms.o

display_obj = \
           main/main.o \
           main/display.o \
           main/event_loop.o \
           main/graphics.o \
           main/three_d.o \
           main/transforms.o \
           $(graphics_obj) \
           callbacks/file.o \
           callbacks/globals.o \
           callbacks/object_ops.o \
           callbacks/quit.o \
           callbacks/render_ops.o \
           callbacks/surface_ops.o \
           callbacks/view_ops.o \
           callbacks/volume_ops.o \
           current_obj/current_obj.o \
           surface_extraction/activity.o \
           surface_extraction/data_structs.o \
           surface_extraction/init_surface.o \
           surface_extraction/extract.o \
           surface_extraction/surface.o \
           surface_extraction/surface_events.o \
           edit_surface/connected.o \
           edit_surface/edit.o \
           edit_surface/segment.o \
           events/clip_plane.o \
           events/film_loop.o \
           events/magnify.o \
           events/mouse.o \
           events/mouse_trans.o \
           events/pick_polygon.o \
           events/pick_view.o \
           events/pick_voxel.o \
           events/virt_sb.o \
           events/window_man.o \
           events/utilities.o \
           immediate_mode/draw_immed.o \
           intersect/intersect.o \
           cursor/cursor.o \
           cursor/cursor_icon.o \
           menu/build_menu.o \
           menu/menu.o \
           menu/input_menu.o \
           menu/menu_update.o \
           menu/selected.o \
           slice_window/draw_slice.o \
           slice_window/slice.o \
           slice_window/slice_events.o \
           alloc.o \
           arguments.o \
           build_bintree.o \
           bintree.o \
           search_bintree.o \
           bitlist.o \
           colours.o \
           geometry.o \
           graphics_io.o \
           hash_table.o \
           intersect.o \
           lines.o \
           marching_cubes.o \
           marching_no_holes.o \
           mr_io.o \
           neighbours.o \
           objects.o \
           object_io.o \
           path_surface.o \
           pixels.o \
           polygons.o \
           random_order.o \
           random.o \
           resample.o \
           roi_io.o \
           string.o \
           volume.o \
           time.o

display_lint = $(display_obj:.o=.ln)

globals.o:  Include/def_globals.h

globals.ln:  Include/def_globals.h

test_obj = test.o \
           time.o \
           $(graphics_obj)

test_lint = $(test_obj:.o=.ln)

display_ngx: $(display_obj)
	$(CC) $(CFLAGS) $(display_obj) -o $@ $(LIBS)

display: $(display_obj)
	$(CC) $(CFLAGS) $(display_obj) -o $@ $(LIBS)

display.pixie: display
	@\rm -f display.Counts
	@pixie display -o $@

lint_display: $(display_lint)
	@echo "Global lint started"
	@$(LINT) -u $(LINTFLAGS) $(display_lint) | filter_lint

test: $(test_obj)
	$(CC) $(CFLAGS) $(test_obj) -o $@ $(LIBS)

lint_test: $(test_lint)
	$(LINT) -u $(LINTFLAGS) $(test_lint)

film_loop_obj = \
                film_loop.o \
                alloc.o \
                files.o \
                points.o \
                object_io.o \
                time.o \
                transforms.o \
                random.o \
                random_order.o \
                graphics_lib/GL_graphics.o

film_loop_lint = $(film_loop_obj:.o=.ln)

film_loop: $(film_loop_obj)
	$(CC) $(CFLAGS) $(film_loop_obj) -o $@ $(LIBS)

lint_film_loop: $(film_loop_lint)
	$(LINT) -u $(LINTFLAGS) $(film_loop_lint)

bintree_obj = test_bintree.o \
              search_bintree.o \
              bintree.o \
              alloc.o \
              time.o \
              files.o \
              points.o \
              random.o \
              geometry.o \
              intersect/intersect.o \
              build_bintree.o

bintree_ln = $(bintree_obj:.o=.ln)

test_bintree: $(bintree_obj)
	$(CC) $(CFLAGS) $(bintree_obj) -o $@ $(LIBS)


lint_bintree: $(bintree_ln)
	$(LINT) -u $(LINTFLAGS) $(bintree_ln)

# -------

timing_obj = timing.c \
             alloc.c \
             files.c \
             time.c

timing_ln = $(timing_obj)

timing: $(timing_obj)
	$(CC) -O $(INCLUDE) $(timing_obj) -o $@ $(LIBS)


lint_timing: $(timing_ln)
	$(LINT) -u $(LINTFLAGS) $(timing_ln)

# -------

test_gl_obj = test_gl.c \
              alloc.o \
              colours.o \
              files.o \
              string.o \
              time.o

test_gl_ln = $(test_gl_obj)

test_gl: $(test_gl_obj)
	$(CC) -O $(INCLUDE) $(test_gl_obj) -o $@ $(LIBS)


lint_test_gl: $(test_gl_ln)
	$(LINT) -u $(LINTFLAGS) $(test_gl_ln)
