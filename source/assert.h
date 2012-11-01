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

/*
 *   assert.h
 *   This file provides an assert macro.
 *   assert(msg, cond)
 */

#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <stdlib.h> // for exit
#include <stdio.h> // for printf

#ifndef NDEBUG

#define assert(msg, cond) { \
if(!(cond)) \
{ \
printf("Assertion failed: %s\n", msg); \
exit(1); \
} \
}

#else  // NDEBUG
#define assert(msg, cond)
#endif // NDEBUG


#endif  // _ASSERT_H_
