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

#ifndef _CART_CHARDEV_H_
#define _CART_CHARDEV_H_
#include "types.h"
#include <stdio.h> // for FILE

extern void cart_init_chardev( char* boot_rom, char* rom );
void cart_chardev_default_cleanup( void );
void cart_chardev_bringup_device( char *cartromName );
extern void cart_c_reset_mbc();
extern void cart_c_cleanup();
extern void ca_write( FILE *fd, unsigned int address, unsigned int data );
extern unsigned char ca_read( FILE *fd, unsigned int address );
extern void ca_read256Bytes( FILE *fd, const unsigned int startAddress, unsigned char *destination );
extern void ca_read4096Bytes( FILE *fd, const unsigned int startAddress, unsigned char *destination );

#endif // _CART_CHARDEV_H_