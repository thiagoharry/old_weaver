#!/bin/bash

# The source to be tested
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){
  struct vector3 *v;
  awake_the_weaver(); // Initializing Weaver API
  XSync(_dpy, 1);
  DEBUG_TIMER_START();
  v = new_vector3(5.242, 8.435, 7.324);
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

echo "set output \"new_vector3.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{new\_vector3(a,b,c)}" >> ../tex/report.tex
echo "Function used for creating a 3d vector." >> ../tex/report.tex
echo "" >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv new_vector3.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/new_vector3.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${sum}\$." >> ../tex/report.tex
cd -
