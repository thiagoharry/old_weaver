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

#ifndef _SHIP1_H_
#define _SHIP1_H_

#include <math.h>
#include "weaver/weaver.h"
#include "dust.h"

//Ship acceleration
#define SHIP_THRUST 50.0
// Ship angular speed
#define SHIP_TURN M_PI / 2
// The strenght of explosion when the ship is destroyed
#define BLOW_UP 100.0
// A ship status
#define DEAD       0
#define ALIVE      1
#define HYPERSPACE 2
// How much fuel a ship has
#define FUEL 1650.0
// How much microseconds a shot exists
#define SHOT_LIFE 2500000
// The shot size (radius)
#define SHOT_SIZE 3.0
// How much time it takes to produce a shot (microseconds)
#define SHOT_PRODUCTION 500000
// How much fuel a travel to hyperspace coasts?
#define HYPER_COAST 165.0
// How much time in microseconds it takes to go to hyperspace
#define HYPER_TIME 1000000

// This is the first spaceship
struct ship1{
  polygon *tank;
  polygon *body;
  polygon *cockpit;
  polygon *front;    // Only a pointer to the ship's front
  unsigned color;
  float x, y;       // The gravity center
  float dx, dy;     // The movement vector
  int status;
  struct timeval time;
  float fuel;
} Ship1;

struct ship2{
  polygon *cockpit;
  polygon *division;
  polygon *body;
  polygon *right_wing;
  polygon *left_wing;
  polygon *right_prop;
  polygon *left_prop;
  polygon *front;
  unsigned color;
  float x, y;
  float dx, dy;
  int status;
  struct timeval time;
  float fuel;
} Ship2;

struct shot{
  circle *shape;
  float dx, dy;
  struct timeval time;
  struct shot *next, *previous;
  unsigned color;
  int owner;
};

struct shot *list_of_shot;

void initialize_ship1(void);
void destroy_ship1(void);
void initialize_ship2(void);
void destroy_ship2(void);
void film_ship1(camera *, unsigned);
void film_ship2(camera *, unsigned);
void move_ship1(float, float);
void move_ship2(float, float);
void blow_up_strenght1(float, float, float *, float *);
void blow_up_strenght2(float, float, float *, float *);
void blow_up1(void);
void blow_up2(void);
struct shot *new_shot(float, float, float, float, unsigned, int); 
void move_shot(int fps);
void clean_shot(void);
void erase_shot(camera *);
void film_shot(camera *);
void burn_shot(void);
int got_shot1(void);
int got_shot2(void);
void gravity_in_shot(int);
void goto_hyperspace1(void);
void goto_hyperspace2(void);
void return_from_hyperspace1(void);
void return_from_hyperspace2(void);
void destroy_shot(void);

#endif
