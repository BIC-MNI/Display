noinst_LIBRARIES = libdummy.a
libdummy_a_SOURCES = build_menu.c cursor_pos.c input_menu.c menu.c selected.c text.c

EXTRA_DIST = Display.menu.base Display.menu.diff-seal

INCLUDES = -I$(top_srcdir)/Include -I$(top_srcdir)/Graphics/Include $(MNI_INCLUDES)

CLEANFILES = Display.menu.include

$(srcdir)/menu.c: Display.menu.include

Display.menu.include: Display.menu.base Display.menu.diff-seal
if USE_SEAL
	patch -o $@ $(srcdir)/Display.menu.base $(srcdir)/Display.menu.diff-seal
else
	cp -f $(srcdir)/Display.menu.base $@
endif

