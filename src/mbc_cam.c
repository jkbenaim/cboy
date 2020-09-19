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
#include "mbc_cam.h"
#include "cpu.h"
#include <stdio.h>

int cam_mode;   // 0=RAM accessible, 1=CAM registers accessible

void mbc_cam_install()
{
	int i;
	// cart bank zero
	for( i=0x00; i<=0x3F; ++i ) {
		readmem[i] = mbc_cam_read_bank_0;
	}
	// cart bank n
	for( i=0x40; i<=0x7F; ++i ) {
		readmem[i] = mbc_cam_read_bank_n;
	}

	// write 0000-1FFF: ram write enable
	for( i=0x00; i<=0x1F; ++i ) {
		writemem[i] = mbc_cam_write_ram_enable;
	}
	// write 2000-3FFF: rom bank select
	for( i=0x20; i<=0x3F; ++i ) {
		writemem[i] = mbc_cam_write_rom_bank_select;
	}
	// write 4000-5FFF: extram bank/mode select
	for( i=0x40; i<=0x5F; ++i ) {
		writemem[i] = mbc_cam_write_extram_bank_select;
	}
	// write 6000-7FFF: nothing
	for( i=0x60; i<=0x7F; ++i ) {
		writemem[i] = mbc_cam_write_dummy;
	}

	// read A000-BFFF: read extram
	for( i=0xA0; i<=0xBF; ++i ) {
		readmem[i] = mbc_cam_read_extram;
	}
	// write A000-BFFF: write extram
	for( i=0xA0; i<=0xBF; ++i ) {
		writemem[i] = mbc_cam_write_extram;
	}

	// set up cart params
	cart.cartrom_bank_zero = cart.cartrom;
	cart.cartrom_bank_n = cart.cartrom + 0x4000;
	cart.extram_bank_num = 0x00;
	cart.extram_bank = cart.extram;
	cam_mode = 0;
}

void _camwrite(uint16_t address, uint8_t data)
{
	if((address & 0x0ff0) == 0x0ff0)
	printf("cam write: (ROM %02x RAM %02x) (%04x) %04x <-- %02x\n", cart.cart_bank_num, cart.extram_bank_num, state.pc, address, data);
}

void _camread(uint16_t address, uint8_t data)
{
	if((address & 0x0ff0) == 0x0ff0)
	printf("cam read : (ROM %02x RAM %02x) (%04x) %04x --> %02x\n", cart.cart_bank_num, cart.extram_bank_num, state.pc, address, data);
}

void mbc_cam_write_dummy(uint16_t address, uint8_t data)
{
	_camwrite(address, data);
}

uint8_t mbc_cam_read_bank_0(uint16_t address)
{
	uint8_t data = cart.cartrom_bank_zero[address];
	//_camread(address, data);
	return data;
}

uint8_t mbc_cam_read_bank_n(uint16_t address) {
	uint8_t data = cart.cartrom_bank_n[address&0x3fff];
	//_camread(address, data);
	return data;
}

// write 0000-1FFF
void mbc_cam_write_ram_enable(uint16_t address, uint8_t data) {
	_camwrite(address, data);
	// TODO
}

// write 2000-3FFF
void mbc_cam_write_rom_bank_select(uint16_t address, uint8_t data) {
	//_camwrite(address, data);
	size_t cartoffset;
	int rombank = (data==0)?1:data;
	cart.cart_bank_num = rombank;
	cartoffset = (rombank*16384) % cart.cartromsize;
	cart.cartrom_bank_n = cart.cartrom + cartoffset;
}

// write 4000-5FFF
void mbc_cam_write_extram_bank_select(uint16_t address, uint8_t data) {
	_camwrite(address, data);
	cart.extram_bank_num = data;
	if( data == 0x10 )
	{
		// CAM mode
		cam_mode = 1;
	}
	else
	{
		// RAM mode
		// access RAM like normal
		cam_mode = 0;
		int bank = data & 0x0F;
		cart.extram_bank = cart.extram + bank*8192;
	}

}

// write 6000-7FFF
void mbc_cam_write_mode_select(uint16_t address, uint8_t data) {
	//_camwrite(address, data);
}

// read A000-BFFF
uint8_t mbc_cam_read_extram(uint16_t address) {
	uint8_t data;
	if( cam_mode == 0 )
	{
		// access RAM like normal
		data = cart.extram_bank[address&0x1fff];
	}
	else
	{
		// access CAM registers
		data = 0; //TODO
	}
	_camread(address, data);
	return data;
}

// write A000-BFFF
void mbc_cam_write_extram(uint16_t address, uint8_t data) {
	_camwrite(address, data);
	if(cam_mode == 0) {
		// access RAM like normal
		cart.extram_bank[address&0x1fff] = data;
	} else {
    // access CAM registers
	switch(address)
    {
      case 0xA000:
        // take a picture?
        switch( data )
        {
          case 0x03:
            {
            // really take that picture
            static int pics_taken = 0;
            pics_taken++;
            if(pics_taken <= 140)
            {
              // the game takes 140 pictures on bootup.
              // no idea why.
              // i don't act on these.
            } else {
//               printf( "Picture taken! [%d]\n", pics_taken );
              // write a checkerboard pattern to photo ram
              int tile_x, tile_y;
              int row_in_tile;
              int ram_address_offset = 0x0100;
              int value_to_write_hi=0, value_to_write_lo=0;
              for(tile_y = 0; tile_y < 14 ; tile_y++)
              for(tile_x = 0; tile_x < 16 ; tile_x++)
              {
                switch((tile_x + tile_y + pics_taken) % 6) {
                  case 0:
                    value_to_write_hi = 0x00;
                    value_to_write_lo = 0x00;
                    break;
                  case 1:
                    value_to_write_hi = 0x00;
                    value_to_write_lo = 0xff;
                    break;
                  case 2:
                    value_to_write_hi = 0xff;
                    value_to_write_lo = 0x00;
                    break;
                  case 3:
                    value_to_write_hi = 0xff;
                    value_to_write_lo = 0xff;
                    break;
                  case 4:
                    value_to_write_hi = 0xff;
                    value_to_write_lo = 0x00;
                    break;
                  case 5:
                    value_to_write_hi = 0x00;
                    value_to_write_lo = 0xff;
                    break;
                }
                for(row_in_tile = 0; row_in_tile < 8 ; row_in_tile++)
                {
                  cart.extram[ram_address_offset + 0] = (uint8_t)value_to_write_lo;
                  cart.extram[ram_address_offset + 1] = (uint8_t)value_to_write_hi;
                  ram_address_offset +=2;
                }
              }
            }
            }
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
}
