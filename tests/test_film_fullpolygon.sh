#!/bin/bash

# The source to be teste
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){\n
  awake_the_weaver(); // Initializing Weaver API\n
  camera *cam = new_camera(0.0, 0.0, (float) window_width, (float) window_height);
  //limit_camera(cam, 1, 1, window_width - 2, window_height -2);
  polygon *poly = new_polygon(0" > src/game.c
j=0
while (( $j < 100 )); do
    echo ",rand() % window_width, rand() % window_height" >> src/game.c
    j=$(($j+1))
done
echo -ne ");
  XSync(_dpy, 1); 
  DEBUG_TIMER_START();\n
  film_fullpolygon(cam, poly, RED);
  XSync(_dpy, 1);
  DEBUG_TIMER_END();\n
  may_the_weaver_sleep();\n
  return 0;\n
}" >> src/game.c
make &> /dev/null
j=0
echo -n "" > data.txt 
while (( $j < 101 )); do
    echo $(($j - 1))"%"
    media=0
    i=0
    while ((  $i < 10 )); do 
	valor=$(./test_project)
	media=$((${media}+${valor}))
	i=$(($i+1)); 
    done
    echo -n ${j}" " >> data.txt
    echo $((${media}/10)) >> data.txt
    
    sed s/\\\\limit_camera/limit_camera/ src/game.c > src/game.c~
    mv src/game.c~ src/game.c
    media0=0
    i=0
    while ((  $i < 10 )); do 
	valor=$(./test_project)
	media0=$((${media0}+${valor}))
	i=$(($i+1)); 
    done
    echo -n ${j}" " >> data2.txt
    echo $((${media0}/10)) >> data2.txt

    sed s/limit_camera/\\\\limit_camera/ src/game.c > src/game.c~
    mv src/game.c~ src/game.c

    sed s/new_polygon"("${j}/new_polygon"("$(($j+1))/ src/game.c > src/game.c~
    cp src/game.c{~,}
    make &> /dev/null
    j=$(($j+1))
done

B0=$(head -n 10 data.txt | tail -n 1 | cut -d " " -f 2)
A0=$((${B0}-$(head -n 1 data.txt | cut -d " " -f 2)))
if [ ${A0} -lt 0 ]; then
    A0=0
fi
A0=$(echo "scale=2; ${A0}/90" | bc -l)

B1=$(head -n 10 data2.txt | tail -n 1 | cut -d " " -f 2)
A1=$((${B1}-$(head -n 1 data2.txt | cut -d " " -f 2)))
if [ ${A1} -lt 0 ]; then
    A1=0
fi
A1=$(echo "scale=2; ${A1}/90" | bc -l)

echo "set output \"film_fullpolygon.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" title \"unlimited camera\" with lines, \"data2.txt\" title \"limited\_camera\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{film\_fullpolygon(a,b)}" >> ../tex/report.tex
echo "Function used for drawing filling a polygon in the" >> ../tex/report.tex
echo "screen, given it's coordinates. " >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv film_fullpolygon.eps ../tex
VALUE=$(tail -n 1 data.txt | cut -d " " -f 2)
VALUE=$(echo "scale=2; ${VALUE}/10000" | bc -l)
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/film_fullpolygon.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a linear theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${A0}x+${B0}\$" >> ../tex/report.tex
echo "for unlimited cameras and \$f(x)=${A1}x+${B1}\$." >> ../tex/report.tex
echo "for limited cameras." >> ../tex/report.tex
cd -
