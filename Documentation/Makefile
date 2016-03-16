# Makefile for the Display documentation
all: Display.pdf Display.html

SOURCES = Display.tex globals.tex colour_list.tex $(wildcard images/*.png)

Display.pdf: $(SOURCES)
	pdflatex Display.tex

Display.html: $(SOURCES) Display.cfg
	htlatex Display.tex Display.cfg

CLEANFILES = \
	Display.4ct \
	Display.4tc \
	Display.aux \
	Display.dvi \
	Display.idv \
	Display.lg \
	Display.log \
	Display.out \
	Display.tmp \
	Display.xref

clean:
	rm -f $(CLEANFILES)
