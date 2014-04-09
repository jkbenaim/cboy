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

#include "cart_chardev.h"
#include "cart.h"
#include "main.h"
#include <stdlib.h> // for exit
#include "mbc.h"
#include "cpu.h"
#include "memory.h"
#include "cartdesc.h"
#include <stdio.h> // for FILE

void cart_init_chardev( char* bootromName, char* cartromName )
{
  // Init bootrom
  cart_init_bootrom( bootromName );
  
  // Init cartrom
  // Allocate memory for the cartrom.
  if( (cart.cartrom = (u8 *)malloc(8*1024*1024)) == NULL )
  {
    fprintf( stderr, "Cart rom malloc failed.\n" );
    exit(1);
  }
  if( (cart.cartromValid = (u8 *)malloc(8*1024*1024)) == NULL )
  {
    fprintf( stderr, "Cart rom malloc failed.\n" );
    exit(1);
  }
  printf("hello\n");
  cart.cleanup = &cart_chardev_default_cleanup;
  cart.cart_bank_num = 1;
  
  // Bring up the device.
  cart_chardev_bringup_device( cartromName );
  
  // Reset the MBC. This lets us do reads.
  cart_c_reset_mbc();
  
  // set the MBC type for later use
  address = 0x147;
  READ_BYTE();
  cart.mbc_type = memByte;
  
  // determine the emulated extram size
  address = 0x149;
  READ_BYTE();
  switch( memByte )
  {
    case 0:
      cart.extram_size = 0;
      break;
    case 1:
      cart.extram_size = 2048;
      break;
    case 2:
      cart.extram_size = 8192;
      break;
    case 3:
      cart.extram_size = 32768;
      break;
    case 4:
      cart.extram_size = 131072;
      break;
  }
  // exception: MBC2 and MBC7 always have extram
  switch( cart.mbc_type )
  {
    case 0x05:  // MBC2
    case 0x06:  // MBC2+BATTERY
      cart.extram_size = 512;
      break;
    case 0x22: // MBC7+?
      cart.extram_size = 32768;
      break;
  }
  
  // allocate memory for extram
  if( (cart.extram = (u8 *)malloc(cart.extram_size)) == NULL )
  {
    fprintf( stderr, "Extram malloc failed.\n" );
    exit(1);
  }
  
  // allocate memory for extram cache (read)
  if( (cart.extramValidRead = (u8 *)malloc(cart.extram_size)) == NULL )
  {
    fprintf( stderr, "Extram malloc failed.\n" );
    exit(1);
  }
  
  // allocate memory for extram cache (write)
  if( (cart.extramValidWrite = (u8 *)malloc(cart.extram_size)) == NULL )
  {
    fprintf( stderr, "Extram malloc failed.\n" );
    exit(1);
  }
  
  // determine whether the extram (if any) is battery-backed
  switch( cart.mbc_type )
  {
    case 0x03:  // MBC1+RAM+BATTERY
    case 0x06:  // MBC2+BATTERY
    case 0x0F:  // MBC3+TIMER+BATTERY
    case 0x10:  // MBC3+TIMERY+RAM+BATTERY
    case 0x13:  // MBC3+RAM+BATTERY
    case 0x1B:  // MBC5+RAM+BATTERY
    case 0x1E:  // MBC5+RUMBLE+RAM+BATTERY
    case 0x22:  // MBC7+RAM+BATTERY
    case 0xFC:  // POCKET CAMERA
    case 0xFE:  // HuC3
      cart.battery_backed = 1;
      break;
    default:
      cart.battery_backed = 0;
      break;
  }
  
  // HACK
  cart.cartromsize = 8*1024*1024;
}

void cart_chardev_default_cleanup()
{
}

void cart_c_cleanup()
{
  
  cart.cleanup();
  
  // free cartrom, bootrom, and extram
  free( cart.cartrom );
  cart.cartrom = NULL;
  free( cart.bootrom );
  cart.bootrom = NULL;
  free( cart.extram );
  cart.extram = NULL;
  
  // free cache validity things
  free( cart.cartromValid );
  cart.cartromValid = NULL;
  free( cart.extramValidRead );
  cart.extramValidRead = NULL;
  free( cart.extramValidWrite );
  cart.extramValidWrite = NULL;
}

void cart_chardev_bringup_device( char *cartromName )
{
  // Open the character device.
  if( !(cart.fd = fopen( cartromName, "r+" )) )
  {
    fprintf( stderr, "Error opening character device.\n");
    exit(1);
  }
  
  // Reset the cart.
  fprintf( stdout, "R\n" );
  fprintf( cart.fd, "R\n" );
  fgetc(cart.fd);       // R
  fgetc(cart.fd);       // E
  fgetc(cart.fd);       // S
  fgetc(cart.fd);       // E
  fgetc(cart.fd);       // T
  fgetc(cart.fd);       // cr
  fgetc(cart.fd);       // lf
  fgetc(cart.fd);       // cr
  fgetc(cart.fd);       // lf
}


void cart_c_reset_mbc()
{
  if( state.bootRomEnabled )
  {
    printf( "MBC reset: boot rom\n" );
    mbc_c_boot_install();
    return;
  }
  
  // install MBC driver
  switch( cart.mbc_type )
  {
    case 0x00:  // ROM ONLY
//     case 0x08:  // ROM+RAM           // no known games use these
//     case 0x09:  // ROM+RAM+BATTERY
      mbc_c_none_install();
      break;
    case 0x01:  // MBC1
    case 0x02:  // MBC1+RAM
    case 0x03:  // MBC1+RAM+BATTERY
//       mbc_c_mbc1_install();
      break;
    case 0x05:  // MBC2
    case 0x06:  // MBC2+BATTERY
//       mbc_mbc2_install();
      break;
    case 0x0F:  // MBC3+TIMER+BATTERY
    case 0x10:  // MBC3+TIMERY+RAM+BATTERY
    case 0x11:  // MBC3
    case 0x12:  // MBC3+RAM
    case 0x13:  // MBC3+RAM+BATTERY
      mbc_c_mbc3_install();
      break;
    case 0x19:  // MBC5
    case 0x1A:  // MBC5+RAM
    case 0x1B:  // MBC5+RAM+BATTERY
    case 0x1C:  // MBC5+RUMBLE
    case 0x1D:  // MBC5+RUMBLE+RAM
    case 0x1E:  // MBC5+RUMBLE+RAM+BATTERY
//       mbc_mbc5_install();
      break;
    case 0x22:  // MBC7+?
//       mbc_mbc7_install();
      break;
    case 0xFC:  // POCKET CAMERA
//       mbc_cam_install();
      break;
    case 0xFE:  // HuC3
//       mbc_huc3_install();
      break;
    default:
      // danger danger
      printf( "MBC C reset: Unhandled cart type: %02Xh %s\n", cart.mbc_type, cartdesc_carttype[cart.mbc_type] );
      exit(1);
      break;
  }
  
  printf( "MBC C reset: %02Xh %s\n", cart.mbc_type, cartdesc_carttype[cart.mbc_type] );
}


void ca_write( FILE *fd, unsigned int address, unsigned char data )
{
  fprintf( stdout, "w%d %d (%04x %02x)\n", address, data, address, data );
  fprintf( fd, "w%d %d\n", address, data );
  fgetc(cart.fd);
  fgetc(cart.fd);
}

void ca_read256Bytes( FILE *fd, const unsigned int startAddress, unsigned char *destination )
{
  unsigned int data;
  fprintf( stdout, "c%d (%04x)\n", startAddress, startAddress );
  fprintf( cart.fd, "c%d\n", startAddress );
  int i;
  for( i=0; i<256; i++ )
  {
    unsigned int temp=0;
    temp=fgetc(cart.fd);
    data = (unsigned char)temp;
//     printf("a%02x\n", temp);
    destination[i] = data;
  }
  fgetc(cart.fd);
  fgetc(cart.fd);
}
