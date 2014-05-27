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
#include "mbc_c_mbc1.h"

int rom_bank_shadow;
int ram_bank_shadow;
int mode_select;

void mbc_c_mbc1_install()
{
  printf("Warning: ROM bank select incomplete\n");
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
    readmem[i]   = mbc_c_mbc1_read_bank_0;
  }
  // read cart bank n
  for( i=0x40; i<=(0x7F); ++i ) {
    readmem[i]   = mbc_c_mbc1_read_bank_n;
  }
  
  // write 0000-1FFF: ram enable
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_c_mbc1_write_ram_enable;
  }
  // write 2000-3FFF: rom bank select (lower 5 bits)
  for( i=0x20; i<=0x3F; ++i ) {
    writemem[i] = mbc_c_mbc1_write_rom_bank_select;
  }
  // write 4000-5FFF: ram bank select / rom bank select (upper 2 bits)
  for( i=0x40; i<=0x5F; ++i ) {
    writemem[i] = mbc_c_mbc1_write_ram_bank_select;
  }
  // write 6000-7FFF: rom/ram mode select
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_c_mbc1_dummy;
  }
  
  // read A000-BFFF: read extram
  // calculate the last address where extram is installed
  int extram_end = 0xA0 + (cart.extram_size>8192?8192:cart.extram_size)/256;
  for( i=0xA0; i<extram_end; ++i ) {
    readmem[i] = mbc_c_mbc1_read_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    readmem[i] = mbc_c_mbc1_read_ff;
  }
  
  // write A000-BFFF: write extram
  for( i=0xA0; i<extram_end; ++i ) {
    writemem[i] = mbc_c_mbc1_write_extram;
  }
  for( i=extram_end; i<=0xBF; ++i ) {
    writemem[i] = mbc_c_mbc1_dummy;
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
  
  cart.cleanup = mbc_c_mbc1_cleanup;
}

void mbc_c_mbc1_read_bank_0()
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

void mbc_c_mbc1_read_bank_n() {
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

void mbc_c_mbc1_dummy() {
  // do nothing
}

void mbc_c_mbc1_read_ff() {
  memByte = 0xFF;
}

// write 0000-1FFFF: ram enable
void mbc_c_mbc1_write_ram_enable() {
  cart.extramEnabled = memByte;
}

// write 2000-3FFF: rom bank select
void mbc_c_mbc1_write_rom_bank_select() {
  size_t offset;
  memByte &= 0x7F;
  if(memByte == 0)
    memByte = 1;
  cart.cart_bank_num = memByte;
  offset = (size_t)memByte*16384 % cart.cartromsize;
  
//   printf( "switch cart bank num: %02X\n", cart.cart_bank_num );
  assert("MBC1 rom bank select: offset computation", offset <= (cart.cartromsize - 16384));
  cart.cartrom_bank_n = cart.cartrom + offset;
  cart.cartromValid_bank_n = cart.cartromValid + offset;
}

// write 4000-5FFF: ram bank select
void mbc_c_mbc1_write_ram_bank_select() {
  int i;
  switch( memByte )
  {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
      cart.extram_bank_num = memByte&0x03;
      cart.extram_bank = cart.extram + memByte*8192;
      cart.extram_bank_validRead = cart.extramValidRead + memByte*8192;
      cart.extram_bank_validWrite = cart.extramValidWrite + memByte*8192;
      
      // calculate the last address where extram is installed
      int extram_end = 0xA0 + (cart.extram_size>8192?8192:cart.extram_size)/256;
      
      // read A000-BFFF: read extram
      for( i=0xA0; i<extram_end; ++i ) {
        readmem[i] = mbc_c_mbc1_read_extram;
      }
      for( i=extram_end; i<=0xBF; ++i ) {
        readmem[i] = mbc_c_mbc1_read_ff;
      }
      
      // write A000-BFFF: write extram
      for( i=0xA0; i<extram_end; ++i ) {
        writemem[i] = mbc_c_mbc1_write_extram;
      }
      for( i=extram_end; i<=0xBF; ++i ) {
        writemem[i] = mbc_c_mbc1_dummy;
      }
      break;
    case 0x08:  // seconds
    case 0x09:  // minutes
    case 0x0A:  // hours
    case 0x0B:  // day bits 0-7
    case 0x0C:  // day bit 8, carry bit, halt flag
      cart.extram_bank_num = memByte;
      // read A000-BFFF: read rtc
      for( i=0xA0; i<=0xBF; ++i ) {
        readmem[i] = mbc_c_mbc1_read_rtc;
      }
      // write A000-BFFF: write rtc
      for( i=0xA0; i<=0xBF; ++i ) {
        writemem[i] = mbc_c_mbc1_write_rtc;
      }
      break;
    default:
      printf("Switching to invalid extram bank %02X \n", memByte );
      break;
  }
}

// write 6000-7FFF: clock data latch
void mbc_c_mbc1_write_clock_data_latch() {
  // TODO
}

// read A000-BFFF extram
void mbc_c_mbc1_read_extram() {
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
    uint8_t buf[256];
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
void mbc_c_mbc1_write_extram() {
  cart.extram_bank[address&0x1fff] = memByte;
  cart.extram_bank_validRead[address&0x1fff] = 1;
  cart.extram_bank_validWrite[address&0x1fff] = 1;
}

// read A000-BFFF rtc
void mbc_c_mbc1_read_rtc() {
  memByte = 0x00;
}

// write A000-BFFF rtc
void mbc_c_mbc1_write_rtc() {
}

void mbc_c_mbc1_cleanup() {
  // save extram back
  
  if( cart.battery_backed )
  {
    // enable extram
    ca_write( cart.fd, 0x0000, 0x0a );
    
    // write to extram
    unsigned int address;
    unsigned char data;
    int bank;
    switch( cart.extram_size )
    {
      case 0:
      case 2048:
      case 8192:
        // switch to bank 0
        ca_write( cart.fd, 0x4000, 0 );
        cart.extram_bank = cart.extram;
        cart.extram_bank_validWrite = cart.extramValidWrite;
        
        // write
        for( address=0xA000; address<(0xA000+cart.extram_size); address++ )
        {
          if(cart.extram_bank_validWrite[address-0xA000] == 1)
          {
            data = cart.extram_bank[address-0xA000];
            ca_write( cart.fd, address, data );
          }
        }
        break;
      case 32768:
        // 32KBytes in 4 banks of 8KBytes each
        for(bank=0; bank<4; bank++)
        {
          // switch to bank
          ca_write( cart.fd, 0x4000, bank );
          cart.extram_bank = cart.extram + bank*0x2000;
          cart.extram_bank_validWrite = cart.extramValidWrite + bank*0x2000;
          
          // write
          for( address=0xA000; address<(0xC000); address++ )
          {
            if(cart.extram_bank_validWrite[address-0xA000] == 1)
            {
              data = cart.extram_bank[address-0xA000];
              ca_write( cart.fd, address, data );
            }
          }
        }
        break;
      default:
        fprintf( stderr, "Invalid extram size when saving extram back.\n" );
        exit(1);
        break;
    }
  }
}

