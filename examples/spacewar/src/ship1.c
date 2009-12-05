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
#include "ship1.h"
#include "star.h"
#include "dust.h"

// A simple initialization
void initialize_ship1(void){
  Ship1.tank = new_polygon(4, 132.0, 182.0, 140.0, 182.0, 140.0, 190.0, 132.0, 190.0);
  Ship1.body = new_polygon(7, 120.0, 190.0, 136.0, 150.0, 152.0, 190.0, 140.0, 186.0, 140.0, 182.0, 132.0, 182.0, 132.0, 186.0);
  Ship1.cockpit = new_polygon(2, 132.0, 162.0, 140.0, 162.0);
  Ship1.front = Ship1.body -> next;
  Ship1.color = BLUE;
  Ship1.x = 136.0;
  Ship1.y = 170.0;
  Ship1.dx = Ship1.dy = 0.0;
  Ship1.status = ALIVE;
  gettimeofday(&(Ship1.time), NULL);
  Ship1.fuel = FUEL;
}

// Free the memory
void destroy_ship1(void){
  destroy_polygon(Ship1.tank);
  destroy_polygon(Ship1.body);
  destroy_polygon(Ship1.cockpit);
}

// Another initialization
void initialize_ship2(void){
  Ship2.cockpit = new_polygon(3, 11.0, 9.0, 21.5, 0.0, 32.0, 9.0);
  Ship2.division = new_polygon(2, 21.5, 0.0, 21.5, 9.0);
  Ship2.body = new_polygon(7, 11.0, 9.0, 32.0, 9.0, 30.5, 12.0, 24.5, 19.5, 21.5, 34.5, 18.5, 19.5, 12.5, 12.0);
  Ship2.right_wing = new_polygon(2, 24.5, 28.5, 30.5, 34.5);
  Ship2.left_wing = new_polygon(2, 18.5, 28.5, 12.5, 34.5);
  Ship2.right_prop = new_polygon(2, 30.5, 25.0, 30.5, 48.0);
  Ship2.left_prop = new_polygon(2, 12.5, 25.5, 12.5, 48.0);
  Ship2.front = Ship2.cockpit -> next;
  Ship2.color = RED;
  Ship2.x = 20.5;
  Ship2.y = 21.0;
  Ship2.dx = Ship2.dy = 0.0;
  Ship2.status = ALIVE;
  gettimeofday(&(Ship2.time), NULL);
  Ship2.fuel = FUEL;
}

// Freeing memory
void destroy_ship2(void){
  destroy_polygon(Ship2.cockpit);
  destroy_polygon(Ship2.division);
  destroy_polygon(Ship2.body);
  destroy_polygon(Ship2.right_wing);
  destroy_polygon(Ship2.left_wing);
  destroy_polygon(Ship2.right_prop);
  destroy_polygon(Ship2.left_prop);
}

void film_ship1(camera *space_camera, unsigned color){
  film_polygon(space_camera, Ship1.tank, color);
  film_polygon(space_camera, Ship1.body, color);
  film_polygon(space_camera, Ship1.cockpit, color);
}

void film_ship2(camera *space_camera, unsigned color){
  film_polygon(space_camera, Ship2.cockpit, color);
  film_polygon(space_camera, Ship2.division, color);
  film_polygon(space_camera, Ship2.body, color);
  film_polygon(space_camera, Ship2.right_wing, color);
  film_polygon(space_camera, Ship2.left_wing, color);
  film_polygon(space_camera, Ship2.right_prop, color);
  film_polygon(space_camera, Ship2.left_prop, color);
}

void move_ship1(float x, float y){
  move_polygon(Ship1.tank, x, y);
  move_polygon(Ship1.body, x, y);
  move_polygon(Ship1.cockpit, x, y);
  Ship1.x += x;
  Ship1.y += y;
}

void move_ship2(float x, float y){
  move_polygon(Ship2.cockpit, x, y);
  move_polygon(Ship2.division, x, y);
  move_polygon(Ship2.body, x, y);
  move_polygon(Ship2.right_wing, x, y);
  move_polygon(Ship2.left_wing, x, y);
  move_polygon(Ship2.right_prop, x, y);
  move_polygon(Ship2.left_prop, x, y);
  Ship2.x += x;
  Ship2.y += y;
}

// Determines the strenght applied in a particle or projectile.
void blow_up_strenght1(float x, float y, float *dx, float *dy){
  float distance, strenght;
  // Calculate the distance                                                                                                                                   
  distance = Ship1.x - x;
  distance *= distance;
  distance += (Ship1.y - y) * (Ship1.y - y);
  distance /= 2;
  // Generating the vector
  strenght = BLOW_UP / (distance * distance);
  strenght = (1000000 ) * strenght;
  strenght *= -1.0;
  *dx += ((Ship1.x - x) / distance) * strenght;
  *dy += ((Ship1.y - y) / distance) * strenght;
}
void blow_up_strenght2(float x, float y, float *dx, float *dy){
  float distance, strenght;
  // Calculate the distance                                                                                                                                   
  distance = Ship2.x - x;
  distance *= distance;
  distance += (Ship2.y - y) * (Ship2.y - y);
  distance /= 2;
  // Generating the vector
  strenght = BLOW_UP / (distance * distance);
  strenght = (1000000 ) * strenght;
  strenght *= -1.0;
  *dx += ((Ship2.x - x) / distance) * strenght;
  *dy += ((Ship2.y - y) / distance) * strenght;
}

// Blow up a ship when it's destroyed
void blow_up1(void){
  polygon *pointer, *first_position;
  struct dust *dust_pointer;
  first_position = pointer = Ship1.tank;
  Ship1.status = DEAD;
  Ship1.fuel = 0.0;
  gettimeofday(&(Ship1.time), NULL);
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship1.color);
    blow_up_strenght1(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 2, dust_pointer -> dy / 2, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 4, dust_pointer -> dy / 4, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 8, dust_pointer -> dy / 8, Ship1.color);
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship1.color);
    blow_up_strenght1(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 2, dust_pointer -> dy / 2, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 4, dust_pointer -> dy / 4, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 8, dust_pointer -> dy / 8, Ship1.color);
    pointer = pointer -> next;
  }while(pointer != first_position);

  first_position = pointer = Ship1.body;
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship1.color);
    blow_up_strenght1(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 2, dust_pointer -> dy / 2, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 4, dust_pointer -> dy / 4, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 8, dust_pointer -> dy / 8, Ship1.color);
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship1.color);
    blow_up_strenght1(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 2, dust_pointer -> dy / 2, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 4, dust_pointer -> dy / 4, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 8, dust_pointer -> dy / 8, Ship1.color);
    pointer = pointer -> next;
  }while(pointer != first_position);

  first_position = pointer = Ship1.cockpit;
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship1.color);
    blow_up_strenght1(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 2, dust_pointer -> dy / 2, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 4, dust_pointer -> dy / 4, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 8, dust_pointer -> dy / 8, Ship1.color);
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship1.color);
    blow_up_strenght1(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 2, dust_pointer -> dy / 2, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 4, dust_pointer -> dy / 4, Ship1.color);
    new_dust(dust_pointer -> shape -> x, dust_pointer -> shape -> y, dust_pointer -> dx / 8, dust_pointer -> dy / 8, Ship1.color);
    pointer = pointer -> next;
  }while(pointer != first_position);
}
void blow_up2(void){
  polygon *pointer, *first_position;
  struct dust *dust_pointer;
  first_position = pointer = Ship2.division;
  Ship2.status = DEAD;
  Ship2.fuel = 0.0;
  gettimeofday(&(Ship2.time), NULL);
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    pointer = pointer -> next;
  }while(pointer != first_position);

  first_position = pointer = Ship2.body;
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    pointer = pointer -> next;
  }while(pointer != first_position);

  first_position = pointer = Ship2.cockpit;
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    pointer = pointer -> next;
  }while(pointer != first_position);

  first_position = pointer = Ship2.left_wing;
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    pointer = pointer -> next;
  }while(pointer != first_position);

  first_position = pointer = Ship2.right_wing;
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    pointer = pointer -> next;
  }while(pointer != first_position);

  first_position = pointer = Ship2.right_prop;
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    pointer = pointer -> next;
  }while(pointer != first_position);

  first_position = pointer = Ship2.left_prop;
  do{
    dust_pointer = new_dust(pointer -> x, pointer -> y, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    dust_pointer = new_dust((pointer -> x + pointer -> previous -> x) / 2, (pointer -> y + pointer -> previous -> y) / 2, 0.0, 0.0, Ship2.color);
    blow_up_strenght2(dust_pointer -> shape -> x, dust_pointer -> shape -> y, &(dust_pointer -> dx), &(dust_pointer -> dy));
    pointer = pointer -> next;
  }while(pointer != first_position);

}

// Creates a new shot
struct shot *new_shot(float x, float y, float dx, float dy, unsigned color, int owner){
  struct shot *new_shot = (struct shot *) malloc(sizeof(struct shot));

  new_shot -> shape = new_circle(x, y, SHOT_SIZE);
  new_shot -> dx = dx;
  new_shot -> dy = dy;
  new_shot -> color = color;
  new_shot -> owner = owner;
  gettimeofday(&(new_shot -> time), NULL);
  new_shot -> previous = new_shot -> next = NULL;
  if(list_of_shot == NULL)
    list_of_shot = new_shot;
  else{
    struct shot *pointer = list_of_shot;
    while(pointer -> next != NULL)
      pointer = pointer -> next;
    pointer -> next = new_shot;
    new_shot -> previous = pointer;
  }
  return new_shot;
}

// Move a shot
void move_shot(int fps){
  struct shot *pointer = list_of_shot;
  while(pointer != NULL){
    pointer -> shape -> x += pointer -> dx / fps;
    pointer -> shape -> y += pointer -> dy / fps;
    pointer = pointer -> next;
  }
}

// If the shot is too old, destroy it
void clean_shot(void){
  struct shot *pointer = list_of_shot;
  struct shot *temp;
  struct timeval time;
  unsigned long elapsed_microseconds;

  while(pointer != NULL){
    gettimeofday(&time, NULL);
    elapsed_microseconds = (time.tv_usec - pointer -> time.tv_usec) +
      1000000 * (time.tv_sec - pointer -> time.tv_sec);
    if(elapsed_microseconds > SHOT_LIFE){
      if(pointer -> previous == NULL){ // It's the first
        if(pointer -> next == NULL){ // It's the only one
          free(pointer);
          list_of_shot = NULL;
          return;
        }
        else{ // The first of many
          pointer = pointer -> next;
          free(pointer -> previous);
          pointer -> previous = NULL;
          list_of_shot = pointer;
          continue;
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

// Erase a shot in the screen
void erase_shot(camera *space_camera){
  struct shot *pointer = list_of_shot;
  while(pointer != NULL){
    if(pointer -> shape -> x < 0.0)
      pointer -> shape -> x += 800.0;
    else if(pointer -> shape -> x > 800.0)
      pointer -> shape -> x -= 800.0;
    if(pointer -> shape -> y < 0.0)
      pointer -> shape -> y += 800.0;
    else if(pointer -> shape -> y > 800.0)
      pointer -> shape -> y -= 800.0;
    film_fullcircle(space_camera, pointer -> shape, BLACK);
    pointer = pointer -> next;
  }
}

// Films a shot with a camera
void film_shot(camera *space_camera){
  struct shot *pointer = list_of_shot;
  while(pointer != NULL){
    film_fullcircle(space_camera, pointer -> shape, pointer -> color);
    pointer = pointer -> next;
  }
}

// If a shot touched a star, destroy it
void burn_shot(void){
  struct shot *pointer = list_of_shot;
  struct shot *temp;
  while(pointer != NULL){
    if(collision_circle_circle(pointer -> shape, Star.shape)){
      if(pointer -> previous == NULL){ // It's the first
        if(pointer -> next == NULL){ // It's the only one
          free(pointer);
          list_of_shot = NULL;
          return;
        }
        else{ // The first of many
          pointer = pointer -> next;
          free(pointer -> previous);
          pointer -> previous = NULL;
          list_of_shot = pointer;
          continue;
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

// Apply the gravity in shots
void gravity_in_shot(int fps){
  struct shot *pointer = list_of_shot;
  while(pointer != NULL){
    star_gravity(pointer -> shape -> x, pointer -> shape -> y, &(pointer -> dx), &(pointer -> dy), fps);
    pointer = pointer -> next;
  }
}

// Determines if a ship was shot by an enemy
int got_shot1(void){
  struct shot *pointer = list_of_shot;
  while(pointer != NULL){
    if(pointer -> owner != 1)
      if(collision_circle_polygon(pointer -> shape, Ship1.body) ||
         collision_circle_polygon(pointer -> shape, Ship1.tank) ||
         collision_circle_polygon(pointer -> shape, Ship1.cockpit))
        return 1;
    pointer = pointer -> next;
  }
  return 0;
}
int got_shot2(void){
  struct shot *pointer = list_of_shot;
  while(pointer != NULL){
    if(pointer -> owner != 2)
      if(collision_circle_polygon(pointer -> shape, Ship2.body) ||
         collision_circle_polygon(pointer -> shape, Ship2.division) ||
         collision_circle_polygon(pointer -> shape, Ship2.cockpit) ||
         collision_circle_polygon(pointer -> shape, Ship2.left_wing) ||
         collision_circle_polygon(pointer -> shape, Ship2.right_wing) ||
         collision_circle_polygon(pointer -> shape, Ship2.right_prop) ||
         collision_circle_polygon(pointer -> shape, Ship2.left_prop))
        return 1;
    pointer = pointer -> next;
  }
  return 0;
}

// Launches a ship to the hyperspace
void goto_hyperspace1(void){
  float dx, dy, temp;
  Ship1.status = HYPERSPACE;
  for(temp = 1.0; temp < 10.0; temp *= 2.0){
    new_dust(Ship1.x, Ship1.y, temp, 0.0, YELLOW);
    new_dust(Ship1.x, Ship1.y, temp, temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, 0.0, temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, -temp, 0.0, YELLOW);
    new_dust(Ship1.x, Ship1.y, -temp, -temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, 0.0, -temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, temp, -temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, -temp, temp, YELLOW);
  }

  dx = (float) (rand() % 400);
  dy = (float) (rand() % 400);
  gettimeofday(&(Ship1.time), NULL);
  move_ship1(dx, dy);
  if(Ship1.dy > 1000.0)
    Ship1.dy = 1000.0;
  if(Ship1.dx > 1000.0)
    Ship1.dx = 1000.0;
}
void goto_hyperspace2(void){
  float dx, dy, temp;
  Ship2.status = HYPERSPACE;
  for(temp = 1.0; temp < 10.0; temp *= 2.0){
    new_dust(Ship2.x, Ship2.y, temp, 0.0, YELLOW);
    new_dust(Ship2.x, Ship2.y, temp, temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, 0.0, temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, -temp, 0.0, YELLOW);
    new_dust(Ship2.x, Ship2.y, -temp, -temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, 0.0, -temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, temp, -temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, -temp, temp, YELLOW);
  }

  dx = (float) (rand() % 400);
  dy = (float) (rand() % 400);
  gettimeofday(&(Ship2.time), NULL);
  move_ship2(dx, dy);
  if(Ship2.dy > 1000.0)
    Ship2.dy = 1000.0;
  if(Ship2.dx > 1000.0)
    Ship2.dx = 1000.0;
}

// Puts a ship in the normal space
void return_from_hyperspace1(void){
  float temp;
  Ship1.status = ALIVE;
  for(temp = 1.0; temp < 10.0; temp *= 2.0){
    new_dust(Ship1.x, Ship1.y, temp, 0.0, YELLOW);
    new_dust(Ship1.x, Ship1.y, temp, temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, 0.0, temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, -temp, 0.0, YELLOW);
    new_dust(Ship1.x, Ship1.y, -temp, -temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, 0.0, -temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, temp, -temp, YELLOW);
    new_dust(Ship1.x, Ship1.y, -temp, temp, YELLOW);
  }
  gettimeofday(&(Ship1.time), NULL);
}

void return_from_hyperspace2(void){
  float temp;
  Ship2.status = ALIVE;
  for(temp = 1.0; temp < 10.0; temp *= 2.0){
    new_dust(Ship2.x, Ship2.y, temp, 0.0, YELLOW);
    new_dust(Ship2.x, Ship2.y, temp, temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, 0.0, temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, -temp, 0.0, YELLOW);
    new_dust(Ship2.x, Ship2.y, -temp, -temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, 0.0, -temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, temp, -temp, YELLOW);
    new_dust(Ship2.x, Ship2.y, -temp, temp, YELLOW);
  }
  gettimeofday(&(Ship2.time), NULL);
}

// Free all the memory used by shots
void destroy_shot(void){
  struct shot *pointer = list_of_shot;
  while(pointer != NULL){
    if(pointer -> next != NULL){
      pointer = pointer -> next;
      free(pointer -> previous);
      pointer -> previous = NULL;
    }
    else{
      free(pointer);
      pointer = list_of_shot = NULL;
    }
  }
}
