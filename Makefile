.SUFFIXES: .ln

OPT = -O

C_utils = ../C_src/utilities

Includes = -IInclude -I../marching_cubes/Include -I$(C_utils)/Include -I/@/portia/usr/include
CFLAGS = $(OPT) $(Includes)
LINTFLAGS = $(Includes)
LIBS = -L/@/portia/usr/lib -lgl -lm

LINT = lint

.c.ln:
	$(LINT) $(LINTFLAGS) -c $<
	@if( "`echo $@ | sed -e 's/.*\///'`" != "$@" ) \
            mv `echo $@ | sed -e 's/.*\///'` $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	\rm -f *.o */*.o *.ln */*.ln

clean_o:
	\rm -f *.o

clean_ln:
	\rm -f *.ln

graphics_obj = graphics_lib/GL_graphics.o \
               globals.o \
               structures/action_table.o \
               structures/event_struct.o \
               structures/fit_view.o \
               structures/lights.o \
               structures/render.o \
               structures/transforms.o \
               structures/view.o \
               $(C_utils)/files.o \
               $(C_utils)/points.o \
               $(C_utils)/progress.o

display_obj = \
           main/main.o \
           main/display.o \
           main/event_loop.o \
           main/graphics.o \
           $(graphics_obj) \
           graphics_io.o \
           callbacks/object_ops.o \
           callbacks/quit.o \
           callbacks/render_ops.o \
           callbacks/view_ops.o \
           events/clip_plane.o \
           events/magnify.o \
           events/mouse.o \
           events/mouse_trans.o \
           events/virt_sb.o \
           events/window_man.o \
           menu/build_menu.o \
           menu/menu.o \
           menu/menu_input.o \
           menu/menu_update.o \
           $(C_utils)/objects.o \
           $(C_utils)/object_io.o \
           $(C_utils)/time.o

display_lint = $(display_obj:.o=.ln)

globals.o:  Include/def_globals.h

test_obj = test.o \
           $(C_utils)/time.o \
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
