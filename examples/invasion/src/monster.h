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

#ifndef _MONSTER_H_
#define _MONSTER_H_

// To what direction the monster is looking?
#define NORTH  0
#define SOUTH  1
#define WEST   2
#define EAST   3

// Monster species
#define MONSTER_SPECIES 1
#define NORMAL 0

struct monster{
  int specie;
  int max_health, health;
  int speed;       // Pixels per second
  int gold;        // The ammount of coins that it holds
  int direction;   // To where is it looking?
  int x, y;        // monster coordinates
  int cx, cy;      // Centi-pixels
  int target;      // To where the monster is going?
  int sucess;      // The monster arrived in the destiny?
  int need_redraw; // It needs to be redraw?
  int distance;    // How distant is the monster from its target?
  struct monster *next, *previous;
};

struct queue_node{
  int value;
  struct queue_node *next, *previous;
};

struct{
  int size;
  struct queue_node *first, *last;
}queue;

struct monster *new_monster(int, int, int, int);
int move_monsters(void);
int distance(int, int);
void initialize_queue(void);
struct queue_node *enqueue(int);
int dequeue(void);
void clean_queue(void);
int position_from_coordinate(int, int);
void move_monster(struct monster *, int direction);
void clean_monsters(void);
int exist_monster_in_position(int);
void draw_monsters(void);

#endif
