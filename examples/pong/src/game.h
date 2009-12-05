/*
  Copyright 2009 Thiago Leucz Astrizi

 This file is part of Pong.

 Pong is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Pong is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with Pong.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _PONG_H_
#define _PONG_H_

#include "weaver/weaver.h"

// The center of the table
#define CENTER_X 265.0
#define CENTER_Y 245.0

#define BALL_INITIAL_SPEED 170.0 // The ball initial speed
#define MEDIUM_SPEED       190.0 // A speed considered "Medium"
#define FAST_SPEED         210.0 // A speed considered "fast".
#define PADDLE_SPEED       230.0 // The paddle speed
#define BALL_MAX_SPEED     280.0 // The ball maximum speed

#define CAMP_X             0.0 // The x-coordinate where 

#define COL_SOUND1 "collision1.ogg"
#define COL_SOUND2 "collision2.ogg"
#define LOST_BALL "crash.ogg"

//-----------------------------------------------------------------------------
// Beginning of ball-related structs and functions
//-----------------------------------------------------------------------------

// This game supports only one ball
struct ball{
  circle *form;            // Stores the position and size
  unsigned color;          // Stores the ball color.
  float dx, dy;            // This is the vector that represents the ball direction
                           // it's lenght is always 1
  float raw_speed;         // How many units of space per second it moves
  float speed;             // How much units of space it should move in each frame
} Ball;

void initialize_ball(void);
void reset_ball(void);
void move_ball(void);
void back_ball(void);

//-----------------------------------------------------------------------------
// Beginning of camp-related structs and functions
//-----------------------------------------------------------------------------

// We support only one camp where the game happens
struct camp{
  unsigned color;
  rectangle *form;
} Camp;

void initialize_camp(void);

//-----------------------------------------------------------------------------
// Beginning of paddle-related structs and functions
//-----------------------------------------------------------------------------
struct paddle{
  unsigned color;
  rectangle *form;
};

float paddle_speed;

typedef struct paddle Paddle;

Paddle *initialize_paddle(float, float);

//-----------------------------------------------------------------------------
// Some usefull macro functions to the game
//-----------------------------------------------------------------------------
#define clean_score_buffer() \
  buffer[0] = '0';           \
  buffer[1] = '\0';          \

#define flush_game() \
  {                                                             \
  char string1[2], string2[2];                                  \
  sprintf(string1, "%d", score1);                               \
  sprintf(string2, "%d", score2);                               \
  fill_screen(BLACK);                                           \
  film_vector3_fullcircle(camera0, Ball.form, Ball.color);      \
  film_rectangle(camera0, Camp.form, Camp.color);               \
  film_fullrectangle(camera0, bar1 -> form, bar1 -> color);     \
  film_fullrectangle(camera0, bar2 -> form, bar2 -> color);     \
  draw_text(15, 10, string1, "12x24", RED);                     \
  draw_text(window_width - 15, 10, string2, "12x24", RED);      \
  draw_text(window_width / 2, 10, "0", "12x24", RED);           \
  flush();                                                      \
  }


#endif
