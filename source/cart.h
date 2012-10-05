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

#ifndef _CART_H_
#define _CART_H_
#include "types.h"
#include <stddef.h> // for size_t

struct cart_s {
  int mbc_type;
  size_t cartromsize;
  int cartrom_num_banks;
  size_t bootromsize;
  int reg_ram_enable; // (byte&0x0F == 0x0A) ? enable RAM : disable RAM
  int reg_rom_bank_low;	// low bits of ROM bank
  int reg_rom_bank_high;  // high bits of ROM bank
  int reg_mode;	// 0: ROM mode, 1: RAM mode
  u8* cartrom;
  u8* bootrom;
  u8* extram;
  u8 cart_bank_num;
  u8 extram_bank_num;
  u8* cartrom_bank_zero;
  u8* cartrom_bank_n;
  u8* extram_bank;
  void (*cleanup)(void);
  char savename[256];
};

#define MAX_CARTROM_SIZE	8388608
#define MAX_BOOTROM_SIZE	2304

extern void cart_init( char* boot_rom, char* rom );
extern void cart_cleanup( void );
extern void cart_disable_bootrom( void );
void cart_init_cartrom( char* cartromName );
void cart_init_bootrom( char* bootromName );
void cart_default_cleanup( void );
extern void cart_reset_mbc( void );
extern struct cart_s cart;

#endif // _CART_H_