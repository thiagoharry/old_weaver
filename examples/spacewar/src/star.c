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

#include <stdlib.h>
#include "star.h"

void initialize_star(void){
  Star.shape = new_circle(400.0, 400.0, MEDIUM);
  Star.state = &(Star.shape -> z);
  gettimeofday(&(Star.time), NULL);
  Star.color = WHITE;
  Star.x = &(Star.shape -> x);
  Star.y = &(Star.shape -> y);
}

void destroy_star(void){
  free(Star.shape);
}

// This controls the growth ans shrink of our star
void star_next_state(void){
  if(*(Star.state) == TINY)
    Star.shape -> z = SMALL;
  else if(*(Star.state) == SMALL){
    if(rand() % 2)
      Star.shape -> z = TINY;
    else
      Star.shape -> z = MEDIUM;
  }
  else if(*(Star.state) == MEDIUM){
    if(rand() % 2)
      Star.shape -> z = SMALL;
    else
      Star.shape -> z = BIG;
  }
  else if(*(Star.state) == BIG){
    if(rand() % 2)
      Star.shape -> z = MEDIUM;
    else
      Star.shape -> z = TITANIC;
  }
  else
    Star.shape -> z = BIG;
}

// This returns 1 if the star is too much time without change in
// it's status
int is_star_tired(void){
  struct timeval time;
  unsigned long elapsed_microseconds;
  gettimeofday(&time, NULL);
  elapsed_microseconds = (time.tv_usec - Star.time.tv_usec) + 
    1000000 * (time.tv_sec - Star.time.tv_sec);
  if(elapsed_microseconds > STAR_PATIENCE){
    gettimeofday(&(Star.time), NULL);
    return 1;
  }
  return 0;
}

// This calculates the star gravitational strenght, the 
// gravity vector and sums the vector with (*dx, *dy)
void star_gravity(float x, float y, float *dx, float *dy, int fps){
  float distance, strenght;
  // Discover the real position
  if(x < 0.0)
    x += 800.0;
  else if(x > 800.0)
    x -= 800.0;
  if(y < 0.0)
    y += 800.0;
  else if(y > 800.0)
  y -= 800.0;
  // Calculate the distance
  distance = *(Star.x) - x;
  distance *= distance;
  distance += (*(Star.y) - y) * (*(Star.y) - y);
  distance /= 2;
  // Generating the vector
  strenght = STAR_GRAVITY / (distance * distance);
  strenght = (1000000 / fps) * strenght;
  //printf("DEBUG: strenght: %f\n", strenght);
  *dx += (((*Star.x) - x) / distance) * strenght;
  *dy += (((*Star.y) - y) / distance) * strenght;
}
