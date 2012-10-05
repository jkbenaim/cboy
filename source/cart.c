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

#include <stdio.h>
#include <sys/stat.h>
#include "cart.h"
#include "memory.h"
#include <stdlib.h> // for exit
#include "mbc.h"
#include "cartdesc.h"
#include "cpu.h"
#include <string.h>

struct cart_s cart;

char* romname2savename( char* savename, char* romname, int savenamelength )
{
  if( romname == NULL )
  {
    // uhhhh
    return NULL;
  }
  
  char* suffix = strrchr( romname, (int)'.' );
  
  if( suffix == NULL )
  {
    // romname doesn't have a dot
    
    // copy the romname to savename
    strncpy( savename, romname, savenamelength - 4 );
    
    // append ".sav" to the savename
    strcat( savename, ".sav" );
  }
  else
  {
    // romname has a dot and suffix
    
    int prefixlength = strlen( romname ) - strlen( suffix );
    
    // copy the romname (up to the dot) to savename
    strncpy( savename, romname, prefixlength );
    
    // null-terminate it
    savename[prefixlength] = '\0';
    
    // append ".sav" to the savename
    strcat( savename, ".sav" );
  }
  
  return savename;
}

void cart_init( char* bootromName, char* cartromName ) {
  
  cart.cleanup = &cart_default_cleanup;
  
  cart_init_cartrom( cartromName );
  cart_init_bootrom( bootromName );
  
  cart.mbc_type = cart.cartrom[0x147];
  
  cart_reset_mbc();
  
}

void cart_init_cartrom( char* cartromName )
{
  struct stat sb;
  FILE *fd;
  // Load the cart rom.
  // First, find its filesize.
  if( stat( cartromName, &sb) == -1 )
  {
    fprintf( stderr, "Couldn't find cart rom: %s.\n", cartromName );
    exit(1);
  }
  
  if( sb.st_size > MAX_CARTROM_SIZE )
    cart.cartromsize = MAX_CARTROM_SIZE;
  else
    cart.cartromsize = sb.st_size;
  
  // Allocate memory for the cartrom.
  if( (cart.cartrom = (u8 *)malloc(cart.cartromsize)) == NULL )
  {
    fprintf( stderr, "Cart rom malloc failed.\n" );
    exit(1);
  }
  
  // Read the cartrom.
  //printf( "Loading cart rom: %s\n", cartromName );
  fd = fopen( cartromName, "r" );
  if( fread( cart.cartrom, cart.cartromsize, 1, fd ) != 1 )
  {
    fprintf( stderr, "Reading cart rom failed.\n" );
    exit(1);
  }
  fclose( fd );
  //printf( "Cart rom: %d bytes read.\n", cart.cartromsize );
  
  cart.cartrom_num_banks = cart.cartromsize / 16384;
  if( cart.cartromsize % 16384 != 0 )
  {
    printf("Warning: cart rom not a multiple of 16384 bytes\n");
    cart.cartrom_num_banks++;
  }
  
  // Set savename
  romname2savename( cart.savename, cartromName, 256 );
}

void cart_init_bootrom( char* bootromName )
{
  struct stat sb;
  FILE *fd;
  // Load the boot rom.
  // First, find its filesize.
  if( stat( bootromName, &sb) == -1 )
  {
    fprintf( stderr, "Couldn't find boot rom: %s.\n", bootromName );
    exit(1);
  }
  
  if( sb.st_size > MAX_BOOTROM_SIZE )
    cart.bootromsize = MAX_BOOTROM_SIZE;
  else
    cart.bootromsize = sb.st_size;
  
  // Allocate memory for the bootrom.
  if( (cart.bootrom = (u8 *)malloc(cart.bootromsize)) == NULL )
  {
    fprintf( stderr, "Boot rom malloc failed.\n" );
    exit(1);
  }
  
  // Read the bootrom.
  //printf( "Loading boot rom: %s\n", bootromName );
  fd = fopen( bootromName, "r" );
  if( fread( cart.bootrom, cart.bootromsize, 1, fd ) != 1 )
  {
    fprintf( stderr, "Reading boot rom failed.\n" );
    exit(1);
  }
  fclose( fd );
  //printf( "Boot rom: %d bytes read.\n", cart.bootromsize );
}

/*
 * void cart_reset_mbc( void )
 * This function installs the appropriate handlers
 * for the current MBC.
 */
void cart_reset_mbc()
{
  if( cart.cleanup != NULL )
    cart.cleanup();
  
  if( state.bootRomEnabled )
  {
    printf( "MBC reset: boot rom\n" );
    mbc_boot_install();
    return;
  }
  
  switch( cart.mbc_type )
  {
    case 0x00:  // ROM ONLY
      mbc_none_install();
      break;
    case 0x01:  // MBC1
    case 0x02:  // MBC1+RAM
    case 0x03:  // MBC1+RAM+BATTERY
      mbc_mbc1_install();
      break;
    case 0x05:  // MBC2
    case 0x06:  // MBC2+BATTERY
      mbc_mbc2_install();
      break;
    case 0x0F:	// MBC3+TIMER+BATTERY
    case 0x10:	// MBC3+TIMERY+RAM+BATTERY
    case 0x11:	// MBC3
    case 0x12:	// MBC3+RAM
    case 0x13:	// MBC3+RAM+BATTERY
      mbc_mbc3_install();
      break;
    case 0x19:  // MBC5
    case 0x1A:  // MBC5+RAM
    case 0x1B:  // MBC5+RAM+BATTERY
    case 0x1C:  // MBC5+RUMBLE
    case 0x1D:  // MBC5+RUMBLE+RAM
    case 0x1E:  // MBC5+RUMBLE+RAM+BATTERY
    case 0x22:
      mbc_mbc5_install();
      break;
    case 0xFC:	// POCKET CAMERA
      mbc_cam_install();
      break;
    default:
      // danger danger
      printf( "MBC reset: Unhandled cart type: %02Xh %s\n", cart.mbc_type, cartdesc_carttype[cart.mbc_type] );
      exit(1);
      break;
  }
  
  printf( "MBC reset: %02Xh %s\n", cart.mbc_type, cartdesc_carttype[cart.mbc_type] );
}
  
void cart_cleanup()
{
  cart.cleanup();
  free( cart.cartrom );
  free( cart.bootrom );
}

void cart_default_cleanup()
{
  // do nothing
}
