#!/bin/bash

# The source to be tested
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){
  struct vector3 *v;
  awake_the_weaver(); // Initializing Weaver API
  v = new_vector3(5.242, 8.435, 7.324);
  XSync(_dpy, 1);
  DEBUG_TIMER_START();
  rotate_circle(v, 3.2, 5.89, 0.345);
  DEBUG_TIMER_STOP();
  destroy_vector3(v);
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

echo "set output \"rotate_circle.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{rotate\_circle(a,b,c)}" >> ../tex/report.tex
echo "Function used for rotating a 3d vector representing a circle." >> ../tex/report.tex
echo "" >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv rotate_circle.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/rotate_circle.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${sum}\$." >> ../tex/report.tex
cd -
