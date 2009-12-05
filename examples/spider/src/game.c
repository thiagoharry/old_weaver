/*
  Copyright 2009 Thiago Leucz Astrizi

 This file is part of spider.

 spider is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 spider is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with spider.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "weaver/weaver.h"
#include "game.h"

int main(int argc, char **argv){
  struct surface *spider, *leaves, *background;
  int x, y;

  awake_the_weaver(); // Initializing Weaver API
  hide_cursor();

  background = new_surface(window_width, window_height);
  spider = new_image("spider.png");
  leaves = new_image("background.png");
  apply_texture(leaves, background);
  apply_texture(leaves, window);
  x = window_width / 2 - spider -> width / 2;
  y = window_height / 2 - spider -> height / 2;


  // Main loop
  for(;;){
    get_input();
    if(keyboard[ESC]){
      break;
    }
    if(keyboard[UP])
      if(y > 0){
        blit_surface(background, window, x, y, spider -> width, spider -> height, x, y);
        y --;
        blit_surface(spider, window, 0, 0, spider -> width, spider -> height, x, y);
      }
    if(keyboard[DOWN])
      if(y < window_height - spider -> height){
        blit_surface(background, window, x, y, spider -> width, spider -> height, x, y);
        y ++;
        blit_surface(spider, window, 0, 0, spider -> width, spider -> height, x, y);
      }
    if(keyboard[LEFT])
      if(x > 0){
        blit_surface(background, window, x, y, spider -> width, spider -> height, x, y);
        x --;
        blit_surface(spider, window, 0, 0, spider -> width, spider -> height, x, y);
      }
    if(keyboard[RIGHT])
      if(x < window_width - spider -> width){
        blit_surface(background, window, x, y, spider -> width, spider -> height, x, y);
        x ++;
        blit_surface(spider, window, 0, 0, spider -> width, spider -> height, x, y);
      }
    
    weaver_rest(10000000);
  }
  may_the_weaver_sleep();
  return 0;
}
