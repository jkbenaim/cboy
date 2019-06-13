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

#ifndef _MBC_HUC1_H_
#define _MBC_HUC1_H_
extern void mbc_huc1_install(void);
uint8_t mbc_huc1_read_ff(uint16_t address);
uint8_t mbc_huc1_read_bank_0(uint16_t address);
uint8_t mbc_huc1_read_bank_n(uint16_t address);
uint8_t mbc_huc1_read_extram(uint16_t address);
uint8_t mbc_huc1_read_extram_disabled(uint16_t address);
void mbc_huc1_write_dummy(uint16_t address, uint8_t data);
void mbc_huc1_write_ram_enable(uint16_t address, uint8_t data);
void mbc_huc1_write_rom_bank_select(uint16_t address, uint8_t data);
void mbc_huc1_write_ram_bank_select(uint16_t address, uint8_t data);
void mbc_huc1_write_mode_select(uint16_t address, uint8_t data);
void mbc_huc1_write_extram(uint16_t address, uint8_t data);
void mbc_huc1_write_extram_disabled(uint16_t address, uint8_t data);
#endif				// _MBC_HUC1_H_
