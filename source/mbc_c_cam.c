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

#include "memory.h"
#include "cart_chardev.h"
#include "cart.h"
#include "mbc_c_cam.h"

int rom_bank_shadow;
int ram_bank_shadow;

void mbc_c_cam_install()
{
  int i;
  
  // invalidate caches
  for( i=0; i<8*1024*1024; ++i )
  {
    cart.cartromValid[i] = 0;
  }
  for( i=0; i<cart.extram_size; ++i )
  {
    cart.extramValidRead[i] = 0;
    cart.extramValidWrite[i] = 0;
  }
  
  // read cart bank zero
  for( i=0x0; i<=(0x3F); ++i ) {
    readmem[i]   = mbc_c_cam_read_bank_0;
  }
  // read cart bank n
  for( i=0x40; i<=(0x7F); ++i ) {
    readmem[i]   = mbc_c_cam_read_bank_n;
  }
  
  // write 0000-1FFF: ram enable
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_c_cam_write_ram_enable;
  }
  // write 2000-3FFF: rom bank select
  for( i=0x20; i<=0x3F; ++i ) {
    writemem[i] = mbc_c_cam_write_rom_bank_select;
  }
  // write 4000-5FFF: ram bank select
  for( i=0x40; i<=0x5F; ++i ) {
    writemem[i] = mbc_c_cam_write_ram_bank_select;
  }
  // write 6000-7FFF: nothing
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_c_cam_dummy;
  }
  
  // read A000-BFFF: read extram
  for( i=0xA0; i<=0xBF; ++i ) {
    readmem[i] = mbc_c_cam_read_extram;
  }
  
  // write A000-BFFF: write extram
  for( i=0xA0; i<=0xBF; ++i ) {
    writemem[i] = mbc_c_cam_write_extram;
  }
  
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
  
  // enable extram, just in case
  ca_write( cart.fd, 0x0000, 0x0a );
  
  rom_bank_shadow = -1;
  cart.cart_bank_num = 1;
  ram_bank_shadow = -1;
  cart.extram_bank_num = 0;
  
  cart.cleanup = mbc_c_cam_cleanup;
}

void mbc_c_cam_read_bank_0()
{
  if( !cart.cartromValid[address] )
  {
    // fill cache
    unsigned int startAddress = address & 0xFF00;
    ca_read256Bytes( cart.fd, startAddress, cart.cartrom+startAddress );
    
    // mark cache as valid
    int i;
    for( i=0; i<256; i++ )
      cart.cartromValid[startAddress+i] = 1;
  }
  // read from cache
  memByte = cart.cartrom[address];
//   printf("Read: %04x:%02x\n", address, memByte);
}

void mbc_c_cam_read_bank_n() {
  if( !cart.cartromValid_bank_n[address-0x4000] )
  {
    // set rom bank
    if( rom_bank_shadow != cart.cart_bank_num )
    {
      ca_write( cart.fd, 0x2000, cart.cart_bank_num );
      rom_bank_shadow = cart.cart_bank_num;
    }
    // fill cache
    unsigned int startAddress = address & 0xFF00;
    ca_read256Bytes( cart.fd, startAddress, cart.cartrom_bank_n+startAddress-0x4000 );
    
    // mark cache as valid
    int i;
    for( i=0; i<256; i++ )
      cart.cartromValid_bank_n[startAddress+i-0x4000] = 1;
  }
  // read from cache
  memByte = cart.cartrom_bank_n[address-0x4000];
}

void mbc_c_cam_dummy() {
  // do nothing
}

void mbc_c_cam_read_ff() {
  memByte = 0xFF;
}

// write 0000-1FFFF: ram enable
void mbc_c_cam_write_ram_enable() {
  cart.extramEnabled = memByte;
}

// write 2000-3FFF: rom bank select
void mbc_c_cam_write_rom_bank_select() {
  size_t offset;
  memByte &= 0x7F;
  if(memByte == 0)
    memByte = 1;
  cart.cart_bank_num = memByte;
  offset = (size_t)memByte*16384 % cart.cartromsize;
  
//   printf( "switch cart bank num: %02X\n", cart.cart_bank_num );
  assert("MBC3 rom bank select: offset computation", offset <= (cart.cartromsize - 16384));
  cart.cartrom_bank_n = cart.cartrom + offset;
  cart.cartromValid_bank_n = cart.cartromValid + offset;
}

// write 4000-5FFF: ram bank select
void mbc_c_cam_write_ram_bank_select() {
  int i;
  switch( memByte )
  {
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x0f:
      // these are all valid extram banks
      cart.extram_bank_num = memByte;
      cart.extram_bank = cart.extram + memByte*8192;
      cart.extram_bank_validRead = cart.extramValidRead + memByte*8192;
      cart.extram_bank_validWrite = cart.extramValidWrite + memByte*8192;
      
      // read A000-BFFF: read extram
      for( i=0xA0; i<0xBF; ++i ) {
        readmem[i] = mbc_c_cam_read_extram;
      }
      
      // write A000-BFFF: write extram
      for( i=0xA0; i<=0xBF; ++i ) {
        writemem[i] = mbc_c_cam_write_extram;
      }
      break;
    case 0x00:
    case 0x10:
      cart.extram_bank_num = memByte;
      // this is the camera register bank
      // read A000-BFFF: read camera
      for( i=0xA0; i<0xBF; ++i ) {
        readmem[i] = mbc_c_cam_read_camera;
      }
      
      // write A000-BFFF: write camera
      for( i=0xA0; i<=0xBF; ++i ) {
        writemem[i] = mbc_c_cam_write_camera;
      }
      break;
    default:
      printf("Switching to invalid extram bank %02X \n", memByte );
      break;
  }
}

// read A000-BFFF camera
void mbc_c_cam_read_camera() {
  // this just gets passed right through
  // set ram bank
  if( ram_bank_shadow != cart.extram_bank_num )
  {
    ca_write( cart.fd, 0x4000, cart.extram_bank_num );
    ram_bank_shadow = cart.extram_bank_num;
  }
  memByte = ca_read( cart.fd, address );
  
  //HACK
  if(address == 0xa000)
    memByte &= 0xFE;
}

// write A000-BFFF camera
void mbc_c_cam_write_camera() {
  // this just gets passed "write" through! haha
  // set ram bank
  if( ram_bank_shadow != cart.extram_bank_num )
  {
    ca_write( cart.fd, 0x4000, cart.extram_bank_num );
    ram_bank_shadow = cart.extram_bank_num;
  }
  ca_write( cart.fd, address, memByte );
  
  // also, we need to invalidate the extram cache due to reasons
  // BUG: need to write changes back first
//   int i;
//   for(i=0; i<0x2000; i++)
//     cart.extramValidRead[i] = 0;
}

// read A000-BFFF extram
void mbc_c_cam_read_extram() {
  if( cart.extramEnabled != 0x0a )
  {
    memByte = 0xff;
    return;
  }
  if( !cart.extram_bank_validRead[address-0xA000] )
  {
    // set ram bank
    if( ram_bank_shadow != cart.extram_bank_num )
    {
      ca_write( cart.fd, 0x4000, cart.extram_bank_num );
      ram_bank_shadow = cart.extram_bank_num;
    }
    // fill cache
    unsigned int startAddress = address & 0xFF00;
    u8 buf[256];
    ca_read256Bytes( cart.fd, startAddress, buf );
    int i;
    for( i=0; i<256; i++ )
    {
      if( cart.extram_bank_validRead[startAddress+i-0xA000] == 0 )
	cart.extram_bank[startAddress+i-0xA000] = buf[i];
    }
    
    // mark cache as valid
    for( i=0; i<256; i++ )
      cart.extram_bank_validRead[startAddress+i-0xA000] = 1;
  }
  // read from cache
  memByte = cart.extram_bank[address - 0xA000];
}

// write A000-BFFF extram
void mbc_c_cam_write_extram() {
  cart.extram_bank[address&0x1fff] = memByte;
  cart.extram_bank_validRead[address&0x1fff] = 1;
  cart.extram_bank_validWrite[address&0x1fff] = 1;
}

void mbc_c_cam_cleanup() {
  // save extram back
  
  // enable extram
  ca_write( cart.fd, 0x0000, 0x0a );
  
  // write to extram
  unsigned int address;
  unsigned char data;
  int bank;
  // 128KBytes in 16 banks of 8KBytes each
  for(bank=0; bank<16; bank++)
  {
    // switch to bank
    ca_write( cart.fd, 0x4000, bank );
    cart.extram_bank = cart.extram + bank*0x2000;
    cart.extram_bank_validWrite = cart.extramValidWrite + bank*0x2000;
    
    // write
    for( address=0xA000; address<0xC000; address++ )
    {
      if(cart.extram_bank_validWrite[address-0xA000] == 1)
      {
	data = cart.extram_bank[address-0xA000];
	ca_write( cart.fd, address, data );
      }
    }
  }
}

