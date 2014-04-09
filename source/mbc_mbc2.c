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
#include "mbc_mbc2.h"
#include <stdio.h>

void mbc_mbc2_install()
{
  int i;
  // cart bank zero
  for( i=0x00; i<=0x3F; ++i ) {
    readmem[i] = mbc_mbc2_read_bank_0;
  }
  // cart bank n
  for( i=0x40; i<=0x7F; ++i ) {
    readmem[i] = mbc_mbc2_read_bank_n;
  }
  
  // write 0000-1FFF: ram enable
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_mbc2_write_ram_enable;
  }
  // write 2000-3FFF: rom bank select
  for( i=0x20; i<=0x3F; ++i ) {
    writemem[i] = mbc_mbc2_write_rom_bank_select;
  }
  // write 4000-5FFF: ram bank select
  for( i=0x40; i<=0x7F; ++i ) {
    writemem[i] = mbc_mbc2_dummy;
  }
  
  // read A000-BFFF: read extram
  for( i=0xA0; i<=0xA1; ++i ) {
    readmem[i] = mbc_mbc2_read_extram;
  }
  for( i=0xA2; i<=0xBF; ++i ) {
    readmem[i] = mbc_mbc2_read_ff;
  }
  // write A000-BFFF: write extram
  for( i=0xA0; i<=0xA1; ++i ) {
    writemem[i] = mbc_mbc2_write_extram;
  }
  for( i=0xA2; i<=0xBF; ++i ) {
    writemem[i] = mbc_mbc2_dummy;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
}

void mbc_mbc2_read_ff()
{
  memByte = 0xff;
}

void mbc_mbc2_dummy()
{
}

void mbc_mbc2_read_bank_0()
{
  memByte = cart.cartrom_bank_zero[address];
}

void mbc_mbc2_read_bank_n() {
  memByte = cart.cartrom_bank_n[address&0x3fff];
}

// write 0000-1FFF
void mbc_mbc2_write_ram_enable() {
}

// write 2000-3FFF
void mbc_mbc2_write_rom_bank_select() {
  size_t offset;
  if( memByte == 0 )
  {
    cart.cart_bank_num = 1;
    offset = (size_t)16384;
  }
  else
  {
    cart.cart_bank_num = memByte;
    offset = (size_t)memByte*16384 % cart.cartromsize;
  }
  
  cart.cartrom_bank_n = cart.cartrom + offset;
}

// read A000-A1FF
void mbc_mbc2_read_extram() {
  memByte = cart.extram[address&0x01ff];
}

// write A000-A1FF
void mbc_mbc2_write_extram() {
  cart.extram[address&0x01ff] = memByte & 0x0F;
}