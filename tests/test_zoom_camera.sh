#!/bin/bash

# The source to be teste
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"
#include \"game.h\"
int main(int argc, char **argv){
  camera *cam = new_camera(0.0, 0.0, 100.0, 100.0);
  awake_the_weaver(); // Initializing Weaver API
  DEBUG_TIMER_START();
  zoom_camera(cam, 0.00001);
  zoom_camera(cam, 0.0001);
  zoom_camera(cam, 0.001);
  zoom_camera(cam, 0.01);
  zoom_camera(cam, 0.1);
  zoom_camera(cam, 1.1);
  zoom_camera(cam, 1.111);
  zoom_camera(cam, 10.0);
  zoom_camera(cam, 100.0);
  zoom_camera(cam, 1000.0);
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
media=$(echo "scale=2; ${media}/1000" | bc -l)

echo "set output \"zoom_camera.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{zoom\_camera(a,b,c)}" >> ../tex/report.tex
echo "Function used to zoom in or out a camera struct." >> ../tex/report.tex
echo "The above graphic had values  magnified 5X." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv zoom_camera.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/zoom_camera.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant" >> ../tex/report.tex
echo "theoretical complexity, the time, in nanosseconds" >> ../tex/report.tex
echo "spent by this function is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${media}\$" >> ../tex/report.tex
echo "in this computer." >> ../tex/report.tex
cd -
