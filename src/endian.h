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

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include "types.h"

#if defined (_BIG_ENDIAN) || ( defined (__BYTE_ORDER__) && (__BYTE_ORDER__ == 4321) )
  #define __BIG_ENDIAN__ 1
#else
  #define __LITTLE_ENDIAN__ 1
#endif

#endif // !_ENDIAN_H_
