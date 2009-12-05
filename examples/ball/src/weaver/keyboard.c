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

#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "keyboard.h"

// This function initializes and cleans our keyboard buffer
void initialize_keyboard(void){
  int i;
  for(i = 0; i < KEYS; i++)
    keyboard[i] = 0;
}

// And this initialize the mouse info
void initialize_mouse(void){
  mouse.pressed = 0;
  mouse.x = mouse.y = 0;
}

// This functions watch for input events. If we get one,
// we stores the information in our buffers
void get_input(void){
  XEvent event;
  KeySym pressed_key, released_key;
  mouse.changed = 0;
  while(XPending(_dpy)){
    XNextEvent(_dpy, &event);
    //printf("DEBUG: %d\n", event.type);
    if(event.type == KeyPress){
      keyboard[ANY] += 1;
      pressed_key = XLookupKeysym(&event.xkey, 0);
      switch(pressed_key){
      case XK_1:
	keyboard[1] += 1;
	break;
      case XK_2:
	keyboard[2] += 1;
	break;
      case XK_Up:
        keyboard[UP] += 1;
        break;
      case XK_Down:
        keyboard[DOWN] += 1;
        break;
      case XK_Left:
        keyboard[LEFT] += 1;
        break;
      case XK_Right:
        keyboard[RIGHT] += 1;
        break;
      case XK_KP_Add:
        keyboard[PLUS] += 1;
        break;
      case XK_KP_Subtract:
        keyboard[MINUS] += 1;
        break;
      case XK_Shift_L:
        keyboard[SHIFT] += 1;
        break;
      case XK_Control_L:
        keyboard[CTRL] += 1;
	keyboard[LEFT_CTRL] += 1;
        break;
      case XK_Control_R:
	keyboard[CTRL] += 1;
	keyboard[RIGHT_CTRL] += 1;
	break;
      case XK_Escape:
	keyboard[ESC] += 1;
        break;
      case XK_a:
        keyboard[A] += 1;
        break;
      case XK_s:
        keyboard[S] += 1;
        break;
      case XK_d:
        keyboard[D] += 1;
        break;
      case XK_w:
        keyboard[W] += 1;
        break;
      case XK_Return:
	keyboard[ENTER] += 1;
	break;
      case XK_F1:
	keyboard[F1] += 1;
	break;
      default:
        break;
      }
    }
    else if(event.type == KeyRelease){
      released_key = XLookupKeysym(&event.xkey, 0);
      keyboard[ANY] = 0;
      switch(released_key){
      case XK_1:
	keyboard[1] = 0;
	break;
      case XK_2:
	keyboard[2] = 0;
	break;
      case XK_Up:
        keyboard[UP] = 0;
        break;
      case XK_Down:
        keyboard[DOWN] = 0;
        break;
      case XK_Left:
        keyboard[LEFT] = 0;
        break;
      case XK_Right:
        keyboard[RIGHT] = 0;
        break;
      case XK_KP_Add:
        keyboard[PLUS] = 0;
        break;
      case XK_KP_Subtract:
        keyboard[MINUS] = 0;
        break;
      case XK_Shift_L:
        keyboard[SHIFT] = 0;
        break;
      case XK_Control_L:
        keyboard[CTRL] = 0;
	keyboard[LEFT_CTRL] = 0;
        break;
      case XK_Control_R:
	keyboard[CTRL] = 0;
	keyboard[RIGHT_CTRL] = 0;
	break;
      case XK_Escape:
	keyboard[ESC] = 0;
	break;
      case XK_a:
        keyboard[A] = 0;
        break;
      case XK_s:
        keyboard[S] = 0;
        break;
      case XK_d:
        keyboard[D] = 0;
        break;
      case XK_w:
        keyboard[W] = 0;
        break;
      case XK_Return:
	keyboard[ENTER] = 0;
	break;
      case XK_F1:
	keyboard[F1] = 0;
	break;
      default:
        break;
      }
   
    }
    else if(event.type == ButtonPress){
      if(! mouse.pressed){
	mouse.pressed = 1;
	mouse.changed = 1;
      }
      mouse.x = event.xbutton.x;
      mouse.y = event.xbutton.y;
    }
    else if(event.type == ButtonRelease){
      if(mouse.pressed){
	mouse.pressed = 0;
	mouse.changed = 1;
      }
      mouse.x = event.xbutton.x;
      mouse.y = event.xbutton.y;
    }
    else if(event.type == MotionNotify){
      mouse.x = event.xmotion.x;
      mouse.y = event.xmotion.y;
    }
  }
}
