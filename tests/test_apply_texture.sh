#!/bin/bash

# The source to be teste
cd tests/test_project
echo -ne "#include \"weaver/weaver.h\"\n
#include \"game.h\"\n
int main(int argc, char **argv){\n
  awake_the_weaver(); // Initializing Weaver API\n
  surface *surf = new_image(\"pixel.png\");\n
  surface *dest = new_surface(1, 1);\n
  flush();\n
  DEBUG_TIMER_START();\n
  apply_texture(surf, dest);\n
  DEBUG_TIMER_STOP();\n
  may_the_weaver_sleep();\n
  return 0;\n
}" > src/game.c
make &> /dev/null
j=1
echo -n "" > data.txt
while (( $j < 101 )); do
    echo $(($j - 1))"%"
    media=0
    i=0
    while ((  $i < 30 )); do
	valor=$(./test_project)
	media=$((${media}+${valor}))
	i=$(($i+1));
    done
    echo -n $(($j*$j))" " >> data.txt
    echo $((${media}/30)) >> data.txt
    sed s/new_surface"("${j},\ ${j}\)\;/new_surface"("$(($j+1)),\ $(($j+1))\)\;/ src/game.c > src/game.c~
    cp src/game.c{~,}
    make &> /dev/null
    j=$(($j+1))
done

echo "set output \"apply_texture.eps\"" > ../gnuplot_instructions.txt
echo "set terminal postscript eps enhanced;" >> ../gnuplot_instructions.txt
echo "plot \"data.txt\" with lines;" >> ../gnuplot_instructions.txt

echo "\subsection{apply\_texture(a,b)}" >> ../tex/report.tex
echo "Function used for filling a surface with a texture" >> ../tex/report.tex
echo "stored in other texture." >> ../tex/report.tex
echo "In this test, \emph{a} is a \$1\times1\$ texture" >> ../tex/report.tex
echo "and \emph{b} is a square surface with \$x\$ pixels." >> ../tex/report.tex
gnuplot ../gnuplot_instructions.txt
mv apply_texture.eps ../tex
VALUE=$(tail -n 1 data.txt | cut -d " " -f 2)
VALUE=$(echo "scale=2; ${VALUE}/10000" | bc -l)
echo "" >> ../tex/report.tex
echo "\includegraphics{tests/tex/apply_texture.eps}" >> ../tex/report.tex
echo "" >> ../tex/report.tex
echo "As this function has a linear theoretical" >> ../tex/report.tex
echo "complexity, the time, in nanosseconds is " >> ../tex/report.tex
echo "approximated by the function \$f(x)=${VALUE}x\$" >> ../tex/report.tex
echo "in this computer." >> ../tex/report.tex
cd -
