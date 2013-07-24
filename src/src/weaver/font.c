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

#include "font.h"
#include "display.h"
#include <X11/Xutil.h>

FT_Library _library;
FT_Face _face;
int _dpi_h, _dpi_v;
int letter_spacing, word_spacing, line_spacing;

// Initializes the FreeType Library
int _initialize_font(void){
  letter_spacing = 0;
  word_spacing = 0;
  line_spacing = 0;
  int error = 0;
  error = FT_Init_FreeType(&_library);
  if(error){
    printf("WARNING: FreeType Library couldn't be initialized.\n");
    return 0;
  }
  _face = NULL;

  /* Getting the DPI */
  _dpi_h = (int) (((((double) window_width * 25.4) /
		    ((double) DisplayWidthMM(_dpy, DefaultScreen(_dpy))))) +
		  0.5);
  _dpi_v = (int) (((((double) window_height * 25.4) /
		    ((double) DisplayHeightMM(_dpy, DefaultScreen(_dpy))))) +
		  0.5);
  return 1;
}

// Loads a font face number 'index' from the 'file' file
int load_font(char *file){
  int error;
  FILE *fp;
  char *path = (char *) malloc(strlen(file)+50);
  path[0] = '\0';

  // Opening file passed as argument
  strcat(path, "/usr/share/games/DUMMY/fonts/");
  strcat(path, file);
  fp = fopen(path, "rb");
  if(!fp){
    path[0] = '\0';
    strcat(path, "fonts/");
    strcat(path, file);
    fp = fopen(path, "rb");
    if(!fp){
      printf("WARNING: The font %s was not found.\n", file);
      free(path);
      return 0;
    }
  }
  fclose(fp);

  // Loading font
  error = FT_New_Face(_library, path, 0, &_face);
  if (error == FT_Err_Unknown_File_Format){
    printf("WARNING: The font %s has a format not supported.\n", file);
    free(path);
    return 0;
  }
  else if (error) {
    printf("WARNING: The font %s appears to be broken.\n", file);
    free(path);
    return 0;
  }
  free(path);
  return 1;
}

unsigned long _unicode_index(char **p){
  unsigned long r;
  if((**p >> 5) == -2){ // 2-byte character
    r = ((**p & 0x1f) << 6) + (*(*p + 1) & 0x3f);
    *p += 1;
    return r;
  }
  return 0;
}

int draw_text(char *text, surface *dst, unsigned x, unsigned y, int size,
	      unsigned color){
  int error, lines, bit, pow, glyph_index = 0, line_surface_x = 0, initial_x = x,
    initial_y = y, last_char_index = 0;
  char *p, *last_drawn_char = text, *data = NULL;
  XImage *ximage = NULL; // Buffer, stores drawn characters in the client
  surface *line_surface = NULL; // Buffer, stores drawn words in the server
  FT_Bool use_kerning = FT_HAS_KERNING(_face);

  if(_face == NULL){
    fprintf(stderr, "WARNING: Trying to write, but no font was selected.\n");
    return 0;
  }

  // Setting font size
  error = FT_Set_Char_Size(_face, 0, size * 64, _dpi_h, _dpi_v);
  if(error){
    fprintf(stderr, "WARNING: Error while setting size in font.\n");
    _face = NULL;
    return 0;
  }

  // 'line_surface' is a buffer able to store each word before drawn it.
  line_surface = new_surface(dst -> width,
			     _face -> size -> metrics.height  >> 6);
  if(line_surface == NULL){
    fprintf(stderr, "WARNING: Unable to create surface to hold text.\n");
    return 0;
  }
  fill_surface(line_surface, color);
  draw_rectangle_mask(line_surface, 0, 0, line_surface -> width,
		      line_surface -> height);

  // Drawing each character in this loop
  for(p = text; *p != '\0'; p++){
    // retrieve glyph index from character code
    last_char_index = glyph_index;
    glyph_index = FT_Get_Char_Index(_face, *p);
    // Treat special unicode characters
    if(glyph_index == 0 && *p < 0){
      glyph_index = FT_Get_Char_Index(_face,  _unicode_index(&p));
    }
    // load glyph image into the slot (erase previous one)
    error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_DEFAULT);
    if(error) // If we can't handle this character, go to the next
      continue;
    // Render the character with the selected font, this creates the bitmap
    error = FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_MONO);
    if(error)
      continue;

    // Spaces and line breaks are handled here (and words are drawn here)
    // When a word is too big to fit in the destiny, it's also splitted here
    if(_face->glyph->bitmap.width <= 0 || _face->glyph->bitmap.rows <= 0 ||
       *p == '\n' ||
       line_surface_x + _face->glyph->bitmap.width > dst -> width){
      if(x + line_surface_x > dst -> width || *p == '\n'){
	x = 0;
	y += line_surface -> height + line_spacing;
	if(line_surface_x + _face->glyph->bitmap.width > dst -> width)
	  p --;
	if(y + line_surface -> height > dst -> height){
	  destroy_surface(line_surface);
	  return (- (last_drawn_char - text + 1) / sizeof(char));
	}
      }
      blit_surface(line_surface, dst, 0, 0, line_surface_x, line_surface -> height,
		   x, y - (_face->size->metrics.ascender >> 6));
      last_drawn_char = p;
      x += line_surface_x + letter_spacing + word_spacing +
	(_face -> glyph -> advance.x >> 6);
      line_surface_x = 0;
      draw_rectangle_mask(line_surface, 0, 0, line_surface -> width,
			  line_surface -> height);
      continue;
    }

    // Kerning
    if(use_kerning && last_char_index && glyph_index){
      FT_Vector delta;
      FT_Get_Kerning(_face, last_char_index, glyph_index, FT_KERNING_DEFAULT,
		     &delta);
      line_surface_x += delta.x >> 6;
    }
    // Creating the XImage used to render each individual character
    data = (char *) malloc(_face->glyph->bitmap.width *
			   _face->glyph->bitmap.rows * 4);
    ximage = XCreateImage(_dpy, _visual, 1, XYBitmap, 0, data,
			  _face->glyph->bitmap.width, _face->glyph->bitmap.rows,
			  8, 0);
    if(!ximage){
      fprintf(stderr, "ERROR: XCreateImage() failed!\n");
      exit(1);
    }
    XInitImage(ximage);
    // Drawing the character in the XImage structure
    for(lines = 0;
	lines < _face->glyph->bitmap.pitch * _face->glyph->bitmap.rows;
	lines += _face->glyph->bitmap.pitch){
      for(bit = 0, pow = 1; bit < _face->glyph->bitmap.width; bit ++, pow *= 2){
	if(pow == 256)
	  pow = 1;
	XPutPixel(ximage, bit, lines / _face->glyph->bitmap.pitch, BLACK);
	if((_face->glyph->bitmap.buffer[lines + bit / 8] / (128/pow)) % 2)
	  XPutPixel(ximage, bit, lines / _face->glyph->bitmap.pitch, WHITE);
      }
    }
    // Transfering the XImage character to the buffer 'line_buffer'
    XSetForeground(_dpy, _mask_gc, ~0l);
    XSetBackground(_dpy, _mask_gc, 0l);
    XPutImage(_dpy, line_surface -> mask, _mask_gc, ximage, 0, 0,
	      line_surface_x,
	      (_face->size->metrics.ascender -
	      _face->glyph->metrics.horiBearingY) >> 6,
	      line_surface -> width, line_surface -> height);
    line_surface_x += (letter_spacing + (_face -> glyph -> advance.x >> 6));
    XDestroyImage(ximage);
  }
  // Finished loop, we just need to draw the last word stored in the buffer:
  if(x + line_surface_x > dst -> width){
    x = 0;
    y += line_surface -> height + line_spacing;
    if(y  + line_surface -> height > dst -> height){
      destroy_surface(line_surface);
      return (- (last_drawn_char - text + 1) / sizeof(char));
    }
  }
  blit_surface(line_surface, dst, 0, 0, line_surface_x, line_surface -> height,
	       x, y - (_face->size->metrics.ascender >> 6));
  destroy_surface(line_surface);
  return dst -> width * (y - initial_y) + (x + line_surface_x - initial_x);
}
