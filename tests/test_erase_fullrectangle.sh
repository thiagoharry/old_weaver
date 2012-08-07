#!/bin/bash

# The source to be teste
cd tests/test_project
cp ../imgs/pixel.png images
echo -ne "#include \"weaver/weaver.h\"
#include \"game.h\"
int main(int argc, char **argv){
  int i;
  rectangle *rect = new_rectangle(1.0, 1.0, (float) (window_width - 2),(float) (window_height - 2));
  camera *cam = new_camera(0.0, 0.0, (float) window_width, (float) window_height);
  awake_the_weaver(); // Initializing Weaver API
  //limit_camera(cam, 1, 1, window_width - 2, window_height -2);
  fill_surface(background, RED);
  DEBUG_TIMER_START();
    erase_fullrectangle(cam, rect);
  DEBUG_TIMER_END();
  may_the_weaver_sleep();
  return 0;
}" > src/game.c
make &> /dev/null
j=1
media=0
media0=0
echo -n "" > data.txt
while (( $j < 101 )); do
    echo $(($j - 1))"%"
    valor=$(./test_project)
    media=$((${media}+${valor}))
    echo -n ${j}" " >> data.txt
    echo ${valor} >> data.txt
    sed s/\\\\limit_camera/limit_camera/ src/game.c > src/game.c~
    mv src/game.c~ src/game.c
    make &> /dev/null
    valor=$(./test_project)
    valor=$((valor/2000))
    media0=$((${media}+${valor}))
    echo -n ${j}" " >> data2.txt
    echo ${valor} >> data2.txt
    sed s/limit_camera/\\\\limit_camera/ src/game.c > src/game.c~
    mv src/game.c~ src/game.c
    make &> /dev/null
    j=$(($j+1))
done
media=$(echo "scale=2; ${media}/100" | bc -l)
media0=$(echo "scale=2; ${media0}/100" | bc -l)

echo "set output \"erase_fullrectangle.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" title \"unlimited camera\" with lines, \"data2.txt\" title \"limited\_camera\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{erase\_fullrectangle(a,b)}" >> ../tex/report.tex
echo "This function draws and fills the perimeter of a" >> ../tex/report.tex
echo "rectangle using the colors in the background" >> ../tex/report.tex
echo "surface. Drawing a rectangle in a arbitrarily big" >> ../tex/report.tex
echo "surfce, given the coordinates have exponential" >> ../tex/report.tex
echo "complexity. But as we are drawing only in the" >> ../tex/report.tex
echo "screen, this limits the time complexity to a" >> ../tex/report.tex
echo "constant. This function have different performances" >> ../tex/report.tex
echo "in limited or unlimited cameras." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv erase_fullrectangle.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/erase_fullrectangle.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant" >> ../tex/report.tex
echo "theoretical complexity, the time, in nanosseconds" >> ../tex/report.tex
echo "spent by this function is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${media}\$" >> ../tex/report.tex
echo "for unlimited cameras and \$f(x)=${media0}\$ for" >> ../tex/report.tex
echo "limited cameras." >> ../tex/report.tex
cd -
