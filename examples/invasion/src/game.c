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

#include <sys/time.h>
#include "weaver/weaver.h"
#include "game.h"

void print_board(void){
  { int i, j;
    for(i = 0; i < game.board.height + 2; i ++){
      for(j = 0; j < game.board.width + 2; j++){
	printf("%d ", game.map[(game.board.width + 2) * i + j]);
      }
      printf("\n");
    }}
}

void monster_attack(void){  
  new_monster(NORMAL, game.board.x + 230, -23, SOUTH);
  new_monster(NORMAL, game.board.x + 253, -90, SOUTH);
  new_monster(NORMAL, game.board.x + 276, -69, SOUTH);
  new_monster(NORMAL, game.board.x + 322, -23, SOUTH);
  new_monster(NORMAL, game.board.x + 207, -69, SOUTH);
  new_monster(NORMAL, game.board.x + 368, -100, SOUTH);
  new_monster(NORMAL, game.board.x + 345, -140, SOUTH);
  new_monster(NORMAL, game.board.x + 299, -80, SOUTH);

  new_monster(NORMAL, -50, game.board.y + 184, EAST);
  new_monster(NORMAL, -23, game.board.y + 207, EAST);
  new_monster(NORMAL, 0, game.board.y + 253, EAST);
  new_monster(NORMAL, -23, game.board.y + 299, EAST);
  new_monster(NORMAL, -46, game.board.y + 230, EAST);
  new_monster(NORMAL, -46, game.board.y + 276, EAST);
}

int main(int argc, char **argv){
  struct surface *background, *wall, *hourglass, *go_away, *number/*, *coin*/;
  int i, j, k;
  int pause = 1;
  int current_time, last_monster_attack;

  awake_the_weaver();

  initialize_game();
  background = new_image("grass.png");
  go_away = new_image("exit.png");
  apply_texture(background, window);
  apply_texture(background, game.background);
  
  
  wall = new_image("wall.png");
  hourglass = new_image("hourglass.png");
  number = new_image("numbers.png");
  
  // Drawing wall
  // First the top wall
  game.map[0] = 1;
  for(i = game.board.x - 23, j = 0; i < window_width; i += 23, j ++)
    if(!(1l << (j - 1) & game.top_holes) || j > game.board.width){
      if(j <= game.board.width + 1)
        game.map[j] = 1;
      blit_surface(wall, window, 0, 0, 23, wall -> height, i, game.board.y - wall -> height);
    }
  blit_surface(game.background, window, game.board.x - 40, game.board.y - 46, 23, 46,  game.board.x - 40, game.board.y - 46);
  // Now the right wall
  for(i = game.board.y - 23, j = 0; i < game.board.y + game.board.height * 23 + 23; i += 23, j ++)
    if(!(1l << (j - 1)  & game.right_targets)){
      if(!(1l << j  & game.right_targets))
	blit_surface(wall, window, 33, 0, 23, wall -> height, game.board.x + game.board.width * 23, i);
      game.map[(game.board.width + 2) * j + game.board.width + 1] = 1;
    }
  // The down wall
  for(i = game.board.x -23, j = 0; i < (game.board.x + game.board.width * 23) + 23; i += 23, j ++)
    if(!(1l << (j - 1) & game.down_targets)){
      game.map[(game.board.width + 2) * (game.board.height + 1)  + j] = 1;
      blit_surface(wall, window, 0, 0, 23, wall -> height, i, game.board.y + game.board.height * 23);
    }
  blit_surface(game.background, window, game.board.x - 40, game.board.y + game.board.height * 23, 
               23, 46,  game.board.x - 40, game.board.y + game.board.height * 23);
  blit_surface(game.background, window, 17 + game.board.x + game.board.width * 23, game.board.y + game.board.height * 23, 
               23, 46, 17 + game.board.x + game.board.width * 23, game.board.y + game.board.height * 23);
  // And the left wall
  for(i = game.board.y - 23, j = 0; i < window_height; i += 23, j ++){
    if(!(1l << (j - 1)  & game.left_holes) || j > game.board.height){
      if(j <= game.board.height + 1)
        game.map[j * (game.board.width + 2)] = 1;
      if(!(1l << j  & game.left_holes))
	blit_surface(wall, window, 30, 0, 23, wall -> height, game.board.x - 19, i);
    }
    else;
  }
  
   draw_city();
  draw_trees();

  blit_surface(go_away, window, 0, 0, go_away -> width, go_away -> height, window_width - go_away -> width, 0);
  blit_surface(hourglass, window, 0, 0, hourglass -> width, hourglass -> height, 
	       window_width - go_away -> width - hourglass -> width - 48 * 3, 0);

 
  draw_money(number);

  i = time(NULL);
  j = time(NULL);

 
  k = 0;

  last_monster_attack = 0;
  
  
  // Beginning of the main loop
  for(;;){

    get_input();
    if(mouse.pressed){ 
      if(mouse.x > window_width - go_away -> width && mouse.y < go_away -> height)
	exit(0); // If we click in the exit icon
      else if(mouse.x > window_width - go_away -> width - hourglass -> width - 48 * 3 &&
	      mouse.x < window_width - go_away -> width - hourglass -> width - 48 * 2 &&
	      mouse.y < 48 && k == 0){ // If we click in the hourglass
	pause *= -1;
	k = 1;
      }
      else if(k == 0){ // Mouse pressed and we are not exiting
	k = 1;
	if(mouse.x >= game.board.x && mouse.x < game.board.x + game.board.width * 23 &&
	   mouse.y >= game.board.y && mouse.y < game.board.y + game.board.height * 23){
	  int position, x, y;
	  position = position_from_coordinate(mouse.x, mouse.y);
	  x = game.board.x - 23 + 23 * (position % (game.board.width + 2));
	  y = game.board.y - 23 + 23 * (position / (game.board.width + 2));
	  if(! exist_monster_in_position(position)){
	    if(game.map[position] == 0x0l){	      
	      game.map[position] = 0x1l;
	      blit_surface(wall, window, 0, 20, 23, 23, x, y);
	    }
	    else{
	      game.map[position] = 0x0l;
	      blit_surface(game.background, window, x, y, 23, 23, x, y);
	    }
	  }
	}
      }
    }
    else if(!mouse.pressed){
      if(k == 1)
	k = 0;
    }

    current_time = j = time(NULL);
    if(pause > 0 && j - i > 0){
      game.player.value += 1;
      i = j;
      draw_money(number);
    }
    if(current_time - last_monster_attack >= 30){
      monster_attack();
      last_monster_attack = time(NULL);
    }
   
    if(pause > 0)
      move_monsters();
	

    clean_monsters();
    weaver_rest(10000000);
    game.fps = fps;
  
  }
  return 1;
}


// This functions resets the game parameters to the default values
void initialize_game(void){
  int i;
  initialize_queue();
  game.board.width = 26;
  game.board.height = 22;
  game.board.x = (window_width - game.board.width * 23) / 2;
  game.board.y = (window_height - game.board.height * 23) / 2;
  game.type = DEFAULT;
  game.top_holes = 255l << 9;
  game.down_targets = game.top_holes;
  game.left_holes = 63l << 8;
  game.right_targets = game.left_holes;
  game.background = new_surface(window_width, window_height);
  game.monsters.images[NORMAL] = new_image("normal.png");
  game.monsters.list = NULL;
  game.level = 1;
  game.map = (unsigned *) malloc(sizeof(int) * (game.board.width + 2) * (game.board.height + 2));
  for(i = 0; i < (game.board.width + 2) * (game.board.height + 2); i ++)
    game.map[i] = 0;
  game.fps = 50;
  game.player.value = 0;
  game.player.life = game.player.life_max = 15;
}

void clean_monster_bit(void){
  int i;
  for(i = 0; i < (game.board.width + 2) * (game.board.height + 2); i ++)
    game.map[i] &= ~0x80l;
}

// This function draws the city
void draw_city(void){
  int i, j;
  int num;
  struct surface *houses;
  houses = new_image("house.png");
  // First the right houses
  for(i = game.board.x + game.board.width * 23 + 23; i < window_width; i += 46)
    for(j = game.board.y; j < game.board.y + game.board.height *23 + 46; j +=46)
      if((!(1l << ((j - game.board.y) / 23)  & game.right_targets)) || j > game.board.y + game.board.height * 23)
        if(!(rand() % 4)){
          num = rand() % NUMBER_OF_HOUSES * 46;
          blit_surface(houses, window, num, 0, 46, 46, i, j);
          blit_surface(houses, game.background, num, 0, 46, 46, i, j);
        }
  // Now the south houses
  for(i = game.board.x; i < window_width; i += 46)
    for(j = game.board.y + game.board.height * 23 + 23; j < window_height; j +=46)
      if(((!(1l << ((i - game.board.x) / 23)  & game.down_targets)) && !(1l << ((i - game.board.x + 23) / 23)  & game.down_targets)) || i > game.board.x + game.board.width * 23)
        if(!(rand() % 4)){
          num = (rand() % NUMBER_OF_HOUSES) * 46;
          blit_surface(houses, window, num, 0, 46, 46, i, j);
          blit_surface(houses, game.background, num, 0, 46, 46, i, j);
        }
}

// This function draws the forest
void draw_trees(void){
  struct surface *tree = new_image("tree.png");
  int i, j;

  // First the top trees
  for(i = game.board.x; i < window_width; i += tree -> width)
    for(j = 0; j < game.board.y - 69; j += 23)
      if((!(1l << ((i - game.board.x) / 23)  & game.top_holes)) || i > game.board.x + game.board.width * 23) 
        if(!(rand() % 3)){
          blit_surface(tree, window, 0, 0, tree -> width, tree -> height, i, j);
	  blit_surface(tree, game.background, 0, 0, tree -> width, tree -> height, i, j);
	}
  // Now the left trees.
  for(i = 0; i < game.board.x - 23; i += tree -> width)
    for(j = 0; j < window_height; j += 23)
      if((!(1l << ((j - game.board.y) / 23)  & (game.left_holes | game.left_holes >> 2))) || j > game.board.y + game.board.height * 23)
        if(!(rand() % 3)){
          blit_surface(tree, window, 0, 0, tree -> width, tree -> height, i, j);
	  blit_surface(tree, game.background, 0, 0, tree -> width, tree -> height, i, j);
	}
}

// This updates the ammount of money that you have
void draw_money(struct surface *my_surf){
  blit_surface(game.background, window, window_width - 30 - 146, 0, 130, 46,  window_width - 30 - 146, 0);
  blit_surface(my_surf, window, (game.player.value % 10) * 46, 0, 46, 46, window_width - 46 - 46, 0);
  blit_surface(my_surf, window, (game.player.value / 10 % 10) * 46, 0, 46, 46, window_width - 46 - 80, 0);
  blit_surface(my_surf, window, (game.player.value / 100 % 10) * 46, 0, 46, 46, window_width - 46 - 113, 0);
  blit_surface(my_surf, window, (game.player.value / 1000 % 10) * 46, 0, 46, 46, window_width - 46 - 146, 0);
}

// And this updates the ammount of life
void draw_life(struct surface *my_surf){
  if(game.player.life < 0)
    game.player.life = 0;
  struct surface *heart = new_image("heart.png");
  blit_surface(game.background, window, window_width - 46 - 7*46, 0, 46 * 3, 46, window_width - 46 - 7*46, 0);
  blit_surface(heart, window, 46, 0, 46, 46, window_width - 46 - 7*46, 0);
  draw_mask(heart, 0, 0, heart -> width, 46 - (int) (((float) game.player.life/ (float) game.player.life_max) * (float) heart -> height));
  blit_surface(heart, window, 0, 0, 46, 46, window_width - 46 - 7*46, 0);
  blit_surface(my_surf, window, (game.player.life % 10) * 46, 0, 46, 46, window_width - 46 - 5*46 - 10, 0);
  blit_surface(my_surf, window, ((game.player.life / 10) % 10) * 46, 0, 46, 46, window_width - 46 - 6*46, 0);
  destroy_surface(heart);
}


// This function receives a coordinate (x, y) and returns the position
// in the game.map where is this coordinate
int position_from_coordinate(int x, int y){

  x -= game.board.x - 23;
  y -= game.board.y - 23;
  
  x /= 23;
  y /= 23;
  if(x < 0 || y < 0 || x > game.board.width + 1 || y > game.board.height + 1)
    return -9000;
  return x + (game.board.width + 2) * y;
}
