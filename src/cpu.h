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

#ifndef _CPU_H_
#define _CPU_H_

#include "types.h"
#include "endian.h"

#ifdef __cplusplus
extern "C" {
#endif

struct state_s {
  pc_t pc;
  uint16_t sp;
#ifdef __BIG_ENDIAN__
  union {
    struct {
      uint8_t h;
      uint8_t l;
    };
    uint16_t hl;
  };
  union {
    struct {
      uint8_t d;
      uint8_t e;
    };
    uint16_t de;
  };
  union {
    struct {
      uint8_t b;
      uint8_t c;
    };
    uint16_t bc;
  };
#else	// __LITTLE_ENDIAN__
  union {
    struct {
      uint8_t l;
      uint8_t h;
    };
    uint16_t hl;
  };
  union {
    struct {
      uint8_t e;
      uint8_t d;
    };
    uint16_t de;
  };
  union {
    struct {
      uint8_t c;
      uint8_t b;
    };
    uint16_t bc;
  };
#endif
  uint8_t a;
  uint_least16_t flag_c;
  uint_least8_t flag_h1;
  uint_least8_t flag_h2;
  uint_least8_t flag_n;
  uint8_t flag_z;
  int vid_mode;
  int old_vid_mode;
  uint8_t joyp;
  uint8_t joyp_buttons;
  uint8_t joyp_directions;
  uint8_t joyp_select;
  uint64_t masterClock; // incremented at 1,048,576 Hz
  uint64_t lastMasterClock; 
  uint8_t div, tima, tma, tac;	// timers
  int serialBitsSent;	// bits sent over link
  int serialClocksUntilNextSend;
  int serialTimeoutClock;
  uint8_t sb, sc;
  uint8_t lcdc;
  uint8_t stat;
  uint8_t pending_stat_interrupts;
  uint8_t scy;
  uint8_t scx;
  uint8_t ly;
  int line_progress;
  uint8_t lyc;
  uint8_t last_ly;
  uint8_t bgp;
  uint8_t obp0, obp1;
  uint8_t wx,wy;
  uint8_t last_line_rendered;
  int bootRomEnabled;	// 0 = disabled, 1 = enabled
  int ime;		// set to IME_ENABLED or IME_DISABLED
  int ie, iflag;
  int halt;		// 0 = not halted, 1 = halted
  uint8_t op;
  uint8_t cb_op;
  int halt_glitch;
  int frame_done;
  int instr_time;
  uint8_t key1;
  int cpu_speed;
  
  //cgb stuff
  uint8_t bgpi;
  uint8_t bgpd[0x40];
  uint8_t obpi;
  uint8_t obpd[0x40];
  uint8_t vbk;
  uint8_t caps;
  uint8_t svbk;
#ifdef __BIG_ENDIAN__
  union {
    struct {
      uint8_t hdma1;
      uint8_t hdma2;
    };
    uint16_t hdma_source;
  };
  union {
    struct {
      uint8_t hdma3;
      uint8_t hdma4;
    };
    uint16_t hdma_destination;
  };
#else	// __LITTLE_ENDIAN__
  union {
    struct {
      uint8_t hdma2;
      uint8_t hdma1;
    };
    uint16_t hdma_source;
  };
  union {
    struct {
      uint8_t hdma4;
      uint8_t hdma3;
    };
    uint16_t hdma_destination;
  };
#endif
  uint8_t hdma5;
}; // state

extern struct state_s state;


/*
 * Flags register masks
 */
#define FLAGS_NOTUSED_0		0x01
#define FLAGS_NOTUSED_1		0x02
#define FLAGS_NOTUSED_2		0x04
#define FLAGS_NOTUSED_3		0x08
#define FLAGS_C			0x10
#define FLAGS_H			0x20
#define FLAGS_N			0x40
#define FLAGS_Z			0x80

/*
 * Flag (re)set macros
 */
#define SET_Z()                 (state.flag_z = 0)
#define RESET_Z()               (state.flag_z = 1)
#define ISSET_Z()               (state.flag_z == 0)

#define SET_N()                 (state.flag_n = 1)
#define RESET_N()               (state.flag_n = 0)
#define ISSET_N()               (state.flag_n != 0)

#define SET_H()                 (state.flag_h1 = state.flag_h2 = 0xff)
#define RESET_H()               (state.flag_h1 = state.flag_h2 = 0x00)
#define ISSET_H()               (((state.flag_h1&0xf)+(state.flag_h2&0x0f))&0xf0)

#define SET_C()                 (state.flag_c = 0x100)
#define RESET_C()               (state.flag_c = 0)
#define ISSET_C()               (state.flag_c & 0x100)

/*
 * Number of cycles for each video mode.
 * {  mode 2: 80 cycles
 *    mode 3: 172 cycles
 *    mode 0: 204 cycles  } x 144
 * mode 1: 4560 cycles
 */
#define CYCLES_MODE_2	80
#define CYCLES_MODE_3	172
#define CYCLES_MODE_0	204
#define CYCLES_MODE_1	4560
#define CYCLES_LINE	(CYCLES_MODE_2 + CYCLES_MODE_3 + CYCLES_MODE_0)
#define CYCLES_FRAME	70224
#define CYCLES_RENDER_LINE	(CYCLES_MODE_2 + CYCLES_MODE_3)

/*
 * Interrupt masks
 */
#define IMASK_VBLANK	0x01
#define IMASK_LCD_STAT	0x02
#define IMASK_TIMER	0x04
#define IMASK_SERIAL	0x08
#define IMASK_JOYPAD	0x10
#define IME_DISABLED	0x00
#define IME_ENABLED	0xFF

/*
 * Interrupt queue message names
 */
typedef enum {
  INTERRUPT_VBLANK = 1,
  INTERRUPT_LCD_STAT,
  INTERRUPT_TIMER,
  INTERRUPT_SERIAL,
  INTERRUPT_JOYPAD
} InterruptMessage_t;

// ops
void UNDEF(void);
void UNDEF_CB(void);
void LOCKUP(void);	// D3,DB,DD,EB,EC,ED,F4,FC,FD
void NOP(void);		// 00
void LD_BC_WORD(void);	// 01
void LD_BC_A(void);	// 02
void INC_BC(void);	// 03
void INC_R(void);
void DEC_R(void);
void LD_B_BYTE(void);	// 06
void RLCA(void);	// 07
void LD_WORD_SP(void);	// 08
void ADD_HL_BC(void);	// 09
void LD_A_BC(void);	// 0A
void DEC_BC(void);	// 0B
void LD_C_BYTE(void);	// 0E
void RRCA(void);	// 0F
void STOP(void);	// 10
void LD_DE_WORD(void);	// 11
void LD_DE_A(void);	// 12
void INC_DE(void);	// 13
void LD_D_BYTE(void);	// 16
void RLA(void);		// 17
void JR_INDEX(void);	// 18
void ADD_HL_DE(void);	// 19
void LD_A_DE(void);	// 1A
void DEC_DE(void);	// 1B
void LD_E_BYTE(void);	// 1E
void RRA(void);		// 1F
void JR_NZ_INDEX(void);	// 20
void LD_HL_WORD(void);	// 21
void LDI_HL_A(void);	// 22
void INC_HL(void);	// 23
void LD_H_BYTE(void);	// 26
void DAA(void);		// 27
void JR_Z_INDEX(void);	// 28
void ADD_HL_HL(void);	// 29
void LDI_A_HL(void);	// 2A
void DEC_HL(void);	// 2B
void LD_L_BYTE(void);	// 2E
void CPL(void);		// 2F
void JR_NC_INDEX(void);	// 30
void LD_SP_WORD(void);	// 31
void LDD_HL_A(void);	// 32
void INC_SP(void);	// 33
void INC_AT_HL(void);	// 34
void DEC_AT_HL(void);	// 35
void LD_HL_BYTE(void);	// 36
void SCF(void);		// 37
void JR_C_INDEX(void);	// 38
void ADD_HL_SP(void);	// 39
void LDD_A_HL(void);	// 3A
void DEC_SP(void);	// 3B
void LD_A_BYTE(void);	// 3E
void CCF(void);		// 3F
void LD_R_R(void);
void LD_B_HL(void);	// 46
void LD_C_HL(void);	// 4E
void LD_D_HL(void);	// 56
void LD_E_HL(void);	// 5E
void LD_H_HL(void);	// 66
void LD_L_HL(void);	// 6E
void LD_HL_R(void);	// 70-75,77
void HALT(void);	// 76
void LD_A_HL(void);	// 7E
void ADD_A_R(void);	// 80-85,87
void ADD_A_HL(void);	// 86
void ADC_A_R(void);	// 88-8D,8F
void ADC_A_HL(void);	// 8E
void SUB_R(void);	// 90-95,97
void SUB_HL(void);	// 96
void SBC_A_R(void);	// 98-9D,9F
void SBC_A_HL(void);	// 9E
void AND_R(void);	// A0-A5,A7
void AND_HL(void);	// A6
void XOR_R(void);	// A8-AD,AF
void XOR_HL(void);	// AE
void OR_R(void);
void OR_HL(void);	// B6
void CP_R(void);	// B8
void CP_HL(void);	// BE
void RET_CC(void);
void POP_BC(void);	// C1
void JP_NZ_ADDR(void);	// C2
void JP_ADDR(void);	// C3
void CALL_NZ(void);	// C4
void PUSH_BC(void);	// C5
void ADD_A_BYTE(void);	// C6
void RST_0(void);	// C7
void RET(void);		// C9
void JP_Z_ADDR(void);	// CA
void CB_PREFIX(void);	// CB
void CALL_Z(void);	// CC
void CALL(void);	// CD
void ADC_A_BYTE(void);	// CE
void RST_8(void);	// CF
void POP_DE(void);	// D1
void JP_NC_ADDR(void);	// D2
void CALL_NC(void);	// D4
void PUSH_DE(void);	// D5
void SUB_A_BYTE(void);	// D6
void RST_10(void);	// D7
void RETI(void);	// D9
void JP_C_ADDR(void);	// DA
void CALL_C(void);	// DC
void SBC_A_BYTE(void);	// DE
void RST_18(void);	// DF
void LD_FF_BYTE_A(void);// E0
void POP_HL(void);	// E1
void LD_FF_C_A(void);	// E2
void PUSH_HL(void);	// E5
void AND_BYTE(void);	// E6
void RST_20(void);	// E7
void ADD_SP_OFFSET(void);	// E8
void JP_HL(void);	// E9
void LD_WORD_A(void);	// EA
void XOR_BYTE(void);	// EE
void RST_28(void);	// EF
void LD_A_FF_BYTE(void);// F0
void POP_AF(void);	// F1
void LD_A_FF_C(void);	// F2
void DI(void);		// F3
void PUSH_AF(void);	// F5
void OR_BYTE(void);	// F6
void RST_30(void);	// F7
void LDHL_SP_OFFSET(void);	// F8
void LD_SP_HL(void);	// F9
void LD_A_WORD(void);	// FA
void EI(void);		// FB
void CP_BYTE(void);	// FE
void RST_38(void);	// FF

// cb ops
void CB_RLC_R(void);	// CB 00-05,07
void CB_RLC_HL(void);	// CB 06
void CB_RRC_R(void);	// CB 08-0D,0F
void CB_RRC_HL(void);	// CB 0E
void CB_RL_R(void);	// CB 10-15,17
void CB_RL_HL(void);	// CB 16
void CB_RR_R(void);	// CB 18-1D,1F
void CB_RR_HL(void);	// CB 1E
void CB_SLA_R(void);	// CB 20-25,27
void CB_SLA_HL(void);	// CB 26
void CB_SRA_R(void);	// CB 28-2D,2F
void CB_SRA_HL(void);	// CB 2E
void CB_SWAP_R(void);	// CB 30-35,37
void CB_SWAP_HL(void);	// CB 36
void CB_SRL_R(void);	// CB 38-3D,3F
void CB_SRL_HL(void);	// CB 3E
void CB_BIT_R(void);	// CB 40-7F (except (HL))
void CB_BIT_HL(void);	// CB 46,4E,56,5E,66,6E,76,7E
void CB_RES_B_R(void);	// CB 80-BF (except (HL))
void CB_RES_B_HL(void);	// CB 86,8E,96,9E,A6,AE,B6,BE
void CB_SET_B_R(void);	// CB C0-FF (except (HL))
void CB_SET_B_HL(void);	// CB C6,CE,D6,DE,E6,EE,F6,FE

int cpu_init( void );
void cpu_do_one_instruction( void );
void cpu_do_one_frame( void );
uint8_t* cpu_getReg( int );
uint8_t cpu_get_flags_register( void );
void cpu_set_flags_register( uint8_t );

extern int stop;
extern int op_lengths[0x100];
extern int op_times[0x100];
extern int op_cb_times[0x100];

#ifdef __cplusplus
}
#endif


#endif // !_CPU_H_
