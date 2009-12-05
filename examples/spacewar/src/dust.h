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

#ifndef _DUST_H_
#define _DUST_H_

#include "weaver/weaver.h"
#include "star.h"

// How much microseconds a dust appear in the screen
#define DUST_LIFE 1000000

struct dust{
  polygon *shape;
  float dx, dy;
  unsigned color;
  struct dust *next, *previous;
  struct timeval time;
};

struct dust *list_of_dust;

void initialize_dust(void);
struct dust *new_dust(float, float, float, float, unsigned);
void move_dust(int);
void clean_dust(void);
void erase_dust(camera *);
void film_dust(camera *);
void burn_dust(void);
void gravity_in_dust(int);
void destroy_dust(void);

#endif
