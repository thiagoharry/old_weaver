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


#ifndef _SPACEWAR_H_
#define _SPACEWAR_H_

#include "weaver/weaver.h"
#include "star.h"
#include "ship1.h"
#include "dust.h"
#include "ai.h"

// Some colors
#define BLUE_SHOT 0xaaaaff
#define RED_SHOT  0xffaaaa

#define draw_ship1(color) \
  film_ship1(camera00, color);\
  film_ship1(camera11, color);\
  film_ship1(camera01, color);\
  film_ship1(camera02, color);\
  film_ship1(camera10, color);\
  film_ship1(camera12, color);\
  film_ship1(camera20, color);\
  film_ship1(camera21, color);\
  film_ship1(camera22, color)

#define draw_ship2(color) \
  film_ship2(camera00, color);\
  film_ship2(camera11, color);\
  film_ship2(camera01, color);\
  film_ship2(camera02, color);\
  film_ship2(camera10, color);\
  film_ship2(camera12, color);\
  film_ship2(camera20, color);\
  film_ship2(camera21, color);\
  film_ship2(camera22, color)

int game(int);
int intro(void);
int controls(void);

#endif
