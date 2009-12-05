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

#include <stdlib.h>
#include "vector3.h"

struct vector3 *new_vector3(float x, float y, float z){
  struct vector3 *new_vector;
  new_vector = (struct vector3 *) malloc(sizeof(struct vector3));
  if(new_vector != NULL){
    new_vector -> previous = new_vector -> next = new_vector -> top = NULL;
    new_vector -> x = x;
    new_vector -> y = y;
    new_vector -> z = z;
  }
  return new_vector;
}
