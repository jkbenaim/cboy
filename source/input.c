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
#include "main.h"
#include "memory.h"
#include "input.h"
#include "cpu.h"
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_SDL
int input_event_filter(const SDL_Event *event)
{
  switch( event->type ) {
    // close button clicked
    case SDL_QUIT:
      stop = 1;
      break;
    
    // handle the keyboard
    case SDL_KEYDOWN:
      switch (event->key.keysym.sym) {
	case SDLK_ESCAPE:
	case SDLK_q:
	  stop = 1;
	  break;
	case SDLK_RETURN:
	  state.joyp_buttons &= ~INPUT_BUTTONS_START;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_TAB:
	  state.joyp_buttons &= ~INPUT_BUTTONS_SELECT;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_s:
	  state.joyp_buttons &= ~INPUT_BUTTONS_A;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_a:
	  state.joyp_buttons &= ~INPUT_BUTTONS_B;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_UP:
	  state.joyp_directions &= ~INPUT_DIRECTIONS_UP;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_DOWN:
	  state.joyp_directions &= ~INPUT_DIRECTIONS_DOWN;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_LEFT:
	  state.joyp_directions &= ~INPUT_DIRECTIONS_LEFT;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_RIGHT:
	  state.joyp_directions &= ~INPUT_DIRECTIONS_RIGHT;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	default:
	  break;
      }
      break;
    case SDL_KEYUP:
      switch (event->key.keysym.sym) {
	case SDLK_RETURN:
	  state.joyp_buttons |= INPUT_BUTTONS_START;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_TAB:
	  state.joyp_buttons |= INPUT_BUTTONS_SELECT;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_s:
	  state.joyp_buttons |= INPUT_BUTTONS_A;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_a:
	  state.joyp_buttons |= INPUT_BUTTONS_B;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_UP:
	  state.joyp_directions |= INPUT_DIRECTIONS_UP;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_DOWN:
	  state.joyp_directions |= INPUT_DIRECTIONS_DOWN;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_LEFT:
	  state.joyp_directions |= INPUT_DIRECTIONS_LEFT;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	case SDLK_RIGHT:
	  state.joyp_directions |= INPUT_DIRECTIONS_RIGHT;
	  state.iflag |= IMASK_JOYPAD;
	  state.halt = 0;
	  break;
	default:
	  break;
      }
      break;
  }
  
  return 0;
}

void input_init()
{
  // set up event filtering
  SDL_SetEventFilter( input_event_filter );
  state.joyp = 0xFF;
  state.joyp_buttons = 0xFF;
  state.joyp_directions = 0xFF;
  state.joyp_select = INPUT_SELECT_BUTTONS;
}

void input_handle()
{
  // Pump events through the filter.
  SDL_PumpEvents();
}
#endif // USE_SDL

#ifdef ARM9
void input_init()
{
}

void input_handle()
{
}
#endif // ARM9