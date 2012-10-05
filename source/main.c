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

#ifdef ARM9
#include <nds.h>
#include <fat.h>
#endif // ARM9

#include <stdlib.h>
#include "types.h"
#include "input.h"
#include "cpu.h"
#include "memory.h"
#include "video.h"
#include "cart.h"
#include "main.h"

int stop = 0;


#ifdef USE_SDL
int main ( int argc, char* argv[] ) {
  printf( " ----------------------- \n" );
  printf( " --- Welcome to cboy --- \n" );
  printf( " ---    (c) 2012     --- \n" );
  printf( " ----------------------- \n" );
  
  if(argc < 3)
  {
    fprintf(stderr, "Usage: cboy boot.bin game.gb\n");
    fprintf(stderr, "If you don't have a bootrom, use quickboot.bin.\n");
    exit(1);
  }
  
  mem_init();
  cpu_init();
  cart_init( argv[1], argv[2] );
  vid_init();
  input_init();
  while(!stop)
  {
    vid_waitForNextFrame();
    input_handle();
    cpu_do_one_frame();
    vid_frame();
  }
  
  cart_cleanup();
  
  return 0;
}
#endif // USE_SDL

#ifdef ARM9
int main ( void ) {
//set the mode for 2 text layers and two extended background layers
        videoSetMode(MODE_5_2D); 

        //set the first two banks as background memory and the third as sub background memory
        //D is not used..if you need a bigger background then you will need to map
        //more vram banks consecutivly (VRAM A-D are all 0x20000 bytes in size)
        vramSetPrimaryBanks(    VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000, 
                VRAM_C_SUB_BG , VRAM_D_LCD); 
        
        

        consoleDemoInit();

//      iprintf("\n\n\tHello DS devers\n");
//      iprintf("\twww.drunkencoders.com\n");
//      iprintf("\t16 bit bitmap demo");

        // set up our bitmap background
        bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
        u16* backBuffer = (u16*)bgGetGfxPtr(bg)/* + 256*256*/;


  iprintf( " ----------------------- \n" );
  iprintf( " --- Welcome to cboy --- \n" );
  iprintf( " ---    (c) 2012     --- \n" );
  iprintf( " ----------------------- \n" );
  
  // init libfat
  if( !fatInitDefault() )
  {
    iprintf("FAT init failed.\n");
    for(;;){}
  }
  else
  {
    iprintf("FAT init succeded.\n");
  }
  
  mem_init();
  cpu_init();
  cart_init( "gb/cgb_rom.bin", "gb/pkmnsilv.gbc" );
  vid_init();
  input_init();
  int frames=0;
  while(!stop)
  {
    input_handle();
    cpu_do_one_frame();
    vid_frame();
    iprintf(".");
  }
  
  cart_cleanup();
  
  return 0;
}
#endif // ARM9