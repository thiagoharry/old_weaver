#!/bin/bash

# The source to be teste
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"
#include \"game.h\"
int main(int argc, char **argv){
  DEBUG_TIMER_START();
  awake_the_weaver(); // Initializing Weaver API
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

echo "set output \"awake_the_weaver.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{awake\_the\_weaver()}" >> ../tex/report.tex
echo "Function used to initialize the Weaver API." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv awake_the_weaver.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/awake_the_weaver.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds spent " >> ../tex/report.tex
echo "by this function is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${media}\$" >> ../tex/report.tex
echo "in this computer." >> ../tex/report.tex
cd -
