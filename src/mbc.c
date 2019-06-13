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

#include "mbc.h"
#include "types.h"

/*
 * int mbc_romSize2numBanks( uint8_t romSize )
 * 
 * Given the romSize byte from the cart header, returns
 * the size of the rom.
 */
int mbc_romSize2numBanks(uint8_t romSize)
{
	switch (romSize) {
	case 0x00:
		return 2;
		break;
	case 0x01:
		return 4;
		break;
	case 0x02:
		return 8;
		break;
	case 0x03:
		return 16;
		break;
	case 0x04:
		return 32;
		break;
	case 0x05:
		return 64;
		break;
	case 0x06:
		return 128;
		break;
	case 0x07:
		return 256;
		break;
	case 0x52:
		return 72;
		break;
	case 0x53:
		return 80;
		break;
	case 0x54:
		return 96;
		break;
	default:
		// uh-oh
		return 2;
		break;
	}
}

int mbc_ramSize2numBytes(uint8_t ramSize)
{
	switch (ramSize) {
	case 0x00:
		return 0;
		break;
	case 0x01:
		return 2048;
		break;
	case 0x02:
		return 8192;
		break;
	case 0x03:
		return 32768;
		break;
	case 0x04:
		return 131072;
		break;
	case 0x05:
		return 65536;
		break;
	default:
		return 0;
		break;
	}
}
