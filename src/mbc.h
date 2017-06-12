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
#include "mbc_boot.h"
#include "mbc_none.h"
#include "mbc_cam.h"
#include "mbc_huc1.h"
#include "mbc_huc3.h"
#include "mbc_mbc1.h"
#include "mbc_mbc2.h"
#include "mbc_mbc3.h"
#include "mbc_mbc5.h"
#include "mbc_mbc7.h"

#ifndef _MBC_H_
#define _MBC_H_

extern int mbc_romSize2numBanks( uint8_t romSize );

#endif // _MBC_H_
