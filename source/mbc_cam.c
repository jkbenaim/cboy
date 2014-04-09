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

#include "memory.h"
#include "cart.h"
#include "mbc_cam.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#define  LOG_TAG    "libcboy"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif // __ANDROID__

int cam_mode;   // 0=RAM accessible, 1=CAM registers accessible

FILE *fd;
int cache[0x2000] = {0};
int cacheValid[0x2000] = {0};

void mbc_cam_install()
{
  // Open the character device.
  
  if( !(fd = fopen( "/dev/ttyACM0", "r+" )) )
  {
    fprintf( stderr, "Error opening character device.\n");
    exit(1);
  }
  
//   address = 0x4000;
//   memByte = 0x10;
//   mbc_cam_write_extram();
//   
//   address = 0xA000;
//   memByte = 0x00;
//   mbc_cam_write_extram();
  
  int i;
  // cart bank zero
  for( i=0x00; i<=0x3F; ++i ) {
    readmem[i] = mbc_cam_read_bank_0;
  }
  // cart bank n
  for( i=0x40; i<=0x7F; ++i ) {
    readmem[i] = mbc_cam_read_bank_n;
  }
  
  // write 0000-1FFF: ram write enable
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_cam_write_extram;
  }
  // write 2000-3FFF: rom bank select
  for( i=0x20; i<=0x3F; ++i ) {
    writemem[i] = mbc_cam_write_rom_bank_select;
  }
  // write 4000-5FFF: extram bank/mode select
  for( i=0x40; i<=0x5F; ++i ) {
    writemem[i] = mbc_cam_write_extram;
  }
  // write 6000-7FFF: nothing
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_cam_write_extram;
  }
  
  // read A000-BFFF: read extram
  for( i=0xA0; i<=0xBF; ++i ) {
    readmem[i] = mbc_cam_read_extram;
  }
  // write A000-BFFF: write extram
  for( i=0xA0; i<=0xBF; ++i ) {
    writemem[i] = mbc_cam_write_extram;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
  cart.extram_bank_num = 0x00;
  cart.extram_bank = cart.extram;
  cam_mode = 0;
}

void mbc_cam_dummy()
{
}

void mbc_cam_read_bank_0()
{
  memByte = cart.cartrom_bank_zero[address];
}

void mbc_cam_read_bank_n() {
  memByte = cart.cartrom_bank_n[address&0x3fff];
}

// write 0000-1FFF
void mbc_cam_write_ram_enable() {
  // TODO
}

// write 2000-3FFF
void mbc_cam_write_rom_bank_select() {
  size_t cartoffset;
  int rombank = (memByte==0)?1:memByte;
  cartoffset = (rombank*16384) % cart.cartromsize;
  cart.cartrom_bank_n = cart.cartrom + cartoffset;
}

// write 4000-5FFF
void mbc_cam_extram_bank_select() {
  if( memByte == 0x10 )
  {
    // CAM mode
    cam_mode = 1;
  }
  else
  {
    // RAM mode
    // access RAM like normal
    cam_mode = 0;
    int bank = memByte & 0x0F;
    cart.extram_bank = cart.extram + bank*8192;
  }
  
}

// write 6000-7FFF
void mbc_cam_write_mode_select() {
}

// read A000-BFFF
int bank = 0;
void mbc_cam_read_extram() {
  
  // exception! if it's 0xA000, read directly, without touching cache
  if( (address == 0xA000) && (bank == 0x10) )
  {
    unsigned int readAddress = address;
    unsigned int data=0;
    
    fprintf( stdout, "b%d\n", readAddress );
    fprintf( fd, "b%d\n", readAddress );
    data=fgetc(fd);
    memByte = (u8)data;
    printf("%d\n", memByte);
    fgetc(fd);
    fgetc(fd);
    return;
  }
//   printf("hi %04X %02X\n", address, bank);
  if( !cacheValid[address - 0xA000] )
  {
    // fill cache
    unsigned int startAddress = address & 0xFF00;
    unsigned int endAddress = startAddress + 255;
    unsigned int readAddress, data;
    fprintf( stdout, "c%d\n", startAddress );
    fprintf( fd, "c%d\n", startAddress );
    for( readAddress = startAddress; readAddress <= endAddress; readAddress++ )
    {
      unsigned int temp=0;
      temp=fgetc(fd);
      data = (u8)temp;
      cache[readAddress - 0xA000] = data;
      cacheValid[readAddress - 0xA000] = 1;
    }
    fgetc(fd);
    fgetc(fd);
  }
  // read from cache
  memByte = cache[address - 0xA000];
    
  
//   if( cam_mode == 0 )
//   {
//     // access RAM like normal
//     memByte = cart.extram_bank[address&0x1fff];
//   }
//   else
//   {
//     // access CAM registers
//     memByte = 0; //TODO
//   }
}

// write A000-BFFF
void mbc_cam_write_extram() {
  if( (address >= 0x4000) && (address <= 0x5FFF) )
  {
    bank = memByte;
  }
  mbc_cam_invalidateCache();
  if(memByte == 0)
    fprintf( stdout, ".");
  else
    fprintf( stdout, "w%d %d\n", address, memByte );
  fprintf( fd, "w%d %d\n", address, memByte );
  fgetc(fd);
  fgetc(fd);
//   if( cam_mode == 0 )
//   {
//     // access RAM like normal
//     cart.extram_bank[address&0x1fff] = memByte;
//   }
//   else
//   {
//     // access CAM registers
//     switch( address )
//     {
//       case 0xA000:
//         // take a picture?
//         switch( memByte )
//         {
//           case 0x03:
//             {
//             // really take that picture
//             static int pics_taken = 0;
//             pics_taken++;
//             if(pics_taken <= 140)
//             {
//               // the game takes 140 pictures on bootup.
//               // no idea why.
//               // i don't act on these.
//             } else {
//               printf( "Picture taken! [%d]\n", pics_taken );
// #ifdef __ANDROID__
//               mbc_cam_getCameraImage();
// #else
//               // write a checkerboard pattern to photo ram
//               int tile_x, tile_y;
//               int row_in_tile;
//               int ram_address_offset = 0x0100;
//               int value_to_write_hi=0, value_to_write_lo=0;
//               for( tile_y = 0; tile_y < 14 ; tile_y++ )
//               for( tile_x = 0; tile_x < 16 ; tile_x++ )
//               {
//                 switch( (tile_x + tile_y + pics_taken) % 6 ) {
//                   case 0:
//                     value_to_write_hi = 0x00;
//                     value_to_write_lo = 0x00;
//                     break;
//                   case 1:
//                     value_to_write_hi = 0x00;
//                     value_to_write_lo = 0xff;
//                     break;
//                   case 2:
//                     value_to_write_hi = 0xff;
//                     value_to_write_lo = 0x00;
//                     break;
//                   case 3:
//                     value_to_write_hi = 0xff;
//                     value_to_write_lo = 0xff;
//                     break;
//                   case 4:
//                     value_to_write_hi = 0xff;
//                     value_to_write_lo = 0x00;
//                     break;
//                   case 5:
//                     value_to_write_hi = 0x00;
//                     value_to_write_lo = 0xff;
//                     break;
//                 }
//                 for( row_in_tile = 0; row_in_tile < 8 ; row_in_tile++ )
//                 {
//                   cart.extram[ram_address_offset + 0] = (u8)value_to_write_lo;
//                   cart.extram[ram_address_offset + 1] = (u8)value_to_write_hi;
//                   ram_address_offset +=2;
//                 }
//               }
// #endif // __ANDROID__
//             }
//             }
//             break;
//           default:
//             break;
//         }
//         break;
//       default:
//         break;
//     }
//   }
}

#ifdef __ANDROID__
void mbc_cam_getCameraImage() {
  LOGI( "picture taken!" );
}
#endif // __ANDROID__

void mbc_cam_invalidateCache()
{
  int i;
  for(i=0; i<0x2000; i++)
    cacheValid[i] = 0;
}
