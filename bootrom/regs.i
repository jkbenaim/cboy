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

; game boy hardware register definitions

.define IF	$FF0F
.define	LCDC	$FF40
.define BGP     $FF47
.define OBP0    $FF48
.define OBP1    $FF49
.define BGPI	$FF68
.define BGPD	$FF69
.define OBPI	$FF6A
.define OBPD	$FF6B
.define SVBK	$FF70

.define R_IF	$0F
.define R_LCDC	$40
.define R_BGP   $47
.define R_OBP0  $48
.define R_OBP1  $49
.define R_BGPI	$68
.define R_BGPD	$69
.define R_OBPI	$6A
.define R_OBPD	$6B
.define R_SVBK  $70
