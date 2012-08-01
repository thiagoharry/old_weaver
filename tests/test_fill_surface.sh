#!/bin/bash

# The source to be tested
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){
  int num;
  surface *surf;
  num = atoi(argv[1]);
  awake_the_weaver(); // Initializing Weaver API  
  surf = new_surface(1, num);
  XSync(_dpy, 1); 
  DEBUG_TIMER_START();
  fill_surface(surf, BLUE);
  XSync(_dpy, 1);
  DEBUG_TIMER_END();
  may_the_weaver_sleep();
  return 0;\n
}" > src/game.c
make &> /dev/null

j=1
sum=0
first=0
last=0
echo -n "" > data.txt 
while (( $j <= 1000 )); do
    echo $(($j /10))"%"
    media=$(./test_project $j)
    echo -n ${j}" " >> data.txt
    echo ${media} >> data.txt
    sum=$((${sum}+${media}))
    j=$(($j+1))
done
first=$(./test_project 1)
last=$(./test_project 1000000)
sum=$((${sum}/1000))
b=${first}
a=$(echo "scale=6; (${last}-${first})/1000000" | bc)

echo "set output \"fill_surface.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{fill\_surface(a,b)}" >> ../tex/report.tex
echo "Function used for turning transparent a " >> ../tex/report.tex
echo "rectangular area in a surface. " >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv fill_surface.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/fill_surface.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a linear theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${a}x+${b}\$." >> ../tex/report.tex
cd -
