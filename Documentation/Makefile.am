
manual_sources = Display.tex david.sty displaywindows.rgb.gz menu.rgb.gz

EXTRA_DIST = Changes.list README Release_notes.v1.1 \
	how_to_texture_map.txt to_dos $(manual_sources)


displaywindows.ps: displaywindows.rgb.gz
	convert displaywindows.rgb.gz displaywindows.ps

menu.ps: menu.rgb.gz
	convert menu.rgb.gz menu.ps

Display.dvi: $(manual_sources) displaywindows.ps menu.ps
	latex Display.tex

Display.ps: Display.dvi
	dvips Display -o Display.ps


CLEANFILES = Display.aux Display.dvi Display.log Display.ps Display.toc \
	displaywindows.ps menu.ps
