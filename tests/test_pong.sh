#!/bin/bash

cd tests
weaver pong
cd pong
cp ../imgs/pong/* images
cp ../sound/pong/* sound
cp ../music/pong/* music
cat > src/game.c <<EOF
/*
  Copyright (C) 2013 Harry

 This file is part of pong.

 pong is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 pong is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with pong.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "weaver/weaver.h"
#include "game.h"

int main(int argc, char **argv){
  awake_the_weaver(); // Initializing Weaver API
  hide_cursor();
  initialize_background();
  draw_surface(background, window, 0, 100);
  draw_rectangle(0, 100, window_width-1, window_height - 101, YELLOW);
  initialize_paddles();
  draw_surface(paddle1 -> image, window, paddle1 -> x, paddle1 -> y);
  draw_surface(paddle2 -> image, window, paddle2 -> x, paddle2 -> y);
  initialize_scores();
  draw_scores();
  initialize_ball();
  draw_surface(ball -> image, window, ball -> x, ball -> y);
  play_music("einklang.ogg");

  // Main loop
  for(;;){
    get_input();
    if(keyboard[ESC] || score1 >= 12 || score2 >= 12){
      break;
    }
    if(keyboard[LEFT_SHIFT])
      move_paddle(paddle1, UP);
    else if(keyboard[LEFT_CTRL])
      move_paddle(paddle1, DOWN);
    if(keyboard[RIGHT_SHIFT])
      move_paddle(paddle2, UP);
    else if(keyboard[RIGHT_CTRL])
      move_paddle(paddle2, DOWN);
    move_ball();
    weaver_rest(10000000);
  }
  may_the_weaver_sleep();
  return 0;
}
EOF
weaver background
cat > src/background.h <<EOF
/*
  Copyright (C) 2013 Harry

 This file is part of pong.

 pong is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 pong is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with pong.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include "weaver/weaver.h"
#include "game.h"

void initialize_background(void);

#endif
EOF
cat >> src/background.c <<EOF
// This function initializes our background. It should be called once in
// the beginning of the program
void initialize_background(void){
  // First we load the file "texture.png" located in the directory "images"
  surface *texture = new_image("texture.png");
  // Now we apply the texture to our bigger surface.
  apply_texture(texture, background);
  // Done. Now we don't need the texture anymore. We destroy the surface
  // that stores the texture.
  destroy_surface(texture);
}
EOF
weaver paddle
cat > src/paddle.h <<EOF
/*
  Copyright (C) 2013 Harry

 This file is part of pong.

 pong is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 pong is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with pong.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _PADDLE_H_
#define _PADDLE_H_

#include "weaver/weaver.h"
#include "game.h"

struct paddle{
  int x, y, cy; // Coordinates
  int speed;
  surface *image;
} *paddle1, *paddle2;

void initialize_paddles(void);
void move_paddle(struct paddle *, int);

#endif
EOF
cat >> src/paddle.c <<EOF
// This initializes the paddles. It should be called once in the
// beginning of the game
void initialize_paddles(void){
  // Allocating space
  paddle1 = (struct paddle *) malloc(sizeof(struct paddle));
  paddle2 = (struct paddle *) malloc(sizeof(struct paddle));
  // Loading the images
  paddle1 -> image = new_image("paddle1.png");
  paddle2 -> image = new_image("paddle2.png");
  // Setting the first paddle coordinates
  paddle1 -> x = 1;
  paddle1 -> y = 100 + ((window_height - 100) / 2) - (paddle1 -> image -> height / 2);
  paddle1 -> cy = 0;
  // Setting the second paddle coordinates
  paddle2 -> x = window_width - paddle2 -> image -> width - 1;
  paddle2 -> y = 100 + ((window_height - 100) / 2) - (paddle2 -> image -> height / 2);
  paddle2 -> cy = 0;
  // Setting speed
  paddle1 -> speed = paddle2 -> speed = 200;
}

// This moves a paddle up or down.
void move_paddle(struct paddle *pad, int direction){
  // We don't move if this would put us outside the game area
  if((direction == UP && pad -> y > 102) ||
     (direction == DOWN && pad -> y + pad -> image -> height < window_height - 1)){
    // Updating paddle cy value
    int move = (pad -> speed * 100) / fps;
    pad -> cy += ((direction == UP)? (-move):(move));
    // If we moved more than 100 "centipixels", we should erase and draw the
    // paddle again
    if(pad -> cy >= 100 || pad -> cy <= -100){
      // First we erase the given paddle drawing a rectangle with the background
      // in the area where the paddle is standing.
      blit_surface(background, window,
		   pad -> x, pad -> y - 100, pad -> image -> width,
		   pad -> image -> height,
		   pad -> x, pad -> y);
      // Next we change the paddle coordinate
      pad -> y += pad -> cy / 100;
      pad -> cy %= 100;
      // And finally we draw the paddle again
      draw_surface(pad -> image, window, pad -> x, pad -> y);
    }
  }
}
EOF
weaver score
cat > src/score.h <<EOF
/*
  Copyright (C) 2013 Harry

 This file is part of pong.

 pong is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 pong is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with pong.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SCORE_H_
#define _SCORE_H_

#include "weaver/weaver.h"
#include "game.h"

int score1, score2;
surface *numbers;
void initialize_scores(void);
void draw_scores(void);

#endif
EOF
cat >> src/score.c <<EOF
// This initialies our score and gets the image with the numbers that we'll
// draw in the screen
void initialize_scores(void){
  score1 = score2 = 0;
  numbers = new_image("numbers.png");
}

// This draws the 2 scores in the screen. As our maximum score is "12", we
// don't treat cases when the sore have more than 2 digits.
void draw_scores(void){
  int digit;
  // All the numbers are 46x46 pixels
  surface *buffer = new_surface(46, 46);
  // Drawing the Player 1 score...
  // The first digit:
  fill_surface(buffer, BLACK);
  digit = score1 / 10;
  blit_surface(numbers, buffer,
	       46*digit, 0, 46, 46,
	       0, 0);
  draw_surface(buffer, window, 27, 27);
  // The second digit:
  fill_surface(buffer, BLACK);
  digit = score1 % 10;
  blit_surface(numbers, buffer,
	       46*digit, 0, 46, 46,
	       0, 0);
  draw_surface(buffer, window, 27+35, 27);
  // Drawing the Player 2 score...
  // The first digit:
  fill_surface(buffer, BLACK);
  digit = score2 / 10;
  blit_surface(numbers, buffer,
	       46*digit, 0, 46, 46,
	       0, 0);
  draw_surface(buffer, window, window_width - 27 - 35 - 46, 27);
  // The second digit:
  fill_surface(buffer, BLACK);
  digit = score2 % 10;
  blit_surface(numbers, buffer,
	       46*digit, 0, 46, 46,
	       0, 0);
  draw_surface(buffer, window, window_width - 27 - 46, 27);
  // Done. Destroying the buffer...
  destroy_surface(buffer);
}
EOF
weaver ball
cat > src/ball.h <<EOF
/*
  Copyright (C) 2013 Harry

 This file is part of pong.

 pong is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 pong is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with pong.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BALL_H_
#define _BALL_H_

#include "weaver/weaver.h"
#include "game.h"

struct ball{
  int x, y, cx, cy; // The ball's coordinates
  int dx, dy; // The ball's slope
  surface *image;
  int speed;
} *ball;

void initialize_ball(void);
void move_ball(void);

#endif
EOF
cat >> src/ball.c <<EOF
// This function initializes the ball
void initialize_ball(void){
  ball = (struct ball *) malloc(sizeof(struct ball));
  ball -> image = new_image("ball.png");
  // Putting the ball in the center of screen:
  ball -> x = (window_width - ball -> image -> width) / 2;
  ball -> y = (window_height - ball -> image -> height) / 2 + 50;
  ball -> cx = ball -> cy = ball -> dy = 0;
  // How many pixels per second the ball starts moving
  // It should be able to cross the screen in 10 seconds
  ball -> speed = window_width / 10;
  // The ball can go left or right. It's random:
  ball -> dx = ((rand()%2)?(ball -> speed):(-ball -> speed));
}

// This function moves the ball in the screen
void move_ball(void){
  // Checking if the ball went off the screen
  if(ball -> x <= -(ball -> image -> width)){
    score2 ++;
    ball -> x = window_width / 2;
    ball -> y = window_height / 2 + 100;
    ball -> dx *= -1;
    draw_scores();
    draw_rectangle(0, 100, window_width-1, window_height - 101, YELLOW);
    play_sound("explosion.ogg");
  }
  else if(ball -> x >= window_width + ball -> image -> width){
    score1 ++;
    ball -> x = window_width / 2;
    ball -> y = window_height / 2 + 100;
    ball -> dx *= -1;
    draw_scores();
    draw_rectangle(0, 100, window_width-1, window_height - 101, YELLOW);
    play_sound("explosion.ogg");
  }
  // Checking if the ball collides with the first paddle:
  else if(ball -> x <= paddle1 -> x + paddle1 -> image -> width &&
	  (ball -> y >= paddle1 -> y - ball -> image -> height &&
	   ball -> y <= paddle1 -> y + paddle1 -> image -> height) &&
	  ball -> dx < 0){
    int ball_center_y = ball -> y + ball -> image -> height / 2;
    int paddle_center_y = paddle1 -> y + paddle1 -> image -> height / 2;
    ball -> speed += 5;
    ball -> dy = -((float)(paddle_center_y - ball_center_y) / 
		  (float)(paddle1 -> image -> height / 2)) * 0.65 * ball -> speed;
    ball -> dx = (int) sqrtf((float)((ball -> speed)*(ball -> speed) - (ball -> dy)*(ball -> dy)));
    play_sound("hit.ogg");
    return;
  }
  // Checking if the ball collides with the second paddle:
  else if(ball -> x + ball -> image -> width >= paddle2 -> x &&
	  (ball -> y >= paddle2 -> y - ball -> image -> height &&
	   ball -> y <= paddle2 -> y + paddle2 -> image -> height) &&
	  ball -> dx > 0){
    int ball_center_y = ball -> y + ball -> image -> height / 2;
    int paddle_center_y = paddle2 -> y + paddle2 -> image -> height / 2;
    ball -> speed += 5;
    ball -> dy = -((float)(paddle_center_y - ball_center_y) / 
		  (float)(paddle2 -> image -> height / 2)) * 0.65 * ball -> speed;
    ball -> dx = -(int) sqrtf((float)((ball -> speed)*(ball -> speed) - (ball -> dy)*(ball -> dy)));
    play_sound("hit.ogg");
    return;
  }
  // Checking if the ball is colliding with the top or bottom of screen
  if(ball -> y <= 100 && ball -> dy < 0){
    ball -> dy *= -1;
    ball -> y = 102;
    draw_rectangle(0, 100, window_width-1, window_height - 101, YELLOW);
    return;
  }
  else if(ball -> y >= window_height - ball -> image -> height && ball -> dy > 0){
    ball -> dy *= -1;
    ball -> y = window_height - ball -> image -> height - 1;
    draw_rectangle(0, 100, window_width-1, window_height - 101, YELLOW);
    return;
  }

  // Updating the values cx and cy
  ball -> cy  += (ball -> dy * 100) / fps;
  ball -> cx += (ball -> dx * 100) / fps;

  // If we moved more than 100 centipixels, we need to update the movement
  if(ball -> cy >= 100 || ball -> cy <= -100 ||
     ball -> cx >= 100 || ball -> cx <= -100){
    blit_surface(background, window,
                 ball -> x, ball -> y - 100, ball -> image -> width,
                 ball -> image -> height,
                 ball -> x, ball -> y);
    // Moving the ball...
    ball -> x += ball -> cx / 100;
    ball -> y += ball -> cy / 100;
    // Updating buffer and screen...
    draw_surface(ball -> image, window, ball -> x, ball -> y);
    // Updating the centipixels offset...
    ball -> cx %= 100;
    ball -> cy %= 100;
  }
}
EOF
make
echo "Game compiled!!!"
echo "Now we'll test if it runs smoothly. Press any key to start the game."
echo "Controls: <SHIFT> and <CTRL> - Control the paddles"
echo "          <ESC>              - Quit"
read
./pong
cd ..
rm -rf pong

