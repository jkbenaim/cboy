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

#ifndef _MBC_MBC1_H_
#define _MBC_MBC1_H_
extern void mbc_mbc1_install( void );
void mbc_mbc1_read_bank_0( void );
void mbc_mbc1_read_bank_n( void );
void mbc_mbc1_write_ram_enable( void );
void mbc_mbc1_write_rom_bank_select( void );
void mbc_mbc1_write_ram_bank_select( void );
void mbc_mbc1_write_mode_select( void );
void mbc_mbc1_read_extram( void );
void mbc_mbc1_write_extram( void );
void mbc_mbc1_read_extram_disabled( void );
void mbc_mbc1_write_extram_disabled( void );
#endif // _MBC_MBC1_H_