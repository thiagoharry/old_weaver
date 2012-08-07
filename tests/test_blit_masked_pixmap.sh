#!/bin/bash

# The source to be tested
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){
  int num;
  surface *src, *dst;
  num = atoi(argv[1]);
  awake_the_weaver(); // Initializing Weaver API
  src = new_surface(num, num);
  dst = new_surface(num, num);
  fill_surface(src, BLUE);
  draw_rectangle_mask(src, num/2, num/2, 1, 1);
  draw_rectangle_mask(dst, 0, 0, 1, 1);
  XSync(_dpy, 1);
  DEBUG_TIMER_START();
  blit_masked_pixmap(src -> pix, dst -> mask, dst, 0, 0, src -> width, src -> height, 0, 0, 0, 0);
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
while (( $j <= 10000 )); do
    echo $(($j /100))"%"
    media=$(./test_project $j)
    echo -n ${j}" " >> data.txt
    echo ${media} >> data.txt
    sum=$((${sum}+${media}))
    j=$(($j+100))
done
first=$(./test_project 1)
last=$(./test_project 10000)

b=${first}
a=$(echo "scale=3; (${last}-${first})/10000" | bc)

echo "set output \"blit_masked_pixmap.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{blit\_masked\_pixmap(a,b,c,d,e,f,g,h,i,j,k)}" >> ../tex/report.tex
echo "Function used for blitting a masked pixmap in some surface." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv blit_masked_pixmap.eps ../tex
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/blit_masked_pixmap.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "This graph shows how many nanosseconds takes to blit an entire \$x \times x\$ masked pixmap in some surface." >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "The time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${a}x+${b}\$." >> ../tex/report.tex
cd -
