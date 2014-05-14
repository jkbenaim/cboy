/*************************************************************************
 *   Cboy, a Game Boy emulator
 *   Copyright (C) 2014 jkbenaim
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

#include <stdio.h>
#include "../source/types.h"
#include "../source/cartdesc.h"

#define READSIZE 0x200

int main( int argc, char* argv[] )
{
  if( argc < 2 )
  {
    fprintf( stderr, "need an argument\n" );
    return 1;
  }
  
  FILE *file;
  
  file = fopen( argv[1], "r" );
  
  if( file == NULL )
  {
    fprintf( stderr, "Error opening file: %s\n", argv[1] );
    return 1;
  }
  
  u8 cart[READSIZE];
  int i=0, byte;
  do
  {
    byte = fgetc(file);
    cart[i] = (u8)byte;
    ++i;
  } while( i<READSIZE && byte != EOF );
  if(byte == EOF) --i;
  fclose(file);
  
  if( i < READSIZE )
  {
    fprintf( stderr, "Error reading file: %s\n", argv[1] );
    fprintf( stderr, "File too small.\n" );
    return 1;
  }
  
  char title[0x11];
  for(i=0;i<0x10;++i)
    title[i] = cart[i + 0x134];
  title[0x10] = '\x00';
  printf("Title: %s\n", title);
  
  u8 cgb_flag = cart[0x143];
  switch( cgb_flag )
  {
    case 0x80:
      printf("CGB compatible\n");
      break;
    case 0xC0:
      printf("CGB required\n");
      break;
    default:
      printf("Not CGB compatible\n");
      break;
  }
  
  u8 sgb_flag = cart[0x146];
  switch( sgb_flag )
  {
    case 0x00:
      printf("No SGB functions\n");
      break;
    case 0x03:
      printf("SGB enhanced\n");
      break;
  }
  
  u8 cart_type = cart[0x147];
  printf("Cart type: %02Xh - %s\n", cart_type, cartdesc_carttype[cart_type] );
  
  u8 rom_size = cart[0x148];
  printf("ROM size: %02Xh - %s\n", rom_size, cartdesc_romsize[rom_size] );
  
  u8 ram_size = cart[0x149];
  printf("RAM size: %02Xh - %s\n", ram_size, cartdesc_ramsize[ram_size] );
  
  u8 version = cart[0x14C];
  printf("Mask ROM version number: %02Xh\n", version);

  return 0;
}
