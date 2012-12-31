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

#ifndef _MBC_MBC3_H_
#define _MBC_MBC3_H_
extern void mbc_mbc3_install( void );
void mbc_mbc3_read_ff( void );
void mbc_mbc3_dummy( void );
void mbc_mbc3_read_bank_0( void );
void mbc_mbc3_read_bank_n( void );
void mbc_mbc3_write_ram_enable();
void mbc_mbc3_write_rom_bank_select();
void mbc_mbc3_write_ram_bank_select();
void mbc_mbc3_write_clock_data_latch();
void mbc_mbc3_read_extram();
void mbc_mbc3_write_extram();
void mbc_mbc3_read_rtc();
void mbc_mbc3_write_rtc();
void mbc_mbc3_cleanup();
#endif // _MBC_MBC3_H_