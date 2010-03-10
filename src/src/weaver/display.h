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

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <X11/Xlib.h>

// Defining colors...
#define NOT_IMPORTANT       0x000000
#define BLACK               0x000000
#define NAVY_BLUE           0x000080
#define DARK_BLUE           0x00008b
#define DUKE_BLUE           0x00009c
#define MEDIUM_BLUE         0x0000cd
#define BLUE                0x0000ff
#define RACING_GREEN        0x004225
#define COBALT              0x0047ab 
#define BRANDEIS_BLUE       0x0070ff
#define CERULEAN            0x007ba7
#define AZURE               0x007fff
#define BONDI_BLUE          0x0095b6
#define BLUE_GREEN          0x00dddd
#define GREEN               0x00ff00
#define CYAN                0x00ffff
#define BRIGHT_TURQUOISE    0x08e9de
#define CERULEAN_BLUE       0x2a52be
#define ARSENIC             0x3b444b
#define BISTRE              0x3d2b1f
#define CHARCOAL            0x464646
#define ARMY_GREEN          0x4b5320
#define CAPUT_MORTUUM       0x592720
#define AIR_FORCE_BLUE      0x5d8aa8
#define CORNFLOWER_BLUE     0x6495ed
#define BRIGHT_GREEN        0x66ff00
#define AUBURN              0x6d351a
#define BYZANTIUM           0x702963
#define CAMOUFLAGE_GREEN    0x78866b
#define BOLE                0x79443b
#define CHOCOLATE           0x7b3f00
#define ASPARAGUS           0x7ba05b
#define AQUAMARINE          0x7fffd4
#define BURGUNDY            0x800020
#define CORDOVAN            0x893f45
#define BLUE_VIOLET         0x8a2be2
#define BURNT_UMBER         0x8a3324
#define CARMINE             0x960018
#define BROWN               0x964b00
#define COPPER_ROSE         0x996666
#define CAROLINA_BLUE       0x99badd
#define COLUMBIA_BLUE       0x9bddff
#define CELADON             0xace1af
#define CARNELIAN           0xb31b1b
#define BRASS               0xb5a642
#define COPPER              0xb87333
#define CARDINAL            0xc41e3a
#define BURNT_ORANGE        0xcc5500
#define CHESTNUT            0xcd5c5c
#define BRONZE              0xcd7f32
#define CINNAMON            0xd2691e
#define CRIMSON             0xdc143c
#define CERISE              0xde3163
#define CHARTREUSE          0xdfff00
#define BABY_BLUE           0xe0ffff
#define CINNABAR            0xe34234
#define ALIZARIN            0xe32636
#define AMARANTH            0xe52b50
#define BURNT_SIENNA        0xe97451
#define CARMINE_PINK        0xeb4d42
#define CERISE_PINK         0xec3b83
#define CARROT_ORANGE       0xed9121
#define BUFF                0xf0dc82
#define ALICE_BLUE          0xf0f8ff
#define AMARANTH_PINK       0xf19cbb
#define BEIGE               0xf5f5dc
#define CHAMPAGNE           0xf7e7ce
#define CORAL_PINK          0xf88379
#define BRINK_PINK          0xfb607f
#define APRICOT             0xfbceb1
#define CORN                0xfbec5d
#define RED                 0xff0000
#define BRIGHT_PINK         0xff007f
#define CORAL_RED           0xff4040
#define BRILLIANT_ROSE      0xff55a3
#define CORAL               0xff7f50
#define ATOMIC_TANGERINE    0xff9966
#define CARNATION_PINK      0xffa6c9
#define CHERRY_BLOSSOM_PINK 0xffb7c5
#define AMBER               0xffbf00
#define COSMIC_LATTE        0xfff8e7
#define CREAM               0xfffdd0
#define YELLOW              0xffff00
#define WHITE               0xffffff

// Some useful macros
#define flush() XFlush(_dpy)
#define fill_screen(x) fill_rectangle(0, 0, window_width, window_height, x)

typedef struct surface{
  Pixmap pix;
  int width;
  int height;
  Pixmap mask;
} surface;

Display *_dpy;
Window _w;
int window_width, window_height;
GC _gc, _mask_gc;
int _screen, _depth;
Visual *_visual;
struct surface *window;
unsigned long transparent_color;

struct surface *new_surface(int, int);
void destroy_surface(struct surface *);
void blit_surface(struct surface *, struct surface *, int, int, int, int, int, int);

void initialize_screen(void);
void hide_cursor(void);
void draw_point(unsigned, unsigned, unsigned);
void draw_line(unsigned, unsigned, unsigned, unsigned, unsigned);
void draw_circle(unsigned, unsigned, unsigned, unsigned);
void fill_circle(unsigned, unsigned, unsigned, unsigned);
void draw_rectangle(unsigned, unsigned, unsigned, unsigned, unsigned);
void fill_rectangle(unsigned, unsigned, unsigned, unsigned, unsigned);
void draw_ellipse(unsigned, unsigned, unsigned, unsigned, unsigned);
void fill_ellipse(unsigned, unsigned, unsigned, unsigned, unsigned);

void draw_text(unsigned, unsigned, char *, char *, unsigned);

void draw_mask(struct surface *, int , int , int , int );

void blit_masked_pixmap(Pixmap, Pixmap, struct surface *, int , 
			int, int, int, int, 
			int, int, int);
void apply_texture(struct surface *, struct surface *);

#endif
