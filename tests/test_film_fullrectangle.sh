#!/bin/bash

# The source to be teste
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"
#include \"game.h\"
int main(int argc, char **argv){
  int i;
  camera *cam = new_camera(0.0, 0.0, (float) window_width, (float) window_height);
  rectangle *rect = new_rectangle(1.0, 1.0, (float) window_width- 1, (float) window_height - 1);
  awake_the_weaver(); // Initializing Weaver API
  DEBUG_TIMER_START();
  for(i = 0; i < 2000; i ++){
    film_fullrectangle(cam, rect, i);
  }
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
    valor=$((valor/2000))
    media=$((${media}+${valor}))
    echo -n ${j}" " >> data.txt
    echo ${valor} >> data.txt
    make &> /dev/null
    j=$(($j+1))
done
media=$(echo "scale=2; ${media}/100" | bc -l)
media0=${media}

echo -ne "#include \"weaver/weaver.h\"
#include \"game.h\"
int main(int argc, char **argv){
  int i;
  camera *cam = new_camera(0.0, 0.0, (float) window_width, (float) window_height);
  limit_camera(cam, 1, 1, window_width - 2, window_height - 2);
  rectangle *rect = new_rectangle(1.0, 1.0, (float) window_width- 1, (float) window_height - 1);
  awake_the_weaver(); // Initializing Weaver API
  DEBUG_TIMER_START();
  for(i = 0; i < 2000; i ++){
    film_fullrectangle(cam, rect, i);
  }
  DEBUG_TIMER_END();
  may_the_weaver_sleep();
  return 0;
}" > src/game.c
make &> /dev/null
j=1
media=0
echo -n "" > data2.txt 
while (( $j < 101 )); do
    echo $(($j - 1))"%"
    valor=$(./test_project)
    valor=$((valor/2000))
    media=$((${media}+${valor}))
    echo -n ${j}" " >> data2.txt
    echo ${valor} >> data2.txt
    make &> /dev/null
    j=$(($j+1))
done
media=$(echo "scale=2; ${media}/100" | bc -l)

echo "set output \"film_fullrectangle.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" title \"unlimited camera\" with lines, \"data2.txt\" title \"limited\_camera\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{film\_fullrectangle(a,b,c)}" >> ../tex/report.tex
echo "Fill a rectangle in an arbitrarily large surface." >> ../tex/report.tex
echo "have exponential complexity. But this function" >> ../tex/report.tex
echo "draw in the screen, so the number of pixels drawn" >> ../tex/report.tex
echo "is limited. So, we assume a constant complexity" >> ../tex/report.tex
echo "for this function." >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "There are differences in the performance if you use" >> ../tex/report.tex
echo "a limited camera or not." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv film_fullrectangle.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/film_fullrectangle.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a constant" >> ../tex/report.tex
echo "theoretical complexity, the time, in nanosseconds" >> ../tex/report.tex
echo "spent by this function is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${media0}\$" >> ../tex/report.tex
echo "for limited cameras and \$f(x)=${media}\$ for" >> ../tex/report.tex
echo "unlimited cameras." >> ../tex/report.tex
cd -
