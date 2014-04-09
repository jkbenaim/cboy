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


#include "memory.h"
#include "cart_chardev.h"
#include "cart.h"
#include "mbc_c_none.h"


void mbc_c_none_install()
{
  int i;
  
  // invalidate caches
  for( i=0; i<8*1024*1024; ++i )
  {
    cart.cartromValid[i] = 0;
  }
  for( i=0; i<cart.extram_size; ++i )
  {
    cart.extramValidRead[i] = 0;
    cart.extramValidWrite[i] = 0;
  }
  
  // cart bank zero
  for( i=0x0; i<=(0x3F); ++i ) {
    readmem[i]   = mbc_c_none_read_bank_0;
    writemem[i]  = mbc_c_none_write_bank_0;
  }
  
  // cart bank n
  for( i=0x40; i<=(0x7F); ++i ) {
    readmem[i]   = mbc_c_none_read_bank_n;
    writemem[i]  = mbc_c_none_write_bank_n;
  }
  // write 0000-7FFF: nothing
  for( i=0x00; i<=0x7F; ++i ) {
    writemem[i] = mbc_c_none_dummy;
  }
  
  // read A000-BFFF: read extram
  // calculate the last address where extram is installed
  int extram_end = 0xA0 + (cart.extram_size>8192?8192:cart.extram_size)/256;
  for( i=0xA0; i<extram_end; ++i ) {
    readmem[i] = mbc_c_none_read_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    readmem[i] = mbc_c_none_read_ff;
  }
  
  // write A000-BFFF: write extram
  for( i=0xA0; i<extram_end; ++i ) {
    writemem[i] = mbc_c_none_write_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    writemem[i] = mbc_c_none_dummy;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
}

void mbc_c_none_read_bank_0()
{
  if( !cart.cartromValid[address] )
  {
    // fill cache
    unsigned int startAddress = address & 0xFF00;
    ca_read256Bytes( cart.fd, startAddress, cart.cartrom+startAddress );
    
    // mark cache as valid
    int i;
    for( i=0; i<256; i++ )
      cart.cartromValid[startAddress+i] = 1;
  }
  // read from cache
  memByte = cart.cartrom[address];
}

void mbc_c_none_read_bank_n() {
  mbc_c_none_read_bank_0();
}

void mbc_c_none_write_bank_0() {
  // do nothing
}



void mbc_c_none_write_bank_n() {
  // do nothing
}

void mbc_c_none_dummy() {
  // do nothing
}

void mbc_c_none_read_ff() {
  memByte = 0xFF;
}

void mbc_c_none_read_extram() {
  memByte = 0xFF;
}

void mbc_c_none_write_extram() {
  // do nothing
}