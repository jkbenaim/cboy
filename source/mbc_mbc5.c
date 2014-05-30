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
#include "mbc_mbc5.h"
#include <stdio.h>
#include "assert.h"

void mbc_mbc5_install()
{
  cart.cleanup = &mbc_mbc5_cleanup;
  
  int i;
  // cart bank zero
  for( i=0x00; i<=0x3F; ++i ) {
    readmem[i] = mbc_mbc5_read_bank_0;
  }
  // cart bank n
  for( i=0x40; i<=0x7F; ++i ) {
    readmem[i] = mbc_mbc5_read_bank_n;
  }
  
  // write 0000-1FFF: ram enable
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_mbc5_write_ram_enable;
  }
  // write 2000-2FFF: rom bank select (low bits)
  for( i=0x20; i<=0x2F; ++i ) {
    writemem[i] = mbc_mbc5_write_rom_bank_select_low;
  }
  // write 3000-3FFF: rom bank select (high bit)
  for( i=0x30; i<=0x3F; ++i ) {
    writemem[i] = mbc_mbc5_write_rom_bank_select_high;
  }
  // write 4000-5FFF: ram bank select
  for( i=0x40; i<=0x5F; ++i ) {
    writemem[i] = mbc_mbc5_write_ram_bank_select;
  }
  // write 6000-7FFF: nothing
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_mbc5_write_dummy;
  }
  
  // read A000-BFFF: read extram
  // calculate the last address where extram is installed
  int extram_end = 0xA0 + (cart.extram_size>8192?8192:cart.extram_size)/256;
  for( i=0xA0; i<extram_end; ++i ) {
    readmem[i] = mbc_mbc5_read_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    readmem[i] = mbc_mbc5_read_ff;
  }
  
  // write A000-BFFF: write extram
  for( i=0xA0; i<extram_end; ++i ) {
    writemem[i] = mbc_mbc5_write_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    writemem[i] = mbc_mbc5_write_dummy;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
  cart.reg_rom_bank_low = 1;
  cart.reg_rom_bank_high = 0;
  cart.extram_bank = cart.extram;
  cart.extram_bank_num = 0;
}

uint8_t mbc_mbc5_read_ff( uint16_t address )
{
  return 0xff;
}

void mbc_mbc5_write_dummy( uint16_t address, uint8_t data )
{
}

uint8_t mbc_mbc5_read_bank_0( uint16_t address )
{
  return cart.cartrom_bank_zero[address];
}

uint8_t mbc_mbc5_read_bank_n( uint16_t address )
{
  return cart.cartrom_bank_n[address&0x3fff];
}

// write 0000-1FFF
// write 0x0A to enable extram, anything else to disable extram
void mbc_mbc5_write_ram_enable( uint16_t address, uint8_t data )
{
  // TODO
}

// write 2000-2FFF
void mbc_mbc5_write_rom_bank_select_low( uint16_t address, uint8_t data )
{
  cart.reg_rom_bank_low = data;
  int bank =  cart.reg_rom_bank_low + (int)(cart.reg_rom_bank_high)*256;
  
  bank = bank % cart.cartrom_num_banks;
  
  if( bank == 0 )
    bank = 1;
  
  
  size_t offset = (size_t)bank*16384;
  cart.cartrom_bank_n = cart.cartrom + offset;
  assert( "MBC5 rom bank select: bank number", bank <= cart.cartrom_num_banks );
  assert( "MBC5 rom bank select: offset computation", offset <= (cart.cartromsize - 16384) );
}

// write 3000-3FFF
void mbc_mbc5_write_rom_bank_select_high( uint16_t address, uint8_t data )
{
  cart.reg_rom_bank_high = data & 0x01;
  int bank =  cart.reg_rom_bank_low + (int)(cart.reg_rom_bank_high)*256;
  
  bank = bank % cart.cartrom_num_banks;
  
  if( bank == 0 )
    bank = 1;
  
  
  size_t offset = (size_t)bank*16384;
  cart.cartrom_bank_n = cart.cartrom + offset;
  assert( "MBC5 rom bank select: bank number", bank <= cart.cartrom_num_banks );
  assert( "MBC5 rom bank select: offset computation", offset <= (cart.cartromsize - 16384) );
}

// write 4000-5FFF
void mbc_mbc5_write_ram_bank_select( uint16_t address, uint8_t data )
{
  if( cart.extram_size == 32768 )
  {
    int bank = data & 0x03;
    cart.extram_bank = cart.extram + bank*8192;
  }
  
  // handle rumble
  switch( cart.mbc_type )
  {
    case 0x1C:  // MBC5+RUMBLE
    case 0x1D:  // MBC5+RUMBLE+RAM
    case 0x1E:  // MBC5+RUMBLE+RAM+BATTERY
      if( data & 0x08 )
        printf( "rumble ON, power: %d\n", data >> 4 );
      break;
    default:
      break;
  }
}

// read A000-BFFF
uint8_t mbc_mbc5_read_extram( uint16_t address )
{
  return cart.extram_bank[address&0x1fff];
}

// write A000-BFFF
void mbc_mbc5_write_extram( uint16_t address, uint8_t data )
{
  cart.extram_bank[address&0x1fff] = data;
}

void mbc_mbc5_cleanup()
{
}
