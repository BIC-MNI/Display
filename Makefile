.SUFFIXES: .ln

OPT = -O

C_utils = ../C_src/utilities

Includes = -I. -I../marching_cubes/Include -I$(C_utils)/Include -I/@/portia/usr/include
CFLAGS = $(OPT) $(Includes)
LINTFLAGS = $(Includes)
LIBS = -L/@/portia/usr/lib -lgl -lm

.c.ln:
	lint $(LINTFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	\rm -f *.o *.ln

clean_o:
	\rm -f *.o

clean_ln:
	\rm -f *.ln

graphics_obj = GL_graphics.o \
               render.o \
               globals.o \
               transforms.o \
               view.o \
               $(C_utils)/files.o \
               $(C_utils)/points.o \
               $(C_utils)/progress.o

display_obj = \
           main.o \
           $(graphics_obj) \
           graphics.o \
           display.o \
           graphics_io.o \
           event_struct.o \
           action_table.o \
           event_loop.o \
           lights.o \
           quit.o \
           fit_view.o \
           mouse.o \
           virt_sb.o \
           magnify.o \
           clip_plane.o \
           mouse_trans.o \
           window_man.o \
           menu.o \
           menu_update.o \
           build_menu.o \
           menu_input.o \
           object_ops.o \
           render_ops.o \
           view_ops.o \
           $(C_utils)/objects.o \
           $(C_utils)/object_io.o \
           $(C_utils)/time.o

display_lint = $(display_obj:.o=.ln)

globals.o:  def_globals.h

test_obj = test.o \
           $(C_utils)/time.o \
           $(graphics_obj)

test_lint = $(test_obj:.o=.ln)

display: $(display_obj)
	$(CC) $(CFLAGS) $(display_obj) -o $@ $(LIBS)

lint_display: $(display_lint)
	lint -u $(LINTFLAGS) $(display_lint)

test: $(test_obj)
	$(CC) $(CFLAGS) $(test_obj) -o $@ $(LIBS)

lint_test: $(test_lint)
	lint -u $(LINTFLAGS) $(test_lint)
