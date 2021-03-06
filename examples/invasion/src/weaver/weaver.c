/*
  Copyright (C) 2009 Thiago Leucz Astrizi

 This file is part of Weaver API.

 Weaver API is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Weaver API is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with Weaver API.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include "weaver.h"

// This function initializes everything
void awake_the_weaver(void){
  initialize_screen();
  initialize_keyboard();
  initialize_mouse();
  //initialize_sound();
  srand(time(NULL));
  gettimeofday(&current_time, NULL);
}

void may_the_weaver_sleep(void){
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

// This function pauses the program for the number of nanoseconds
// passed as argumment
void weaver_rest(long nanoseconds){
  struct timespec req = {0, nanoseconds}; 
  nanosleep(&req, NULL);
  _b_frame.tv_sec = current_time.tv_sec;
  _b_frame.tv_usec = current_time.tv_usec;
  gettimeofday(&current_time, NULL);
  fps = 1000000 / (1000000 * (current_time.tv_sec - _b_frame.tv_sec) + current_time.tv_usec - _b_frame.tv_usec);
}


// This centers a camera represented by a struct vector4
void center_camera(struct vector4 *camera, float x, float y){
  camera -> x = x - (camera -> w / 2);
  camera -> y = y - (camera -> z / 2);
}

// This function limits the area where a camera can draw things in the screen
void limit_camera(struct vector4 *camera, int x, int y, int width, int height){
  camera -> previous = (struct vector4 *) (long) x;
  camera -> top = (struct vector4 *) (long) y;
  camera -> next = (struct vector4 *) (long) width;
  camera -> down = (struct vector4 *) (long) height;
}

// This shows a rectangle (or square) in the area covered by the camera
void film_rectangle(struct vector4 *camera, struct vector4 *rectangle, unsigned color){
  int x, y, height, width;
  x = (int) (((rectangle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((rectangle -> y - camera -> y) / camera -> z) * window_height);
  height = (int) ((rectangle -> z / camera -> z) * window_height);
  width = (int) ((rectangle -> w / camera -> w) * window_width);
  draw_rectangle(x, y, width, height, color);
}

void film_fullrectangle(struct vector4 *camera, struct vector4 *rectangle, unsigned color){
  int x, y, height, width;
  x = (int) (((rectangle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((rectangle -> y - camera -> y) / camera -> z) * window_height);
  height = (int) ((rectangle -> z / camera -> z) * window_height);
  width = (int) ((rectangle -> w / camera -> w) * window_width);
  fill_rectangle(x, y,width, height, color);
}


// This shows a circle in the screen if it's in the area covered by the camera
void film_vector3_circle(struct vector4 *camera, struct vector3 *circle, unsigned color){
  int x, y, height, width, limited_camera;
  limited_camera = 0;

  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL && camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / (float) window_width) + (long) (camera -> previous);
    width = (int) ((float) width * ((float) (long) camera -> next) / (float) window_width);
  }
  if(camera -> top != NULL && camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / (float) window_height) + (long) (camera -> top);
    height = (int) ((float) height * ((float) (long) camera -> down) / (float) window_height);
  }
  if(limited_camera){
    if(x < (long) camera -> previous + (long) camera -> next && x > (long) camera -> previous &&
       y < (long) camera -> top + (long) camera -> down && y > (long) camera -> top)
      draw_ellipse(x, y, width, height, color);
  }
  else
    draw_ellipse(x, y, width, height, color);
}

// This draws a full circle in the screen if it's in the area covered by the camera
void film_vector3_fullcircle(struct vector4 *camera, struct vector3 *circle, unsigned color){
  int x, y, height, width, limited_camera;
  limited_camera = 0;

  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL && camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / (float) window_width) + (long) (camera -> previous);
    width = (int) ((float) width * ((float) (long) camera -> next) / (float) window_width);
  }
  if(camera -> top != NULL && camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / (float) window_height) + (long) (camera -> top);
    height = (int) ((float) height * ((float) (long) camera -> down) / (float) window_height);
  }
  if(limited_camera){
    if(x < (long) camera -> previous + (long) camera -> next && x > (long) camera -> previous &&
       y < (long) camera -> top + (long) camera -> down && y > (long) camera -> top)
      fill_ellipse(x, y, width, height, color);
  }
  else
    fill_ellipse(x, y, width, height, color);
}

// This films a full polygon
void film_fullpolygon(struct vector4 *camera, struct vector2 *polygon, unsigned color){
  struct vector2 *current_vertex = polygon;
  XPoint *points;
  int number_of_points = 0;
  
  if(polygon == NULL || polygon -> next == polygon)
    return;

  // Discovering the number of points
  do{
    number_of_points ++;
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);

  // Allocating space for the XPoints
  points = (XPoint *) malloc(sizeof(XPoint) * number_of_points);
  //Getting thew points coordinates
  number_of_points = 0;
  do{
    points[number_of_points].x =  (int) (((current_vertex -> x - camera -> x) / camera -> w) * window_width);
    points[number_of_points].y =  (int) (((current_vertex -> y - camera -> y) / camera -> z) * window_height);
    number_of_points ++;
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);

  XSetForeground(_dpy, _gc, color);
  XFillPolygon(_dpy, _w, _gc, points, number_of_points, Convex, CoordModeOrigin);
  free(points);
}

// This films an empty polygon
/// WARNING: This is spaghety code that ran out of control. It must be rewritten with
/// modularity in mind...    
void film_polygon(struct vector4 *camera, struct vector2 *polygon, unsigned color){
  struct vector2 *current_vertex = polygon;
  struct vector2 *next;
  int x1, y1, x2, y2;
  int limited_camera;

  // First we handle the degenerate cases
  // This is a "Empty Polygon". Don't draw anything.
  if(polygon == NULL)
    return;

  // This is a henagon. A polygon with only one vertex
  // They can be used as particles, so it's usefull to draw them
  if(polygon -> next == polygon){
    int limited_camera =0;
    x1 = (int) (((polygon -> x - camera -> x) / camera -> w) * window_width);
    y1 = (int) (((polygon -> y - camera -> y) / camera -> z) * window_height);
    // If our camera is limited, we need some more calculations
    if(camera -> previous != NULL && camera -> next != NULL){
      limited_camera = 1;
      x1 = (int) ((float) x1 * ((float) (long) camera -> next) / (float) window_width) + (long) (camera -> previous);
    }
    if(camera -> top != NULL && camera -> down != NULL){
      limited_camera = 1;
      y1 = (int) ((float) y1 * ((float) (long) camera -> down) / (float) window_height) + (long) (camera -> top);
    }
    if((!limited_camera) || (x1 < (long) camera -> previous + (long) camera -> next && x1 > (long) camera -> previous &&
			     y1 < (long) camera -> top + (long) camera -> down && y1 > (long) camera -> top))
      draw_point(x1, y1, color);
    // We draw the point only if it's inside the camera limits
    return;
  }  

  // Now the usual case. We have a polygon with more than one vertex.
  do{
    next = current_vertex -> next;
    limited_camera = 0;

    x1 = (int) (((current_vertex -> x - camera -> x) / camera -> w) * window_width);
    y1 = (int) (((current_vertex -> y - camera -> y) / camera -> z) * window_height);
    x2 = (int) (((next -> x - camera -> x) / camera -> w) * window_width);
    y2 = (int) (((next -> y - camera -> y) / camera -> z) * window_height);

    if(camera -> previous != NULL && camera -> next != NULL){
      x1 = (int) ((float) x1 * ((float) (long) camera -> next) / (float) window_width) + (long) (camera -> previous);
      x2 = (int) ((float) x2 * ((float) (long) camera -> next) / (float) window_width) + (long) (camera -> previous);
      limited_camera ++;
    }
    if(camera -> top != NULL && camera -> down != NULL){
      y1 = (int) ((float) y1 * ((float) (long) camera -> down) / (float) window_height) + (long) (camera -> top);
      y2 = (int) ((float) y2 * ((float) (long) camera -> down) / (float) window_height) + (long) (camera -> top);
      limited_camera ++;
    }

    // In fact, we must handle 4 cases: The 2 dots are inside the limit, the 2 are outside, only the first is
    // inside or only the second is inside.
    if(limited_camera){
      if((x1 < (long) camera -> previous + (long) camera -> next && x1 > (long) camera -> previous &&
	  y1 < (long) camera -> top + (long) camera -> down && y1 > (long) camera -> top)){
	// The first dot is inside!
	if(x2 < (long) camera -> previous + (long) camera -> next && x2 > (long) camera -> previous &&
	   y2 < (long) camera -> top + (long) camera -> down && y2 > (long) camera -> top){
	  // The 2 dots are inside!
	  draw_line(x1, y1, x2, y2, color);
	}
	else{
	  // (x2, y2) is outside
	  if(x2 > (long) camera -> previous + (long) camera -> next){ // x2 is far east
	    float a = ((float) (y2 - y1)) / ((float) (x2 - x1)); // We already know that x2 != x1
	    float b = y1 - a * x1;
	    x2 = (int) ((long) camera -> previous + (long) camera -> next);
	    y2 = a * (float) x2 + b;
	    if(y2 > (long) camera -> top && y2 < (long) camera -> top + (long) camera -> down)
	      draw_line(x1, y1, x2, y2, color);
	    else{
	      if(y2 <= (long) camera -> top){
		y2 = (long) camera -> top;
		if(a != 0.0){
		  x2 = (y2 - b) / a;
		  draw_line(x1, y1, x2, y2, color);
		}
	      }
	      else if(y2 >= (long) camera -> top + (long) camera -> down){
		y2 = (long) camera -> top + (long) camera -> down;
		if(a != 0.0){
		  x2 = (y2 - b) / a;
		  draw_line(x1, y1, x2, y2, color);
		}
	      }
	    } // End of else
	  }
	  else if(x2 < (long) camera -> previous){ // x2 is far west
	    float a = ((float) (y2 - y1)) / ((float) (x2 - x1)); // We already know that x2 != x1
	    float b = y1 - a * x1;
	    x2 = (int) (long) camera -> previous;
	    y2 = a * (float) x2 + b;
	    if(y2 > (long) camera -> top && y2 < (long) camera -> top + (long) camera -> down)
	      draw_line(x1, y1, x2, y2, color);
	    else{
	      if(y2 <= (long) camera -> top){
                y2 = (long) camera -> top;
                if(a != 0.0){
                  x2 = (y2 - b) / a;
                  draw_line(x1, y1, x2, y2, color);
                }
              }
              else if(y2 >= (long) camera -> top + (long) camera -> down){
                y2 = (long) camera -> top + (long) camera -> down;
                if(a != 0.0){
                  x2 = (y2 - b) / a;
                  draw_line(x1, y1, x2, y2, color);
                }
              }	      
	    }
	  }
	  else{ // x2 is not outside the screen. But y2 is
	    float a = ((float) (y2 - y1)) / ((float) (x2 - x1)); // We already know that x2 != x1
	    float b = (float) y1 - a * (float) x1;
	    if(y2 <= (long) camera -> top){
	      y2 = (long) camera -> top;
	      if(a != 0.0){
		x2 = (y2 - b) / a;
		if(x2 > (long) camera -> previous && x2 < (long) camera -> previous + (long) camera -> next)
		  draw_line(x1, y1, x2, y2, color);
	      }
	    }
	    else if(y2 >= (long) camera -> top + (long) camera -> down){
	      y2 = (long) camera -> top + (long) camera -> down;
	      if(a != 0.0){
		x2 = (int) ((float) y2 - b) / a;
		if(x2 > (long) camera -> previous && x2 < (long) camera -> previous + (long) camera -> next)
		  draw_line(x1, y1, x2, y2, color);
	      }
	    }

	  }
	}
      }
      else{
	// The first dot is outside.
	if(x2 < (long) camera -> previous + (long) camera -> next && x2 > (long) camera -> previous &&
           y2 < (long) camera -> top + (long) camera -> down && y2 > (long) camera -> top){
	  // Only the second dot is inside
	  // (x1, y1) is outside
	  if(x1 > (long) camera -> previous + (long) camera -> next){ // x1 is far east
	    float a = ((float) (y1 - y2)) / ((float) (x1 - x2)); // We already know that x2 != x1
	    float b = y2 - a * x2;
	    x1 = (int) ((long) camera -> previous + (long) camera -> next);
	    y1 = a * (float) x1 + b;
	    if(y1 > (long) camera -> top && y1 < (long) camera -> top + (long) camera -> down)
	      draw_line(x1, y1, x2, y2, color);
	    else{
	      if(y1 <= (long) camera -> top){
		y1 = (long) camera -> top;
		if(a != 0.0){
		  x1 = (y1 - b) / a;
		  draw_line(x1, y1, x2, y2, color);
		}
	      }
	      else if(y1 >= (long) camera -> top + (long) camera -> down){
		y1 = (long) camera -> top + (long) camera -> down;
		if(a != 0.0){
		  x1 = (y1 - b) / a;
		  draw_line(x1, y1, x2, y2, color);
		}
	      }
	    } // End of else
	  }
	  else if(x1 < (long) camera -> previous){ // x2 is far west
	    float a = ((float) (y1 - y2)) / ((float) (x1 - x2)); // We already know that x2 != x1
	    float b = y2 - a * x2;
	    x1 = (int) (long) camera -> previous;
	    y1 = a * (float) x1 + b;
	    if(y1 > (long) camera -> top && y1 < (long) camera -> top + (long) camera -> down)
	      draw_line(x1, y1, x2, y2, color);
	    else{
	      if(y1 <= (long) camera -> top){
                y1 = (long) camera -> top;
                if(a != 0.0){
                  x1 = (y1 - b) / a;
                  draw_line(x1, y1, x2, y2, color);
                }
              }
              else if(y1 >= (long) camera -> top + (long) camera -> down){
                y1 = (long) camera -> top + (long) camera -> down;
                if(a != 0.0){
                  x1 = (y1 - b) / a;
                  draw_line(x1, y1, x2, y2, color);
                }
              }	      
	    }
	  }
	  else{ // x1 is not outside the screen. But y1 is
	    float a = ((float) (y1 - y2)) / ((float) (x1 - x2)); // We already know that x2 != x1
	    float b = y2 - a * x2;
	    if(y1 <= (long) camera -> top){
	      y1 = (long) camera -> top;
	      if(a != 0.0){
		x1 = (y1 - b) / a;
		if(x1 > (long) camera -> previous && x1 < (long) camera -> previous + (long) camera -> next)
		  draw_line(x1, y1, x2, y2, color);
	      }
	    }
	    else if(y1 >= (long) camera -> top + (long) camera -> down){
	      y1 = (long) camera -> top + (long) camera -> down;
	      if(a != 0.0){
		x1 = (y1 - b) / a;
		if(x1 > (long) camera -> previous && x1 < (long) camera -> previous + (long) camera -> next)
		  draw_line(x1, y1, x2, y2, color);
	      }
	    }
	  }
	}
	// Else the 2 are outside and there's nothing to do.
      }
    }
    else
      // We don't have a limited camera. Much easier!
      draw_line(x1, y1, x2, y2, color);
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
}

// This detects collision between a rectangle and a circle
int collision_rectangle_circle(struct vector4 *rectangle, struct vector3 *circle){
  // They collide in the X-axis?
  if(!((circle -> x + circle -> z < rectangle -> x && circle -> x - circle -> z < rectangle -> x &&
     circle -> x + circle -> z < rectangle -> x + rectangle -> w &&
      circle -> x - circle -> z < rectangle -> x + rectangle -> w) ||
     (circle -> x + circle -> z > rectangle -> x && circle -> x - circle -> z > rectangle -> x &&
     circle -> x + circle -> z > rectangle -> x + rectangle -> w &&
      circle -> x - circle -> z > rectangle -> x + rectangle -> w)))
    if(!((circle -> y + circle -> z < rectangle -> y && circle -> y - circle -> z < rectangle -> y &&
     circle -> y + circle -> z < rectangle -> y + rectangle -> z &&
	  circle -> y - circle -> z < rectangle -> y + rectangle -> z) ||
	 (circle -> y + circle -> z > rectangle -> y && circle -> y - circle -> z > rectangle -> y &&
     circle -> y + circle -> z > rectangle -> y + rectangle -> z &&
	  circle -> y - circle -> z > rectangle -> y + rectangle -> z)))
      return 1;
  

  // By default, if no collision were detected, no collision happened
  return 0;
}



// This function detects collisions between two circles. If they are colliding,
// it returns 1. Else, it returns 0.
int collision_circle_circle(struct vector3 *circle1, struct vector3 *circle2){
  float dx = circle2 -> x - circle1 -> x;
  float dy = circle2 -> y - circle1 -> y;
  float distance = dx * dx + dy * dy;
  float minimum = circle1 -> z + circle2 -> z;
  
  minimum *= minimum;
  if(distance < minimum)
    return 1;
  else
    return 0;
}

// This determines if a circle and a polygon are colliding.
int collision_circle_polygon(struct vector3 *circle, struct vector2 *polygon){
  struct vector2 *current, *next, *first;
  float a, b, dist;
  
  current = first = polygon;
  next = current -> next;
  
  if(next == current){ // We have a degenerate polygon with only one vertex
    dist = (current -> x - circle -> x);
    dist *= dist; // dx * dx
    dist += (current -> y - circle -> y) * (current -> y - circle -> y);
    if(sqrt(dist) < circle -> z)
      return 1;
    else
      return 0;    
  }
  
  do{
    // If it's not a vertical segment:
    if(current -> x != next -> x){
      a = (next -> y - current -> y) / (next -> x - current -> x);
      b = current -> y - a * current -> x;
      // Rect equation: ax + b = y
      dist = circle -> x * a - circle -> y + b;
      if(dist < 0.0)
        dist *= -1;
      dist /= sqrt(a * a + 1);
      // distance between rect and  point computed
      if(dist < circle -> z){
	// A collision happened or not
	if(!((circle -> x + circle -> z < current -> x && circle -> x + circle -> z < next-> x &&
              circle -> x - circle -> z < current -> x && circle -> x - circle -> z < next-> x) ||
             (circle -> x + circle -> z > current -> x && circle -> x + circle -> z > next-> x &&
              circle -> x - circle -> z > current -> x && circle -> x - circle -> z > next-> x))){
	  if(!((circle -> y + circle -> z < current -> y && circle -> y + circle -> z < next-> y &&
		circle -> y - circle -> z < current -> y && circle -> y - circle -> z < next-> y) ||
	       (circle -> y + circle -> z > current -> y && circle -> y + circle -> z > next-> y &&
		circle -> y - circle -> z > current -> y && circle -> y - circle -> z > next-> y))){
	    return 1;
	  }
        }
      }
    }
    else{ // We have a vertical segment
      if(!((circle -> x - circle -> z < current -> x && circle -> x + circle -> z < current -> x) ||
           (circle -> x - circle -> z > current -> x && circle -> x + circle -> z > current -> x))){
	// A collision happened... or not?
	if(!((circle -> y + circle -> z < current -> y && circle -> y + circle -> z < next-> y &&
              circle -> y - circle -> z < current -> y && circle -> y - circle -> z < next-> y) ||
             (circle -> y + circle -> z > current -> y && circle -> y + circle -> z > next-> y &&
	      circle -> y - circle -> z > current -> y && circle -> y - circle -> z > next-> y))){
          return 1; // COLLISION!
	}
      }
    }
    current = next;
    next = current -> next;
  }while(current != first);
  
  return 0;
}

// This determines collisions between 2 polygons
int collision_polygon_polygon(struct vector2 *poly1, struct vector2 *poly2){
  struct vector2 *first, *current, *next;
  struct vector2 *first2, *current2, *next2;

  current = first = poly1;
  next = current -> next;
  current2 = first2 = poly2;
  next2 = current2 -> next;

  if(current == next){ // This polygon is a henagon
    if(current2 == next2){ // We have 2 henagons!
      if(current -> x == current2 -> x &&
         current -> y == current2 -> y)
        return 1;
      else
        return 0;
    }
    else{ // The second polygon is not a henagon
      float a, b; // ax + b = y
      do{
        a = (next2 -> y - current2 -> y) / (next2 -> x - current2 -> x);
        b = current2 -> y - a * current2 -> x;
        if(a * current -> x + b == current -> y)
          return 1; // Detected a collision (unlikely to happen with henagons).
        current2 = next2;
        next2 = current2 -> next;
      }while(current2 != first2);
    }
  }
  else{ // The first polygon is not a henagon
    if(current2 == next2){ // But in this case, the second is
      float a, b;
      do{
        a = (next -> y - current -> y) / (next -> x - current -> x);
        b = current -> y - a * current -> x;
        if(a * current2 -> x + b == current2 -> y)
          return 1; // Collision detected.
        current = next;
        next = current -> next;
      }while(current != first);
    }
    else{ // The most common case. No henagons
      float a1, a2, b1, b2, x, y;
      a1 = a2 = b1 = b2 = y = 0.0;

      do{
        do{
          if(next -> x != current -> x){ // The first line is not vertical
            if(next2 -> x != current2 -> x){ // The second line is not vertical
              a1 = (next -> y - current -> y) / (next -> x - current -> x);
              b1 = current -> y - a1 * current -> x;
              a2 = (next2 -> y - current2 -> y) / (next2 -> x - current2 -> x);
              b2 = current2 -> y - a2 * current2 -> x;
              if(a1 != a2){ // Not paralells
                x = (b2 - b1) / (a1 - a2);
                if(!((x < current -> x && x < next -> x) ||
                     (x > current -> x && x > next -> x)))
                  return 1; // Collision !
              }
              else if(b1 == b2)
                return 1;
            }
            else{ // Only the second line is vertical
              y = a1 * current2 -> x + b1;
              if(!((y < current -> y && y < next -> y) ||
                   (y > current -> y && y > next -> y)))
                return 1; // Collision !
            }
          }
          else{ // The first line is vertical
            if(next2 -> x != current2 -> x){ // But the second is not
              y = a2 * current -> x + b2;
              if(!((y < current2 -> y && y < next2 -> y) ||
                   (y > current2 -> y && y > next2 -> y)))
                return 1; // Other collision case.
            }
            else{ // The 2 lines are vertical
              if(current -> y == current2 -> y)
                return 1;
            }
          }
        }while(current2 != first2);
        current = next;
        next = current -> next;
      }while(current != first);
    }
  }

  return 0;
}

// This moves a polygon vertically
void move_polygon_v(struct vector2 *polygon, float distance){
  struct vector2 *current, *first;
  current = first = polygon;
  do{
    current -> y -= distance;
    current = current -> next;
  }while(current != first);
}
