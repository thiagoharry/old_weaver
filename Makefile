
SCRIPT_DIR=/usr/bin
DATA_DIR=/usr/share/weaver

test_dependencies:
############################### GCC TEST ######################################
# Note that if GCC test fails, there's no sense in running the other tests
	@echo "Testing dependencies..."
	@/bin/echo -e "#include \"dummy.h\" \n" > dummy.c
	@/bin/echo -e "int main(int argc, char **argv){ return 1; }" >> dummy.c
	@touch dummy.h
	@echo -n "Checking for GCC Compiler..."
	@gcc dummy.c 2> /dev/null || touch ERROR
	@if [ -e a.out ]; then echo "OK"; rm a.out; else \
	/bin/echo -e "\033[31mFAILED\033[m"; rm ERROR dummy.c; \
	rm dummy.h; false; fi
############################## X11 TEST #######################################
	@echo "#include <X11/Xlib.h>" > dummy.h
	@echo "#include <X11/Xutil.h>" >> dummy.h
	@echo -n "Testing X11 libraries......."
	@gcc dummy.c -lX11 2> /dev/null || touch ERROR
	@if [ -e a.out ]; then echo "OK"; rm a.out; else \
	/bin/echo -e "\033[31mFAILED\033[m"; fi
############################## XDBE TEST ######################################
	@echo "#include <X11/Xlib.h>" > dummy.h
	@echo "#include <X11/extensions/Xdbe.h>" >> dummy.h
	@echo -n "Testing XDBE extension......"
	@gcc dummy.c -lX11 -lXext 2> /dev/null || touch ERROR
	@if [ -e a.out ]; then echo "OK"; rm a.out; else \
	/bin/echo -e "\033[31mFAILED\033[m"; fi
############################ MATH TEST ########################################
	@echo -n "Testing Math libraries......"
	@echo "#include <math.h>" > dummy.h
	@gcc dummy.c -lm 2> /dev/null || touch ERROR
	@if [ -e a.out ]; then echo "OK"; rm a.out; else \
	/bin/echo -e "\033[31mFAILED\033[m"; fi
########################### VORBIS TEST #######################################
	@echo -n "Testing Vorbis libraries...."
	@echo "#include <vorbis/vorbisfile.h>" > dummy.h
	@gcc dummy.c -lvorbisfile 2> /dev/null || touch ERROR
	@if [ -e a.out ]; then echo "OK"; rm a.out; else \
	/bin/echo -e "\033[31mFAILED\033[m"; fi
############################ ALSA TEST ########################################
	@echo -n "Testing ALSA libraries......"
	@echo "#include <alsa/asoundlib.h>" > dummy.h
	@gcc dummy.c -lasound 2> /dev/null || touch ERROR
	@if [ -e a.out ]; then echo "OK"; rm a.out; else \
	/bin/echo -e "\033[31mFAILED\033[m"; fi
############################# PNG TEST ########################################
	@echo -n "Testing PNG libraries......."
	@echo "#include <png.h>" > dummy.h
	@gcc dummy.c -lpng 2> /dev/null || touch ERROR
	@if [ -e a.out ]; then echo "OK"; rm a.out; else \
	/bin/echo -e "\033[31mFAILED\033[m"; fi
########################### FREETYPE TEST #####################################
	@echo -n "Testing FreeType libraries.."
	@echo "#include <ft2build.h>" > dummy.h
	@echo "#include FT_FREETYPE_H" >> dummy.h
	@gcc dummy.c -lfreetype $$(freetype-config --cflags) 2> /dev/null || \
	touch ERROR
	@if [ -e a.out ]; then echo "OK"; rm a.out; else \
	/bin/echo -e "\033[31mFAILED\033[m"; fi
############################# FINISHING #######################################
	@rm dummy.c dummy.h
	@echo $$ERROR
	@if [ ! -e ERROR ]; then echo "Your system is ready to host Weaver."; \
	else \
	echo "Your system misses requirements to became a Weaver habitat.";\
	rm ERROR; false; fi
install: test_dependencies
	install -D -m 0755 weaver ${SCRIPT_DIR}/weaver
	install -d ${DATA_DIR}/images
	install -d ${DATA_DIR}/fonts
	install -d ${DATA_DIR}/music
	install -D -m 0644 src/LICENSE ${DATA_DIR}/LICENSE
	install -D -m 0644 src/Makefile ${DATA_DIR}/Makefile
	install -D -m 0644 reserved_words ${DATA_DIR}
	install -d ${DATA_DIR}/sound
	install -d ${DATA_DIR}/src
	install -D -m 0644 src/src/game.c ${DATA_DIR}/src
	install -D -m 0644 src/src/game.h ${DATA_DIR}/src
	install -d ${DATA_DIR}/src/weaver
	install -D -m 0644  src/src/weaver/*c ${DATA_DIR}/src/weaver
	install -D -m 0644  src/src/weaver/*h ${DATA_DIR}/src/weaver
	@echo "Weaver Framework was installed with success."
uninstall:
	rm -rf ${SCRIPT_DIR}/weaver
	rm -rf ${DATA_DIR}
aux:
	ghc --make misc/src2html.hs
test: test_dependencies
	tests/test_start.sh
	tests/test_apply_texture.sh
	tests/test_awake_the_weaver.sh
	tests/test_blit_masked_pixmap.sh
	tests/test_blit_surface.sh
	tests/test_center_camera.sh
	tests/test_destroy_polygon.sh
	tests/test_destroy_surface.sh
	tests/test_draw_circle.sh
	tests/test_draw_ellipse.sh
	tests/test_draw_line.sh
	tests/test_draw_point.sh
	tests/test_draw_rectangle.sh
	tests/test_draw_rectangle_mask.sh
	tests/test_erase_circle.sh
	tests/test_erase_fullcircle.sh
	tests/test_get_input.sh
	tests/test_hide_cursor.sh
	tests/test_fill_circle.sh
	tests/test_fill_ellipse.sh
	tests/test_fill_rectangle.sh
	tests/test_fill_surface.sh
	tests/test_film_circle.sh
	tests/test_film_fullrectangle.sh
	tests/test_film_rectangle.sh
	tests/test_flush.sh
	tests/test_limit_camera.sh
	tests/test_may_the_weaver_sleep.sh
	tests/test_move_circle.sh
	tests/test_move_polygon.sh
	tests/test_new_image.sh
	tests/test_new_polygon.sh
	tests/test_rotate_circle.sh
	tests/test_rotate_polygon.sh
	tests/test_new_surface.sh
	tests/test_new_vector2.sh
	tests/test_new_vector3.sh
	tests/test_new_vector4.sh
	tests/test_weaver_rest.sh
	tests/test_zoom_camera.sh
	tests/test_end.sh
test_apply_texture: test_dependencies
	tests/test_start.sh
	tests/test_apply_texture.sh
	tests/test_end.sh
test_awake_the_weaver: test_dependencies
	tests/test_start.sh
	tests/test_awake_the_weaver.sh
	tests/test_end.sh
test_blit_masked_pixmap: test_dependencies
	tests/test_start.sh
	tests/test_blit_masked_pixmap.sh
	tests/test_end.sh
test_blit_surface: test_dependencies
	tests/test_start.sh
	tests/test_blit_surface.sh
	tests/test_end.sh
test_center_camera: test_dependencies
	tests/test_start.sh
	tests/test_center_camera.sh
	tests/test_end.sh
test_destroy_polygon: test_dependencies
	tests/test_start.sh
	tests/test_destroy_polygon.sh
	tests/test_end.sh
test_destroy_surface: test_dependencies
	tests/test_start.sh
	tests/test_destroy_surface.sh
	tests/test_end.sh
test_draw_circle: test_dependencies
	tests/test_start.sh
	tests/test_draw_circle.sh
	tests/test_end.sh
test_draw_ellipse: test_dependencies
	tests/test_start.sh
	tests/test_draw_ellipse.sh
	tests/test_end.sh
test_draw_line: test_dependencies
	tests/test_start.sh
	tests/test_draw_line.sh
	tests/test_end.sh
test_draw_point: test_dependencies
	tests/test_start.sh
	tests/test_draw_point.sh
	tests/test_end.sh
test_draw_rectangle: test_dependencies
	tests/test_start.sh
	tests/test_draw_rectangle.sh
	tests/test_end.sh
test_draw_rectangle_mask: test_dependencies
	tests/test_start.sh
	tests/test_draw_rectangle_mask.sh
	tests/test_end.sh
test_erase_circle: test_dependencies
	tests/test_start.sh
	tests/test_erase_circle.sh
	tests/test_end.sh
test_erase_fullcircle: test_dependencies
	tests/test_start.sh
	tests/test_erase_fullcircle.sh
	tests/test_end.sh
test_fill_circle: test_dependencies
	tests/test_start.sh
	tests/test_fill_circle.sh
	tests/test_end.sh
test_fill_ellipse: test_dependencies
	tests/test_start.sh
	tests/test_fill_ellipse.sh
	tests/test_end.sh
test_fill_rectangle: test_dependencies
	tests/test_start.sh
	tests/test_fill_rectangle.sh
	tests/test_end.sh
test_fill_surface: test_dependencies
	tests/test_start.sh
	tests/test_fill_surface.sh
	tests/test_end.sh
test_film_circle: test_dependencies
	tests/test_start.sh
	tests/test_film_circle.sh
	tests/test_end.sh
test_film_fullrectangle: test_dependencies
	tests/test_start.sh
	tests/test_film_fullrectangle.sh
	tests/test_end.sh
test_film_polygon: test_dependencies
	tests/test_start.sh
	tests/test_film_polygon.sh
	tests/test_end.sh
test_film_rectangle: test_dependencies
	tests/test_start.sh
	tests/test_film_rectangle.sh
	tests/test_end.sh
test_flush: test_dependencies
	tests/test_start.sh
	tests/test_flush.sh
	tests/test_end.sh
test_get_input: test_dependencies
	tests/test_start.sh
	tests/test_get_input.sh
	tests/test_end.sh
test_hide_cursor: test_dependencies
	tests/test_start.sh
	tests/test_hide_cursor.sh
	tests/test_end.sh
test_limit_camera: test_dependencies
	tests/test_start.sh
	tests/test_limit_camera.sh
	tests/test_end.sh
test_may_the_weaver_sleep: test_dependencies
	tests/test_start.sh
	tests/test_may_the_weaver_sleep.sh
	tests/test_end.sh
test_move_circle: test_dependencies
	tests/test_start.sh
	tests/test_move_circle.sh
	tests/test_end.sh
test_move_polygon: test_dependencies
	tests/test_start.sh
	tests/test_move_polygon.sh
	tests/test_end.sh
test_new_image: test_dependencies
	tests/test_start.sh
	tests/test_new_image.sh
	tests/test_end.sh
test_new_polygon: test_dependencies
	tests/test_start.sh
	tests/test_new_polygon.sh
	tests/test_end.sh
test_rotate_circle: test_dependencies
	tests/test_start.sh
	tests/test_rotate_circle.sh
	tests/test_end.sh
test_rotate_polygon: test_dependencies
	tests/test_start.sh
	tests/test_rotate_polygon.sh
	tests/test_end.sh
test_new_surface: test_dependencies
	tests/test_start.sh
	tests/test_new_surface.sh
	tests/test_end.sh
test_new_vector2: test_dependencies
	tests/test_start.sh
	tests/test_new_vector2.sh
	tests/test_end.sh
test_new_vector3: test_dependencies
	tests/test_start.sh
	tests/test_new_vector3.sh
	tests/test_end.sh
test_new_vector4: test_dependencies
	tests/test_start.sh
	tests/test_new_vector4.sh
	tests/test_end.sh
test_weaver_rest: test_dependencies
	tests/test_start.sh
	tests/test_weaver_rest.sh
	tests/test_end.sh
test_zoom_camera: test_dependencies
	tests/test_start.sh
	tests/test_zoom_camera.sh
	tests/test_end.sh
clean:
	find . -name "*~" -exec rm -f {} \;
	find . -name "*\#" -exec rm -f {} \;
	find . -name "*.o" -exec rm -f {} \;
	find . -name "*.dvi" -exec rm -f {} \;
	find . -name "*.log" -exec rm -f {} \;
	find . -name "*.aux" -exec rm -f {} \;
	find . -name "*.eps" -exec rm -f {} \;
	find . -name "*.pdf" -exec rm -f {} \;
	find . -name "*.hi" -exec rm -f {} \;
	rm -rf tests/gnuplot_instructions.txt
	rm -rf tests/test_project
