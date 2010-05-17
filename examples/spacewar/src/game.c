/*
  Copyright (C) 2009,2010 Thiago Leucz Astrizi

 This file is part of Spacewar.

 Spacewar is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Foobar is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with Spacewar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "game.h"

int main(int argc, char **argv){
  int var;
  for(;;){
    // Shows the start screen
    var = intro();
    if(var <= 2) // A normal game
      game(var);
    else if(var == 3) // Shows help
      controls();
  }
  // WARNING: The control will never arrive here
}

int game(int players){
  camera *camera00, *camera01, *camera02, *camera10, *camera11, *camera12, *camera20, *camera21, *camera22;
  int limit_h1, limit_h2, limit_v1, limit_v2;
  int got_news, fps;
  struct timeval moment;
  unsigned elapsed_microseconds;
  int ai_move = NONE;
  int winner = 0;
 
  fps = 100;

  // Initialization
  awake_the_weaver();
  hide_cursor();

  // Used to draw the squared frame that set the limit to our arena
  limit_h1 = window_height / 2 - 401;
  limit_h2 = limit_h1 + 802;
  limit_v1 =  window_width / 2 - 401;
  limit_v2 = limit_v1 + 802;

  // We need a lot of cameras to cover a wide area. Later they will send all the
  // images to the same part of screen. Because of this, when the ship goes
  // far east, it appears far west
  camera00 = new_camera(-800.0, -800.0, 800.0, 800.0);
  camera01 = new_camera(-800.0, 0.0, 800.0, 800.0);
  camera02 = new_camera(-800.0, 800.0, 800.0, 800.0);
  camera10 = new_camera(0.0, -800.0, 800.0, 800.0);
  camera11 = new_camera(0.0, 0.0, 800.0, 800.0);
  camera12 = new_camera(0.0, 800.0, 800.0, 800.0);
  camera20 = new_camera(800.0, -800.0, 800.0, 800.0);
  camera21 = new_camera(800.0, 0.0, 800.0, 800.0);
  camera22 = new_camera(800.0, 800.0, 800.0, 800.0);
  limit_camera(camera00, window_width / 2 - 400, window_height / 2 - 400, 800, 800);
  limit_camera(camera01, window_width / 2 - 400, window_height / 2 - 400, 800, 800);
  limit_camera(camera02, window_width / 2 - 400, window_height / 2 - 400, 800, 800);
  limit_camera(camera10, window_width / 2 - 400, window_height / 2 - 400, 800, 800);
  limit_camera(camera11, window_width / 2 - 400, window_height / 2 - 400, 800, 800);
  limit_camera(camera12, window_width / 2 - 400, window_height / 2 - 400, 800, 800);
  limit_camera(camera20, window_width / 2 - 400, window_height / 2 - 400, 800, 800);
  limit_camera(camera21, window_width / 2 - 400, window_height / 2 - 400, 800, 800);
  limit_camera(camera22, window_width / 2 - 400, window_height / 2 - 400, 800, 800);

  // Draw the limit frame
  draw_rectangle(limit_v1, limit_h1, 802, 802, WHITE);

  // Some more initializations
  initialize_star();
  initialize_dust();
  initialize_ship1();
  initialize_ship2();
  if(players == 1)
    initialize_ai();

  // Putting the ships in the right place
  move_ship2(644.0, 608.5);
  rotate_polygon(Ship1.tank, Ship1.x, Ship1.y, 3.0 * M_PI / 4.0);
  rotate_polygon(Ship1.body, Ship1.x, Ship1.y, 3.0 * M_PI / 4.0);
  rotate_polygon(Ship1.cockpit, Ship1.x, Ship1.y, 3.0 * M_PI / 4.0);
  rotate_polygon(Ship2.division, Ship2.x, Ship2.y, - M_PI / 4);
  rotate_polygon(Ship2.body, Ship2.x, Ship2.y, - M_PI / 4);
  rotate_polygon(Ship2.cockpit, Ship2.x, Ship2.y, - M_PI / 4);
  rotate_polygon(Ship2.left_wing, Ship2.x, Ship2.y, - M_PI / 4);
  rotate_polygon(Ship2.right_wing, Ship2.x, Ship2.y, - M_PI / 4);
  rotate_polygon(Ship2.right_prop, Ship2.x, Ship2.y, - M_PI / 4);
  rotate_polygon(Ship2.left_prop, Ship2.x, Ship2.y, - M_PI / 4);
 

  // Main loop
  for(;;){

    // Erase the ships old position
    if(Ship1.status == ALIVE){
      draw_ship1(BLACK);
    }
    if(Ship2.status == ALIVE){
      draw_ship2(BLACK);
    }
    
    // The AI must decide what to do
    if(players == 1)
      ai_move = ai_decision();

    // Getting player(s) input
    get_input();
    if((players == 2 && keyboard[LEFT_CTRL]) || (players == 1 && ai_move & HYPER)){
      // Blue ship goes to hyperspace
      if(Ship1.status == ALIVE){
        if(Ship1.fuel > HYPER_COAST){
          goto_hyperspace1();
	  play_soundfile("hyper.ogg");
          Ship1.fuel -= HYPER_COAST;
        }
      }
    }
    if((players == 2 && keyboard[D]) || (players == 1 && ai_move & MOVE_R)){
      if(Ship1.status == ALIVE){
	// Blue ship turn right
        rotate_polygon(Ship1.tank, Ship1.x, Ship1.y, SHIP_TURN / fps);
        rotate_polygon(Ship1.body, Ship1.x, Ship1.y, SHIP_TURN / fps);
        rotate_polygon(Ship1.cockpit, Ship1.x, Ship1.y, SHIP_TURN / fps);
      }
    }
    if(((players == 2) && keyboard[A]) || (players == 1 && ai_move & MOVE_L)){
      if(Ship1.status == ALIVE){
	// Blue ship turn left
        film_ship1(camera22, BLACK);
        rotate_polygon(Ship1.tank, Ship1.x, Ship1.y, - SHIP_TURN / fps);
        rotate_polygon(Ship1.body, Ship1.x, Ship1.y, - SHIP_TURN / fps);
        rotate_polygon(Ship1.cockpit, Ship1.x, Ship1.y, - SHIP_TURN / fps);
      }
    }
    if((players == 2 && keyboard[W]) ||(players == 1 && ai_move & PROP) ){
      if(Ship1.status == ALIVE){
	// Blue ship uses propulsor
        if(Ship1.fuel > 0.0){
          Ship1.dx += ((Ship1.front -> x - Ship1.x) / 20.0) * SHIP_THRUST / fps;
          Ship1.dy += ((Ship1.front -> y - Ship1.y) / 20.0) * SHIP_THRUST / fps;
          // Smoke!
          if(rand() % 2)
            new_dust(Ship1.tank -> x, Ship1.tank -> y, 
                     - ((Ship1.front -> x - Ship1.x) / 20.0) * SHIP_THRUST / fps, 
                     - ((Ship1.front -> y - Ship1.y) / 20.0) * SHIP_THRUST / fps, WHITE);
          if(rand() % 2)
            new_dust(Ship1.tank -> next -> x, Ship1.tank -> next -> y,
                     - ((Ship1.front -> x - Ship1.x) / 20.0) * SHIP_THRUST / fps,
                     - ((Ship1.front -> y - Ship1.y) / 20.0) * SHIP_THRUST / fps, WHITE);
          if(rand() % 2)
            new_dust(Ship1.tank -> next -> next -> x, Ship1.tank -> next -> next -> y,
                     - ((Ship1.front -> x - Ship1.x) / 20.0) * SHIP_THRUST / fps,
                      - ((Ship1.front -> y - Ship1.y) / 20.0) * SHIP_THRUST / fps, WHITE);
          if(rand() % 2)
            new_dust(Ship1.tank -> previous -> x, Ship1.tank -> previous -> y,
                      - ((Ship1.front -> x - Ship1.x) / 20.0) * SHIP_THRUST / fps,
                     - ((Ship1.front -> y - Ship1.y) / 20.0) * SHIP_THRUST / fps, WHITE);
          if(rand() % 2)
             new_dust((Ship1.tank -> x + Ship1.tank -> next -> x) / 2, (Ship1.tank -> y + Ship1.tank -> next -> y) / 2,
                      - ((Ship1.front -> x - Ship1.x) / 20.0) * SHIP_THRUST / fps,
                      - ((Ship1.front -> y - Ship1.y) / 20.0) * SHIP_THRUST / fps, WHITE);
          Ship1.fuel -= 100.0/ (float) fps;
        }
      }
    }
    if((players == 2 && keyboard[S]) || (players == 1 && ai_move & SHOT)){
      if(Ship1.status == ALIVE){
	// Blue ship shots!
        gettimeofday(&moment, NULL);
        elapsed_microseconds = (moment.tv_usec - Ship1.time.tv_usec) +
          1000000 * (moment.tv_sec - Ship1.time.tv_sec);
        if(elapsed_microseconds > SHOT_PRODUCTION){
          struct shot *shot_pointer;
          gettimeofday(&(Ship1.time), NULL);
          shot_pointer = new_shot(Ship1.front -> x, Ship1.front -> y, 0.0, 0.0, BLUE_SHOT, 1);
           blow_up_strenght1(shot_pointer -> shape -> x, shot_pointer -> shape -> y, &(shot_pointer -> dx), &(shot_pointer -> dy)); 
        }
      }
    }
    if(keyboard[RIGHT_CTRL]){
      if(Ship2.status == ALIVE){
	// Red ship goes to hyperspace
        if(Ship2.fuel > HYPER_COAST){
          goto_hyperspace2();
          Ship2.fuel -= HYPER_COAST;
	  play_soundfile("hyper.ogg");
        }
      }
    }
    if(keyboard[LEFT]){
      if(Ship2.status == ALIVE){
        // Red ship turns left
        rotate_polygon(Ship2.division, Ship2.x, Ship2.y, - SHIP_TURN / fps);
        rotate_polygon(Ship2.body, Ship2.x, Ship2.y, - SHIP_TURN / fps);
        rotate_polygon(Ship2.cockpit, Ship2.x, Ship2.y, - SHIP_TURN / fps);
        rotate_polygon(Ship2.left_wing, Ship2.x, Ship2.y, - SHIP_TURN / fps);
        rotate_polygon(Ship2.right_wing, Ship2.x, Ship2.y, - SHIP_TURN / fps);
        rotate_polygon(Ship2.right_prop, Ship2.x, Ship2.y, - SHIP_TURN / fps);
        rotate_polygon(Ship2.left_prop, Ship2.x, Ship2.y, - SHIP_TURN / fps);
      }
    }
    if(keyboard[RIGHT]){
      if(Ship2.status == ALIVE){
        // Red ship turns right
        rotate_polygon(Ship2.division, Ship2.x, Ship2.y, SHIP_TURN / fps);
        rotate_polygon(Ship2.body, Ship2.x, Ship2.y, SHIP_TURN / fps);
        rotate_polygon(Ship2.cockpit, Ship2.x, Ship2.y, SHIP_TURN / fps);
        rotate_polygon(Ship2.left_wing, Ship2.x, Ship2.y, SHIP_TURN / fps);
        rotate_polygon(Ship2.right_wing, Ship2.x, Ship2.y, SHIP_TURN / fps);
        rotate_polygon(Ship2.right_prop, Ship2.x, Ship2.y, SHIP_TURN / fps);
        rotate_polygon(Ship2.left_prop, Ship2.x, Ship2.y, SHIP_TURN / fps);
      }
    }
    if(keyboard[DOWN]){
      if(Ship2.status == ALIVE){
	// Red ship shots
        gettimeofday(&moment, NULL);
        elapsed_microseconds = (moment.tv_usec - Ship2.time.tv_usec) +
          1000000 * (moment.tv_sec - Ship2.time.tv_sec);
        if(elapsed_microseconds > SHOT_PRODUCTION){
          struct shot *shot_pointer;
          gettimeofday(&(Ship2.time), NULL);
          shot_pointer = new_shot(Ship2.front -> x, Ship2.front -> y, 0.0, 0.0, RED_SHOT, 2);
          blow_up_strenght2(shot_pointer -> shape -> x, shot_pointer -> shape -> y, &(shot_pointer -> dx), &(shot_pointer -> dy));
        }
      }
    }
    if(keyboard[UP]){
      if(Ship2.status == ALIVE){
	//~Red ship uses propulsor
        if(Ship2.fuel > 0.0){
          Ship2.dx += ((Ship2.front -> x - Ship2.x) / 20.0) * SHIP_THRUST / fps;
          Ship2.dy += ((Ship2.front -> y - Ship2.y) / 20.0) * SHIP_THRUST / fps;
          // Smoke!                                                                                                                                           
          if(rand() % 4)
            new_dust(Ship2.left_wing -> x, Ship2.left_wing -> y,
                     - ((Ship2.front -> x - Ship2.x) / 20.0) * SHIP_THRUST / fps,
                     - ((Ship2.front -> y - Ship2.y) / 20.0) * SHIP_THRUST / fps, WHITE);
          if(rand() % 4)
            new_dust(Ship2.right_wing -> x, Ship2.right_wing -> y,
                     - ((Ship2.front -> x - Ship2.x) / 20.0) * SHIP_THRUST / fps,
                     - ((Ship2.front -> y - Ship2.y) / 20.0) * SHIP_THRUST / fps, WHITE);
          if(rand() % 4)
            new_dust(Ship2.body -> previous -> previous -> previous -> x, Ship2.body -> previous -> previous -> previous -> y,
                     - ((Ship2.front -> x - Ship2.x) / 20.0) * SHIP_THRUST / fps,
                     - ((Ship2.front -> y - Ship2.y) / 20.0) * SHIP_THRUST / fps, WHITE);
          Ship2.fuel -= 100.0/ (float) fps;
        }
      }
    }
    if(keyboard[ESC]) // Esc pressed. Quit game.
      break;    
    
    // Apply gravity in everything
    if(Ship1.status == ALIVE)
      star_gravity(Ship1.x, Ship1.y, &(Ship1.dx), &(Ship1.dy), fps);
    if(Ship2.status == ALIVE)
      star_gravity(Ship2.x, Ship2.y, &(Ship2.dx), &(Ship2.dy), fps);
    gravity_in_dust(fps);
    gravity_in_shot(fps);
    burn_dust();
    burn_shot();
     
    // Moving the 2 ships
     if(Ship1.status == ALIVE){
       move_ship1(Ship1.dx / (float) fps, Ship1.dy / (float) fps);
       if(Ship1.x < 0.0)
         move_ship1(800.0, 0.0);
       else if(Ship1.x > 800.0)
         move_ship1(-800.0, 0.0);
       if(Ship1.y < 0.0)
         move_ship1(0.0, 800.0);
       else if(Ship1.y > 800.0)
         move_ship1(0.0, -800.0);
     }
     if(Ship2.status == ALIVE){
       move_ship2(Ship2.dx / (float) fps, Ship2.dy / (float) fps);
       if(Ship2.x < 0.0)
         move_ship2(800.0, 0.0);
       else if(Ship2.x > 800.0)
         move_ship2(-800.0, 0.0);
       if(Ship2.y < 0.0)
         move_ship2(0.0, 800.0);
       else if(Ship2.y > 800.0)
         move_ship2(0.0, -800.0);
     } 
     
     // The star will grow or shrink
     if(is_star_tired()){
       float temp = Star.shape -> z;
       Star.shape -> z = TITANIC + 100.0;
       film_fullcircle(camera11, Star.shape, BLACK);
       Star.shape -> z = temp;
       star_next_state();
       film_fullcircle(camera11, Star.shape, Star.color);
       got_news = 1;
       flush();
     }

     
     // Drawing ships and updating their status
     if(Ship1.status == ALIVE){
       draw_ship1(Ship1.color);
     }
     else if(Ship1.status == HYPERSPACE){
       // If we are too much time in hyperspace, come back
       gettimeofday(&moment, NULL);
       elapsed_microseconds = (moment.tv_usec - Ship1.time.tv_usec) +
	 1000000 * (moment.tv_sec - Ship1.time.tv_sec);
       if(elapsed_microseconds > HYPER_TIME)
	 return_from_hyperspace1();
     }
     else if(Ship1.status == DEAD){
       // If the ship were destroyed, determines the winner and after some time, quit
       gettimeofday(&moment, NULL);
       elapsed_microseconds = (moment.tv_usec - Ship1.time.tv_usec) +
	 1000000 * (moment.tv_sec - Ship1.time.tv_sec);
       if(elapsed_microseconds > 1000000){
	 if(elapsed_microseconds > 3000000)
	   break;
	 else{
	   if(winner == 0){
	     if(Ship2.status != DEAD)
	       winner = 2;
	     else
	       winner = 3;
	   }
	 }	  
       }
     }
     if(Ship2.status == ALIVE){
       draw_ship2(Ship2.color);
     }
     else if(Ship2.status == HYPERSPACE){
       // If we are too much time in hyperspace, come back
       gettimeofday(&moment, NULL);
       elapsed_microseconds = (moment.tv_usec - Ship2.time.tv_usec) +
	 1000000 * (moment.tv_sec - Ship2.time.tv_sec);
       if(elapsed_microseconds > HYPER_TIME)
	 return_from_hyperspace2();
     }
     else if(Ship2.status == DEAD){
       // If the ship were destroyed, determines the winner and after some time, quit
       gettimeofday(&moment, NULL);
       elapsed_microseconds = (moment.tv_usec - Ship2.time.tv_usec) +
	 1000000 * (moment.tv_sec - Ship2.time.tv_sec);
       if(elapsed_microseconds > 1000000){
	 if(elapsed_microseconds > 3000000)
	   break;
	 else{
	   if(winner == 0){
            if(Ship1.status != DEAD)
              winner = 1;
            else
              winner = 3;
	   }
	 }
       }
     }
     
     // Updating the dust drawing in the screen
     erase_dust(camera00);
     erase_dust(camera01);
     erase_dust(camera02);
     erase_dust(camera10);
     erase_dust(camera11);
     erase_dust(camera12);
     erase_dust(camera20);
     erase_dust(camera21);
     erase_dust(camera22);
     move_dust(fps);
     clean_dust();
     film_dust(camera00);
     film_dust(camera01);
     film_dust(camera02);
     film_dust(camera10);
     film_dust(camera11);
     film_dust(camera12);
     film_dust(camera20);
     film_dust(camera21);
     film_dust(camera22);

     // Now we are handling now the shots
     erase_shot(camera00);
     erase_shot(camera01);
     erase_shot(camera02);
     erase_shot(camera10);
     erase_shot(camera11);
     erase_shot(camera12);
     erase_shot(camera20);
     erase_shot(camera21);
     erase_shot(camera22);
     move_shot(fps);
     clean_shot();
     film_shot(camera00);
     film_shot(camera01);
     film_shot(camera02);
     film_shot(camera10);
     film_shot(camera11);
     film_shot(camera12);
     film_shot(camera20);
     film_shot(camera21);
     film_shot(camera22);    

     // If some shot touches the star or an enemy shot, blowup it
     if(Ship1.status == ALIVE){
       if(collision_circle_polygon(Star.shape, Ship1.body) ||
	  collision_circle_polygon(Star.shape, Ship1.cockpit) ||
	  collision_circle_polygon(Star.shape, Ship1.tank) ||
	  got_shot1()){
	 draw_ship1(BLACK);
	 blow_up1();
	 play_soundfile("explosion.ogg");
       }
     }
     if(Ship2.status == ALIVE){
       if(collision_circle_polygon(Star.shape, Ship2.body) ||
	  collision_circle_polygon(Star.shape, Ship2.cockpit) ||
	  collision_circle_polygon(Star.shape, Ship2.division) ||
	  collision_circle_polygon(Star.shape, Ship2.left_wing) ||
	  collision_circle_polygon(Star.shape, Ship2.right_wing) ||
	  collision_circle_polygon(Star.shape, Ship2.left_prop) ||
	  collision_circle_polygon(Star.shape, Ship2.right_prop) ||
	  got_shot2()){
	 draw_ship2(BLACK);
	 blow_up2();
	 play_soundfile("explosion.ogg");
       }
     }
     
     // Drawing again the limit frame
     draw_rectangle(limit_v1, limit_h1, 802, 802, WHITE);
     flush();
    
    
     // Drawing the ammount of blue ship fuel 
     draw_rectangle((window_width / 2) - 420, (window_height / 2) - 100, 10, 200, GREEN);
     fill_rectangle((window_width / 2) - 419, (window_height / 2) - 99, 9, 199, BLACK);
     fill_rectangle((window_width / 2) - 419,
		    (window_height / 2) - 99 + 200 - (200 * (Ship1.fuel / FUEL)), 
		    9, (199 * (Ship1.fuel / FUEL)), 
		    BURNT_ORANGE);
     
     // Drawing the ammount of red ship fuel
     draw_rectangle((window_width / 2) + 410, (window_height / 2) - 100, 10, 200, GREEN);
     fill_rectangle((window_width / 2) + 411, (window_height / 2) - 99, 9, 199, BLACK);
     fill_rectangle((window_width / 2) + 411,
		    (window_height / 2) - 99 + 200 - (200 * (Ship2.fuel / FUEL)),
		    9, (199 * (Ship2.fuel / FUEL)),
		    BURNT_ORANGE);
     
     // If someone won the game, print the result in the screen
     if(winner != 0){
       if(winner == 1)
	 draw_text(window_width / 2 - 90, 10, "WINNER: Blue Ship", "12x24", BLUE);
       else if(winner == 2)
	 draw_text(window_width / 2 - 90, 10, "WINNER: Red Ship", "12x24", RED);
       else if(winner == 3)
	 draw_text(window_width / 2 - 90, 10, "WINNER: None", "12x24", YELLOW);
     }
     
     // Settinf frame rate and waiting some microseconds
     weaver_rest(10000000);
     flush();
  }

  // Cleaning the memory. We hate memory leaks.
  destroy_dust();
  destroy_shot();
  destroy_star();
  destroy_ship1();
  destroy_ship2();
  free(camera00);
  free(camera01);
  free(camera02);
  free(camera10);
  free(camera11);
  free(camera12);
  free(camera20);
  free(camera21);
  free(camera22);

  return 0;
}

int intro(void){
  int i;
  struct timespec req = {0, 10000000}; // 0.01 seconds
  camera *cam;
  polygon *spacewar[14];

  awake_the_weaver();
  hide_cursor();
  // Creating the SPACEWAR! logo
  spacewar[0] = new_polygon(19, 6.0, 2.0, 5.0, 0.0, 2.0, 0.0, 0.0, 2.0, 0.0, 4.0,
			    3.0, 7.0, 5.0, 7.0, 5.0, 11.0, 0.0, 11.0, 1.0, 12.0,
			    5.0, 12.0, 7.0, 10.0, 7.0, 8.0, 6.0, 7.0, 6.0, 6.0,
			    5.0, 6.0, 2.0, 3.0, 2.0, 1.0, 6.0, 1.0);
  spacewar[1] = new_polygon(8,
			    10.0, 12.0, 10.0, 0.0, 16.0, 0.0, 18.0, 2.0, 18.0, 5.0,
			    16.0, 7.0, 12.0, 7.0, 12.0, 12.0);
  spacewar[2] = new_polygon(6,
			    13.0, 2.0, 14.0, 2.0, 15.0, 3.0, 15.0, 4.0, 14.0, 5.0,
			    13.0, 5.0);
  spacewar[3] = new_polygon(7,
			    19.0, 12.0, 24.0, 0.0, 29.0, 12.0, 27.0, 12.0, 26.0, 9.0,
			    22.0, 9.0, 21.0, 12.0);
  spacewar[4] = new_polygon(3, 23.0, 7.0, 24.0, 4.0, 25.0, 7.0);
  spacewar[5] = new_polygon(19 ,
			     39.0, 10.0, 39.0, 11.0, 38.0, 12.0, 34.0, 12.0, 31.0, 9.0,
			     31.0, 3.0, 34.0, 0.0, 38.0, 0.0, 39.0, 1.0, 39.0, 2.0,
			     38.0, 2.0, 37.0, 1.0, 36.0, 1.0, 33.0, 4.0, 33.0, 8.0,
			     34.0, 9.0, 35.0, 11.0, 37.0, 11.0, 38.0, 10.0);
  spacewar[6] = new_polygon(12,
			     42.0, 0.0, 49.0, 0.0, 49.0, 1.0, 44.0, 1.0, 44.0, 5.0,
			     47.0, 5.0, 47.0, 6.0, 44.0, 6.0, 44.0, 11.0, 49.0, 11.0,
			     49.0, 12.0, 42.0, 12.0);
  spacewar[7] = new_polygon(10,
			    51.0, 0.0, 55.5, 12.0, 58.5, 6.0, 61.5, 12.0, 66.0, 0.0,
			    64.0, 0.0, 61.0, 9.0, 58.5, 0.0, 55.5, 7.0, 53.0, 0.0);
  spacewar[8] = new_polygon(7,
			    66.0, 12.0, 71.0, 0.0, 76.0, 12.0, 75.0, 12.0, 73.0, 9.0,
			    69.0, 9.0, 68.0, 12.0); 
  spacewar[9] = new_polygon(3, 70.0, 7.0, 71.0, 4.0, 72.0, 7.0);
  spacewar[10] = new_polygon(11,
			     78.0, 12.0, 78.0, 0.0, 84.0, 0.0, 86.0, 2.0, 86.0, 5.0,
			     84.0, 7.0, 84.0, 8.0, 87.0, 12.0, 85.0, 12.0, 81.0, 7.0,
			     80.0, 12.0);
  spacewar[11] = new_polygon(4, 81.0, 5.0, 81.0, 2.0, 83.0, 2.0, 83.0, 5.0);
  spacewar[12] = new_polygon(5,
			     90.0, 0.0, 92.0, 0.0, 92.0, 6.0, 91.0, 8.0, 90.0, 6.0);
  spacewar[13] = new_polygon(4, 90.0, 10.0, 92.0, 10.0, 92.0, 12.0, 90.0, 12.0);
  
  cam = new_camera_w(-0.5, -12.0, 93.0);

  for(;;){
    get_input();
    if(keyboard[1])// 1 player mode
      return 1;
    else if(keyboard[2]) // 2 players mode
      return 2;
    else if(keyboard[F1]) // Help
      return 3; 
    else if(keyboard[ESC]) // Exit
      exit(0);
    for(i = 0; i < 14 ; i++) // Drawing logo
      film_polygon(cam, spacewar[i], WHITE);
    
    // Drawing instructions
    draw_text(window_width / 2 - 100, window_height / 2, "Press <1> to play against the CPU.", "12x24", YELLOW);
    draw_text(window_width / 2 - 100, window_height / 2 + 16, "Press <2> to play against a human.", "12x24", YELLOW);
    draw_text(window_width / 2 - 100, window_height / 2 + 32, "Press <F1> for instructions.", "12x24", YELLOW);
    draw_text(window_width / 2 - 100, window_height / 2 + 48, "Press <Esc> to exit.", "12x24", YELLOW);
    flush();
    nanosleep(&req, NULL);
  }
  for(i = 0; i < 14 ; i++)
    free(spacewar[i]);
  free(cam);
   
}

int controls(void){
  struct timespec req = {0, 10000000}; // 0.01 seconds
  camera *camera0;
  int i;
  
  awake_the_weaver();
  hide_cursor();
  camera0 = new_camera_w(0.0, -250.0, 1200.0);
  initialize_ship1();
  initialize_ship2();
  move_ship1(-10.0, 0.0 + camera0 -> z / 5);
  move_ship2(0.0, 150.0 + camera0 -> z / 5);

 
    for(i = 0; i < 7; i ++){
      film_ship1(camera0, Ship1.color);
      film_ship2(camera0, Ship2.color);
      move_ship1(200.0, 0.0);
      move_ship2(200.0, 0.0);
    }

  draw_text(10, 10, "RED SHIP", "12x24", RED);
  draw_text(10, 40, "<UP>:         Use propulsor.", "12x24", RED);
  draw_text(10, 60, "<LEFT>:       Turn left.", "12x24", RED);
  draw_text(10, 80, "<RIGHT>:      Turn right.", "12x24", RED);
  draw_text(10, 100, "<DOWN>:       Shot.", "12x24", RED);
  draw_text(10, 120, "<Right Ctrl>: Go to Hyperspace.", "12x24", RED);
  draw_line(window_width / 2, 0, window_width / 2, 140, WHITE);
  draw_text(window_width / 2 + 10, 10, "BLUE SHIP", "12x24", BLUE);
  draw_text(window_width / 2 + 10, 40, "<W>:             Use propulsor.", "12x24", BLUE);
  draw_text(window_width / 2 + 10, 60, "<A>:             Turn left.", "12x24", BLUE);
  draw_text(window_width / 2 + 10, 80, "<D>:             Turn right.", "12x24", BLUE);
  draw_text(window_width / 2 + 10, 100, "<S>:             Shot.", "12x24", BLUE);
  draw_text(window_width / 2 + 10, 120, "<Ctrl Esquerdo>: Go to Hyperspace.", "12x24", BLUE);
  draw_line(0, 140, window_width, 140, WHITE);
  draw_text(15, 165, "STAR: In the center of screen exists a white dwarf star. It's gravitational strenght is huge.", "12x24", YELLOW);
  draw_text(15, 185, "         Every ship that touches the star is destroyed by the hight temperature.", "12x24", YELLOW);
  draw_rectangle(10, 150, 600, 40, WHITE);
  draw_text(window_width / 2 - 200, 205, "WEAPONS: The red and blue ships have cannons able to store star", "12x24", GREEN); 
  draw_text(window_width / 2 - 200, 225, "            energy and produce plasm spheres under hight pressure that can be shoot agains", "12x24", GREEN);
  draw_text(window_width / 2 - 200, 245, "            enemy ships. The contact with red plasm destroys blue ships while red ships' blindage", "12x24", GREEN);
  draw_text(window_width / 2 - 200, 265, "            doesn't protect them against blue plasm.", "12x24", GREEN);
  draw_rectangle(window_width / 2 - 202, 190, 602, 80, WHITE);
  draw_text(10, 290, "HYPERSPACE: Ships can travel to hyperspace. Doing this, they disappear", "12x24", 0x00ffff);
  draw_text(10, 310, "             from the tradictional space for 1 second, becoming unreacheble. After", "12x24", 0x00ffff);
  draw_text(10, 330, "             some time, they return in a random point in the battlefield.", "12x24", 0x00ffff);
  draw_text(10, 350, "             This maneuver is dangerous and spend about 10% of the total fuel.", "12x24", 0x00ffff);
  draw_rectangle(8, 278, 500, 80, WHITE);
  draw_text(window_width / 2 - 200, 380, "PROPULSOR: Propulsors propel the ships forward. But this spends fuel.", "12x24", 0x9955ff);
  draw_text(window_width / 2 - 200, 400, "           A ship can store enough fuel to use a propulsor for 18 seconds.", "12x24", 0x9955ff);
  draw_rectangle(window_width / 2 - 202, 367, 600, 40, WHITE);
  draw_line(0, 450, window_width, 450, WHITE);

  for(;;){
    get_input();
    if(keyboard[ANY]) // Exit
      break;
  
     nanosleep(&req, NULL);
  }
  
  destroy_ship1();
  destroy_ship2();
  return 0;
}
