.SUFFIXES: .tex .dvi .DVI .ps .rgb .view .viewps .print .html .bbl

.tex.dvi:
	-latex $<

.tex.DVI: FORCE
	-latex $<

.tex.bbl:
	bibtex $*

.rgb.ps:
	tops $< -eps -rgb > $@

.dvi.view:
	xdvi $<

.dvi.ps:
	dvips $* -o $@

.ps.print:
	lp $<

.ps.viewps:
	ghostview $<

.tex.html:
	latex2html -split 4 $<

FORCE:

clean:
	\rm -f *.aux *.log *.dvi *.toc *.bbl *.blg *.dep
