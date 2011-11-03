#!/bin/bash

echo "\end{document}" >> tests/tex/report.tex
latex tests/tex/report.tex
dvipdf report.dvi
rm -rf *.aux *.dvi *.log
evince report.pdf || okular report.pdf || kpdf report.pdf || xpdf report.pdf ||
true