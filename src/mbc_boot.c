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

/*
 * This is a pseudo-MBC used during bootup
 * that exposes the boot ROM image.
 */

#include "memory.h"
#include "cart.h"
#include "mbc_boot.h"

void mbc_boot_install()
{
	int i;
	// cart bank zero
	for (i = 0x0; i <= (0x3F); ++i) {
		readmem[i] = mbc_boot_read_bank_0;
		writemem[i] = mbc_boot_write_bank_0;
	}

	// cart bank n
	for (i = 0x40; i <= (0x7F); ++i) {
		readmem[i] = mbc_boot_read_bank_n;
		writemem[i] = mbc_boot_write_bank_n;
	}

	// set up cart params
	cart.cartrom_bank_zero = cart.cartrom;
	cart.cartrom_bank_n = cart.cartrom + 0x4000;
}

uint8_t mbc_boot_read_bank_0(uint16_t address)
{
	if ((address < 0x100) || ((address >= 0x200) && (address < 0x900))) {
		return cart.bootrom[address];
	} else {
		return cart.cartrom[address];
	}
}

void mbc_boot_write_bank_0(uint16_t address, uint8_t data)
{
	// do nothing
}

// cart bank n
uint8_t mbc_boot_read_bank_n(uint16_t address)
{
	return cart.cartrom_bank_n[address & 0x3fff];
}

void mbc_boot_write_bank_n(uint16_t address, uint8_t data)
{
	// do nothing
}
