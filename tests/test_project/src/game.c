#include "weaver/weaver.h"

#include "game.h"

int main(int argc, char **argv){

  awake_the_weaver(); // Initializing Weaver API

  surface *surf = new_image("pixel.png");

  surface *dest = new_surface(101, 101);

  flush();

  DEBUG_TIMER_START();

  apply_texture(surf, dest);

  DEBUG_TIMER_END();

  may_the_weaver_sleep();

  return 0;

}