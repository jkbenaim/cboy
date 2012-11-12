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

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "memory.h"
#include "cpu.h"
#include "video.h"
#include "input.h"
#include "cart.h"
#include "mbc.h"

// u8 ram[0x1];
u8 vram_bank_zero[0x4000];	// 8000-9FFF
u8 vram_bank_one[0x4000];	// 8000-9FFF
u8 ram[0x2000];			// A000-BFFF (8 banks of 4KB each)
u8 wram_bank_zero[0x1000];	// C000-CFFF
u8 wram_bank_n[0x1000];		// D000-DFFF
u8 oam[0xA0];			// FE00-FE9F
u8 hram[0x7F];			// FF80-FFFE

void (*readmem[0x101])(void);
void (*writemem[0x101])(void);

address_t address;
u8 memByte;
u16 memWord;

void mem_init( void ) {
  int i;
  
  // cart banks are set up by mbc handler
  
  // vram
  for( i=0x80; i<=(0x9F); ++i ) {
    readmem[i]   = read_vram_bank_zero;
    writemem[i]  = write_vram_bank_zero;
  }
  
  // extram is set up by mbc handler
  
  // wram bank zero
  for( i=0xC0; i<=(0xCF); ++i ) {
    readmem[i]   = read_wram_bank_zero;
    writemem[i]  = write_wram_bank_zero;
  }
  
  // wram bank n
  for( i=0xD0; i<=(0xDF); ++i ) {
    readmem[i]   = read_wram_bank_n;
    writemem[i]  = write_wram_bank_n;
  }
  
  // echo ram
  for( i=0xE0; i<=(0xFD); ++i ) {
    readmem[i]   = read_echoram;
    writemem[i]  = write_echoram;
  }
  
  // OAM
  readmem[0xFE]   = read_oam;
  writemem[0xFE]  = write_oam;
  
  // special
  readmem[0xFF]  = read_special;
  writemem[0xFF] = write_special;
  
  // out of bounds - results in a crash
  readmem[0x100]  = read_out_of_bounds;
  writemem[0x100] = write_out_of_bounds;
}

// nothing
void read_nothing () {
  // TODO
  printf("Unimplemented memory read at address %04X, pc: %04X\n", address, state.pc);
  exit(1);
}

void write_nothing() {
  // TODO
  printf("Unimplemented memory write at address %04X, pc: %04X\n", address, state.pc);
  exit(1);
}


// vram
void select_vram_bank( u8 num ) {
  int i;
  if( (num & 0x01) == 0 )
    for( i=0x80; i<=(0x9F); ++i ) {
      readmem[i]   = read_vram_bank_zero;
      writemem[i]  = write_vram_bank_zero;
    }
  else
    for( i=0x80; i<=(0x9F); ++i ) {
      readmem[i]   = read_vram_bank_one;
      writemem[i]  = write_vram_bank_one;
    }
}
  
void read_vram_bank_zero() {
  memByte = vram_bank_zero[address & 0x1FFF];
}

void write_vram_bank_zero() {
  vram_bank_zero[address & 0x1FFF] = memByte;
}

void read_vram_bank_one() {
  memByte = vram_bank_one[address & 0x1FFF];
}

void write_vram_bank_one() {
  vram_bank_one[address & 0x1FFF] = memByte;
}




// wram bank zero
void read_wram_bank_zero() {
  memByte = wram_bank_zero[address & 0x0FFF];
}

void write_wram_bank_zero() {
  wram_bank_zero[address & 0x0FFF] = memByte;
}


// wram bank n
void read_wram_bank_n() {
  memByte = wram_bank_n[address & 0x0FFF];
}

void write_wram_bank_n() {
  wram_bank_n[address & 0x0FFF] = memByte;
}


// echo ram
void read_echoram() {
  address -= 0x2000;
  READ_BYTE();
}

void write_echoram() {
  address -= 0x2000;
  WRITE_BYTE();
}


// oam
void read_oam() {
  if(address > 0xFE9F)
    return;
  memByte = oam[address & 0x00FF];
}

void write_oam() {
  if(address > 0xFE9F)
    return;
  oam[address & 0x00FF] = memByte;
}

// "special" is the stuff at 0xFF00 and up
void read_special() {
  if(address >= 0xFF80 && address <= 0xFFFE)
  {
    // hram
    memByte = hram[address - 0xFF80];
    return;
  }
  
  switch(address) {
    case ADDR_JOYP:
      switch( state.joyp_select )
      {
	case INPUT_SELECT_BUTTONS:
	  memByte = state.joyp_buttons;
	  break;
	case INPUT_SELECT_DIRECTIONS:
	default:
	  memByte = state.joyp_directions;
	  break;
      }
//       printf("OMG JOYP READ\n");
      break;
    case ADDR_SB:
      memByte = state.sb;
//       printf("SB read: %02X\n", state.sb);
      break;
    case ADDR_SC:
      memByte = state.sc;
//       printf("SC read: %02X\n", state.sc);
      break;
    case ADDR_DIV:
      memByte = state.div;
//       printf("TIMER: DIV read\n");
      break;
    case ADDR_TIMA:
      memByte = state.tima;
//       printf("TIMER: TIMA read\n");
      break;
    case ADDR_TMA:
      memByte = state.tma;
//       printf("TIMER: TMA read\n");
      break;
    case ADDR_TAC:
      memByte = state.tac;
//       printf("TIMER: TAC read\n");
      break;
    case ADDR_IFLAG:
      memByte = state.iflag;
      break;
    case ADDR_NR10:
      memByte = state.nr10 | 0x80;;
      break;
    case ADDR_NR11:
      memByte = state.nr11 | 0x3F;
      break;
    case ADDR_NR12:
      memByte = state.nr12 | 0x00;
      break;
    case ADDR_NR13:
      memByte = state.nr13 | 0xFF;
      break;
    case ADDR_NR14:
      memByte = state.nr14 | 0xBF;
      break;
    case ADDR_NR20:
      memByte = state.nr20 | 0xFF;
      break;
    case ADDR_NR21:
      memByte = state.nr21 | 0x3F;
      break;
    case ADDR_NR22:
      memByte = state.nr22 | 0x00;
      break;
    case ADDR_NR23:
      memByte = state.nr23 | 0xBF;
      break;
    case ADDR_NR24:
      memByte = state.nr24 | 0xFF;
      break;
    case ADDR_NR30:
      memByte = state.nr30 | 0x7F;
      break;
    case ADDR_NR31:
      memByte = state.nr31 | 0xFF;
      break;
    case ADDR_NR32:
      memByte = state.nr32 | 0x9F;
      break;
    case ADDR_NR33:
      memByte = state.nr33 | 0xFF;
      break;
    case ADDR_NR34:
      memByte = state.nr34 | 0xBF;
      break;
    case ADDR_NR41:
      memByte = state.nr41 | 0xFF;
      break;
    case ADDR_NR42:
      memByte = state.nr42 | 0x00;
      break;
    case ADDR_NR43:
      memByte = state.nr43 | 0x00;
      break;
    case ADDR_NR44:
      memByte = state.nr44 | 0xBF;
      break;
    case ADDR_NR50:
      memByte = state.nr50 | 0x00;
      break;
    case ADDR_NR51:
      memByte = state.nr51 | 0x00;
      break;
    case ADDR_NR52:
      memByte = state.nr52 | 0x70;
      break;
    case ADDR_WAVERAM_0:
    case ADDR_WAVERAM_1:
    case ADDR_WAVERAM_2:
    case ADDR_WAVERAM_3:
    case ADDR_WAVERAM_4:
    case ADDR_WAVERAM_5:
    case ADDR_WAVERAM_6:
    case ADDR_WAVERAM_7:
    case ADDR_WAVERAM_8:
    case ADDR_WAVERAM_9:
    case ADDR_WAVERAM_A:
    case ADDR_WAVERAM_B:
    case ADDR_WAVERAM_C:
    case ADDR_WAVERAM_D:
    case ADDR_WAVERAM_E:
    case ADDR_WAVERAM_F:
      memByte = state.waveram[address & 0x000F];
      break;
    case ADDR_LCDC:
      memByte = state.lcdc;
      break;
    case ADDR_STAT:
      memByte = state.stat;
//       printf("Read STAT\n");
      break;
    case ADDR_SCY:
      memByte = state.scy;
      break;
    case ADDR_SCX:
      memByte = state.scx;
      break;
    case ADDR_LY:
      memByte = state.ly;
      break;
    case ADDR_BGP:
      memByte = state.bgp;
      break;
    case ADDR_OBP0:
      memByte = state.obp0;
      break;
    case ADDR_OBP1:
      memByte = state.obp1;
      break;
    case ADDR_WX:
      memByte = state.wx;
      break;
    case ADDR_WY:
      memByte = state.wy;
      break;
    case ADDR_CAPS:
      memByte = state.caps;
      break;
    case ADDR_HDMA1:
      memByte = state.hdma1;
      break;
    case ADDR_HDMA2:
      memByte = state.hdma2;
      break;
    case ADDR_HDMA3:
      memByte = state.hdma3;
      break;
    case ADDR_HDMA4:
      memByte = state.hdma4;
      break;
    case ADDR_HDMA5:
      memByte = state.hdma5;
      break;
    case ADDR_VBK:
      memByte = state.vbk;
      break;
    case ADDR_RP:
      // TODO
      memByte = 0;
      printf("Read IR port\n");
      break;
    case ADDR_BGPI:
      memByte = state.bgpi;
      break;
    case ADDR_BGPD:
      memByte = state.bgpd[ state.bgpi & 0x3F ];
      break;
    case ADDR_OBPI:
      memByte = state.obpi;
      break;
    case ADDR_OBPD:
      memByte = state.obpd[ state.obpi & 0x3F ];
      break;
    case ADDR_SVBK:
      // TODO
//       printf("Read SVBK\n");
      break;
    case ADDR_IE:
      memByte = state.ie;
      break;
    default:
      memByte = 0xFF;
      break;
  }
}

void write_special() {
  if(address >= 0xFF80 && address <= 0xFFFE)
  {
    // hram
    hram[address - 0xFF80] = memByte;
    return;
  }
  
  switch(address) {
    case ADDR_JOYP:
      if( (memByte & INPUT_SELECT_BUTTONS) == 0 )
	state.joyp_select = INPUT_SELECT_BUTTONS;
      else if( (memByte & INPUT_SELECT_DIRECTIONS) == 0 )
	state.joyp_select = INPUT_SELECT_DIRECTIONS;
      break;
    case ADDR_SB:
      state.sb = memByte;
//       printf("SB written: %02X\n", state.sb);
      break;
    case ADDR_SC:
//       printf("SC written: %02X\n", memByte);
      state.sc = memByte;
      state.serialBitsSent = 0;
      if( state.sc & SC_CLOCK_SPEED )
      {
        // fast transfer
        state.serialClocksUntilNextSend = 16; // 262144Hz or 524288Hz
      } else {
        // slow transfer
        state.serialClocksUntilNextSend = 512; // 8192Hz or 16384Hz
      }
      break;
    case ADDR_CBOY:
      printf("CBOY: %02X\n", memByte);
      break;
    case ADDR_DIV:
      state.div = memByte;
//       printf("TIMER: DIV write\n");
      break;
    case ADDR_TIMA:
      state.tima = memByte;
//       printf("TIMER: TIMA write\n");
      break;
    case ADDR_TMA:
      state.tma = memByte;
//       printf("TIMER: TMA write\n");
      break;
    case ADDR_TAC:
      state.tac = memByte;
//       printf("TIMER: TAC write\n");
      break;
    case ADDR_IFLAG:
      state.iflag = memByte;
      break;
    case ADDR_NR10:
      state.nr10 = memByte;
      break;
    case ADDR_NR11:
      state.nr11 = memByte;
      break;
    case ADDR_NR12:
      state.nr12 = memByte;
      break;
    case ADDR_NR13:
      state.nr13 = memByte;
      break;
    case ADDR_NR14:
      state.nr14 = memByte;
      break;
    case ADDR_NR20:
      state.nr20 = memByte;
      break;
    case ADDR_NR21:
      state.nr21 = memByte;
      break;
    case ADDR_NR22:
      state.nr22 = memByte;
      break;
    case ADDR_NR23:
      state.nr23 = memByte;
      break;
    case ADDR_NR24:
      state.nr24 = memByte;
      break;
    case ADDR_NR30:
      state.nr30 = memByte;
      break;
    case ADDR_NR31:
      state.nr31 = memByte;
      break;
    case ADDR_NR32:
      state.nr32 = memByte;
      break;
    case ADDR_NR33:
      state.nr33 = memByte;
      break;
    case ADDR_NR34:
      state.nr34 = memByte;
      break;
    case ADDR_NR41:
      state.nr41 = memByte;
      break;
    case ADDR_NR42:
      state.nr42 = memByte;
      break;
    case ADDR_NR43:
      state.nr43 = memByte;
      break;
    case ADDR_NR44:
      state.nr44 = memByte;
      break;
    case ADDR_NR50:
      state.nr50 = memByte;
      break;
    case ADDR_NR51:
      state.nr51 = memByte;
      break;
    case ADDR_NR52:
      state.nr52 = memByte;
      break;
    case ADDR_WAVERAM_0:
    case ADDR_WAVERAM_1:
    case ADDR_WAVERAM_2:
    case ADDR_WAVERAM_3:
    case ADDR_WAVERAM_4:
    case ADDR_WAVERAM_5:
    case ADDR_WAVERAM_6:
    case ADDR_WAVERAM_7:
    case ADDR_WAVERAM_8:
    case ADDR_WAVERAM_9:
    case ADDR_WAVERAM_A:
    case ADDR_WAVERAM_B:
    case ADDR_WAVERAM_C:
    case ADDR_WAVERAM_D:
    case ADDR_WAVERAM_E:
    case ADDR_WAVERAM_F:
      state.waveram[address & 0x000F] = memByte;
      break;
    case ADDR_LCDC:
      state.lcdc = memByte;
//       printf("LCDC: %02X\n", state.lcdc);
      break;
    case ADDR_STAT:
      state.stat = memByte & 0xF8;
//      printf("STAT: %02X\n", state.stat);
      break;
    case ADDR_SCY:
      state.scy = memByte;
      break;
    case ADDR_SCX:
      state.scx = memByte;
      break;
    case ADDR_LY:
      // This register is read-only.
      break;
    case ADDR_LYC:
      state.lyc = memByte;
      break;
    case ADDR_DMA:
      // OAM DMA
      // Source:	XX00-XX9F
      // Destination:	FE00-FE9F
      // TODO: restrict access to memory during DMA, etc.
      {
      int i;
      int sourceAddress = (int)(memByte) << 8;
      for( i=0x00; i<=0x9F; ++i)
      {
	address = sourceAddress + i;
	READ_BYTE();
	address = 0xFE00 + i;
	WRITE_BYTE();
      }
      }
      break;
    case ADDR_BGP:
      state.bgp = memByte;
      break;
    case ADDR_OBP0:
      state.obp0 = memByte;
      break;
    case ADDR_OBP1:
      state.obp1 = memByte;
      break;
    case ADDR_WX:
      state.wx = memByte;
      break;
    case ADDR_WY:
      state.wy = memByte;
      break;
    case ADDR_CAPS:
      printf("Wrote CAPS: %02X\n", memByte);
      state.caps = memByte;
      break;
    case ADDR_KEY1:
      printf("Wrote KEY1: %02X\n", memByte);
      break;
    case ADDR_VBK:
      state.vbk = memByte;
      select_vram_bank( memByte );
      break;
    case ADDR_ROM_DISABLE:
      if(memByte != 0)
      {
	state.bootRomEnabled = 0;
	cart_reset_mbc();
      }
      break;
    case ADDR_HDMA1:
      state.hdma1 = memByte;
      break;
    case ADDR_HDMA2:
      state.hdma2 = memByte;
      break;
    case ADDR_HDMA3:
      state.hdma3 = memByte;
      break;
    case ADDR_HDMA4:
      state.hdma4 = memByte;
      break;
    case ADDR_HDMA5:
      state.hdma5 = memByte;
      if( state.hdma5 == 0xFF ) break;
      if( (state.hdma5 & 0x80) == 0 )
      {
	// general-purpose DMA
	int source = state.hdma_source & 0xFFF0;
	int dest = 0x8000 + (state.hdma_destination & 0x1FF0);
	printf("HDMA: source=%04X, dest=%04X\n", source, dest);
	
	int i;
	for(i=0; i<(state.hdma5+1)*16; ++i)
	{
	  address = source + i;
	  READ_BYTE();
	  address = dest + i;
	  WRITE_BYTE();
	}
	state.hdma5 = 0xFF;
      }
      else
      {
	// h-blank DMA
      // TODO
	printf("WARNING: h-blank dma supressed\n");
	state.hdma5 = 0xFF;
      }
      break;
    case ADDR_RP:
      // TODO
      printf("Wrote IR port: %02X\n", memByte);
      break;
    case ADDR_BGPI:
      state.bgpi = memByte;
      break;
    case ADDR_BGPD:
      state.bgpd[ state.bgpi & 0x3F ] = memByte;
      if( state.bgpi & 0x80 )
	state.bgpi = 0x80 + ((state.bgpi + 1) & 0x3F);
      break;
    case ADDR_OBPI:
      state.obpi = memByte;
      break;
    case ADDR_OBPD:
//       printf("Wrote OBPD.\n");
      state.obpd[ state.obpi & 0x3F ] = memByte;
      if( state.obpi & 0x80 )
	state.obpi = 0x80 + ((state.obpi + 1) & 0x3F);
      break;
    case ADDR_SVBK:
      // TODO
//       printf("Wrote SVBK: %02X\n", memByte);
      break;
    case ADDR_IE:
      state.ie = memByte;
      break;
    default:
//       printf("%04X %02X\n",address,memByte);
      break;
  }
}

// out of bounds - results in a crash
void read_out_of_bounds( void )
{
  fprintf( stderr, "Out-of-bounds read, address: %04X, pc: %04X\n", address, state.pc );
  exit(1);
}

void write_out_of_bounds( void )
{
  fprintf( stderr, "Out-of-bounds write, address: %04X, pc: %04X\n", address, state.pc );
  exit(1);
}

void read_word( void ) {
  int a, b;
  READ_BYTE();
  a = memByte;
  address++;
  READ_BYTE();
  b = memByte;
  address--;
  memWord = (b<<8) + a;
}

void write_word( void ) {
  address %= 0x10000;
  int a, b;
  a = memWord & 0xff;
  b = (memWord>>8) & 0xff;
  memByte = a;
  WRITE_BYTE();
  memByte = b;
  address++;
  WRITE_BYTE();
  address--;
}
