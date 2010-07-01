CC=gcc -Wall -O2 -g $$(freetype-config --cflags)
W_DIR=src/weaver/ 
PROG=dummy
WEAVER_OBJ=display.o keyboard.o vector2.o vector3.o vector4.o weaver.o sound.o image.o font.o
PROG_OBJ=game.o
PROG_HEAD=src/game.h
INSTALL_DIR=/usr/games
DATA_DIR=/usr/share/games/dummy

#######################################################
all: ${WEAVER_OBJ} ${PROG_OBJ}
	${CC} ${CFLAGS} -g -o ${PROG} ${WEAVER_OBJ} ${PROG_OBJ} -lX11 -lm -lvorbisfile -lasound -lpng -lfreetype $$(freetype-config --cflags)
#GAME_START
game.o: src/game.c ${PROG_HEAD}
	${CC} -c src/game.c
#GAME_END
#WEAVER_START
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
font.o: src/weaver/font.c src/weaver/font.h
	${CC} -c src/weaver/font.c
#WEAVER_END
clean:
	rm -rf *~ *.o ./\#*
#######################################################
install:
	install -d ${DATA_DIR}
	install -D -s ${PROG} ${INSTALL_DIR}/${PROG}
	cp -r sound ${DATA_DIR}/
	cp -r images ${DATA_DIR}/
	cp -r fonts ${DATA_DIR}/
	cp -r music ${DATA_DIR}/
	chmod -R +r ${DATA_DIR}
uninstall:
	rm -rf ${INSTALL_DIR}/${PROG}
	rm -rf ${DATA_DIR}/