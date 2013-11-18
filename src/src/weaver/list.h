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

#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>
#include <stdio.h>

struct list{
  int size;
  struct _element *first;
  void * (*constructor)(void *);
  void (*destructor)(void *);
};

struct _element{
  struct _element *next;
  void *value;
};

struct list *new_list(void * (*constructor)(void *),
		      void (*destructor)(void *));
void insert_list(struct list *, void *);
void destroy_list(struct list *);
void *list_ref(struct list *, int);
void list_filter(struct list *, int (*filter)(void *));
void list_map(struct list *, void * (*func)(void *));

#endif
