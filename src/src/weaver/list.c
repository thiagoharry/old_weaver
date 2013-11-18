/*
  Copyright (C) 2009-2013 Thiago Leucz Astrizi

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

#include "list.h"

struct list *new_list(void * (*constructor)(void *),
		      void (*destructor)(void *)){
  struct list *new_list = NULL;
  new_list = (struct list *) malloc(sizeof(struct list));
  if(new_list){
    new_list -> size = 0;
    new_list -> constructor = constructor;
    new_list -> destructor = destructor;
    new_list -> first = NULL;
  }
  return new_list;
}

void insert_list(struct list *l, void *val){
  void *v = l -> constructor(val);
  struct _element *el = (struct _element *) malloc(sizeof(struct _element));
  if(el){
    l -> size ++;
    el -> next = l -> first;
    el -> value = v;
    l -> first = el;
  }
}

void destroy_list(struct list *l){
  struct _element *next_p, *current_p = l -> first;
  while(current_p != NULL){
    next_p = current_p -> next;
    l -> destructor(current_p);
    free(current_p);
    current_p = next_p;
  }
  free(l);
}

void *list_ref(struct list *l, int n){
  struct _element *p = l -> first;
  int count = 0;
  while(count < n && p != NULL){
    p = p -> next;
    count ++;
  }
  if(p != NULL)
    return p -> value;
  return NULL;
}

void list_filter(struct list *l, int (*filter)(void *)){
  struct _element *last_p = NULL, *p = l -> first;
  while(p != NULL){
    if(!filter(p -> value)){
      l -> size --;
      if(last_p)
	last_p -> next = p -> next;
      else
	l -> first = p -> next;
      l -> destructor(p -> value);
      free(p);
      if(last_p)
	p = last_p;
      else{
	p = l -> first;
	continue;
      }
    }
    last_p = p;
    p = p -> next;
  }
}

void list_map(struct list *l, void *(*function)(void *)){
  struct _element *p = l -> first;
  void *val;
  while(p != NULL){
    if((val = function(p -> value)) != NULL){
      l -> destructor(p -> value);
      p -> value = val;
    }
    p = p -> next;
  }
}

