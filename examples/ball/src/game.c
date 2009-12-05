/*
  Copyright (C) 2009 Thiago Leucz Astrizi

 This file is part of ball.

 ball is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 ball is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with ball.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "weaver/weaver.h"
#include "game.h"

#define RADIUS 20

int main(int argc, char **argv){
  int circle_x, circle_y, dx, dy;
  struct surface *ball;

  awake_the_weaver(); // Initializing Weaver API
  hide_cursor();

  ball = new_image("ball.png");
  dx = dy = 2;
  circle_x = window_width / 2;
  circle_y = window_height / 2;

  // Main loop
  for(;;){
    get_input();
    if(keyboard[ESC]){
      break;
    }

    fill_rectangle(circle_x - RADIUS, circle_y - RADIUS, 2*RADIUS, 2*RADIUS, BLACK);

    circle_x += dx;
    circle_y += dy;

    if(circle_y < RADIUS || circle_y > window_height - RADIUS){
      dy *= -1;
      play_soundfile("hit.ogg");
    }
    if(circle_x < RADIUS || circle_x > window_width - RADIUS){
      dx *= -1;
      play_soundfile("hit.ogg");
    }

    blit_surface(ball, window, 0, 0, 2 * RADIUS, 2 * RADIUS, circle_x - RADIUS, circle_y - RADIUS);    

    weaver_rest(10000000);
  }
  may_the_weaver_sleep();
  return 0;
}
