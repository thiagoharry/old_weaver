#!/bin/bash

# The source to be teste
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"
#include \"game.h\"
int main(int argc, char **argv){
  awake_the_weaver(); // Initializing Weaver API
  DEBUG_TIMER_START();
  weaver_rest(0);
  DEBUG_TIMER_END();
  may_the_weaver_sleep();
  return 0;
}" > src/game.c
make &> /dev/null
j=1
media=0
echo -n "" > data.txt 
while (( $j < 101 )); do
    echo $(($j - 1))"%"
    valor=$(./test_project)
    media=$((${media}+${valor}))
    echo -n ${j}" " >> data.txt
    echo ${valor} >> data.txt
    make &> /dev/null
    j=$(($j+1))
done
media=$(echo "scale=2; ${media}/100" | bc -l)

echo "set output \"weaver_rest.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{weaver\_rest(a)}" >> ../tex/report.tex
echo "Function used to create a pause in the Weaver API." >> ../tex/report.tex
echo "As it uses the \emph{nanosleep} C function, it's" >> ../tex/report.tex
echo "known that it's theoretical complexity is" >> ../tex/report.tex
echo "exponential. It's necessary to create a controlled" >> ../tex/report.tex
echo "delay." >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As there's no use in measuring a controlled delay," >> ../tex/report.tex
echo "this test is using only \$0\$ as argument. This" >> ../tex/report.tex
echo "turns the function complexity to a constant, that" >> ../tex/report.tex
echo "enable us to measure the time spent by other" >> ../tex/report.tex
echo "functions used in \emph{weaver\_rest}, except" >> ../tex/report.tex
echo "\emph{nanosleep}." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv weaver_rest.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/weaver_rest.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function (in this test) has a constant" >> ../tex/report.tex
echo "theoretical complexity, the time, in nanosseconds" >> ../tex/report.tex
echo "spent by this function is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${media}\$" >> ../tex/report.tex
echo "in this computer." >> ../tex/report.tex
cd -
