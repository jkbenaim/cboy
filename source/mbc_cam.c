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

// TODO: actually save the cart ram to disk.

#include "memory.h"
#include "cart.h"
#include "mbc_cam.h"
#include <stdio.h>

u8 cam_extram[131072];

u8 cam_ram_bank;
int cam_mode;   // 0=RAM accessible, 1=CAM registers accessible

void mbc_cam_install()
{
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
    writemem[i] = mbc_cam_write_ram_enable;
  }
  // write 2000-3FFF: rom bank select
  for( i=0x20; i<=0x3F; ++i ) {
    writemem[i] = mbc_cam_write_rom_bank_select;
  }
  // write 4000-5FFF: extram bank/mode select
  for( i=0x40; i<=0x5F; ++i ) {
    writemem[i] = mbc_cam_extram_bank_select;
  }
  // write 6000-7FFF: nothing
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_cam_dummy;
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
  cam_ram_bank = 0x00;
  cam_mode = 0;
  cart.extram = cam_extram;
  
  // clear extram
  int index;
  for( index=0; index<131072; index++ )
    cam_extram[index] = 0x41;
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
    cart.extram = cam_extram + bank*8192;
  }
  
}

// write 6000-7FFF
void mbc_cam_write_mode_select() {
}

// read A000-BFFF
void mbc_cam_read_extram() {
  if( cam_mode == 0 )
  {
    // access RAM like normal
    memByte = cart.extram[address&0x1fff];
  }
  else
  {
    // access CAM registers
    memByte = 0; //TODO
  }
}

// int pics_taken = 0;
// write A000-BFFF
void mbc_cam_write_extram() {
  if( cam_mode == 0 )
  {
    // access RAM like normal
    cart.extram[address&0x1fff] = memByte;
  }
  else
  {
    // access CAM registers
    switch( address )
    {
      case 0xA000:
        // take a picture?
        switch( memByte )
        {
          case 0x03:
            // really take that picture
//             printf( "Picture taken! [%d]\n", pics_taken++ );
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
}
