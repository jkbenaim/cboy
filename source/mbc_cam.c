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

u8 cam_extram[32768]; //overkill


u8 cam_rom_bank_low; // bits 0-4
u8 cam_rom_bank_high;  // bit 5-6 (stored in bits 0-1)
u8 cam_ram_bank;
u8 cam_mode_select;

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
  
  // write 0000-1FFF: ram enable
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_cam_write_ram_enable;
  }
  // write 2000-3FFF: rom bank select
  for( i=0x20; i<=0x3F; ++i ) {
    writemem[i] = mbc_cam_write_rom_bank_select;
  }
  // write 4000-5FFF: ram bank select
  for( i=0x40; i<=0x5F; ++i ) {
    writemem[i] = mbc_cam_write_ram_bank_select;
  }
  // write 6000-7FFF: mode select
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_cam_write_mode_select;
  }
  
  // read A000-BFFF: read extram/timer
  for( i=0xA0; i<=0xBF; ++i ) {
    readmem[i] = mbc_cam_read_extram;
  }
  // write A000-BFFF: write extram/timer
  for( i=0xA0; i<=0xBF; ++i ) {
    writemem[i] = mbc_cam_write_extram;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
  cam_rom_bank_low = 0x01;
  cam_rom_bank_high = 0x00;
  cam_ram_bank = 0x00;
  cam_mode_select = 0x00;
  cart.extram = cam_extram;
  cart.extram_bank = cam_extram;
}

void mbc_cam_regs_changed()
{
  int rambank;
  int rombank = cam_rom_bank_low;
  if( rombank == 0 )
    rombank++;
  
  if( cam_mode_select == 0 )
  {
    // ROM mode
    rombank += (cam_rom_bank_high & 0x03) << 5;
    rambank = 0;
  } else {
    // RAM mode
    rambank = cam_rom_bank_high & 0x03;
  }
  
  size_t cartoffset;
  cartoffset = (rombank*16384) % cart.cartromsize;
  cart.cartrom_bank_n = cart.cartrom + cartoffset;
  cart.extram_bank = cart.extram + rambank * 8192;
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
}

// write 2000-3FFF
void mbc_cam_write_rom_bank_select() {
  cam_rom_bank_low = memByte;
  mbc_cam_regs_changed();
}

// write 4000-5FFF
void mbc_cam_write_ram_bank_select() {
  cam_rom_bank_high = memByte;
  mbc_cam_regs_changed();
}

// write 6000-7FFF
void mbc_cam_write_mode_select() {
  cam_mode_select = memByte;
  mbc_cam_regs_changed();
}

// read A000-BFFF
void mbc_cam_read_extram() {
  memByte = cart.extram_bank[address&0x1fff];
}

// write A000-BFFF
void mbc_cam_write_extram() {
  cart.extram_bank[address&0x1fff] = memByte;
}