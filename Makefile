include ../C_dev/Makefile.include

OPT = -g

INCLUDE = -IInclude -I$(C_UTILS_INCLUDE) -I/@/portia/usr/include

#LIBS = -L/@/portia/usr/lib -lgl -lm
LIBS = -lgl -lm

graphics_obj = graphics_lib/GL_graphics.o \
               globals.o \
               structures/action_table.o \
               structures/event_struct.o \
               structures/fit_view.o \
               structures/lights.o \
               structures/render.o \
               structures/view.o \
               $(C_UTILS_SRC)/files.o \
               $(C_UTILS_SRC)/points.o \
               $(C_UTILS_SRC)/progress.o \
               $(C_UTILS_SRC)/transforms.o

display_obj = \
           main/main.o \
           main/display.o \
           main/event_loop.o \
           main/graphics.o \
           $(graphics_obj) \
           graphics_io.o \
           callbacks/file.o \
           callbacks/globals.o \
           callbacks/object_ops.o \
           callbacks/quit.o \
           callbacks/render_ops.o \
           callbacks/view_ops.o \
           callbacks/volume_ops.o \
           current_obj/current_obj.o \
           events/clip_plane.o \
           events/magnify.o \
           events/mouse.o \
           events/mouse_trans.o \
           events/pick_view.o \
           events/virt_sb.o \
           events/window_man.o \
           events/utilities.o \
           immediate_mode/draw_immed.o \
           menu/build_menu.o \
           menu/menu.o \
           menu/input_menu.o \
           menu/menu_update.o \
           menu/selected.o \
           slice_window/slice.o \
           slice_window/draw_slice.o \
           $(C_UTILS_SRC)/alloc.o \
           $(C_UTILS_SRC)/bitlist.o \
           $(C_UTILS_SRC)/colours.o \
           $(C_UTILS_SRC)/lines.o \
           $(C_UTILS_SRC)/mr_io.o \
           $(C_UTILS_SRC)/objects.o \
           $(C_UTILS_SRC)/object_io.o \
           $(C_UTILS_SRC)/random_order.o \
           $(C_UTILS_SRC)/random.o \
           $(C_UTILS_SRC)/resample.o \
           $(C_UTILS_SRC)/roi_io.o \
           $(C_UTILS_SRC)/string.o \
           $(C_UTILS_SRC)/volume.o \
           $(C_UTILS_SRC)/time.o

display_lint = $(display_obj:.o=.ln)

globals.o:  Include/def_globals.h

test_obj = test.o \
           $(C_UTILS_SRC)/time.o \
           $(graphics_obj)

test_lint = $(test_obj:.o=.ln)

display: $(display_obj)
	$(CC) $(CFLAGS) $(display_obj) -o $@ $(LIBS)

lint_display: $(display_lint)
	$(LINT) -u $(LINTFLAGS) $(display_lint)

test: $(test_obj)
	$(CC) $(CFLAGS) $(test_obj) -o $@ $(LIBS)

lint_test: $(test_lint)
	$(LINT) -u $(LINTFLAGS) $(test_lint)
