noinst_LIBRARIES = libdummy.a
libdummy_a_SOURCES = build_menu.c cursor_pos.c input_menu.c menu.c selected.c text.c

EXTRA_DIST = Display.menu.include

INCLUDES = -I$(top_srcdir)/Include -I$(top_srcdir)/Graphics/Include $(MNI_INCLUDES)

CLEANFILES = Display.menu.include

menu.c: Display.menu.include

Display.menu.include: Display.menu.base Display.menu.diff-seal
if USE_SEAL
	patch -o $@ Display.menu.base Display.menu.diff-seal
else
	cp -f Display.menu.base $@
endif

