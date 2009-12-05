/*
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

#include <math.h>
#include "ai.h"
#include "star.h"
#include "ship1.h"

// This function only sets the ai's status as "START".
void initialize_ai(void){
  ai_status = START;
}

// Our AI knows how to compute distances
float distance(float x1, float y1, float x2, float y2){
  float dist;
  dist = x1 - x2;
  dist *= dist;
  dist += (y1 - y2) * (y1 - y2);
  return sqrt(dist);
}

// The AI knows how to determine it's own speed
float my_speed(void){
  return sqrt((Ship1.dx * Ship1.dx) + (Ship1.dy * Ship1.dy));
}

// The AI decision is a very simple list of ifs and elses
int ai_decision(void){
  struct shot *pointer = list_of_shot;
  float x_left, y_left, x_right, y_right, real_x, real_y;

  // Determining initial strategy. It's random
  if(ai_status == START){
    ai_status = rand() % 4 + 1;
    return NONE;
  }
  // In this case, we start the game turning left
  else if(ai_status == START_MOVE1){
    if(my_speed() >= GOOD_SPEED){
      ai_status = FIGHTING;
      return NONE;
    }
    else
      return MOVE_L | PROP;
  }
  // Here our initial strategy is moving right
  else if(ai_status == START_MOVE2){
    if(my_speed() >= GOOD_SPEED){
      ai_status = FIGHTING;
      return NONE;
    }
    else return MOVE_R | PROP;
  }
  // Here, we don't turn. Only use a propulsor to build speed
  else if(ai_status == START_MOVE3){
    if(my_speed() >= GOOD_SPEED){
      ai_status = FIGHTING;
      return NONE;
    }
    else return PROP;
  }
  // In the majority of time, we are in FIGHTING stage
  else if(ai_status == FIGHTING){
    // If the star is too next, go to hyperspace!
    if(distance(Ship1.x, Ship1.y, *(Star.x), *(Star.y)) <= TITANIC * 2){
      return HYPER | SHOT;
    }
    else{
      // If an enemy shot is next, go to hyperspace
      while(pointer != NULL){
	if(pointer -> owner != 1)
	  if(distance(Ship1.x, Ship1.y, pointer -> shape -> x, pointer -> shape -> y) <= 35.0)
	    return HYPER | SHOT;
      pointer = pointer -> next;
      }
      // If the enemy is not reachable, do nothing
      if(Ship2.status != ALIVE)
	return NONE;
      
      // Ajusting player position to aim
      if(Ship1.front -> x - Ship2.x > 400.0){
	real_x = Ship2.x + 800.0;
      }
      else if(Ship1.front -> x - Ship2.x < -400.0){
	real_x = Ship2.x - 800.0;
      }
      else{
	real_x = Ship2.x;
      }     
      if(Ship1.front -> y - Ship2.y > 400.0)
	real_y = Ship2.y + 800.0;
      else if(Ship1.front -> y - Ship2.y < -400.0)
	real_y = Ship2.y - 800.0;
      else real_y = Ship2.y;
      
      // Moving to the right position that we already calculated. And then, shot.
      rotate_polygon(Ship1.body, Ship1.x, Ship1.y, - SHIP_TURN / 100);
      x_left = Ship1.front -> x;
      y_left = Ship1.front -> y;
      rotate_polygon(Ship1.body, Ship1.x, Ship1.y, 2 * SHIP_TURN / 100);
      x_right = Ship1.front -> x;
      y_right = Ship1.front -> y;
      rotate_polygon(Ship1.body, Ship1.x, Ship1.y, - SHIP_TURN / 100);
      if(distance(x_left, y_left, real_x, real_y) <= distance(x_right, y_right, real_x, real_y))
	return MOVE_L | SHOT;
      else
	return MOVE_R | SHOT;
    }
  }
  return NONE;
}
