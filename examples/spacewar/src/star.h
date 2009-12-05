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

#ifndef _STAR_H_
#define _STAR_H_

#include <sys/time.h>
#include "weaver/weaver.h"

// Star size
#define TINY    26.0
#define SMALL   29.0
#define MEDIUM  32.0
#define BIG     35.0
#define TITANIC 38.0
// How many microseconds our star can stay without moving
#define STAR_PATIENCE 100000ul 
// The star gravity (gravitational constant * star mass * ship mass)
#define STAR_GRAVITY 300000.0

// This is our little star in the center of screen
struct star{
  circle *shape;
  float *state;
  struct timeval time;
  unsigned color;
  float *x, *y;
} Star;

void initialize_star(void);
void star_next_state(void);
int is_star_tired(void);
void star_gravity(float, float, float *, float *, int);
void destroy_star(void);

#endif
