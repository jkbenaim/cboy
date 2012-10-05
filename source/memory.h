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

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"
#include "cart.h"
#include "assert.h"

#ifndef NDEBUG
#define READ_BYTE() { \
assert( "Reading byte: address is non-negative", address >= 0 ); \
assert( "Reading byte: address is less than 0x10000", address < 0x10000 ); \
readmem[address>>8](); \
}
#define READ_WORD() read_word()
#define WRITE_BYTE() writemem[address>>8]()
#define WRITE_WORD() write_word()
#else // NDEBUG
#define READ_BYTE() readmem[address>>8]()
#define READ_WORD() read_word()
#define WRITE_BYTE() writemem[address>>8]()
#define WRITE_WORD() write_word()
#endif

extern void read_word( void );
extern void write_word( void );

#define ADDR_JOYP	0xFF00
#define ADDR_SB		0xFF01
#define ADDR_SC		0xFF02
#define ADDR_CBOY	0xFF03
#define ADDR_DIV	0xFF04
#define ADDR_TIMA	0xFF05
#define ADDR_TMA	0xFF06
#define ADDR_TAC	0xFF07
#define ADDR_IFLAG	0xFF0F
#define ADDR_NR10	0xFF10
#define ADDR_NR11	0xFF11
#define ADDR_NR12	0xFF12
#define ADDR_NR13	0xFF13
#define ADDR_NR14	0xFF14
#define ADDR_NR20	0xFF15
#define ADDR_NR21	0xFF16
#define ADDR_NR22	0xFF17
#define ADDR_NR23	0xFF18
#define ADDR_NR24	0xFF19
#define ADDR_NR30	0xFF1A
#define ADDR_NR31	0xFF1B
#define ADDR_NR32	0xFF1C
#define ADDR_NR33	0xFF1D
#define ADDR_NR34	0xFF1E
#define ADDR_NR41	0xFF20
#define ADDR_NR42	0xFF21
#define ADDR_NR43	0xFF22
#define ADDR_NR44	0xFF23
#define ADDR_NR50	0xFF24
#define ADDR_NR51	0xFF25
#define ADDR_NR52	0xFF26
#define ADDR_WAVERAM_0	0xFF30
#define ADDR_WAVERAM_1	0xFF31
#define ADDR_WAVERAM_2	0xFF32
#define ADDR_WAVERAM_3	0xFF33
#define ADDR_WAVERAM_4	0xFF34
#define ADDR_WAVERAM_5	0xFF35
#define ADDR_WAVERAM_6	0xFF36
#define ADDR_WAVERAM_7	0xFF37
#define ADDR_WAVERAM_8	0xFF38
#define ADDR_WAVERAM_9	0xFF39
#define ADDR_WAVERAM_A	0xFF3A
#define ADDR_WAVERAM_B	0xFF3B
#define ADDR_WAVERAM_C	0xFF3C
#define ADDR_WAVERAM_D	0xFF3D
#define ADDR_WAVERAM_E	0xFF3E
#define ADDR_WAVERAM_F	0xFF3F
#define ADDR_LCDC	0xFF40
#define ADDR_STAT	0xFF41
#define ADDR_SCY	0xFF42
#define ADDR_SCX	0xFF43
#define ADDR_LY		0xFF44
#define ADDR_LYC	0xFF45
#define ADDR_DMA	0xFF46
#define ADDR_BGP	0xFF47
#define ADDR_OBP0	0xFF48
#define ADDR_OBP1	0xFF49
#define ADDR_WY		0xFF4A
#define ADDR_WX		0xFF4B
#define ADDR_CAPS	0xFF4C	// CGB bios writes here to indicate compatibility level.
				// 04h=regular gameboy, 80h=cgb compatible, C0h=cgb required
				// I'm not sure if this is correct.
#define ADDR_KEY1	0xFF4D	// cpu speed switch (cgb only)
#define ADDR_VBK	0xFF4F	// select VRAM bank (cgb only)
#define ADDR_ROM_DISABLE	0xFF50
#define ADDR_HDMA1	0xFF51	// new DMA source, high (cgb only)
#define ADDR_HDMA2	0xFF52	// new DMA source, low (cgb only)
#define ADDR_HDMA3	0xFF53	// new DMA destination, high (cgb only)
#define ADDR_HDMA4	0xFF54	// new DMA destination, low (cgb only)
#define ADDR_HDMA5	0xFF55	// new DMA length/mode/start (cgb only)
#define ADDR_RP		0xFF56	// IR port (cgb only)

#define ADDR_BGPI	0xFF68	// background palette index (cgb only)
#define ADDR_BGPD	0xFF69	// background palette data (cgb only)
#define ADDR_OBPI	0xFF6A	// sprite palette index (cgb only)
#define ADDR_OBPD	0xFF6B	// sprite data index (cgb only)
#define ADDR_SVBK	0xFF70	// select WRAM bank (cgb only)
#define ADDR_IE		0xFFFF

#define LCDC_LCD_ENABLE			0x80	// 0=Off, 1=On
#define LCDC_WINDOW_TILE_MAP_SELECT	0x40	// 0=9800-9BFF, 1=9C00-9FFF
#define LCDC_WINDOW_ENABLE		0x20	// 0=Off, 1=On
#define LCDC_TILE_DATA_SELECT		0x10	// 0=8800-97FF, 1=8000-8FFF
#define LCDC_BG_TILE_MAP_SELECT		0x08	// 0=9800-9BFF, 1=9C00,9FFF
#define LCDC_OBJ_SIZE			0x04	// 0=8x8, 1=8x16
#define LCDC_OBJ_DISPLAY		0x02	// 0=Off, 1=On
#define LCDC_BG_ENABLE			0x01	// 0=Off, 1=On

#define SC_TRANSFER		0x80	// 0=No Transfer, 1=Start
#define SC_CLOCK_SPEED		0x02	// 0=Normal, 1=Fast (CGB only)
#define SC_SHIFT_CLOCK		0x01	// 0=External Clock, 1=Internal Clock

extern void (*readmem[0x101])(void);
extern void (*writemem[0x101])(void);

extern address_t address;
extern u8 memByte;
extern u16 memWord;

extern u8 vram_bank_zero[0x4000];
extern u8 vram_bank_one[0x4000];
extern u8 oam[0xA0];
extern u8 hram[0x7F];


void mem_init( void );

// nothing
void read_nothing( void );
void write_nothing( void );

// cart bank zero
void read_cart_bank_zero( void );
void write_cart_bank_zero( void );

// cart bank n
void read_cart_bank_n( void );
void write_cart_bank_n( void );

// vram
void select_vram_bank( u8 num );
void read_vram_bank_zero( void );
void write_vram_bank_zero( void );
void read_vram_bank_one( void );
void write_vram_bank_one( void );

// ram
void read_ram( void );
void write_ram( void );

// wram bank zero
void read_wram_bank_zero( void );
void write_wram_bank_zero( void );

// wram bank n
void read_wram_bank_n( void );
void write_wram_bank_n( void );

// echo ram
void read_echoram( void );
void write_echoram( void );

// oam
void read_oam( void );
void write_oam( void );

// special
void read_special( void );
void write_special( void );

// out of bounds
void read_out_of_bounds( void );
void write_out_of_bounds( void );

#endif // !_MEMORY_H_
