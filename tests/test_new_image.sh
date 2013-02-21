#!/bin/bash

# The source to be tested
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){
  surface *surf;
  awake_the_weaver(); // Initializing Weaver API
  XSync(_dpy, 1);
  DEBUG_TIMER_START();
  surf = new_image(\"pixel.png\");
  XSync(_dpy, 1);
  DEBUG_TIMER_STOP();
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
    echo "$((${j}/10))%"
    convert -resize "${j}x${j}" images/pixel.png images/pixel.png
    media=$(./test_project)
    echo -n ${j}" " >> data.txt
    echo ${media} >> data.txt
    sum=$((${sum}+${media}))
    j=$(($j+10))
done
convert -resize "1x1" images/pixel.png images/pixel.png
first=$(./test_project)
convert -resize "1000x1000" images/pixel.png images/pixel.png
last=$(./test_project)
b=${first}
a=$(echo "scale=3; (${last}-${first})/1000" | bc)

echo "set output \"new_image.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{new\_image(a)}" >> ../tex/report.tex
echo "Function used for creating a new surface from a PNG file." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv new_image.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/new_image.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "The graph shows how much time in nanosseconds it takes to create a new surface \$x \times x \$ from a PNG file." >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a linear theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${a}x+${b}\$." >> ../tex/report.tex
cd -
