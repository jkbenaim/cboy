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

#ifndef _INPUT_H_
#define _INPUT_H_

#include "types.h"
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_SELECT_BUTTONS	0x20
#define INPUT_SELECT_DIRECTIONS	0x10
#define INPUT_BUTTONS_START	0x08
#define INPUT_BUTTONS_SELECT	0x04
#define INPUT_BUTTONS_B		0x02
#define INPUT_BUTTONS_A		0x01
#define INPUT_DIRECTIONS_DOWN	0x08
#define INPUT_DIRECTIONS_UP	0x04
#define INPUT_DIRECTIONS_LEFT	0x02
#define INPUT_DIRECTIONS_RIGHT	0x01

void input_init();
int input_event_filter(void *dontcare, SDL_Event *event);
void input_handle();

#ifdef __cplusplus
}
#endif

#endif // _INPUT_H_
