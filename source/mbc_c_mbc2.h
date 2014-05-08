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

#ifndef _MBC_C_MBC2_H_
#define _MBC_C_MBC2_H_
void mbc_c_mbc2_install( void );
extern void mbc_c_mbc2_read_bank_0( void );
extern void mbc_c_mbc2_write_bank_0( void );
extern void mbc_c_mbc2_read_bank_n( void );
extern void mbc_c_mbc2_write_bank_n( void );
extern void mbc_c_mbc2_dummy( void );
extern void mbc_c_mbc2_read_extram( void );
extern void mbc_c_mbc2_write_extram( void );
extern void mbc_c_mbc2_read_ff( void );
void mbc_c_mbc2_write_ram_enable( void );
void mbc_c_mbc2_write_rom_bank_select( void );
void mbc_c_mbc2_write_ram_bank_select( void );
void mbc_c_mbc2_write_clock_data_latch( void );
void mbc_c_mbc2_cleanup( void );

#endif // _MBC_C_MBC2_H_