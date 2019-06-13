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

#ifndef _MBC_BOOT_H_
#define _MBC_BOOT_H_
void mbc_boot_install();
extern uint8_t mbc_boot_read_bank_0(uint16_t address);
extern void mbc_boot_write_bank_0(uint16_t address, uint8_t data);
extern uint8_t mbc_boot_read_bank_n(uint16_t address);
extern void mbc_boot_write_bank_n(uint16_t address, uint8_t data);

#endif				// _MBC_BOOT_H_
