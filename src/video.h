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
#include "types.h"

#ifndef _VIDEO_H_
#define _VIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sprite attribute flags
#define	SPRITE_PRIORITY		0x80	// 0=Sprite above BG, 1=Sprite behind BG color 1-3
#define	SPRITE_YFLIP		0x40	// 0=Normal, 1=Vertically mirrored
#define SPRITE_XFLIP		0x20	// 0=Normal, 1=Horizontally mirrored
#define SPRITE_DMG_PAL		0x10	// 0=OBP0, 1=OBP1
#define SPRITE_VRAM_BANK	0x08	// 0=Bank 0, 1=Bank 1 (CGB only)
#define SPRITE_CGB_PAL_MASK	0x07	// OBP0-7 (CGB only)

// Background map attribute flags (CGB only)
#define BG_OAM_PRIORITY		0x80	// 0=Use OAM priority bit, 1=BG priority
#define BG_YFLIP		0x40	// 0=Normal, 1=Vertically mirrored
#define BG_XFLIP		0x20	// 0=Normal, 1=Horizontally mirrored
#define BG_NOTUSED		0x10
#define BG_VRAM_BANK		0x08	// 0=Bank 0, 1=Bank 1
#define BG_CGB_PAL_MASK		0x07	// BGP0-7

extern char inval_palette;
void vid_init(void);
void vid_waitForNextFrame();
void vid_render_line();
void vid_frame();
extern pixel_t palette[4];

#ifdef __cplusplus
}
#endif
#endif				// _VIDEO_H_
