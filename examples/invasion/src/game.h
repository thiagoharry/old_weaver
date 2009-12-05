/*
 This file is part of Tower Defense.

 Tower Defense is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Foobar is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with Weaver API.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _GAME_H_
#define _GAME_H_

#include "weaver/weaver.h"
#include "monster.h"


#define NUMBER_OF_HOUSES 5

// The game types
#define CENTER_TARGET 0
#define EASY_TOP      1
#define EASY_SIDE     2
#define DEFAULT       3

// How data is stored in the map:
// We use currently 7 bits.
// The last 4 bits is the code that represents the kind of tower that
// it's in that position. The codes are:
// 0000: No tower
// 0001: A wall. Indestructible.


struct{
  // This stores our board size and position
  struct{
    int x, y;
    int width, height;
  } board;
  // This is information about the monsters
  struct{
    struct surface *images[MONSTER_SPECIES];
    struct monster *list;
  } monsters;
  // This is information about all the towers
  struct{
    struct tower *list;
    int number;
  } towers;
  // And about the player. Money and life.
  struct{
    int life, life_max;
    int value;
  } player;
  // And finnaly about the cursor
  struct{
    int current_position;
    int last_position;
    int green;
  } cursor;
  struct surface *background;
  int type;
  int top_holes, left_holes;
  int down_targets, right_targets;
  int level;
  unsigned *map; // The current configuration
  int fps;
  camera *cam;
 } game;

void initialize_game(void);
void finish_game(void);
void draw_city(void);
void draw_trees(void);
void draw_wall(void);
void draw_money(struct surface *);
void draw_life(struct surface *);
void clean_monster_bit(void);
void print_board(void);
int position_from_coordinate(int, int);

#endif
