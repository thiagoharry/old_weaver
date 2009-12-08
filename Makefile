SCRIPT_DIR=/usr/bin
DATA_DIR=/usr/share/weaver

test:
	@echo "Testing dependencies..."
	@/bin/echo -e "#include \"dummy.h\" \n\nint main(int argc, char **argv){ return 1; }" > dummy.c
	@echo "#include <X11/Xlib.h>" >> dummy.h
	@echo "#include <X11/Xutil.h>" >> dummy.h
	@echo -n "Testing X11 libraries....."
	@gcc dummy.c -lX11
	@echo "OK"
	@echo -n "Testing Math libraries...."
	@echo "#include <math.h>" >> dummy.h
	@gcc dummy.c -lm
	@echo "OK"
	@echo -n "Testing Vorbis libraries.."
	@echo "#include <vorbis/vorbisfile.h>" >> dummy.h
	@gcc dummy.c -lvorbisfile
	@echo "OK"
	@echo -n "Testing ALSA libraries...."
	@echo "#include <alsa/asoundlib.h>" >> dummy.h
	@gcc dummy.c -lasound
	@echo "OK"
	@echo -n "Testing PNG libraries....."
	@echo "#include <png.h>" >> dummy.h
	@gcc dummy.c -lpng
	@echo "OK"
	@rm dummy.c a.out dummy.h
	@echo "Your system is ready to host Weaver."
install: test
	install -D -m 0755 weaver ${SCRIPT_DIR}/weaver
	install -d ${DATA_DIR}/images
	install -D -m 0644 src/LICENSE ${DATA_DIR}/LICENSE
	install -D -m 0644 src/Makefile ${DATA_DIR}/Makefile
	install -d ${DATA_DIR}/sound
	install -d ${DATA_DIR}/src
	install -D -m 0644 src/src/game.{c,h} ${DATA_DIR}/src
	install -d ${DATA_DIR}/src/weaver
	install -D -m 0644  src/src/weaver/*.{c,h} ${DATA_DIR}/src/weaver
	@echo "Weaver Framework was installed with success."
uninstall:
	rm -rf ${SCRIPT_DIR}/weaver
	rm -rf ${DATA_DIR}