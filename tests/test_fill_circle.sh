#!/bin/bash

# The source to be tested
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){
  int x, y, color;
  awake_the_weaver(); // Initializing Weaver API
  x = window_width / 2; y = window_height / 2; color = rand();
  XSync(_dpy, 1);
  DEBUG_TIMER_START();
  fill_circle(x, y, y, color);
  XSync(_dpy, 1);
  DEBUG_TIMER_END();
  may_the_weaver_sleep();
  return 0;\n
}" > src/game.c
make &> /dev/null
j=0
sum=0
echo -n "" > data.txt
while (( $j < 100 )); do
    echo "${j}%"
    media=$(./test_project $j)
    echo -n ${j}" " >> data.txt
    echo ${media} >> data.txt
    sum=$((${sum}+${media}))
    j=$(($j+1))
done
sum=$((${sum}/100))

echo "set output \"fill_circle.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{fill\_circle(a,b,c,d)}" >> ../tex/report.tex
echo "Function used for filling a circle in the" >> ../tex/report.tex
echo "screen, given it's coordinates. " >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv fill_circle.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/fill_circle.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${sum}\$." >> ../tex/report.tex
cd -
