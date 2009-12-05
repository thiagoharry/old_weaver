CC=gcc -Wall -O2 -g
W_DIR=src/weaver/ 
PROG=spacewar
WEAVER_OBJ=display.o keyboard.o vector2.o vector3.o vector4.o weaver.o sound.o image.o
PROG_OBJ=game.o star.o ship1.o dust.o ai.o
INSTALL_DIR=/usr/games
DATA_DIR=/usr/share/games/spacewar

#######################################################
all: ${WEAVER_OBJ} ${PROG_OBJ}
	${CC} ${CFLAGS} -g -o ${PROG} ${WEAVER_OBJ} ${PROG_OBJ} -lX11 -lm -lvorbisfile -lasound -lpng
#######################################################
game.o: src/game.c src/game.h
	${CC} -c src/game.c
star.o: src/star.c src/star.h
	${CC} -c src/star.c
ship1.o: src/ship1.c src/ship1.h src/dust.h src/star.h
	${CC} -c src/ship1.c
dust.o: src/dust.c src/dust.h src/star.h
	${CC} -c src/dust.c
ai.o: src/ai.c src/ai.h
	${CC} -c src/ai.c
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