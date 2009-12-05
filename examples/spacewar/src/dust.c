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

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "dust.h"

// An initialization function
void initialize_dust(void){
  list_of_dust = NULL;
}

// Creates a new dust particle. Used in propulsors and explosions
struct dust *new_dust(float x, float y, float dx, float dy, unsigned color){
  struct dust *new_dust = (struct dust *) malloc(sizeof(struct dust));
  
  new_dust -> shape = new_polygon(1, x, y);
  new_dust -> dx = dx;
  new_dust -> dy = dy;
  new_dust -> color = color;
  gettimeofday(&(new_dust -> time), NULL);
  new_dust -> previous = new_dust -> next = NULL;
  if(list_of_dust == NULL)
    list_of_dust = new_dust;
  else{
    struct dust *pointer = list_of_dust;
    while(pointer -> next != NULL)
      pointer = pointer -> next;
    pointer -> next = new_dust;
    new_dust -> previous = pointer;
  }
  return new_dust;
}

// Move a dust particle
void move_dust(int fps){
  struct dust *pointer = list_of_dust;
  while(pointer != NULL){
    pointer -> shape -> x += pointer -> dx / fps;
    pointer -> shape -> y += pointer -> dy / fps;
    pointer = pointer -> next;
  }
}

// If a dust particle is too old, destroy it
void clean_dust(void){
  struct dust *pointer = list_of_dust;
  struct dust *temp;
  struct timeval time;
  unsigned long elapsed_microseconds;

  while(pointer != NULL){
    gettimeofday(&time, NULL);
    elapsed_microseconds = (time.tv_usec - pointer -> time.tv_usec) +
      1000000 * (time.tv_sec - pointer -> time.tv_sec);
    if(elapsed_microseconds > DUST_LIFE){
      if(pointer -> previous == NULL){ // It's the first
        if(pointer -> next == NULL){ // It's the only one
          free(pointer);
          list_of_dust = NULL;
          return;
        }
        else{ // The first of many
	  if(pointer -> next != NULL){
	    pointer = pointer -> next;
	    free(pointer -> previous);
	    pointer -> previous = NULL;
	    list_of_dust = pointer;
	    continue;
	  }
	  else{
	    free(pointer);
	    list_of_dust = NULL;
	    return;
	  }
        }
      }
      else if(pointer -> next != NULL){
        temp = pointer;
        pointer = pointer -> previous;
        pointer -> next = temp -> next;
        pointer -> next -> previous = pointer;
        free(temp);
      } 
      else{
        pointer -> previous -> next = NULL;
        free(pointer);
        return;
      }
    }
    pointer = pointer -> next;
  }
}

// Erase a dust particle in the screen
void erase_dust(camera *space_camera){
  struct dust *pointer = list_of_dust;
  while(pointer != NULL){
    if(pointer -> shape -> x < 0.0)
      pointer -> shape -> x += 800.0;
    else if(pointer -> shape -> x > 800.0)
      pointer -> shape -> x -= 800.0;
    if(pointer -> shape -> y < 0.0)
      pointer -> shape -> y += 800.0;
    else if(pointer -> shape -> y > 800.0)
      pointer -> shape -> y -= 800.0;
    film_polygon(space_camera, pointer -> shape, BLACK);
    pointer = pointer -> next;
  }
}

// Film a dust particle using a camera
void film_dust(camera *space_camera){
  struct dust *pointer = list_of_dust;
  while(pointer != NULL){
    film_polygon(space_camera, pointer -> shape, pointer -> color);
    pointer = pointer -> next;
  }
}

// If a dust particle arrived in the star, destroy it.
void burn_dust(void){
  struct dust *pointer = list_of_dust;
  struct dust *temp;
  float distance;
  while(pointer != NULL){
    distance = pointer -> shape -> x - *(Star.x);
    distance *= distance;
    distance += (pointer -> shape -> y - *(Star.y)) * (pointer -> shape -> y - *(Star.y));
    distance = sqrt(distance);
    if(distance < Star.shape -> z){
      if(pointer -> previous == NULL){ // It's the first
        if(pointer -> next == NULL){ // It's the only one
          free(pointer);
          list_of_dust = NULL;
          return;
        }
        else{ // The first of many
	  if(pointer -> next != NULL){
	    pointer = pointer -> next;
	    free(pointer -> previous);
	    pointer -> previous = NULL;
	    list_of_dust = pointer;
	    continue;
	  }
	  else{
	    free(pointer);
	    list_of_dust = NULL;
	    return;
	  }
        }
      }
      else if(pointer -> next != NULL){
        temp = pointer;
        pointer = pointer -> previous;
        pointer -> next = temp -> next;
        pointer -> next -> previous = pointer;
        free(temp);
      }
      else{
        pointer -> previous -> next = NULL;
        free(pointer);
        return;
      }
    }
    pointer = pointer -> next;
  }
}

// Apply the star gravity in all the dust particles
void gravity_in_dust(int fps){
  struct dust *pointer = list_of_dust;
  while(pointer != NULL){
    star_gravity(pointer -> shape -> x, pointer -> shape -> y, &(pointer -> dx), &(pointer -> dy), fps);
    pointer = pointer -> next;
  }
}

// Free ll the memory allocated to all the dust particles
void destroy_dust(void){
  struct dust *pointer = list_of_dust;
  while(pointer != NULL){
    if(pointer -> next != NULL){
      pointer = pointer -> next;
      free(pointer -> previous);
      pointer -> previous = NULL;
    }
    else{
      free(pointer);
      pointer = list_of_dust = NULL;
    }
  }
}
