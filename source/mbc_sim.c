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
#include "mbc_sim.h"
#include <stdio.h>


FILE *fd;

int cache_rom[8*1024*1024] = {0};
int cacheValid_rom[8*1024*1024] = {0};
int romBank = 1;
int cache_ram[0x2000] = {0};
int cacheValid_ram[0x2000] = {0};

void mbc_sim_install()
{
  // Open the character device.
  
  if( !(fd = fopen( "/dev/ttyACM0", "r+" )) )
  {
    fprintf( stderr, "Error opening character device.\n");
    exit(1);
  }
  cart.cleanup = &mbc_sim_cleanup;
  
  int i;
  // read 0000-3FFFF
  for( i=0x00; i<=0x3F; ++i ) {
    readmem[i] = mbc_sim_read_rom_0;
  }
  // read 4000-7FFFF
  for( i=0x40; i<=0x7F; ++i ) {
    readmem[i] = mbc_sim_read_rom_n;
  }
  // write 0000-1FFF: nothing
  for( i=0x00; i<=0x1F; ++i ) {
    writemem[i] = mbc_sim_write_invalidate_ram;
  }
  // write 2000-3FFF: nothing
  for( i=0x20; i<=0x3F; ++i ) {
    writemem[i] = mbc_sim_write_invalidate_rom;
  }
  // write 4000-5FFF: nothing
  for( i=0x40; i<=0x5F; ++i ) {
    writemem[i] = mbc_sim_write_invalidate_rom_ram;
  }
  // write 6000-7FFF: nothing
  for( i=0x60; i<=0x7F; ++i ) {
    writemem[i] = mbc_sim_write_invalidate_rom_ram;
  }
  // read A000-BFFF: read extram
  for( i=0xA0; i<=0xBF; ++i ) {
    readmem[i] = mbc_sim_read_ram;
  }
  // write A000-BFFF: write extram
  for( i=0xA0; i<=0xBF; ++i ) {
    writemem[i] = mbc_sim_write_invalidate_ram;
  }
  // set up cart params
  cart.cartrom_bank_zero = cart.cartrom;
  cart.cartrom_bank_n = cart.cartrom + 0x4000;
}

void mbc_sim_read_ff()
{
  memByte = 0xff;
}

void mbc_sim_dummy()
{
}

void mbc_sim_read_rom_0()
{
  if( !cacheValid_rom[address] )
  {
    // fill cache
    unsigned int startAddress = address & 0xF000;
    unsigned int endAddress = startAddress + 4095;
    unsigned int readAddress, data;
    fprintf( stdout, "d%d, (%04x)\n", startAddress, startAddress );
    fprintf( fd, "d%d\n", startAddress );
    for( readAddress = startAddress; readAddress <= endAddress; readAddress++ )
    {
      unsigned int temp=0;
      temp=fgetc(fd);
      data = (u8)temp;
      cache_rom[readAddress] = data;
      cacheValid_rom[readAddress] = 1;
    }
    fgetc(fd);
    fgetc(fd);
  }
  // read from cache
  memByte = cache_rom[address];
}

void mbc_sim_read_rom_n()
{
  if( !cacheValid_rom[address + 0x4000*romBank] )
  {
    // fill cache
    unsigned int startAddress = address & 0xF000;
    unsigned int endAddress = startAddress + 4095;
    unsigned int readAddress, data;
    fprintf( stdout, "d%d, (%04x)\n", startAddress, startAddress );
    fprintf( fd, "d%d\n", startAddress );
    for( readAddress = startAddress; readAddress <= endAddress; readAddress++ )
    {
      unsigned int temp=0;
      temp=fgetc(fd);
      data = (u8)temp;
      cache_rom[readAddress + 0x4000*romBank] = data;
      cacheValid_rom[readAddress + 0x4000*romBank] = 1;
    }
    fgetc(fd);
    fgetc(fd);
  }
  // read from cache
  memByte = cache_rom[address + 0x4000*romBank];
}

void mbc_sim_read_ram()
{
  if( !cacheValid_ram[address - 0xA000] )
  {
    // fill cache
    unsigned int startAddress = address & 0xF000;
    unsigned int endAddress = startAddress + 4095;
    unsigned int readAddress, data;
    fprintf( stdout, "d%d, (%04x)\n", startAddress, startAddress );
    fprintf( fd, "d%d\n", startAddress );
    for( readAddress = startAddress; readAddress <= endAddress; readAddress++ )
    {
      unsigned int temp=0;
      temp=fgetc(fd);
      data = (u8)temp;
      cache_ram[readAddress - 0xA000] = data;
      cacheValid_ram[readAddress - 0xA000] = 1;
    }
    fgetc(fd);
    fgetc(fd);
  }
  // read from cache
  memByte = cache_ram[address - 0xA000];
}

void mbc_sim_write_invalidate_rom()
{
//   mbc_sim_invalidate_rom();
  fprintf( stdout, "w%d %d (%04x %02x)\n", address, memByte, address, memByte );
  fprintf( fd, "w%d %d\n", address, memByte );
  fgetc(fd);
  fgetc(fd);
  
  int temp = memByte;
  if(temp == 0)
    temp = 1;
  temp = temp & 0x0F;
  romBank = temp;
}

void mbc_sim_write_invalidate_ram()
{
  mbc_sim_invalidate_ram();
  fprintf( stdout, "w%d %d (%04x %02x)\n", address, memByte, address, memByte );
  fprintf( fd, "w%d %d\n", address, memByte );
  fgetc(fd);
  fgetc(fd);
}


void mbc_sim_write_invalidate_rom_ram()
{
//   mbc_sim_invalidate_rom();
  mbc_sim_invalidate_ram();
  fprintf( stdout, "w%d %d (%04x %02x)\n", address, memByte, address, memByte );
  fprintf( fd, "w%d %d\n", address, memByte );
  fgetc(fd);
  fgetc(fd);
}

void mbc_sim_cleanup() {
  fclose( fd );
}

void mbc_sim_invalidate_rom() {
  int i;
  for(i=0; i<0x4000; i++)
  {
    cacheValid_rom[i] = 0;
  }
}

void mbc_sim_invalidate_ram() {
  int i;
  for(i=0; i<0x2000; i++)
  {
    cacheValid_ram[i] = 0;
  }
}

void mbc_sim_invalidate_rom_ram() {
  mbc_sim_invalidate_ram();
  mbc_sim_invalidate_rom();
}

