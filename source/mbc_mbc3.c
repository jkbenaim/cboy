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
#include "mbc_mbc3.h"
#include <stdio.h>
#include "assert.h"
#include <time.h>

void mbc_mbc3_install()
{
  cart.cleanup = &mbc_mbc3_cleanup;
  
  int i;
  // cart bank zero
  for( i=0x00; i<=0x3F; ++i ) {
    readmem[i] = mbc_mbc3_read_bank_0;
  }
  // cart bank n
  for( i=0x40; i<=0x7F; ++i ) {
    readmem[i] = mbc_mbc3_read_bank_n;
  }
  
  // write 0000-1FFF: ram enable
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_mbc3_write_ram_enable;
  }
  // write 2000-3FFF: rom bank select
  for( i=0x20; i<=0x3F; ++i ) {
    writemem[i] = mbc_mbc3_write_rom_bank_select;
  }
  // write 4000-5FFF: ram bank select
  for( i=0x40; i<=0x5F; ++i ) {
    writemem[i] = mbc_mbc3_write_ram_bank_select;
  }
  // write 6000-7FFF: clock data latch
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_mbc3_write_clock_data_latch;
  }
  
  // read A000-BFFF: read extram
  // calculate the last address where extram is installed
  int extram_end = 0xA0 + (cart.extram_size>8192?8192:cart.extram_size)/256;
  for( i=0xA0; i<extram_end; ++i ) {
    readmem[i] = mbc_mbc3_read_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    readmem[i] = mbc_mbc3_read_ff;
  }
  
  // write A000-BFFF: write extram
  for( i=0xA0; i<extram_end; ++i ) {
    writemem[i] = mbc_mbc3_write_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    writemem[i] = mbc_mbc3_dummy;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
  cart.extram_bank = cart.extram;
}

void mbc_mbc3_read_ff()
{
  memByte = 0xff;
}

void mbc_mbc3_dummy()
{
}

void mbc_mbc3_read_bank_0()
{
  memByte = cart.cartrom_bank_zero[address];
}

void mbc_mbc3_read_bank_n() {
  memByte = cart.cartrom_bank_n[address&0x3fff];
}

// write 0000-1FFF
void mbc_mbc3_write_ram_enable() {
}

// write 2000-3FFF
void mbc_mbc3_write_rom_bank_select() {
//   printf("%02X ", memByte );
  size_t offset;
  memByte &= 0x7F;
  cart.cart_bank_num = memByte;
  if( memByte == 0 )
    offset = (size_t)16384;
  else
    offset = (size_t)memByte*16384 % cart.cartromsize;
  
//   printf( "switch cart bank num: %02X\n", cart.cart_bank_num );
  assert("MBC3 rom bank select: offset computation", offset <= (cart.cartromsize - 16384));
  cart.cartrom_bank_n = cart.cartrom + offset;
}

// write 4000-5FFF
void mbc_mbc3_write_ram_bank_select() {
  int i;
  switch( memByte )
  {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
//       printf("Switching to RAM bank %02X \n", memByte );
      cart.extram_bank_num = memByte;
      cart.extram_bank = cart.extram + memByte*8192;
      // read A000-BFFF: read extram
      // calculate the last address where extram is installed
      int extram_end = 0xA0 + (cart.extram_size>8192?8192:cart.extram_size)/256;
      for( i=0xA0; i<extram_end; ++i ) {
        readmem[i] = mbc_mbc3_read_extram;
      }
      for( i=extram_end; i<=0xBF; ++i ) {
        readmem[i] = mbc_mbc3_read_ff;
      }
      
      // write A000-BFFF: write extram
      for( i=0xA0; i<extram_end; ++i ) {
        writemem[i] = mbc_mbc3_write_extram;
      }
      for( i=extram_end; i<=0xBF; ++i ) {
        writemem[i] = mbc_mbc3_dummy;
      }
      break;
    case 0x08:	// seconds
    case 0x09:	// minutes
    case 0x0A:	// hours
    case 0x0B:	// day bits 0-7
    case 0x0C:	// day bit 8, carry bit, halt flag
//       printf("Switching to RTC bank %02X \n", memByte );
      cart.extram_bank_num = memByte;
      // read A000-BFFF: read rtc
      for( i=0xA0; i<=0xBF; ++i ) {
	readmem[i] = mbc_mbc3_read_rtc;
      }
      // write A000-BFFF: write rtc
      for( i=0xA0; i<=0xBF; ++i ) {
	writemem[i] = mbc_mbc3_write_rtc;
      }
      break;
    default:
      printf("Switching to invalid extram bank %02X \n", memByte );
      break;
  }
}

// write 6000-7FFF
void mbc_mbc3_write_clock_data_latch() {
}

// read A000-BFFF extram
void mbc_mbc3_read_extram() {
  memByte = cart.extram_bank[address&0x1fff];
}

// write A000-BFFF extram
void mbc_mbc3_write_extram() {
  cart.extram_bank[address&0x1fff] = memByte;
}

// read A000-BFFF rtc
void mbc_mbc3_read_rtc() {
  memByte = 0x00;
}

// write A000-BFFF rtc
void mbc_mbc3_write_rtc() {
}

void mbc_mbc3_cleanup() {
}