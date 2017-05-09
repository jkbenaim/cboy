/*************************************************************************
 *   Cboy, a Game Boy emulator
 *   Copyright (C) 2012 jkbenaim
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#include <stdlib.h>
#include "types.h"
#include "input.h"
#include "cpu.h"
#include "memory.h"
#include "video.h"
#include "cart.h"
#include "main.h"
#include "audio.h"

int stop = 0;
int pause = 0;

#ifdef USE_SDL
int main ( int argc, char* argv[] ) {
  printf( " ----------------------- \n" );
  printf( " --- Welcome to cboy --- \n" );
  printf( " ---  (c) 2012-2016  --- \n" );
  printf( " ----------------------- \n" );
  
  if(argc < 3)
  {
    fprintf(stderr, "Usage: cboy boot.bin game.gb\n");
    fprintf(stderr, "If you don't have a bootrom, use quickboot.bin.\n");
    exit(1);
  }
  
  mem_init();
//   audio_init();
  if( ! cpu_init() )
  {
    fprintf( stderr, "cpu init failed\n" );
    return 1;
  }
  cart_init( argv[1], argv[2] );
  vid_init();
  input_init();
  while(!stop)
  {
    vid_waitForNextFrame();
    input_handle();
    if(!pause)
    {
      cpu_do_one_frame();
      vid_frame();
    }
  }
  
  cart_cleanup();
//   audio_cleanup();
  
  return 0;
}
#endif // USE_SDL