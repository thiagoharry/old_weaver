#!/bin/bash

# The source to be teste
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"
#include \"game.h\"
int main(int argc, char **argv){
  camera *cam = new_camera(0.0, 0.0, 100.0, 100.0);
  awake_the_weaver(); // Initializing Weaver API
  DEBUG_TIMER_START();
  limit_camera(cam, 0, 0, 1, 1);
  limit_camera(cam, 1, 1, 2, 2);
  limit_camera(cam, 2, 2, 4, 4);
  limit_camera(cam, 3, 3, 8, 8);
  limit_camera(cam, 4, 4, 16, 16);
  limit_camera(cam, 5, 5, 32, 32);
  limit_camera(cam, 6, 6, 64, 64);
  limit_camera(cam, 7, 7, 128, 128);
  limit_camera(cam, 8, 8, 256, 256);
  limit_camera(cam, 9, 9, 512, 512);
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

echo "set output \"limit_camera.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{limit\_camera(a,b,c)}" >> ../tex/report.tex
echo "Function used to limit the camera where a camera " >> ../tex/report.tex
echo "can draw. The above graphic had the real values" >> ../tex/report.tex
echo "magnified 10\$\times\$." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv limit_camera.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/limit_camera.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant" >> ../tex/report.tex
echo "theoretical complexity, the time, in nanosseconds" >> ../tex/report.tex
echo "spent by this function is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${media}\$" >> ../tex/report.tex
echo "in this computer." >> ../tex/report.tex
cd -
