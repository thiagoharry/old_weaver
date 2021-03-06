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

#ifndef _PADDLE_H_
#define _PADDLE_H_

#include "display.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "keyboard.h"
#include "sound.h"
#include "image.h"

// Macro functions
#define new_camera(x, y, width, height) new_vector4(width, x, y, height)
#define new_camera_w(x, y, width) new_vector4(width, x, y, width * ((float) window_height / (float) window_width))
#define new_camera_h(x, y, height) new_vector4(height * ((float) window_width / (float) window_height), x, y, height)
#define new_circle(x, y, z) new_vector3(x, y, z)
#define new_rectangle(x, y, w, z) new_vector4(w, x, y, z)
#define film_circle(x, y, z) film_vector3_circle(x, y, z)
#define film_fullcircle(x, y, z) film_vector3_fullcircle(x, y, z)

// Functions
void awake_the_weaver(void);
void may_the_weaver_sleep(void);
void weaver_rest(long);

void center_camera(struct vector4 *, float, float);
void limit_camera(struct vector4 *, int, int, int, int);

void film_vector3_circle(struct vector4 *, struct vector3 *, unsigned);
void film_vector3_fullcircle(struct vector4 *, struct vector3 *, unsigned);
void film_rectangle(struct vector4 *, struct vector4 *, unsigned);
void film_fullrectangle(struct vector4 *, struct vector4 *,unsigned);


void film_polygon(struct vector4 *, struct vector2 *, unsigned);
void film_fullpolygon(struct vector4 *, struct vector2 *, unsigned);

int collision_circle_circle(struct vector3 *, struct vector3 *);
int collision_circle_polygon(struct vector3 *, struct vector2 *);
int collision_polygon_polygon(struct vector2 *, struct vector2 *);
int collision_rectangle_circle(struct vector4 *, struct vector3 *);

void move_polygon_v(struct vector2 *, float); 

struct timeval _b_frame, current_time;
int fps;

typedef struct vector2 polygon;
typedef struct vector3 circle;
typedef struct vector4 camera;
typedef struct vector4 rectangle;

#endif
