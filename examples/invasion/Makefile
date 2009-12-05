CC=gcc -Wall -O2 -g
W_DIR=src/weaver/ 
PROG=invasion
WEAVER_OBJ=display.o keyboard.o vector2.o vector3.o vector4.o weaver.o sound.o image.o
PROG_OBJ=game.o monster.o
INSTALL_DIR=/usr/games
DATA_DIR=/usr/share/games/invasion

#######################################################
all: ${WEAVER_OBJ} ${PROG_OBJ}
	${CC} ${CFLAGS} -g -o ${PROG} ${WEAVER_OBJ} ${PROG_OBJ} -lX11 -lm -lvorbisfile -lasound -lpng
#######################################################
game.o: src/game.c src/game.h
	${CC} -c src/game.c
monster.o: src/monster.c src/monster.h
	${CC} -c src/monster.c
#######################################################
image.o: src/weaver/image.c src/weaver/image.h
	${CC} -c src/weaver/image.c
display.o: src/weaver/display.c src/weaver/display.h
	${CC} -c src/weaver/display.c
keyboard.o: src/weaver/keyboard.c src/weaver/keyboard.h
	${CC} -c src/weaver/keyboard.c
vector2.o: src/weaver/vector2.c src/weaver/vector2.h
	${CC} -c src/weaver/vector2.c
vector3.o: src/weaver/vector3.c src/weaver/vector3.h
	${CC} -c src/weaver/vector3.c
vector4.o: src/weaver/vector4.c src/weaver/vector4.h
	${CC} -c src/weaver/vector4.c
weaver.o: src/weaver/weaver.c src/weaver/weaver.h
	${CC} -c src/weaver/weaver.c
sound.o: src/weaver/sound.c src/weaver/sound.h
	${CC} -c src/weaver/sound.c
#######################################################
clean:
	rm -rf *~ *.o ./\#*
#######################################################
install:
	install -D -s ${PROG} ${INSTALL_DIR}/${PROG}
	cp -r sound ${DATA_DIR}/
	cp -r images ${DATA_DIR}/
uninstall:
	rm -rf ${INSTALL_DIR}/${PROG}
	rm -rf ${DATA_DIR}/