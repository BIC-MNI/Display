include ../../C_dev/Makefile.include

OPT = $(OPT_O)

OPT_g = -g
OPT_O = -DNO_DEBUG_ALLOC  -O

#INCLUDE = -IInclude -I$(C_UTILS_INCLUDE) -I/@/yorick/usr/include
INCLUDE = -IInclude -I$(C_UTILS_INCLUDE)

#LIBS = -L/@/portia/usr/lib -lgl -lm
LIBS = -lgl_s -lm

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
               stdio_decomp.o \
               points.o \
               progress.o \
               transforms.o

test_obj = test.o \
           ../time.o

test_lns = $(test_obj:.o=.ln)

test: $(test_obj)
	$(CC) $(test_obj) $(LIBS) -o $@

lint_test: $(test_lns)
	$(LINT) -u $(test_lns)

test.pixie: test
	pixie test -o $@

prof:
	prof -pixie test -heavy >&! prof.heavy
	prof -pixie test -proc >&! prof.proc
