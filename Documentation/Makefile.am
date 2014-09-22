
manual_sources = Display.tex displaywindows.rgb.gz menu.rgb.gz

EXTRA_DIST = Changes.list README Release_notes.v1.1 \
	how_to_texture_map.txt to_dos $(manual_sources)

# Convert from "native" graphics to something LaTeX can handle.

GRAPHICS_EXT = jpg

displaywindows.$(GRAPHICS_EXT): displaywindows.rgb.gz
	convert displaywindows.rgb.gz displaywindows.$(GRAPHICS_EXT)

menu.$(GRAPHICS_EXT): menu.rgb.gz
	convert menu.rgb.gz menu.$(GRAPHICS_EXT)

# Go straight to PDF so that JPG files work as expected.

Display.pdf: $(manual_sources) displaywindows.$(GRAPHICS_EXT) menu.$(GRAPHICS_EXT)
	pdflatex Display.tex


CLEANFILES = Display.aux Display.log Display.toc \
	displaywindows.$(GRAPHICS_EXT) menu.$(GRAPHICS_EXT)
