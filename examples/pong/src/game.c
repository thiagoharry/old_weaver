/*
  Copyright (C) 2009,2010 Thiago Leucz Astrizi

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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "game.h"

int main(int argc, char **argv){
  int got_news;
  int score_marked, score1, score2, total_score;
  char buffer[4];
  camera *camera0;
  Paddle *bar1, *bar2;

  printf("Pong - Written by Thiago \"Harry\" Leucz Astrizi\n");
  printf("2009 - (ᵓ) Copyleft - All wrongs reversed\n\n");

  //---------------------------------------------------------------------------
  // Beginning the initialization step
  //---------------------------------------------------------------------------
  awake_the_weaver();  
  hide_cursor();
  initialize_ball();
  initialize_camp();
  score_marked = 0;
  
  camera0 = new_camera_h(0.0, 0.0, 530.0);
 
  bar1 = initialize_paddle(0.0, 230.0);
  bar2 = initialize_paddle(522.0, 230.0);
  
  center_camera(camera0, 265.0, 245.0);
      
  got_news = 1;
  score1 = score2 = total_score = 0;
  flush_game();
  clean_keyboard();
  fps = 100;
  //---------------------------------------------------------------------------
  // Ending the initialization step
  //---------------------------------------------------------------------------
    
    // Beginning of the main loop
  for(;;){
    //-------------------------------------------------------------------------
    // Beginning the "start of loop" step
    //-------------------------------------------------------------------------
    if(got_news){               // Updating the screen if we have news from...
      flush();                  // ... the last iteration.
      got_news = 0;
    }
    paddle_speed = PADDLE_SPEED / fps; // Discovering the speed of ball and paddles 
    Ball.speed = Ball.raw_speed / fps; // according with the fps measure
    //-------------------------------------------------------------------------
    // Ending the "start of loop" step
    //-------------------------------------------------------------------------


    //-------------------------------------------------------------------------
    // Beginning the input step
    //-------------------------------------------------------------------------
    get_input();
    if(keyboard[UP]){
      film_fullrectangle(camera0, bar2 -> form, BLACK);
      bar2 -> form -> y -= paddle_speed; // Moves the second paddle up if possible
      if(bar2 -> form -> y < 0){
        bar2 -> form -> y += paddle_speed;
      }
      film_fullrectangle(camera0, bar2 -> form, bar2 -> color);
      got_news = 1;
    }
    if(keyboard[DOWN]){// Moves the second paddle down if possible
      film_fullrectangle(camera0, bar2 -> form, BLACK);
      bar2 -> form -> y += paddle_speed;
      if(bar2 -> form -> y > 460.0){
        bar2 -> form -> y -= paddle_speed;
      }
      film_fullrectangle(camera0, bar2 -> form, bar2 -> color);
      got_news = 1;
    }
    if(keyboard[SHIFT]){ // Moves the first paddle up if possible
      film_fullrectangle(camera0, bar1 -> form, BLACK);
      bar1 -> form -> y -= paddle_speed;
      if(bar1 -> form -> y < 0)
        bar1 -> form -> y += paddle_speed;
      film_fullrectangle(camera0, bar1 -> form, bar1 -> color);
      got_news = 1;
    }
    if(keyboard[CTRL]){ // Moves the first paddle down if possible
      film_fullrectangle(camera0, bar1 -> form, BLACK);
      bar1 -> form -> y += paddle_speed;;
      if(bar1 -> form -> y > 460.0)
        bar1 -> form -> y -= paddle_speed;
      film_fullrectangle(camera0, bar1 -> form, bar1 -> color);
      got_news = 1;
    }
    if(keyboard[ESC])
      exit(0);
    //-------------------------------------------------------------------------
    // Ending the input step
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Beginning the "ball movement" step
    //-------------------------------------------------------------------------
    film_fullcircle(camera0, Ball.form, BLACK);
    move_ball();
    // If the ball collided with the second paddle:
    if(collision_rectangle_circle(bar2 -> form, Ball.form)){
      back_ball();
      if(Ball.raw_speed < BALL_MAX_SPEED)
        Ball.raw_speed *= 1.02;
      else
        Ball.raw_speed = BALL_MAX_SPEED;
      Ball.dx *= -1.0;
      sprintf(buffer, "%d", total_score);
      draw_text(window_width / 2, 10, buffer, "12x24", BLACK);
      total_score ++;
      sprintf(buffer, "%d", total_score);
      draw_text(window_width / 2, 10, buffer, "12x24", RED);
      play_soundfile(COL_SOUND1);
    }
    // If collided with the first paddle
    else if(collision_rectangle_circle(bar1 -> form, Ball.form)){
      Ball.form -> x -= Ball.dx * Ball.speed;
      if(Ball.raw_speed < BALL_MAX_SPEED)
        Ball.raw_speed *= 1.02;
      else
        Ball.raw_speed = BALL_MAX_SPEED;
      Ball.dx *= -1.0;
      sprintf(buffer, "%d", total_score);
      draw_text(window_width / 2, 10, buffer, "12x24", BLACK);
      total_score ++;
      sprintf(buffer, "%d", total_score);
      draw_text(window_width / 2, 10, buffer, "12x24", RED);
      play_soundfile(COL_SOUND2);
    }
    if(Ball.form -> y < 4.0){ // Collided in the top
      back_ball();
      Ball.dy *= -1.0;
    }
    else if(Ball.form -> y > 486.0){ // Collided in the bottom
      back_ball();
      Ball.dy *= -1.0;
    }
    if(Ball.form -> x < 4.0){ // Player 2 scores
      score2 += 1;
      score_marked = 1;
    }
    else if(Ball.form -> x > 526.0){ // Player 1 scores
      score1 += 1;
      score_marked = 1;
    }
    film_fullcircle(camera0, Ball.form, Ball.color);
    //-------------------------------------------------------------------------
    // End of "ball movement" step
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Beginning the scoring step
    //-------------------------------------------------------------------------
    if(score_marked){
      //play_soundfile(LOST_BALL);
      clean_score_buffer();
      total_score = 0;
      clean_keyboard();
      flush_game();
      if(score1 == 10 || score2 == 10){
        sleep(1);
        exit(0);
      }
      else{
        play_soundfile(LOST_BALL);
        film_fullcircle(camera0, Ball.form, BLACK);
        reset_ball();
      }
      score_marked = 0;
    }
    //-------------------------------------------------------------------------
    // Ending the scoring step
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Beginning the "end of loop" step
    //-------------------------------------------------------------------------
    
    weaver_rest(10000000);
    
    //-------------------------------------------------------------------------
    // Ending the "end of loop" step
    //-------------------------------------------------------------------------
      
  }
  // End of the main loop

  return 0;
}


// Set the values for our ball
void initialize_ball(void){
  Ball.form = new_circle(CENTER_X, CENTER_Y, 4.0); /* The ball starts in the center
						      of the table and has 4 units of
						      radius. */
  Ball.color = rand(); // The ball color is random
  
  Ball.dx = 0.5; // Setting the ball slope. The vector lenght should be 1
  Ball.dy = sqrt(1 - Ball.dx * Ball.dx);

  // Now we decide the directions randomly
  if(rand() % 2)
    Ball.dy *= -1.0;
  if(rand() % 2)
    Ball.dx *= -1.0;

  // It will starts moving 
  Ball.raw_speed = BALL_INITIAL_SPEED;
  Ball.speed = 0.0;
}

// Put the ball in the center of the table
void reset_ball(void){
  Ball.form -> x = CENTER_X;
  Ball.form -> y = CENTER_Y;
  
  Ball.dx *= -1.0;

  if(rand() % 2)
    Ball.dy *= -1.0;

  if(Ball.raw_speed > FAST_SPEED)
    Ball.raw_speed = MEDIUM_SPEED;

}

// Makes a ball move forward
void move_ball(void){
  Ball.form -> x += Ball.dx * Ball.speed;
  Ball.form -> y += Ball.dy * Ball.speed;
}


// Makes a ball moves backward
void back_ball(void){
  Ball.form -> x -= Ball.dx * Ball.speed;
  Ball.form -> y -= Ball.dy * Ball.speed;
}

// Initializes the camp
void initialize_camp(void){
  Camp.color = rand();
  Camp.form = new_rectangle(-1.0, -1.0, 532.0, 492.0);
}

// Initializes a paddle in a position
Paddle *initialize_paddle(float x, float y){
  Paddle *new_paddle = (Paddle *) malloc(sizeof(Paddle));
  new_paddle -> color = rand();
  new_paddle -> form = new_rectangle(x, y, 8.0, 30.0);
  return new_paddle;
}
