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
#include "mbc_huc3.h"
#include <stdio.h>
#include "cpu.h"

int huc3_ram_mode = 0;

void mbc_huc3_install()
{
  cart.cleanup = &mbc_huc3_cleanup;
  cart.huc3_ram_mode = 0;
  
  int i;
  // cart bank zero
  for( i=0x00; i<=0x3F; ++i ) {
    readmem[i] = mbc_huc3_read_bank_0;
  }
  // cart bank n
  for( i=0x40; i<=0x7F; ++i ) {
    readmem[i] = mbc_huc3_read_bank_n;
  }
  
  // write 3000-3FFF: rom bank select (high bit)
  for( i=0x30; i<=0x3F; ++i ) {
    writemem[i] = mbc_huc3_write_rom_bank_select_high;
  }
  // TODO
  for( i=0x00; i<=0x7F; ++i ) {
    writemem[i] = mbc_huc3_write_dummy;
  }
  // write 0000-1FFF: ram enable? or something
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_huc3_write_ram_enable;
  }
  // write 2000-20FF: rom bank select
  for( i=0x20; i<=0x20; ++i ) {
    writemem[i] = mbc_huc3_write_rom_bank_select_low;
  }
  // write 4000-40FF: ram bank select?
  for( i=0x40; i<=0x40; ++i ) {
    writemem[i] = mbc_huc3_write_ram_bank_select;
  }
  // write 6000-7FFF: select ROM or RAM mode, like MBC1. but this mapper is always in RAM mode.
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_huc3_write_dummy;
  }
  
  // read A000-BFFF: read extram
  // calculate the last address where extram is installed
  int extram_end = 0xA0 + (cart.extram_size>8192?8192:cart.extram_size)/256;
  for( i=0xA0; i<extram_end; ++i ) {
    readmem[i] = mbc_huc3_read_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    readmem[i] = mbc_huc3_read_ff;
  }
  
  // write A000-BFFF: write extram
  for( i=0xA0; i<extram_end; ++i ) {
    writemem[i] = mbc_huc3_write_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    writemem[i] = mbc_huc3_write_dummy;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
  cart.reg_rom_bank_low = 1;
  cart.reg_rom_bank_high = 0;
  cart.extram_bank = cart.extram;
  cart.extram_bank_num = 0;
}

uint8_t mbc_huc3_read_ff( uint16_t address )
{
  return 0xff;
}

void mbc_huc3_write_dummy( uint16_t address, uint8_t data )
{
  printf("write: pc:%04X, %04X:%02X\n", state.pc, address, data);
}

uint8_t mbc_huc3_read_bank_0( uint16_t address )
{
  return cart.cartrom_bank_zero[address];
}

uint8_t mbc_huc3_read_bank_n( uint16_t address ) {
  return cart.cartrom_bank_n[address&0x3fff];
}

// write 0000-1FFF
void mbc_huc3_write_ram_enable( uint16_t address, uint8_t data ) {
  printf("write: pc:%04X, %04X:%02X\n", state.pc, address, data);
  // TODO
  cart.huc3_ram_mode = data;
}

// write 2000-2FFF
void mbc_huc3_write_rom_bank_select_low( uint16_t address, uint8_t data ) {
  printf("write: pc:%04X, %04X:%02X\n", state.pc, address, data);
  cart.reg_rom_bank_low = data;
  int bank =  cart.reg_rom_bank_low + (int)(cart.reg_rom_bank_high)*256;
  
  bank = bank % cart.cartrom_num_banks;
  
  if( bank == 0 )
    bank = 1;
  
  
  size_t offset = (size_t)bank*16384;
  cart.cartrom_bank_n = cart.cartrom + offset;
}

// write 3000-3FFF
void mbc_huc3_write_rom_bank_select_high( uint16_t address, uint8_t data ) {
  printf("write: pc:%04X, %04X:%02X\n", state.pc, address, data);
  cart.reg_rom_bank_high = data & 0x01;
  int bank =  cart.reg_rom_bank_low + (int)(cart.reg_rom_bank_high)*256;
  
  bank = bank % cart.cartrom_num_banks;
  
  if( bank == 0 )
    bank = 1;
  
  
  size_t offset = (size_t)bank*16384;
  cart.cartrom_bank_n = cart.cartrom + offset;
}

// write 4000-40FF
void mbc_huc3_write_ram_bank_select( uint16_t address, uint8_t data ) {
  printf("write: pc:%04X, %04X:%02X\n", state.pc, address, data);
  if( cart.extram_size == 32768 )
  {
    int bank = data & 0x03;
    cart.extram_bank = cart.extram + bank*8192;
  }
}

// read A000-BFFF
uint8_t mbc_huc3_read_extram( uint16_t address ) {
  uint8_t data;
  switch( cart.huc3_ram_mode )
  {
    default:
    case 0:
      data = 0xff;
      break;
    case 0x0A:
      data = cart.extram_bank[address&0x1fff];
      break;
    case 0x0C:
    case 0x0D:
      data = 0x01;
      break;
  }
  printf("read: pc:%04X, %04X:%02X\n", state.pc, address, data);
  return data;
}

// write A000-BFFF
void mbc_huc3_write_extram( uint16_t address, uint8_t data ) {
  printf("write: pc:%04X, %04X:%02X\n", state.pc, address, data);
  cart.extram_bank[address&0x1fff] = data;
}

void mbc_huc3_cleanup() {
}
