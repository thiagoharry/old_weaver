/*
  Copyright (C) 2009 Thiago Leucz Astrizi

 This file is part of Weaver API.

 Weaver API is free software: you can redistribute it and/or modify
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

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

// Macro functions
#define clean_keyboard() initialize_keyboard()

// Number of keys watched
#define KEYS 28

// Position to each key
#define UP          10
#define RIGHT       11
#define DOWN        12
#define LEFT        13
#define PLUS        14
#define MINUS       15
#define SHIFT       16
#define CTRL        17
#define ESC         18
#define A           19
#define S           20
#define D           21
#define W           22
#define ENTER       23
#define LEFT_CTRL   24
#define RIGHT_CTRL  25
#define F1          26
#define ANY         27

int keyboard[KEYS];

struct{
  int pressed;
  int changed;
  int x, y;
} mouse;

void initialize_keyboard(void);
void get_input(void);
void initialize_mouse(void);

#endif
