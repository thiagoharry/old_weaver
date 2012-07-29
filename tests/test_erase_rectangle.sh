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
    erase_rectangle(cam, rect);
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
    sed s/\\\\limit_camera/limit_camera/ src/game.c > src/game.c~
    mv src/game.c~ src/game.c
    make &> /dev/null
    valor=$(./test_project)
    media0=$((${media0}+${valor}))
    echo -n ${j}" " >> data2.txt
    echo ${valor} >> data2.txt
    sed s/limit_camera/\\\\limit_camera/ src/game.c > src/game.c~
    mv src/game.c~ src/game.c
    make &> /dev/null
    j=$(($j+1))
done
media=$(echo "scale=2; ${media}/100" | bc -l)
media0=$(echo "scale=2; ${media0}/100" | bc -l)


echo "set output \"erase_rectangle.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" title \"unlimited camera\" with lines, \"data2.txt\" title \"limited\_camera\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{erase\_rectangle(a,b)}" >> ../tex/report.tex
echo "This function draw the perimeter of a rectangle" >> ../tex/report.tex
echo "using the colors in the background surface. Drawing" >> ../tex/report.tex
echo "a rectangle in a arbitrarily big surface, given the" >> ../tex/report.tex
echo "coordinates have exponential complexity. But as" >> ../tex/report.tex
echo "we are drawing only in the screen, this limits " >> ../tex/report.tex
echo "the time complexity to a constant. This function" >> ../tex/report.tex
echo "have different performances in limited or unlimited" >> ../tex/report.tex
echo "cameras." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv erase_rectangle.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/erase_rectangle.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant" >> ../tex/report.tex
echo "theoretical complexity, the time, in nanosseconds" >> ../tex/report.tex
echo "spent by this function is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${media}\$" >> ../tex/report.tex
echo "for unlimited cameras and \$f(x)=${media0}\$ for" >> ../tex/report.tex
echo "limited cameras." >> ../tex/report.tex
cd -
