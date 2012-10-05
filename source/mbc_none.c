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
#include "mbc_none.h"
#include <stdio.h>


void mbc_none_install()
{
  int i;
  
  // cart bank zero
  for( i=0x0; i<=(0x3F); ++i ) {
    readmem[i]   = mbc_none_read_bank_0;
    writemem[i]  = mbc_none_write_bank_0;
  }
  
  // cart bank n
  for( i=0x40; i<=(0x7F); ++i ) {
    readmem[i]   = mbc_none_read_bank_n;
    writemem[i]  = mbc_none_write_bank_n;
  }
  
  // extram
  for( i=0xA0; i<=(0xBF); ++i ) {
    readmem[i]   = mbc_none_read_extram;
    writemem[i]  = mbc_none_write_extram;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
}

void mbc_none_read_bank_0()
{
  memByte = cart.cartrom_bank_zero[address];
}

void mbc_none_write_bank_0() {
  // do nothing
}


// cart bank n
void mbc_none_read_bank_n() {
  memByte = cart.cartrom_bank_n[address&0x3fff];
}

void mbc_none_write_bank_n() {
  // do nothing
}


// extram
void mbc_none_read_extram() {
}

void mbc_none_write_extram() {
}