#!/bin/bash

# The source to be tested
cd tests/test_project
j=0
echo -n "" > data.txt
while (( $j < 10001 )); do
    echo $((${j}/100))"%"

    echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){
  int num = atoi(argv[1]);
  awake_the_weaver(); // Initializing Weaver API\n
  polygon *poly = new_polygon(num" > src/game.c
    i=0
    str=""
    while (( $i < $j )); do
	str=${str}", ${i}.0, ${i}.0"
	i=$(($i+1))
    done
    echo -ne "${str});
  XSync(_dpy, 1);
  DEBUG_TIMER_START();
  rotate_polygon(poly, ${i}, ${i}, 0.05);
  DEBUG_TIMER_STOP();
  destroy_polygon(poly);
  may_the_weaver_sleep();
  return 0;\n
}" >> src/game.c
    make &> /dev/null

    media=0
    for (( k=0; (($k < 10)); k=$((${k}+1)) )); do
	inc=$(./test_project ${j})
	media=$((${media}+${inc}))
    done
    media=$((${media}/10))
    echo -n ${j}" " >> data.txt
    echo ${media} >> data.txt
    j=$(($j+500))
done

B0=$(head -n 1 data.txt | cut -d " " -f 2)
A0=$(tail -n 1 data.txt | cut -d " " -f 2)
A0=$(echo "scale=4; $((${A0}-${B0}))/10000" | bc -l)

echo "set output \"rotate_polygon.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{rotate\_polygon(a,b)}" >> ../tex/report.tex
echo "Function used for rotating a polygon." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv rotate_polygon.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/rotate_polygon.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a linear theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${A0}x+${B0}\$." >> ../tex/report.tex
cd -
