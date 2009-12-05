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

#include <stdlib.h>
#include <stdio.h>
#include "monster.h"
#include "game.h"

// This function creates a new monster. Specie is the monster species,
// x, y is where the monster appears and target is where is the monster objective 
struct monster *new_monster(int specie, int x, int y, int target){
  struct monster *new_monster = (struct monster *) malloc(sizeof(struct monster)); 
  new_monster -> x = x;
  new_monster -> y = y;
  new_monster -> cx = 0;
  new_monster -> cy = 0;
  new_monster -> specie = specie;
  new_monster -> health = new_monster -> max_health = 25;
  new_monster -> speed = 54;
  new_monster -> gold = 1;
  new_monster -> target = target;
  new_monster -> direction = target;
  new_monster -> sucess = 0;
  new_monster -> previous = new_monster -> next = NULL;
  new_monster -> need_redraw = 1;
  new_monster -> distance = 6000;
  if(game.monsters.list == NULL)
    game.monsters.list = new_monster;
  else{
    struct monster *pointer = game.monsters.list;
    while(pointer -> next != NULL)
      pointer = pointer -> next;
    pointer -> next = new_monster;
    new_monster -> previous = pointer;
  }
  return new_monster;
}


// This function moves all the monsters
int move_monsters(void){
  struct monster *pointer = game.monsters.list;
  int got_target = 0;

  while(pointer != NULL){

    // If we are outside the walls:                                                                                          
    if((pointer -> target == SOUTH && (pointer -> y < game.board.y - 23 || 
				       pointer -> y > game.board.y + game.board.height * 23))  ||
       (pointer -> target == EAST && ((pointer -> x < game.board.x - 23) || 
				      (pointer -> x > game.board.x + game.board.width * 23)))){
      move_monster(pointer, pointer -> target);
      if(!(pointer -> sucess)){   
	if((pointer -> target == SOUTH && pointer -> y > game.board.y + game.board.height * 23) ||
	   (pointer -> target == EAST && pointer -> x > game.board.x + game.board.width * 23)){
          pointer -> distance = -1;
	  pointer -> sucess = 1;
	  got_target = 1;
	  game.player.life --;
	}
      }  
    }
    else{ // We are inside the walls
      if(distance(position_from_coordinate(pointer -> x, pointer -> y), pointer -> target) == 0){
        pointer -> distance = 0;
	move_monster(pointer, pointer -> target);
	continue;
      }
      int north, south, east, west;
      if((pointer -> x - game.board.x) % 23 == 0){ 
	if((pointer -> y - game.board.y) % 23 == 0){// We are not between 2 squares
	  north = distance(position_from_coordinate(pointer -> x, pointer -> y - 23), pointer -> target);
	  south = distance(position_from_coordinate(pointer -> x, pointer -> y + 23), pointer-> target);
	  east = distance(position_from_coordinate(pointer -> x + 23, pointer -> y), pointer-> target);
	  west =  distance(position_from_coordinate(pointer -> x - 23, pointer -> y), pointer-> target);

	  if(north <= south && north <= east && north <= west){
            pointer -> distance = north;
	    if(north < 9000)
	      move_monster(pointer, NORTH);
	  }
	  else if(south <= east && south <= west){
            pointer -> distance = south;
	    if(south < 9000)
	      move_monster(pointer, SOUTH);
	  }
	  else if(east <= west){
            pointer -> distance = east;
	    if(east < 9000)
	      move_monster(pointer, EAST);
	  }
	  else{
            pointer -> distance = west;
	    if(west < 9000)
	      move_monster(pointer, WEST);
	  }
	}
	else{ // We have 2 vertical options
	  north = distance(position_from_coordinate(pointer -> x, pointer -> y) , pointer -> target);
	  south = distance(position_from_coordinate(pointer -> x, pointer -> y + 23), pointer -> target);
	  if(north <= south){
            pointer -> distance = north;
	    if(north < 9000)
	      move_monster(pointer, NORTH);
	  }
	  else{
            pointer -> distance = south;
	    if(south < 9000)
	      move_monster(pointer, SOUTH);
	  }
	}
      }
      else{ // We have 2 horizontal options

	east = distance(position_from_coordinate(pointer -> x + 23, pointer -> y), pointer -> target);
	west = distance(position_from_coordinate(pointer -> x, pointer -> y), pointer -> target);

	       
	if(east <= west){
          pointer -> distance = east;
	  if(east < 9000)
	    move_monster(pointer, EAST);
	}
	else{
          pointer -> distance = west;
	  if(west < 9000)
	    move_monster(pointer, WEST);
	}
      }
    }
    pointer = pointer -> next;
  } 

  
  return got_target;
}

// This function determines the distance between a tile in our game board
// And a given target (SOUTH or EAST)
int distance(int position, int target){
  int found = 0;
  int i;
  int dist = -1;
  int current_level = 0;
  // Mark the point where we are
  if(position < 0 || game.map[position] != 0)
    return 9000;
  game.map[position] = 0x80l;

  enqueue(position);
  for(i = 0; !found; i ++){

    fflush(NULL);
    position = dequeue();

    if(position == -1){
      dist = 9000;
      found = 1;
      break;
    }
    
    if(current_level == 0){
      dist ++;
      current_level = queue.size;
      
    }
    else
      current_level --;
      
    // We arrived in the end?
    if(target == EAST){
      if((position % (game.board.width + 2)) > game.board.width){
	found = 1;
      }
    }
    else if(position / (game.board.width + 2) >= game.board.height + 1)
      found = 1;
    if(!found){ // We are not in the objective
      // Let's try to enqueue the north
      if(position >= game.board.width + 1 && (game.map[position - game.board.width -2] == 0x0l)){

	enqueue(position - game.board.width - 2);
	game.map[position - game.board.width - 2] = 0x80l;
      }
      // Let's try to enqueue the south
      if(position <= ((game.board.width + 2) * (game.board.height + 2) - 1) && (game.map[position + game.board.width + 2] == 0x0l)){

	enqueue(position + game.board.width + 2);
	game.map[position + game.board.width + 2] = 0x80l;
      }
      // Let's try to enqueue the west
      if(position % (game.board.width + 2) && (game.map[position - 1] == 0x0l)){

	enqueue(position - 1);
	game.map[position - 1] = 0x80l;
      } 
      // Let's try to enqueue the east
      if(position % (game.board.width + 2) < game.board.width + 1 && (game.map[position + 1] == 0x0l)){

	enqueue(position + 1);
	game.map[position + 1] = 0x80l;
      }
    }
  }
  //printf("END\n");
  clean_monster_bit();
  clean_queue();
  return dist;
}



// This function initializes the queue used in
// Breadth-first searches
void initialize_queue(void){
  queue.size = 0;
  queue.first = queue.last = NULL;
}

// This function enqueues a new data in our queue
struct queue_node *enqueue(int data){
  struct queue_node *new_node = (struct queue_node *) malloc(sizeof(struct queue_node));
  if(new_node != NULL){
    if(queue.size == 0){
      queue.first = queue.last = new_node;
      new_node -> previous = new_node -> next = NULL;
      new_node -> value = data;
    }
    else{
      new_node -> value = data;
      new_node -> next = queue.last;
      new_node -> previous = NULL;
      new_node -> next -> previous = new_node;
      queue.last = new_node;
    }
    queue.size ++;
  } 
  return new_node;
}

// This function dequeues a data from our queue
int dequeue(void){
  int ret;
  if(queue.size == 0){
  
    return -1;
  }
  else if(queue.size == 1){
    ret = queue.first -> value;
    free(queue.first);
    queue.first = queue.last = NULL;
  }
  else{ // WARNING: Segmentation fault here!!!
    struct queue_node *pointer;
    pointer = queue.first;
    ret = queue.first -> value;
    queue.first = queue.first -> previous;
    // WARNING: It happens here: 
    queue.first -> next = NULL;
    free(pointer);    
  }
  queue.size --;
 
  return ret;
}

// This function cleans all the content from our queue
void clean_queue(void){
  while(queue.size > 0){
    dequeue();
  }
}

void move_monster(struct monster *pointer, int direction){
  pointer -> direction = direction;
  if(direction == NORTH){
    pointer -> cy -= (int) ((float) pointer -> speed * 100.0) / ((float) game.fps);
    if(pointer -> cy <= -100){
      // Erasing monster
      blit_surface(game.background, window, pointer -> x , pointer -> y - 5, 23, 3, pointer -> x, pointer -> y - 5);
      blit_masked_pixmap(game.background -> pix, game.monsters.images[pointer -> specie] -> mask, 
			 window, pointer -> x, pointer -> y, 23, 23, 
			 pointer -> direction * game.monsters.images[pointer -> specie] -> height, 0, pointer -> x, pointer -> y);
      pointer -> need_redraw = 1;
    }
    pointer -> y += pointer -> cy / 100;
    pointer -> cy %= 100;
  }
  else if(direction == SOUTH){
    pointer -> cy += (int) ((float) pointer -> speed * 100.0) / ((float) game.fps);
    if(pointer -> cy >= 100){
      // Erasing monster
      blit_surface(game.background, window, pointer -> x , pointer -> y - 5, 23, 3, pointer -> x, pointer -> y - 5);
      blit_masked_pixmap(game.background -> pix, game.monsters.images[pointer -> specie] -> mask, 
			 window, pointer -> x, pointer -> y, 23, 23, 
			 pointer -> direction * game.monsters.images[pointer -> specie] -> height, 0, pointer -> x, pointer -> y);
      pointer -> need_redraw = 1;
    }
    pointer -> y += pointer -> cy / 100;
    pointer -> cy %= 100;
  }
  else if(direction == WEST){
    pointer -> cx -= (int) ((float) pointer -> speed * 100.0) / ((float) game.fps);
    if(pointer -> cx <= -100){
      // Erasing monster
      blit_surface(game.background, window, pointer -> x , pointer -> y - 5, 23, 3, pointer -> x, pointer -> y - 5);
      blit_masked_pixmap(game.background -> pix, game.monsters.images[pointer -> specie] -> mask, 
			 window, pointer -> x, pointer -> y, 23, 23, 
			 pointer -> direction * game.monsters.images[pointer -> specie] -> height, 0, pointer -> x, pointer -> y);
      pointer -> need_redraw = 1;
    }
    pointer -> x += pointer -> cx / 100;
    pointer -> cx %= 100;
  }
  else if(direction == EAST){
    pointer -> cx += (int) ((float) pointer -> speed * 100.0) / ((float) game.fps);
    if(pointer -> cx >= 100){
      // Erasing monster
      blit_surface(game.background, window, pointer -> x , pointer -> y - 5, 23, 3, pointer -> x, pointer -> y - 5);
      blit_masked_pixmap(game.background -> pix, game.monsters.images[pointer -> specie] -> mask, 
			 window, pointer -> x, pointer -> y, 23, 23, 
			 pointer -> direction * game.monsters.images[pointer -> specie] -> height, 0, pointer -> x, pointer -> y);
      pointer -> need_redraw = 1;
    }
    pointer -> x += pointer -> cx / 100;
    pointer -> cx %= 100;
  }
  if(pointer -> need_redraw){
    fill_rectangle(pointer -> x, pointer -> y - 5, 23, 3, RED);
    fill_rectangle(pointer -> x, pointer -> y - 5, (int) (23.0 * (pointer -> health / pointer -> max_health)), 3, GREEN);
    blit_surface(game.monsters.images[pointer -> specie], window,
		 pointer -> direction * game.monsters.images[pointer -> specie] -> height, 0,
		 game.monsters.images[pointer -> specie] -> height, game.monsters.images[pointer -> specie] -> height,
		 (int) pointer -> x, (int) pointer -> y);
    pointer -> need_redraw = 0;
  }

}

// This function removes from the game dead monsters or monsters
// that trespassed our screen limits
void clean_monsters(void){
  struct monster *pointer = game.monsters.list;
  struct monster *temp;
  while(pointer != NULL){
    if(pointer -> x > window_width + 23 || pointer -> y > window_height + 23){
      if(pointer -> previous == NULL){ //It's the first
	if(pointer -> next == NULL){ // It's the only one
	  free(pointer);
	  game.monsters.list = NULL;
	  return;
	}
	else{ // It's the first of many
	  pointer = pointer -> next;
	  free(pointer -> previous);
	  pointer -> previous = NULL;
	  game.monsters.list = pointer;
	  continue;  
	}
      }
      else if(pointer -> next != NULL){ // It's in the middle
	temp = pointer;
	pointer = pointer -> previous;
	pointer -> next = temp -> next;
	pointer -> next -> previous = pointer;
	free(temp);
      }
      else{ // The last one
	pointer -> previous -> next = NULL;
	free(pointer);
	return;
      }
    }
    pointer = pointer -> next;
  }
}

// This function returns 1 if exist a monster in a given position
// and returns 0 otherwise
int exist_monster_in_position(int position){
  int x, y;
  struct monster *pointer = game.monsters.list;
  x = game.board.x - 23 + 23 * (position % (game.board.width + 2));
  y = game.board.y - 23 + 23 * (position / (game.board.width + 2));
  while(pointer != NULL){
    if(pointer -> x > x - 22 && pointer -> x < x + 23 + 22 &&
       pointer -> y > y - 23 && pointer -> y < y + 23 + 22)
      return 1;    
    pointer = pointer -> next;
  }
  return 0;
}
