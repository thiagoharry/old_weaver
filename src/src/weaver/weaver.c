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
#include <float.h>
#include <signal.h>
#include <limits.h>
#include "weaver.h"
#include "display.h"

// This function initializes everything
void awake_the_weaver(void){
  _initialize_screen();
  _initialize_keyboard();
  _initialize_mouse();
  //initialize_sound();
  //_initialize_font();
  srand(time(NULL));
  gettimeofday(&current_time, NULL);
  fps = 50;
  _music = 0;
}

void may_the_weaver_sleep(void){
  stop_music();
  // It's causing a segmentation fault in some systems...
  // Why?
  //png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

// This function pauses the program for the number of nanoseconds
// passed as argumment
void weaver_rest(long nanoseconds){
  XdbeSwapInfo info;
  info.swap_window = _w;
  info.swap_action = XdbeCopied;
  struct timespec req = {0, nanoseconds};
  XFlush(_dpy);
  nanosleep(&req, NULL);
  _b_frame.tv_sec = current_time.tv_sec;
  _b_frame.tv_usec = current_time.tv_usec;
  gettimeofday(&current_time, NULL);
  fps = 1000000 / (1000000 * (current_time.tv_sec - _b_frame.tv_sec) + \
		   current_time.tv_usec - _b_frame.tv_usec);
  if(fps == 0) fps = 1;
  XdbeSwapBuffers(_dpy, &info, 1);
  XFlush(_dpy);
  XSync(_dpy, False);
}


// This centers a camera represented by a struct vector4
void center_camera(struct vector4 *camera, float x, float y){
  camera -> x = x - (camera -> w / 2);
  camera -> y = y - (camera -> z / 2);
}

// This functions zooms in and out wihout change the central point (focus)
void zoom_camera(struct vector4 *cam, float zoom){
  float x, y;
  // Storing the central point
  x = cam -> x + (cam -> w / 2);
  y = cam -> y + (cam -> z / 2);
  // Zooming
  cam -> w /= zoom;
  cam -> z /= zoom;
  // Restoring the central point
  cam -> x = x - (cam -> w / 2);
  cam -> y = y - (cam -> z / 2);
}

// This function limits the area where a camera can draw things in the screen
void limit_camera(struct vector4 *camera, int x, int y, int width, int height){
  camera -> previous = (struct vector4 *) (long) x;
  camera -> top = (struct vector4 *) (long) y;
  camera -> next = (struct vector4 *) (long) width;
  camera -> down = (struct vector4 *) (long) height;
}

// This erases a rectangle from the screen
void _erase_rectangle(struct vector4 *camera, struct vector4 *rectangle,
		      int full){
   int x, y, height, width, limited_camera = 0;
  x = (int) (((rectangle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((rectangle -> y - camera -> y) / camera -> z) * window_height);
  height = (int) ((rectangle -> z / camera -> z) * window_height);
  width = (int) ((rectangle -> w / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
               (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
                   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
               (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
                    (float) window_height);
  }
  if(limited_camera){
    if(rectangle -> x + rectangle -> w > camera -> x &&
       rectangle -> x < camera -> x + camera -> w &&
       rectangle -> y + rectangle -> z > camera -> y &&
       rectangle -> y < camera -> y + camera -> z){
      int surf_x, surf_y, surf_w, surf_h;
      surf_x =  (int) (long) camera -> previous;
      surf_y = (int) (long) camera -> top;
      surf_w = (int) (long) camera -> next;
      surf_h = (int) (long) camera -> down;
      if(x > 0){
	if(x > surf_w)
	  return; // Outside range
	surf_x += x;
	surf_w -= x;
	x = 0;
      }
      if(y > 0){
	if(y > surf_h)
	  return; // Outside range
	surf_y += y;
	surf_h -= y;
	y = 0;
      }
      if(x + width < surf_w){
	surf_w = x + width;
	surf_w ++;
      }
      if(y + height < surf_h){
	surf_h = y + height;
	surf_h ++;
      }
      // If we are here, the rectangle appears in the camera
      // Creating a temporary surface
      struct surface *surf = new_surface(surf_w, surf_h);
      blit_surface(background, surf, surf_x, surf_y, 
		   surf_w, surf_h, 0, 0);
      draw_rectangle_mask(surf, 0, 0, surf_w, surf_h);
      // Drawing the rectangle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);
      if(full)
	XFillRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);
      
      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   surf_x, surf_y);
      destroy_surface(surf);
    }
  }
  else{
    struct surface *surf = new_surface(width + 1, height + 1);
    draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
    blit_surface(background, surf, x, y, width + 1, height + 1, 0, 0);
    XSetForeground(_dpy, _mask_gc, ~0l);
    XDrawRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, width, height);
    if(full)
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, width, height);
    draw_surface(surf, window, x, y);
    destroy_surface(surf);
  }
}

// This shows a rectangle (or square) in the area covered by the camera
void film_rectangle(struct vector4 *camera, struct vector4 *rectangle, 
		    unsigned color){
  int x, y, height, width, limited_camera = 0;
  x = (int) (((rectangle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((rectangle -> y - camera -> y) / camera -> z) * window_height);
  height = (int) ((rectangle -> z / camera -> z) * window_height);
  width = (int) ((rectangle -> w / camera -> w) * window_width);

  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
               (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
                   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
               (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
                    (float) window_height);
  }
  if(limited_camera){
    if(rectangle -> x + rectangle -> w > camera -> x &&
       rectangle -> x < camera -> x + camera -> w &&
       rectangle -> y + rectangle -> z > camera -> y &&
       rectangle -> y < camera -> y + camera -> z){
      // If we are here, the rectangle appears in the camera
      // Creating a temporary and transparent surface
      struct surface *surf = new_surface((long) camera -> next, 
                                         (long) camera -> down);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
                     surf -> height);
      
      // Drawing the rectangle in the surface
      XSetForeground(_dpy, _gc, color);
      XDrawRectangle(_dpy, surf -> pix, _gc, x, y, width, height);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);
      
      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      destroy_surface(surf);
    }
  }
  else
    draw_rectangle(x, y, width, height, color);
}

void film_fullrectangle(struct vector4 *camera, struct vector4 *rectangle, 
			unsigned color){
  int x, y, height, width, limited_camera = 0;
  x = (int) (((rectangle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((rectangle -> y - camera -> y) / camera -> z) * window_height);
  height = (int) ((rectangle -> z / camera -> z) * window_height);
  width = (int) ((rectangle -> w / camera -> w) * window_width);

  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
               (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
                   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
               (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
                    (float) window_height);
  }

  if(limited_camera){
    if(rectangle -> x + rectangle -> w > camera -> x &&
       rectangle -> x < camera -> x + camera -> w &&
       rectangle -> y + rectangle -> z > camera -> y &&
       rectangle -> y < camera -> y + camera -> z){
      // If we are here, the rectangle appears in the camera
      // Creating a temporary and transparent surface
      struct surface *surf = new_surface((long) camera -> next, 
                                         (long) camera -> down);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
                     surf -> height);
      
      // Drawing the rectangle in the surface
      XSetForeground(_dpy, _gc, color);
      XDrawRectangle(_dpy, surf -> pix, _gc, x, y, width, height);
      XFillRectangle(_dpy, surf -> pix, _gc, x, y, width, height);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);

      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      destroy_surface(surf);
    }
  }
  else
    fill_rectangle(x, y,width, height, color);
}

// Erase a circle from the screen
void erase_circle(struct vector4 *camera, struct vector3 *circle){
  int x, y, height, width, limited_camera;
  limited_camera = 0;
  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
	       (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
		   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
	       (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
		    (float) window_height);
  }
  if(limited_camera){
    if(circle -> x < (long) camera -> x + (long) camera -> w + 
       (long) (2 * circle -> z) && 
       circle -> x > (long) camera -> x - (long) (2 * circle -> z) &&
       circle -> y < (long) camera -> y + (long) camera -> z + 
       (long) (2 * circle -> z) && 
       circle -> y > (long) camera -> y - (long) (2 * circle -> z)){
      int surf_x, surf_y, surf_w, surf_h;
      surf_x =  (int) (long) camera -> previous;
      surf_y = (int) (long) camera -> top;
      surf_w = (int) (long) camera -> next;
      surf_h = (int) (long) camera -> down;
      if(x - width /2 > 0){ 
	if(x - width /2 > surf_w){
	  return; // Circle outside camera range
	}
	else{
	  surf_x += x - width /2;
	  surf_w -= x - width /2;
	  x -= surf_x - (int) (long) camera -> previous;
	  
	}
      }
      
      if(y - height /2 > 0){
	if(y - height /2 > surf_h){
	  return; // Circle outside camera range
	}
	else{
	  surf_y += y - height /2;
	  surf_h -= y - height /2;
	  y -= surf_y - (int) (long) camera -> top;
	
	}
      }	
      x -= width / 2;
      y -= height / 2;
      if(width < surf_w)
	surf_w = width + 1;
      if(height  < surf_h)
	surf_h = height + 1;
      
      if(surf_w <= 0 || surf_h <= 0)
	return;

      // Creating a surface with part of the background
      surface *surf = new_surface(surf_w, surf_h);
      draw_rectangle_mask(surf, 0, 0, surf_w, surf_h);
      blit_surface(background, surf, surf_x,
		   surf_y, surf_w, surf_h,
		   0, 0);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawArc(_dpy, surf -> mask, _mask_gc, x, 
	       y, 
	       width, height, 0, 23040);
      // Blitting the surface in the screen
       blit_surface(surf, window, 0, 0,
		   surf_w, surf_h,
		   surf_x, 
		   surf_y);
      destroy_surface(surf);
    }
  }
  else{
    surface *surf = new_surface(width + 1, height + 1);
    draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
    blit_surface(background, surf, x - width / 2,
		   y - height / 2, surf -> width, surf -> height,
		   0, 0);
    // Drawing the circle in the transparency map
    XSetForeground(_dpy, _mask_gc, ~0l);
    XDrawArc(_dpy, surf -> mask, _mask_gc, 0, 0, 
	     width, height, 0, 23040);
    // Blitting the surface in the screen
    blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		 x - width / 2, y - height / 2);
    destroy_surface(surf);
  }  
}

// Erase a full circle from the screen
void erase_fullcircle(struct vector4 *camera, struct vector3 *circle){
  int x, y, height, width, limited_camera;
  limited_camera = 0;
  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
	       (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
		   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
	       (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
		    (float) window_height);
  }
  if(limited_camera){
    if(circle -> x < (long) camera -> x + (long) camera -> w + 
       (long) (2 * circle -> z) && 
       circle -> x > (long) camera -> x - (long) (2 * circle -> z) &&
       circle -> y < (long) camera -> y + (long) camera -> z + 
       (long) (2 * circle -> z) && 
       circle -> y > (long) camera -> y - (long) (2 * circle -> z)){
      int surf_x, surf_y, surf_w, surf_h;
      surf_x =  (int) (long) camera -> previous;
      surf_y = (int) (long) camera -> top;
      surf_w = (int) (long) camera -> next;
      surf_h = (int) (long) camera -> down;
      if(x - width /2 > 0){ 
	if(x - width /2 > surf_w){
	  return; // Circle outside camera range
	}
	else{
	  surf_x += x - width /2;
	  surf_w -= x - width /2;
	  x -= surf_x - (int) (long) camera -> previous;
	  
	}
      }
      
      if(y - height /2 > 0){
	if(y - height /2 > surf_h){
	  return; // Circle outside camera range
	}
	else{
	  surf_y += y - height /2;
	  surf_h -= y - height /2;
	  y -= surf_y - (int) (long) camera -> top;
	
	}
      }	
      x -= width / 2;
      y -= height / 2;
      if(width < surf_w)
	surf_w = width + 1;
      if(height  < surf_h)
	surf_h = height + 1;
      
      if(surf_w <= 0 || surf_h <= 0)
	return;

      // Creating a surface with part of the background
      surface *surf = new_surface(surf_w, surf_h);
      draw_rectangle_mask(surf, 0, 0, surf_w, surf_h);
      blit_surface(background, surf, surf_x,
		   surf_y, surf_w, surf_h,
		   0, 0);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawArc(_dpy, surf -> mask, _mask_gc, x, 
	       y, 
	       width, height, 0, 23040);
      XFillArc(_dpy, surf -> mask, _mask_gc, x, 
	       y, 
	       width, height, 0, 23040);
      // Blitting the surface in the screen
       blit_surface(surf, window, 0, 0,
		   surf_w, surf_h,
		   surf_x, 
		   surf_y);
      destroy_surface(surf);
    }
  }
  else{
    surface *surf = new_surface(width + 1, height + 1);
    draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
    blit_surface(background, surf, x - width / 2,
		   y - height / 2, surf -> width, surf -> height,
		   0, 0);
    // Drawing the circle in the transparency map
    XSetForeground(_dpy, _mask_gc, ~0l);
    XDrawArc(_dpy, surf -> mask, _mask_gc, 0, 0, 
	     width, height, 0, 23040);
    XFillArc(_dpy, surf -> mask, _mask_gc, 0, 0, 
	     width, height, 0, 23040);
    // Blitting the surface in the screen
    blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		 x - width / 2, y - height / 2);
    destroy_surface(surf);
  }  
}

// This shows a circle in the screen if it's in the area covered by the camera
void film_circle(struct vector4 *camera, struct vector3 *circle, 
		 unsigned color){
  int x, y, height, width, limited_camera;
  limited_camera = 0;

  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
	       (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
		   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
	       (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
		    (float) window_height);
  }
  if(limited_camera){
    if(circle -> x < (long) camera -> x + (long) camera -> w + 
       (long) (2 * circle -> z) && 
       circle -> x > (long) camera -> x - (long) (2 * circle -> z) &&
       circle -> y < (long) camera -> y + (long) camera -> z + 
       (long) (2 * circle -> z) && 
       circle -> y > (long) camera -> y - (long) (2 * circle -> z)){
      // Creating a temporary and transparent surface
      struct surface *surf = new_surface((long) camera -> next, 
					 (long) camera -> down);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
		     surf -> height);
      
      // Drawing the circle in the surface
      XSetForeground(_dpy, _gc, color);
      XDrawArc(_dpy, surf -> pix, _gc, x - width / 2, y - height / 2, width, 
	       height, 0, 23040);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawArc(_dpy, surf -> mask, _mask_gc, x - width / 2, y - height / 2, 
	       width, height, 0, 23040);
      
      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      destroy_surface(surf);
    }
  }
  else
    draw_ellipse(x, y, width, height, color);
}

// This draws a full circle in the screen if it's in the area covered by the 
// camera
void film_fullcircle(struct vector4 *camera, struct vector3 *circle, 
		     unsigned color){
  int x, y, height, width, limited_camera;
  limited_camera = 0;

  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
	       (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
		   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
	       (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
		    (float) window_height);
  }
  if(limited_camera){
    if(circle -> x < (long) camera -> x + (long) camera -> w + 
       (long) (2 * circle -> z) && 
       circle -> x > (long) camera -> x - (long) (2 * circle -> z) &&
       circle -> y < (long) camera -> y + (long) camera -> z + 
       (long) (2 * circle -> z) && 
       circle -> y > (long) camera -> y - (long) (2 * circle -> z)){
      // Creating a temporary and transparent surface
      struct surface *surf = new_surface((long) camera -> next, 
					 (long) camera -> down);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
		     surf -> height);
      
      // Drawing the circle in the surface
      XSetForeground(_dpy, _gc, color);
      XDrawArc(_dpy, surf -> pix, _gc, x - width / 2, y - height / 2, width, 
	       height, 0, 23040);
      XFillArc(_dpy, surf -> pix, _gc, x - width / 2, y - height / 2, width, 
	       height, 0, 23040);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawArc(_dpy, surf -> mask, _mask_gc, x - width / 2, y - height / 2, 
	       width, height, 0, 23040);
      XFillArc(_dpy, surf -> mask, _mask_gc, x - width / 2, y - height / 2, 
	       width, height, 0, 23040);

      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      destroy_surface(surf);
    }
  }
  else
    fill_ellipse(x, y, width, height, color);
}

// This films a full polygon
void _film_fullpolygon(struct vector4 *camera, struct vector2 *polygon, 
		       unsigned color, int erase){
  struct vector2 *current_vertex = polygon;
  XPoint *points;
  int limited_camera = 0, number_of_points = 0;
  float smallest_x = FLT_MAX, smallest_y = FLT_MAX;
  float biggest_x  = FLT_MIN, biggest_y  = FLT_MIN;

  if(!erase)
    XSetFillRule(_dpy, _gc, WindingRule);
  else
    XSetFillRule(_dpy, _mask_gc, WindingRule);
  
  /* Empty polygon or henagon.*/
  if(polygon == NULL || polygon -> next == polygon)
    return;

  if(camera -> previous != NULL || camera -> next != NULL)
    limited_camera = 1;
  
  // Discovering the number of points, and also
  // the extreme points
  do{
    number_of_points ++;
    if(erase){
      if(current_vertex -> x < smallest_x)
	smallest_x = current_vertex -> x;
      if(current_vertex -> y < smallest_y)
	smallest_y = current_vertex -> y;
      if(current_vertex -> x > biggest_x)
	biggest_x = current_vertex -> x;
      if(current_vertex -> y > biggest_y)
	biggest_y = current_vertex -> y;
    }
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
  
  // Allocating space for the XPoints
  points = (XPoint *) malloc(sizeof(XPoint) * number_of_points);
  
  //Getting the points coordinates 
  number_of_points = 0;
  current_vertex = polygon;
  do{
    points[number_of_points].x = (int) (((current_vertex -> x - camera -> x)/
					 camera -> w) * window_width);
    points[number_of_points].y = (int) (((current_vertex -> y - camera -> y)/
					 camera -> z) * window_height);
    if(erase && !limited_camera){  
      points[number_of_points].x -= (int) (((smallest_x - camera -> x)/
		      camera -> w) * window_width);
      points[number_of_points].y -= (int) (((smallest_y - camera -> y)/
		      camera -> z) * window_height);
    }
    number_of_points ++;
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
  // Correcting values if we are under a limited camera
  if(limited_camera){
    int width = (int) ((float) ((int) (((biggest_x - smallest_x)/
					camera -> w) * window_width)) * 
		       ((float) (long) camera -> next) / 
		       (float) window_width) + 1;
    
    int height =  (int) ((float) ((int) (((biggest_y - smallest_y)/
					camera -> z) * window_height)) * 
		       ((float) (long) camera -> down) / 
		       (float) window_height) +1;
    
    int x = (int) ((float) ((int) (((smallest_x - camera -> x)/
					  camera -> w) * window_width)) * 
			 ((float) (long) camera -> next) / 
		   (float) window_width) + (int) (long) camera -> previous;
    int y = (int) ((float) ((int) (((smallest_y - camera -> y)/
				    camera -> z) * window_height)) * 
		   ((float) (long) camera -> down) / 
		   (float) window_height) + (int) (long) camera -> top;
    if(x < (int) (long) camera -> previous){
      width -= (int) (long) camera -> previous - x;
      x = (int) (long) camera -> previous;
    }
    else if(x > (int) (long) camera -> previous + (int) (long) camera -> next)
      x = (int) (long) camera -> previous + (int) (long) camera -> next;
    if(y < (int) (long) camera -> top){
      height -= (int) (long) camera -> top - y;
      y = (int) (long) camera -> top;
    }
    else if(y > (int) (long) camera -> top + (int) (long) camera -> down)
      y = (int) (long) camera -> top + (int) (long) camera -> down;
    if(width > (int) (long) camera -> next)
      width = (int) (long) camera -> next;
    if(height > (int) (long) camera -> down)
      height = (int) (long) camera -> down - 1; 
    if(x + width > (long) camera -> previous + (long) camera -> next)
      width = (long) camera -> previous + (long) camera -> next - x;
    if(y + height > (long) camera -> top + (long) camera -> down)
      height = (long) camera -> top + (long) camera -> down - y;
    
    
    if(erase && (width <= 0 || height <= 0))
      return;
    
    number_of_points = 0;
    current_vertex = polygon;
    do{
      points[number_of_points].x = (int) ((float) points[number_of_points].x * 
                                          ((float) (long) camera -> next) / 
                                          (float) window_width);
      points[number_of_points].y = (int) ((float) points[number_of_points].y * 
                                          ((float) (long) camera -> down) / 
                                          (float) window_height);
      if(erase){
	points[number_of_points].x += (long) camera -> previous - x;
	points[number_of_points].y += (long) camera -> top - y;
      }
      number_of_points ++;
      current_vertex = current_vertex -> next;
    }while(current_vertex != polygon);
    
    
    if(erase){
      struct surface *surf = new_surface(width, height);
      draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
      blit_surface(background, surf, x, y, width, height, 0, 0);
      // Drawing the polygon in the surface
      XSetForeground(_dpy, _mask_gc, ~0l);
      XFillPolygon(_dpy, surf -> mask, _mask_gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
      draw_surface(surf, window, x, y);
      destroy_surface(surf);
    }
    else{
      struct surface *surf = new_surface((long) camera -> next, 
					 (long) camera -> down);
      XSetFillRule(_dpy, _mask_gc, WindingRule);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
		     surf -> height);
      
      // Drawing the polygon in the surface
      XSetForeground(_dpy, _gc, color);
      XFillPolygon(_dpy, surf -> pix, _gc, points, number_of_points, Complex, 
		   CoordModeOrigin);
      
      
      // Drawing the polygon in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XFillPolygon(_dpy, surf -> mask, _mask_gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
      
      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      
      
      destroy_surface(surf);
    }
  }
  else{
    if(erase){
      int x = (int) (((smallest_x - camera -> x)/
		      camera -> w) * window_width);
      int y = (int) (((smallest_y - camera -> y)/
		      camera -> z) * window_height);
      int width = (int) (((biggest_x - smallest_x)/
		      camera -> w) * window_width) + 1;
      int height = (int) (((biggest_y- smallest_y)/
		      camera -> z) * window_height) + 1;
      surface *surf = new_surface(width, height);
      draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
      blit_surface(background, surf, x, y, width, height, 0, 0);
      XSetForeground(_dpy, _mask_gc, ~0l);
      XFillPolygon(_dpy, surf -> mask, _mask_gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
      draw_surface(surf, window, x, y);      
      destroy_surface(surf);
    }
    else{
      XSetForeground(_dpy, _gc, color);
      XFillPolygon(_dpy, window -> pix, _gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
    }
  }
  free(points);
  
}

// This films a henagon
void _film_henagon(struct vector4 *cam, struct vector2 *henagon, 
		   unsigned color, int erase){
  int x, y;
  x = (int) (((henagon -> x - cam -> x) / cam -> w) * window_width);
  y = (int) (((henagon -> y - cam -> y) / cam -> z) * window_height);
  if(erase)
    blit_surface(background, window, x, y, 1, 1, x, y);
  else
    draw_point(x, y, color);
}

void _film_limited_henagon(struct vector4 *cam, struct vector2 *henagon, 
			   unsigned color, int erase){
    int x, y;
    x = (int) (((henagon -> x - cam -> x) / cam -> w));
    x= (int) ((float) x * ((float) (long) cam -> next)  + 
	      (long) (cam -> previous));
    y = (int) (((henagon -> y - cam -> y) / cam -> z));
    y = (int) ((float) y * ((float) (long) cam -> down)) + 
      (long) (cam -> top);
    if(erase)
      blit_surface(background, window, x, y, 1, 1, x, y);
    else
      draw_point(x, y, color);
}

// Films an empty polygon with a not limited camera
void _film_normal_polygon(struct vector4 *cam, struct vector2 *poly,
			  unsigned color){
  struct vector2 *current_vertex = poly;
  struct vector2 *next;
  int x1, x2, y1, y2;

  do{
    next = current_vertex -> next;

    x1 = (int) (((current_vertex -> x - cam -> x) / cam -> w) * 
		window_width);
    y1 = (int) (((current_vertex -> y - cam -> y) / cam -> z) * 
		window_height);
    x2 = (int) (((next -> x - cam -> x) / cam -> w) * window_width);
    y2 = (int) (((next -> y - cam -> y) / cam -> z) * window_height);

    draw_line(x1, y1, x2, y2, color);

    current_vertex = next;
  } while(current_vertex != poly);
}

// This draws a polygon in the screen filming it with a limited camera.
void _film_limited_polygon(struct vector4 *cam, struct vector2 *poly,
			   unsigned color){
  struct vector2 *current_vertex = poly;
  struct vector2 *next;
  int x1, x2, y1, y2;
  int smallest_x = INT_MAX, smallest_y = INT_MAX;
  surface *surf = new_surface((long) cam -> next, 
			      (long) cam -> down);
  
  blit_surface(background, surf, (int) cam -> previous, (int) cam -> top,
	       surf -> width, surf -> height, 0, 0);
  draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
  XSetForeground(_dpy, _mask_gc, ~0l);
  XSetForeground(_dpy, _gc, color);
  do{
    x1 = (int) ((((current_vertex -> x - cam -> x) / cam -> w) *
		 ((float) (long) cam -> next) +
		 (long) (cam -> previous)));
    if(x1 <= smallest_x)
      smallest_x = x1;
    y1 = (int) (((current_vertex -> y - cam -> y) / cam -> z) * 
		((float) (long) cam -> down) +
		(long) (cam -> top));
    if(y1 < smallest_y)
      smallest_y = y1;
    current_vertex = current_vertex -> next;
  }while(current_vertex != poly);

  do{
    next = current_vertex -> next;

    x1 = (int) ((((current_vertex -> x - cam -> x) / cam -> w) *
		 ((float) (long) cam -> next) +
		 (long) (cam -> previous)));
    x1 -= smallest_x;
    y1 = (int) (((current_vertex -> y - cam -> y) / cam -> z) * 
		((float) (long) cam -> down) +
		(long) (cam -> top));
    y1 -= smallest_y;
    x2 = (int) ((((next -> x - cam -> x) / cam -> w) *
		 ((float) (long) cam -> next) +
		 (long) (cam -> previous)));
    x2 -= smallest_x;
    y2 = (int) (((next -> y - cam -> y) / cam -> z) * 
		((float) (long) cam -> down) +
		(long) (cam -> top));
    y2 -= smallest_y;

    XDrawLine(_dpy, surf -> mask, _mask_gc, x1, y1, x2, y2);
    XDrawLine(_dpy, surf -> pix, _gc, x1, y1, x2, y2);

    current_vertex = current_vertex -> next;
  }while(current_vertex != poly);
  draw_surface(surf, window, (int) cam -> previous, (int) cam -> top);
  destroy_surface(surf);
}

//Erases an empty polygon with a not limited camera
void _erase_normal_polygon(struct vector4 *cam, struct vector2 *poly){
  struct vector2 *current_vertex = poly;
  struct vector2 *next;
  int x1, x2, y1, y2;
  int smallest_x = INT_MAX, smallest_y = INT_MAX, biggest_x = INT_MIN, 
    biggest_y = INT_MIN;
  surface *surf;
  // First we get the polygon width, height and position
  do{
    
    x1 = (int) (((current_vertex -> x - cam -> x) / cam -> w) * 
		window_width);
    y1 = (int) (((current_vertex -> y - cam -> y) / cam -> z) * 
		window_height);
    if(x1 <= smallest_x) smallest_x = x1;
    if(y1 <= smallest_y) smallest_y = y1;
    if(x1 >= biggest_x) biggest_x = x1;
    if(y1 >= biggest_y) biggest_y = y1;

    current_vertex = current_vertex -> next;
  }while(current_vertex != poly);
  
  // Preparing the buffer
  biggest_y ++;
  biggest_x ++;
  surf = new_surface(biggest_x - smallest_x, biggest_y - smallest_y);
  draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
  blit_surface(background, surf, smallest_x, smallest_y,
	       surf -> width, surf -> height, 
	       0, 0);
  XSetForeground(_dpy, _mask_gc, ~0l);
  // Now we can draw it
  do{
    next = current_vertex -> next;
    x1 = (int) (((current_vertex -> x - cam -> x) / cam -> w) * 
		window_width) - smallest_x;
    y1 = (int) (((current_vertex -> y - cam -> y) / cam -> z) * 
		window_height) - smallest_y;
    x2 = (int) (((next -> x - cam -> x) / cam -> w) * window_width) - 
      smallest_x;
    y2 = (int) (((next -> y - cam -> y) / cam -> z) * window_height) - 
      smallest_y;

    XDrawLine(_dpy, surf -> mask, _mask_gc, x1, y1, x2, y2);

    current_vertex = current_vertex -> next;
  }while(current_vertex != poly);

  draw_surface(surf, window, smallest_x, smallest_y);
  destroy_surface(surf);
}

// This films an empty polygon
void _film_polygon(struct vector4 *camera, struct vector2 *polygon, 
		   unsigned color, int erase){
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
  else if(polygon -> next == polygon){
    if(polygon -> x < camera -> x)
      return;
    if(polygon -> y < camera -> y)
      return;
    if(polygon -> x > camera -> x + camera -> w)
      return;
    if(polygon -> y > camera -> y + camera -> z)
      return;
    // If we are here, we need to draw the dot
    // The camera is limited?
    if(camera -> previous != NULL && camera -> next != NULL)
      _film_limited_henagon(camera, polygon, color, erase);
    else
      _film_henagon(camera, polygon, color, erase);
    return;
  }  
  else{
    if(camera -> previous == NULL && camera -> next == NULL){
      if(!erase){
	_film_normal_polygon(camera, polygon, color);
	return;
      }
      else{
	_erase_normal_polygon(camera, polygon);
	return;
      }
    }
    else{
       if(!erase){
	 _film_limited_polygon(camera, polygon, color);
	return;
      }
    }
  }

  // Now the usual case. We have a polygon with more than one vertex.
  do{
    next = current_vertex -> next;
    limited_camera = 0;

    x1 = (int) (((current_vertex -> x - camera -> x) / camera -> w) * 
		window_width);
    y1 = (int) (((current_vertex -> y - camera -> y) / camera -> z) * 
		window_height);
    x2 = (int) (((next -> x - camera -> x) / camera -> w) * window_width);
    y2 = (int) (((next -> y - camera -> y) / camera -> z) * window_height);

    if(camera -> previous != NULL && camera -> next != NULL){
      x1 = (int) ((float) x1 * ((float) (long) camera -> next) / 
		  (float) window_width) + (long) (camera -> previous);
      x2 = (int) ((float) x2 * ((float) (long) camera -> next) / 
		  (float) window_width) + (long) (camera -> previous);
      limited_camera ++;
    }
    if(camera -> top != NULL && camera -> down != NULL){
      y1 = (int) ((float) y1 * ((float) (long) camera -> down) / 
		  (float) window_height) + (long) (camera -> top);
      y2 = (int) ((float) y2 * ((float) (long) camera -> down) / 
		  (float) window_height) + (long) (camera -> top);
      limited_camera ++;
    }

    // In fact, we must handle 4 cases: The 2 dots are inside the limit, 
    //the 2 are outside, only the first is
    // inside or only the second is inside.
    if(limited_camera){
      if((x1 < (long) camera -> previous + (long) camera -> next && x1 > 
	  (long) camera -> previous &&
	  y1 < (long) camera -> top + (long) camera -> down && y1 > 
	  (long) camera -> top)){
	// The first dot is inside!
	if(x2 < (long) camera -> previous + (long) camera -> next && x2 > 
	   (long) camera -> previous &&
	   y2 < (long) camera -> top + (long) camera -> down && y2 > 
	   (long) camera -> top){
	  // The 2 dots are inside!
	  draw_line(x1, y1, x2, y2, color);
	}
	else{
	  // (x2, y2) is outside
	  if(x2 > (long) camera -> previous + (long) camera -> next){ 
	    // x2 is far east
	    float a = ((float) (y2 - y1)) / ((float) (x2 - x1)); // We already know that x2 != x1
	    float b = y1 - a * x1;
	    x2 = (int) ((long) camera -> previous + (long) camera -> next);
	    y2 = a * (float) x2 + b;
	    if(y2 > (long) camera -> top && y2 < (long) camera -> top + 
	       (long) camera -> down)
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
	    float a = ((float) (y2 - y1)) / ((float) (x2 - x1)); 
	    // We already know that x2 != x1
	    float b = y1 - a * x1;
	    x2 = (int) (long) camera -> previous;
	    y2 = a * (float) x2 + b;
	    if(y2 > (long) camera -> top && y2 < (long) camera -> top + 
	       (long) camera -> down)
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
	    float a = ((float) (y2 - y1)) / ((float) (x2 - x1)); 
	    // We already know that x2 != x1
	    float b = (float) y1 - a * (float) x1;
	    if(y2 <= (long) camera -> top){
	      y2 = (long) camera -> top;
	      if(a != 0.0){
		x2 = (y2 - b) / a;
		if(x2 > (long) camera -> previous && x2 < 
		   (long) camera -> previous + (long) camera -> next)
		  draw_line(x1, y1, x2, y2, color);
	      }
	    }
	    else if(y2 >= (long) camera -> top + (long) camera -> down){
	      y2 = (long) camera -> top + (long) camera -> down;
	      if(a != 0.0){
		x2 = (int) ((float) y2 - b) / a;
		if(x2 > (long) camera -> previous && x2 < 
		   (long) camera -> previous + (long) camera -> next)
		  draw_line(x1, y1, x2, y2, color);
	      }
	    }

	  }
	}
      }
      else{
	// The first dot is outside.
	if(x2 < (long) camera -> previous + (long) camera -> next && x2 > 
	   (long) camera -> previous &&
           y2 < (long) camera -> top + (long) camera -> down && y2 > 
	   (long) camera -> top){
	  // Only the second dot is inside
	  // (x1, y1) is outside
	  if(x1 > (long) camera -> previous + (long) camera -> next){ 
	    // x1 is far east
	    float a = ((float) (y1 - y2)) / ((float) (x1 - x2)); 
	    // We already know that x2 != x1
	    float b = y2 - a * x2;
	    x1 = (int) ((long) camera -> previous + (long) camera -> next);
	    y1 = a * (float) x1 + b;
	    if(y1 > (long) camera -> top && y1 < (long) camera -> top + 
	       (long) camera -> down)
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
	    float a = ((float) (y1 - y2)) / ((float) (x1 - x2)); 
	    // We already know that x2 != x1
	    float b = y2 - a * x2;
	    x1 = (int) (long) camera -> previous;
	    y1 = a * (float) x1 + b;
	    if(y1 > (long) camera -> top && y1 < (long) camera -> top + 
	       (long) camera -> down)
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
	  else{
	    // x1 is not outside the screen. But y1 is
	    float a = ((float) (y1 - y2)) / ((float) (x1 - x2)); 
	    // We already know that x2 != x1
	    float b = y2 - a * x2;
	    if(y1 <= (long) camera -> top){
	      y1 = (long) camera -> top;
	      if(a != 0.0){
		x1 = (y1 - b) / a;
		if(x1 > (long) camera -> previous && x1 < 
		   (long) camera -> previous + (long) camera -> next)
		  draw_line(x1, y1, x2, y2, color);
	      }
	    }
	    else if(y1 >= (long) camera -> top + (long) camera -> down){
	      y1 = (long) camera -> top + (long) camera -> down;
	      if(a != 0.0){
		x1 = (y1 - b) / a;
		if(x1 > (long) camera -> previous && x1 < 
		   (long) camera -> previous + (long) camera -> next)
		  draw_line(x1, y1, x2, y2, color);
	      }
	    }
	  }
	}
	// Else the 2 are outside and there's nothing to do.
      }
    }
    else{
      // We don't have a limited camera. Much easier!
      if(!erase){
	draw_line(x1, y1, x2, y2, color);
      }
      else{
	int width = (x1 - x2 >= 0)?(x1 - x2):(x2 - x1);
	int height = (y1 - y2 >= 0)?(y1 - y2):(y2 - y1);
	int x = (x1 >= x2)?(x2):(x1);
	int y = (y1 >= y2)?(y2):(y1);
	if(!width){
	  blit_surface(background, window, x, y, 1, height, x, y);
	  continue;
	}
	if(!height){
	   blit_surface(background, window, x, y, width, 1, x, y);
	  continue;
	}
	surface *surf = new_surface(width, height);
	draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
	blit_surface(background, surf, x, y,
		     surf -> width, surf -> height, 
		     0, 0);
	// Drawing the edge in the surface
	if((x1 <= x2 && y1 <= y2) || ((x1 >= x2 && y1 >= y2))){ 
	  XSetForeground(_dpy, _mask_gc, ~0l);
	  XDrawLine(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
		    surf-> height);
	 
	}

	else{
	  XSetForeground(_dpy, _mask_gc, ~0l);
	  XDrawLine(_dpy, surf -> mask, _mask_gc, surf -> width, 0, 0, 
		    surf-> height);
	
	  
	}

	draw_surface(surf, window, x, y);
	
	destroy_surface(surf);
      }
    }
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
}

// This detects collision between two rectangles
int collision_rectangle_rectangle(struct vector4 *r1, struct vector4 *r2){
  float dx = (r2 -> x - r1 -> x < 0) ? (r1 -> x - r2 -> x) : 
    (r2 -> x - r1 -> x);
  if((r1 -> x < r2 -> x) ? (dx <= r1 -> w) : (dx <= r2 -> w)){
    float dy = (r2 -> y - r1 -> y < 0) ? (r1 -> y - r2 -> y) : 
      (r2 -> y - r1 -> y);
    if((r1 -> y < r2 -> y) ? (dy <= r1 -> z) : (dy <= r2 -> z))
      return 1;
  }
  return 0;
}

// This detects collision between a rectangle and a circle
int collision_rectangle_circle(struct vector4 *rectangle, 
			       struct vector3 *circle){
  // They collide in the X-axis?
  if(!((circle -> x + circle -> z < rectangle -> x && circle -> x - 
	circle -> z < rectangle -> x &&
     circle -> x + circle -> z < rectangle -> x + rectangle -> w &&
      circle -> x - circle -> z < rectangle -> x + rectangle -> w) ||
     (circle -> x + circle -> z > rectangle -> x && circle -> x - 
      circle -> z > rectangle -> x &&
     circle -> x + circle -> z > rectangle -> x + rectangle -> w &&
      circle -> x - circle -> z > rectangle -> x + rectangle -> w)))
    if(!((circle -> y + circle -> z < rectangle -> y && circle -> y - 
	  circle -> z < rectangle -> y &&
	  circle -> y + circle -> z < rectangle -> y + rectangle -> z &&
	  circle -> y - circle -> z < rectangle -> y + rectangle -> z) ||
	 (circle -> y + circle -> z > rectangle -> y && circle -> y - 
	  circle -> z > rectangle -> y &&
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

// This determines if a rectangle and a polygon are colliding
int collision_rectangle_polygon(struct vector4 *r, struct vector2 *p){
  struct vector2 *current, *next, *first;
  float x1, x2, y1, y2;

  current = first = p;
  next = current -> next;

  if(next == current){ // We have a degenerated polygon with a single vertex
    if(p -> x >= r -> x && p -> x <= r -> x + r -> w)
      if(p -> y >= r -> y && p -> y <= r -> y + r -> z)
	return 1;
    return 0;
  }

  do{ // Loops in all the polygon's vertices
    x1 = MIN(current -> x, next -> x);
    x2 = MAX(current -> x, next -> x);
    if(!(x1 > r -> x + r -> w || x2 < r -> x)){
      y1 = MIN(current -> y, next -> y);
      y2 = MAX(current -> y, next -> y);
      if(!(y1 > r -> y + r -> z || y2 < r -> y)){ // The edge is next. Perhaps
	                                          //it's colliding...
	if(x1 == x2 || y1 == y2){ // We have a vertical or horizontal edge
	  return 1; // It certainly collides in this case
	}
	else{ // We have a non-vertical edge
	  float a = (next -> y - current -> y) / (next -> x - current -> x);
	  float b = current -> y - a * current -> x; 
	  // Now we have an ax+b=y rect equation
	  if((a * r -> x + b >= r -> y) && (a * r -> x + b <= r -> y + r -> z))
	    return 1; // Collided with the left side
	  if((a * (r -> x + r -> w) + b >= r -> y) && 
	     (a * (r -> x + r -> w) + b <= r -> y + r -> z))
	    return 1; // Collided with the right side
	  if(((r -> y - b)/a >= r -> x) && ((r -> y - b)/a <= r -> x + r -> w))
	    return 1; // Collided with the top side
	  if(((r -> y + r -> z - b)/a >= r -> x) &&
	     ((r -> y + r -> z - b)/a <= r -> x + r -> w))
	    return 1; // Collided with the down side
	}
      }
    }

    current = next;
    next = current -> next;
  }while(current != first);
  
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
      return 1; // A collision happenned
    else
      return 0; // No collision happening
  }
 
  // This loops a number of times equal the number of polygon's sides
  // when the polygon isn's degenerated.
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
	if(!((circle -> x + circle -> z < current -> x && circle -> x + 
	      circle -> z < next-> x &&
              circle -> x - circle -> z < current -> x && circle -> x - 
	      circle -> z < next-> x) ||
             (circle -> x + circle -> z > current -> x && circle -> x + 
	      circle -> z > next-> x &&
              circle -> x - circle -> z > current -> x && circle -> x - 
	      circle -> z > next-> x))){
	  if(!((circle -> y + circle -> z < current -> y && circle -> y + 
		circle -> z < next-> y &&
		circle -> y - circle -> z < current -> y && circle -> y - 
		circle -> z < next-> y) ||
	       (circle -> y + circle -> z > current -> y && circle -> y + 
		circle -> z > next-> y &&
		circle -> y - circle -> z > current -> y && circle -> y - 
		circle -> z > next-> y))){
	    return 1;
	  }
        }
      }
    }
    else{ // We have a vertical segment
      if(!((circle -> x - circle -> z < current -> x && circle -> x + 
	    circle -> z < current -> x) ||
           (circle -> x - circle -> z > current -> x && circle -> x + 
	    circle -> z > current -> x))){
	// A collision happened... or not?
	if(!((circle -> y + circle -> z < current -> y && circle -> y + 
	      circle -> z < next-> y &&
              circle -> y - circle -> z < current -> y && circle -> y - 
	      circle -> z < next-> y) ||
             (circle -> y + circle -> z > current -> y && circle -> y + 
	      circle -> z > next-> y &&
	      circle -> y - circle -> z > current -> y && circle -> y - 
	      circle -> z > next-> y))){
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
