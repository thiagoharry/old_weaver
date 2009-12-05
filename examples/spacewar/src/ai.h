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

#ifndef _IA_H_
#define _IA_H_


// Artificial intelligence moves
#define NONE    0 // Do nothing
#define MOVE_L  1 // Turn left
#define MOVE_R  2 // Turn right
#define PROP    4 // Use propulsion
#define SHOT    8 // Shot!
#define HYPER  16 // Go to hyperspace

// Artificial intelligence status
#define START       0
#define START_MOVE1 1
#define START_MOVE2 2
#define START_MOVE3 3
#define FIGHTING    4

// A speedconsidered good for the AI
#define GOOD_SPEED 50.0

int ai_status;

void initialize_ai(void);
int ai_decision(void);
float distance(float, float, float, float);

#endif
