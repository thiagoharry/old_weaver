#!/bin/bash

echo "Starting Weaver Tests..."
echo "Checking for test dependencies..."

echo -n "LaTeX........"
if latex --version &> /dev/null ; then
    echo "OK"
else
    /bin/echo -e "\033[31mFAILED\033[m"
fi

echo -n "Gnuplot......"
if gnuplot --version &> /dev/null ; then
    echo "OK"
else
    /bin/echo -e "\033[31mFAILED\033[m"
fi

echo -n "ImageMagick.."
if convert --version &> /dev/null ; then
    echo "OK"
else
    /bin/echo -e "\033[31mFAILED\033[m"
fi

rm -rf tests/test_project
./weaver tests/test_project
cp -r tests/imgs/* tests/test_project/images

echo "\documentclass{article}" > tests/tex/report.tex
echo "\usepackage{graphicx}" >> tests/tex/report.tex
echo "\title{Weaver Tests -- Report}" >> tests/tex/report.tex
echo "\begin{document}" >> tests/tex/report.tex
echo "\maketitle" >> tests/tex/report.tex
echo "This is a report with the results of a battery" >> tests/tex/report.tex
echo "tests conducted in a machine called " >> tests/tex/report.tex
hostname >> tests/tex/report.tex
echo "." >> tests/tex/report.tex
echo "\begin{itemize}" >> tests/tex/report.tex
echo "\item" >> tests/tex/report.tex
echo "\textbf{Operating System Version: }" >> tests/tex/report.tex
echo "\begin{verbatim}" >> tests/tex/report.tex
cat /etc/issue >> tests/tex/report.tex
echo "\end{verbatim}" >> tests/tex/report.tex
echo "\item" >> tests/tex/report.tex
echo "\textbf{Weaver Version: }\\\\" >> tests/tex/report.tex
./weaver --version >> tests/tex/report.tex
echo "\end{itemize}" >> tests/tex/report.tex
echo "\section{Machine Info}" >> tests/tex/report.tex
echo "\subsection{CPU Info}" >> tests/tex/report.tex
echo "\begin{verbatim}" >> tests/tex/report.tex
cat /proc/cpuinfo >> tests/tex/report.tex
echo "\end{verbatim}" >> tests/tex/report.tex
echo "\subsection{Memory Info}" >> tests/tex/report.tex
echo "\begin{verbatim}" >> tests/tex/report.tex
cat /proc/meminfo >> tests/tex/report.tex
echo "\end{verbatim}" >> tests/tex/report.tex
echo "\section{Function Tests}" >> tests/tex/report.tex
