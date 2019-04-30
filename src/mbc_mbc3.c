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
    writemem[i] = mbc_mbc3_write_dummy;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
  cart.extram_bank = cart.extram;
}

uint8_t mbc_mbc3_read_ff( uint16_t address )
{
  return 0xff;
}

void mbc_mbc3_write_dummy( uint16_t address, uint8_t data )
{
}

uint8_t mbc_mbc3_read_bank_0( uint16_t address )
{
  return cart.cartrom_bank_zero[address];
}

uint8_t mbc_mbc3_read_bank_n( uint16_t address )
{
  return cart.cartrom_bank_n[address&0x3fff];
}

// write 0000-1FFF
void mbc_mbc3_write_ram_enable( uint16_t address, uint8_t data )
{
  // TODO
}

// write 2000-3FFF
void mbc_mbc3_write_rom_bank_select( uint16_t address, uint8_t data ) {
  size_t offset;
  data &= 0x7F;
  cart.cart_bank_num = data;
  if( data == 0 )
    offset = (size_t)16384;
  else
    offset = (size_t)data*16384 % cart.cartromsize;
  
//   printf( "switch cart bank num: %02X\n", cart.cart_bank_num );
  cart.cartrom_bank_n = cart.cartrom + offset;
}

// write 4000-5FFF
void mbc_mbc3_write_ram_bank_select( uint16_t address, uint8_t data ) {
  int i;
  switch( data )
  {
    case 0 ... 7 :
//       printf("Switching to RAM bank %02X \n", data );
      cart.extram_bank_num = data;
      cart.extram_bank = cart.extram + data*8192;
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
        writemem[i] = mbc_mbc3_write_dummy;
      }
      break;
    case 0x08:	// seconds
    case 0x09:	// minutes
    case 0x0A:	// hours
    case 0x0B:	// day bits 0-7
    case 0x0C:	// day bit 8, carry bit, halt flag
//       printf("Switching to RTC bank %02X \n", data );
      cart.extram_bank_num = data;
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
      printf("Switching to invalid extram bank %02X \n", data );
      break;
  }
}

// write 6000-7FFF
void mbc_mbc3_write_clock_data_latch( uint16_t address, uint8_t data ) {
}

// read A000-BFFF extram
uint8_t mbc_mbc3_read_extram( uint16_t address ) {
  return cart.extram_bank[address&0x1fff];
}

// write A000-BFFF extram
void mbc_mbc3_write_extram( uint16_t address, uint8_t data ) {
  cart.extram_bank[address&0x1fff] = data;
}

// read A000-BFFF rtc
uint8_t mbc_mbc3_read_rtc( uint16_t address ) {
  return 0x00;
}

// write A000-BFFF rtc
void mbc_mbc3_write_rtc( uint16_t address, uint8_t data ) {
}

void mbc_mbc3_cleanup() {
}
