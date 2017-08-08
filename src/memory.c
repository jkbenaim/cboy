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
#include "audio.h"

uint8_t ram[0x1];
uint8_t vram_bank_zero[0x2000];	// 8000-9FFF
uint8_t vram_bank_one[0x2000];	// 8000-9FFF
uint8_t wram[0x8000];
uint8_t *wram_bank_zero;	// C000-CFFF
uint8_t *wram_bank_n;		// D000-DFFF
uint8_t oam[0xA0];		// FE00-FE9F
uint8_t hram[0x7F];		// FF80-FFFE

uint8_t (*readmem[0x101])(uint16_t address);
void (*writemem[0x101])(uint16_t address, uint8_t data);

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
    wram_bank_zero = wram;
    for( i=0xC0; i<=(0xCF); ++i ) {
        readmem[i]   = read_wram_bank_zero;
        writemem[i]  = write_wram_bank_zero;
    }

    // wram bank n
    wram_bank_n = wram + 0x1000;
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
uint8_t read_nothing( uint16_t address ) {
    // TODO
    printf("Unimplemented memory read at address %04X, pc: %04X\n", address, state.pc);
    exit(1);
    return 0;
}

void write_nothing( uint16_t address, uint8_t data ) {
    // TODO
    printf("Unimplemented memory write at address %04X, pc: %04X\n", address, state.pc);
    exit(1);
}


// vram bank switch
void select_vram_bank( uint8_t num ) {
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

extern int pause;
uint8_t read_vram_bank_zero( uint16_t address ) {
    //   if( state.vid_mode == 3 )
    //   {
    //     return 0xff;
    //   }
    //   else
    return vram_bank_zero[address & 0x1FFF];
}

void write_vram_bank_zero( uint16_t address, uint8_t data) {
    //   if( state.vid_mode != 3 )
    vram_bank_zero[address & 0x1FFF] = data;
}

uint8_t read_vram_bank_one( uint16_t address ) {
    //   if( state.vid_mode == 3 )
    //     return 0xff;
    //   else
    return vram_bank_one[address & 0x1FFF];
}

void write_vram_bank_one( uint16_t address, uint8_t data ) {
    //   if( state.vid_mode != 3 )
    vram_bank_one[address & 0x1FFF] = data;
}




// wram bank zero
uint8_t read_wram_bank_zero( uint16_t address ) {
    return wram_bank_zero[address & 0x0FFF];
}

void write_wram_bank_zero( uint16_t address, uint8_t data ) {
    wram_bank_zero[address & 0x0FFF] = data;
}


// wram bank n
uint8_t read_wram_bank_n( uint16_t address ) {
    return wram_bank_n[address & 0x0FFF];
}

void write_wram_bank_n( uint16_t address, uint8_t data ) {
    wram_bank_n[address & 0x0FFF] = data;
}


// echo ram
uint8_t read_echoram( uint16_t address ) {
    return read_byte(address - 0x2000);
}

void write_echoram( uint16_t address, uint8_t data ) {
    write_byte(address - 0x2000, data);
}


// oam
uint8_t read_oam( uint16_t address ) {
    address &= 0xFF;
    uint8_t data;

    if(address > 0x9F)
        data = 0;
    else
        data = oam[address];

    return data;
}

void write_oam( uint16_t address, uint8_t data ) {
    address &= 0xFF;
    if(address > 0x9F)
        return;
    oam[address] = data;
}

// "special" is the stuff at 0xFF00 and up
uint8_t read_special( uint16_t address ) {
    if(address >= 0xFF80 && address <= 0xFFFE)
    {
        // hram
        return hram[address - 0xFF80];
    }

    switch(address) {
        case ADDR_JOYP:
            switch( state.joyp_select )
            {
                case INPUT_SELECT_BUTTONS:
                    return state.joyp_buttons;
                    break;
                case INPUT_SELECT_DIRECTIONS:
                default:
                    return state.joyp_directions;
                    break;
            }
            break;
        case ADDR_SB:
            return state.sb;
            break;
        case ADDR_SC:
            return state.sc;
            break;
        case ADDR_DIV:
            return state.div;
            break;
        case ADDR_TIMA:
            return state.tima;
            break;
        case ADDR_TMA:
            return state.tma;
            break;
        case ADDR_TAC:
            return state.tac;
            break;
        case ADDR_IFLAG:
            return state.iflag;
            break;
        case ADDR_NR10:
        case ADDR_NR11:
        case ADDR_NR12:
        case ADDR_NR13:
        case ADDR_NR14:
        case ADDR_NR20:
        case ADDR_NR21:
        case ADDR_NR22:
        case ADDR_NR23:
        case ADDR_NR24:
        case ADDR_NR30:
        case ADDR_NR31:
        case ADDR_NR32:
        case ADDR_NR33:
        case ADDR_NR34:
        case ADDR_NR41:
        case ADDR_NR42:
        case ADDR_NR43:
        case ADDR_NR44:
        case ADDR_NR50:
        case ADDR_NR51:
        case ADDR_NR52:
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
            return audio_read( address );
            break;
        case ADDR_LCDC:
            return state.lcdc;
            break;
        case ADDR_STAT:
            return state.stat;
            break;
        case ADDR_SCY:
            return state.scy;
            break;
        case ADDR_SCX:
            return state.scx;
            break;
        case ADDR_LY:
            return state.ly;
            break;
        case ADDR_BGP:
            return state.bgp;
            break;
        case ADDR_OBP0:
            return state.obp0;
            break;
        case ADDR_OBP1:
            return state.obp1;
            break;
        case ADDR_WX:
            return state.wx;
            break;
        case ADDR_WY:
            return state.wy;
            break;
        case ADDR_KEY1:
            return state.key1 | 0x7E;
            break;
        case ADDR_CAPS:
            return state.caps;
            break;
        case ADDR_HDMA1:
            return state.hdma1;
            break;
        case ADDR_HDMA2:
            return state.hdma2;
            break;
        case ADDR_HDMA3:
            return state.hdma3;
            break;
        case ADDR_HDMA4:
            return state.hdma4;
            break;
        case ADDR_HDMA5:
            return state.hdma5;
            break;
        case ADDR_VBK:
            return state.vbk;
            break;
        case ADDR_RP:
            // IR port - TODO
            return 0;
            break;
        case ADDR_BGPI:
            return state.bgpi;
            break;
        case ADDR_BGPD:
            return state.bgpd[ state.bgpi & 0x3F ];
            break;
        case ADDR_OBPI:
            return state.obpi;
            break;
        case ADDR_OBPD:
            return state.obpd[ state.obpi & 0x3F ];
            break;
        case ADDR_SVBK:
            return state.svbk;
            break;
        case ADDR_IE:
            return state.ie;
            break;
        default:
            return 0xFF;
            break;
    }
}

void write_special( uint16_t address, uint8_t data ) {
    if(address >= 0xFF80 && address <= 0xFFFE)
    {
        // hram
        hram[address - 0xFF80] = data;
        return;
    }

    switch(address) {
        case ADDR_JOYP:
            if( (data & INPUT_SELECT_BUTTONS) == 0 )
                state.joyp_select = INPUT_SELECT_BUTTONS;
            else if( (data & INPUT_SELECT_DIRECTIONS) == 0 )
                state.joyp_select = INPUT_SELECT_DIRECTIONS;
            break;
        case ADDR_SB:
            state.sb = data;
            //       printf("SB written: %02X\n", state.sb);
            break;
        case ADDR_SC:
            //       printf("SC written: %02X\n", data);
            state.sc = data;
            state.serialBitsSent = 0;
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
            printf("CBOY: %02X\n", data);
            break;
        case ADDR_DIV:
            state.div = data;
            //       printf("TIMER: DIV write\n");
            break;
        case ADDR_TIMA:
            state.tima = data;
            //       printf("TIMER: TIMA write\n");
            break;
        case ADDR_TMA:
            state.tma = data;
            //       printf("TIMER: TMA write\n");
            break;
        case ADDR_TAC:
            state.tac = data;
            //       printf("TIMER: TAC write\n");
            break;
        case ADDR_IFLAG:
            state.iflag = data;
            break;
        case ADDR_NR10:
        case ADDR_NR11:
        case ADDR_NR12:
        case ADDR_NR13:
        case ADDR_NR14:
        case ADDR_NR20:
        case ADDR_NR21:
        case ADDR_NR22:
        case ADDR_NR23:
        case ADDR_NR24:
        case ADDR_NR30:
        case ADDR_NR31:
        case ADDR_NR32:
        case ADDR_NR33:
        case ADDR_NR34:
        case ADDR_NR41:
        case ADDR_NR42:
        case ADDR_NR43:
        case ADDR_NR44:
        case ADDR_NR50:
        case ADDR_NR51:
        case ADDR_NR52:
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
            audio_write( address, data );
            break;
        case ADDR_LCDC:
            state.lcdc = data;
            if( (state.lcdc & LCDC_LCD_ENABLE) == 0 )
                state.ly = 0;
            break;
        case ADDR_STAT:
            {
                uint8_t readOnlyBits = state.stat & 0x87;
                uint8_t writableBits = data & 0x78;
                state.stat = writableBits | readOnlyBits | 0x80;
                //       printf("STAT: %02X\n", state.stat);
                //       int i;
                //       for(i=0;i<8;i++)
                //         if(state.stat & 1<<i)
                //           switch(i)
                //           {
                //             case 7:
                //               printf("\tbit 7\n");
                //               break;
                //             case 6:
                //               printf("\tLYC=LY interrupt (ly=%d, lyc=%d)\n", state.ly, state.lyc);
                //               break;
                //             case 5:
                //               printf("\tMode 2 OAM interrupt\n");
                //               break;
                //             case 4:
                //               printf("\tMode 1 V-Blank Interrupt\n");
                //               break;
                //             case 3:
                //               printf("\tMode 0 V-Blank Interrupt\n");
                //               break;
                //             default:
                //               break;
                //           }
            }
            break;
        case ADDR_SCY:
            state.scy = data;
            break;
        case ADDR_SCX:
            state.scx = data;
            break;
        case ADDR_LY:
            // This register is read-only.
            break;
        case ADDR_LYC:
            state.lyc = data;
            break;
        case ADDR_DMA:
            // OAM DMA
            // Source:	XX00-XX9F
            // Destination:	FE00-FE9F
            // TODO: restrict access to memory during DMA, etc.
            {
                int i;
                int sourceAddress = (int)(data) << 8;
                for( i=0x00; i<=0x9F; ++i)
                {
                    uint8_t temp = read_byte(sourceAddress + i);
                    write_byte(0xFE00 + i, temp);
                }
            }
            break;
        case ADDR_BGP:
            state.bgp = data;
            break;
        case ADDR_OBP0:
            inval_palette = 1;
            state.obp0 = data;
            break;
        case ADDR_OBP1:
            inval_palette = 1;
            state.obp1 = data;
            break;
        case ADDR_WX:
            state.wx = data;
            break;
        case ADDR_WY:
            state.wy = data;
            break;
        case ADDR_CAPS:
            printf("Wrote CAPS: %02X\n", data);
            inval_palette = 1;
            state.caps = data;
            break;
        case ADDR_KEY1:
            printf("Wrote KEY1: %02X\n", data);
            state.key1 = (state.key1&0xFE) | (data&0x01);
            break;
        case ADDR_VBK:
            state.vbk = data;
            select_vram_bank( data );
            break;
        case ADDR_ROM_DISABLE:
            if((data != 0) && state.bootRomEnabled == 1)
            {
                state.bootRomEnabled = 0;
                cart_reset_mbc();
            }
            break;
        case ADDR_HDMA1:
            state.hdma1 = data;
            break;
        case ADDR_HDMA2:
            state.hdma2 = data;
            break;
        case ADDR_HDMA3:
            state.hdma3 = data;
            break;
        case ADDR_HDMA4:
            state.hdma4 = data;
            break;
        case ADDR_HDMA5:
            state.hdma5 = data;
            //       if( state.hdma5 == 0xFF ) break;
            if( (state.hdma5 & 0x80) == 0 )
            {
                // general-purpose DMA
                int source = state.hdma_source & 0xFFF0;
                int dest = 0x8000 + (state.hdma_destination & 0x1FF0);
                int length = state.hdma5 & 0x7F;
                printf("general HDMA: source=%04X, dest=%04X, length=%02X\n", source, dest, length);

                int i;
                for(i=0; i<(length+1)*16; ++i)
                {
                    uint8_t temp = read_byte(source+i);
                    write_byte(dest+i, temp);
                }
                state.hdma5 = 0xFF;
                state.instr_time += (length+1)*2;
            }
            else
            {
                // h-blank DMA
                // TODO
                int source = state.hdma_source & 0xFFF0;
                int dest = 0x8000 + (state.hdma_destination & 0x1FF0);
                int length = state.hdma5 & 0x7F;
                printf("h-blank HDMA: source=%04X, dest=%04X, length=%02X\n", source, dest, length);
                // 	state.hdma5 &= 0x7F;
            }
            break;
        case ADDR_RP:
            // TODO
            printf("Wrote IR port: %02X\n", data);
            break;
        case ADDR_BGPI:
            state.bgpi = data;
            break;
        case ADDR_BGPD:
            inval_palette = 1;
            state.bgpd[ state.bgpi & 0x3F ] = data;
            if( state.bgpi & 0x80 )
                state.bgpi = 0x80 + ((state.bgpi + 1) & 0x3F);
            break;
        case ADDR_OBPI:
            state.obpi = data;
            break;
        case ADDR_OBPD:
            //       printf("Wrote OBPD.\n");
            inval_palette = 1;
            state.obpd[ state.obpi & 0x3F ] = data;
            if( state.obpi & 0x80 )
                state.obpi = 0x80 + ((state.obpi + 1) & 0x3F);
            break;
        case ADDR_SVBK:
            // this selects the WRAM bank in CGB mode
            //       printf("Wrote SVBK: %02X\n", data);

            // we're in CGB mode
            state.svbk = data & 0x07;

            if(state.svbk == 0)
                wram_bank_n = wram + 0x1000;
            else
                wram_bank_n = wram + 0x1000 * state.svbk;

            break;
        case ADDR_IE:
            state.ie = data;
            break;
        default:
            //       printf("%04X %02X\n",address,data);
            break;
    }
}

// out of bounds - results in a crash
uint8_t read_out_of_bounds( uint16_t address )
{
    fprintf( stderr, "Out-of-bounds read, address: %04X, pc: %04X\n", address, state.pc );
    exit(1);
}

void write_out_of_bounds( uint16_t address, uint8_t data )
{
    fprintf( stderr, "Out-of-bounds write, address: %04X, pc: %04X\n", address, state.pc );
    exit(1);
}

uint8_t read_byte( uint16_t address ) {
    return readmem[address>>8](address);
}

void write_byte( uint16_t address, uint8_t data ) {
    writemem[address>>8](address, data);
}

uint16_t read_word( uint16_t address ) {
    int a, b;
    a = read_byte(address);
    b = read_byte(address+1);
    return (b<<8) + a;
}

void write_word(  uint16_t address, uint16_t data  ) {
    address %= 0x10000;
    int a, b;
    a = data & 0xff;
    b = (data>>8) & 0xff;
    write_byte(address  , a);
    write_byte(address+1, b);
}
