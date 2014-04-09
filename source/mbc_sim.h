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

#ifndef _MBC_SIM_H_
#define _MBC_SIM_H_
extern void mbc_sim_install( void );
void mbc_sim_read_ff();
void mbc_sim_dummy();
void mbc_sim_read_rom_0();
void mbc_sim_read_rom_n();
void mbc_sim_read_ram();
void mbc_sim_write_invalidate_rom();
void mbc_sim_write_invalidate_ram();
void mbc_sim_write_invalidate_rom_ram();
void mbc_sim_cleanup();
void mbc_sim_invalidate_rom();
void mbc_sim_invalidate_ram();
void mbc_sim_invalidate_rom_ram();

#endif // _MBC_SIM_H_