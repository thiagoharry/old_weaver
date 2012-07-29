#!/bin/bash

# The source to be tested
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){
  int num;
  num = atoi(argv[1]);
  awake_the_weaver(); // Initializing Weaver API  
  XSync(_dpy, 1); 
  DEBUG_TIMER_START();
  draw_line(0, 0, num, num, BLUE);
  XSync(_dpy, 1);
  DEBUG_TIMER_END();
  may_the_weaver_sleep();
  return 0;\n
}" > src/game.c
make &> /dev/null
j=0
sum=0
echo -n "" > data.txt 
while (( $j < 1000 )); do
    echo $(($j /10))"%"
    media=$(./test_project $j)
    echo -n ${j}" " >> data.txt
    echo ${media} >> data.txt
    sum=$((${sum}+${media}))
    j=$(($j+1))
done
sum=$((${sum}/1000))

echo "set output \"draw_line.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{draw\_line(a,b,c,d)}" >> ../tex/report.tex
echo "Function used for drawing a line in the" >> ../tex/report.tex
echo "screen, given it's coordinates. " >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv draw_line.eps ../tex
VALUE=$(tail -n 1 data.txt | cut -d " " -f 2)
VALUE=$(echo "scale=2; ${VALUE}/10000" | bc -l)
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/draw_line.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${sum}\$." >> ../tex/report.tex
cd -
