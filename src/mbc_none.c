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
#include "mbc_none.h"
#include <stdio.h>

void mbc_none_install()
{
	cart.cleanup = &mbc_none_cleanup;

	int i;
	// cart bank zero
	for (i = 0x00; i <= 0x3F; ++i) {
		readmem[i] = mbc_none_read_bank_0;
	}
	// cart bank n
	for (i = 0x40; i <= 0x7F; ++i) {
		readmem[i] = mbc_none_read_bank_n;
	}
	// write 0000-7FFF: nothing
	for (i = 0x00; i <= 0x7F; ++i) {
		writemem[i] = mbc_none_write_dummy;
	}

	// read A000-BFFF: read extram
	// calculate the last address where extram is installed
	int extram_end =
	    0xA0 + (cart.extram_size > 8192 ? 8192 : cart.extram_size) / 256;
	for (i = 0xA0; i < extram_end; ++i) {
		readmem[i] = mbc_none_read_extram;
	}
	for (i = extram_end; i <= 0xBF; ++i) {
		readmem[i] = mbc_none_read_ff;
	}

	// write A000-BFFF: write extram
	for (i = 0xA0; i < extram_end; ++i) {
		writemem[i] = mbc_none_write_extram;
	}
	for (i = extram_end; i <= 0xBF; ++i) {
		writemem[i] = mbc_none_write_dummy;
	}

	// set up cart params
	cart.cartrom_bank_zero = cart.cartrom;
	cart.cartrom_bank_n = cart.cartrom + 0x4000;
}

uint8_t mbc_none_read_ff(uint16_t address)
{
	return 0xff;
}

void mbc_none_write_dummy(uint16_t address, uint8_t data)
{
}

uint8_t mbc_none_read_bank_0(uint16_t address)
{
	return cart.cartrom_bank_zero[address];
}

// cart bank n
uint8_t mbc_none_read_bank_n(uint16_t address)
{
	return cart.cartrom_bank_n[address & 0x3fff];
}

// extram
uint8_t mbc_none_read_extram(uint16_t address)
{
	return cart.extram_bank[address & 0x1fff];
}

void mbc_none_write_extram(uint16_t address, uint8_t data)
{
	cart.extram_bank[address & 0x1fff] = data;
}

void mbc_none_cleanup()
{
}
