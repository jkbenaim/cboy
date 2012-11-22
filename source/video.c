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
#include "video.h"
#include "memory.h"
#include "cpu.h"
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif
#ifdef __ANDROID__
#include <android/bitmap.h>
#endif // __ANDROID__
#include "assert.h"

pixel_t pixmem[160*144];

#ifdef USE_SDL
SDL_Surface *screen;
#endif

void vid_drawOpaqueSpan( u8 pal, u16 vramAddr, int x, int y, int vramBank, int xFlip ) {
  
  // Is this span off the left of the screen?
  if(x<-7)
    return;
  
  // Is this span off the top of the screen?
  if(y<0)
    return;
  
  if((x-8)>160)
    return;
  
  // Set up local palette.
  // FIXME The names of these variables are fucking confusing
  pixel_t myPalette[4];
  if( state.caps & 0x04 )
  {
    // DMG mode
    // colors need to be translated through BOTH the DMG and CGB palettes
    pixel_t cgbPalette[4];
    cgbPalette[0] = state.bgpd[(pal*8)+0] + (state.bgpd[(pal*8)+1]<<8);
    cgbPalette[1] = state.bgpd[(pal*8)+2] + (state.bgpd[(pal*8)+3]<<8);
    cgbPalette[2] = state.bgpd[(pal*8)+4] + (state.bgpd[(pal*8)+5]<<8);
    cgbPalette[3] = state.bgpd[(pal*8)+6] + (state.bgpd[(pal*8)+7]<<8);
    
    myPalette[0] = cgbPalette[ (state.bgp)      & 0x3 ];
    myPalette[1] = cgbPalette[ (state.bgp >> 2) & 0x3 ];
    myPalette[2] = cgbPalette[ (state.bgp >> 4) & 0x3 ];
    myPalette[3] = cgbPalette[ (state.bgp >> 6) & 0x3 ];
  } else {
    // CGB mode
    myPalette[0] = state.bgpd[(pal*8)+0] + (state.bgpd[(pal*8)+1]<<8);
    myPalette[1] = state.bgpd[(pal*8)+2] + (state.bgpd[(pal*8)+3]<<8);
    myPalette[2] = state.bgpd[(pal*8)+4] + (state.bgpd[(pal*8)+5]<<8);
    myPalette[3] = state.bgpd[(pal*8)+6] + (state.bgpd[(pal*8)+7]<<8);
  }
  
  
  int lineStart = 160 * y;
//   u32 *pixmem = (u32*) screen->pixels;
  
  
  // fill pixel array with span
  pixel_t pixels[8];	// pixel 7 at the left, pixel 0 at the right
  int lowBits, highBits;
  if( vramBank == 0 )
  {
    lowBits = vram_bank_zero[vramAddr];
    highBits = vram_bank_zero[vramAddr + 1];
  }
  else
  {
    lowBits = vram_bank_one[vramAddr];
    highBits = vram_bank_one[vramAddr + 1];
  }

  int p;
  for( p=0; p<8; ++p )
  {
    int color;
    int mask;
    color = 0;
    mask = 1<<p;
    if( lowBits & mask )
      color = 1;
    if( highBits & mask )
      color += 2;
    pixels[p] = myPalette[color];
  }
  
  // Is the span partially offscreen?
  int spanStart = 0;
  int spanEnd = 7;
  // Partially off the left side of the screen
  if( x < 0 )
    spanStart = -x;
  // Partially off the right side of the screen
  if( x > (160-8) )
    spanEnd = 159 - x;
  
  // xFlip?
  if( xFlip ) {
  // Draw the span from right to left.
    for( p=spanStart; p<=spanEnd; ++p )
      pixmem[ lineStart + x + p ] = pixels[ p ];
  } else {
    // No
  // Draw the span from left to right.
    for( p=spanStart; p<=spanEnd; ++p )
      pixmem[ lineStart + x + p ] = pixels[ 7-p ];
  }
  
//   // Draw the span.
//   for( p=spanStart; p<=spanEnd; ++p )
//   {
//     pixmem[ lineStart + x + p ] = pixels[ 7-p ];
//   }
  
}

void vid_drawTransparentSpan( u8 pal, u16 vramAddr, int x, int y, int vramBank, int xFlip ) {
  
  // Is this span off the left of the screen?
  if(x<-7)
    return;
  
  // Is this span off the top of the screen?
  if(y<0)
    return;
  
  if((x-8)>160)
    return;
  
  // Set up local palette.
  // FIXME The names of these variables are fucking confusing
  pixel_t myPalette[4];
  if( state.caps & 0x04 )
  {
    // DMG mode
    // colors need to be translated through BOTH the DMG and CGB palettes
    pixel_t cgbPalette[4];
    cgbPalette[0] = state.obpd[(pal*8)+0] + (state.obpd[(pal*8)+1]<<8);
    cgbPalette[1] = state.obpd[(pal*8)+2] + (state.obpd[(pal*8)+3]<<8);
    cgbPalette[2] = state.obpd[(pal*8)+4] + (state.obpd[(pal*8)+5]<<8);
    cgbPalette[3] = state.obpd[(pal*8)+6] + (state.obpd[(pal*8)+7]<<8);
    
    int dmgPalette = (pal==0) ? state.obp0 : state.obp1;
    
    myPalette[0] = cgbPalette[ (dmgPalette)      & 0x3 ];
    myPalette[1] = cgbPalette[ (dmgPalette >> 2) & 0x3 ];
    myPalette[2] = cgbPalette[ (dmgPalette >> 4) & 0x3 ];
    myPalette[3] = cgbPalette[ (dmgPalette >> 6) & 0x3 ];
  } else {
    // CGB mode
    myPalette[0] = state.obpd[(pal*8)+0] + (state.obpd[(pal*8)+1]<<8);
    myPalette[1] = state.obpd[(pal*8)+2] + (state.obpd[(pal*8)+3]<<8);
    myPalette[2] = state.obpd[(pal*8)+4] + (state.obpd[(pal*8)+5]<<8);
    myPalette[3] = state.obpd[(pal*8)+6] + (state.obpd[(pal*8)+7]<<8);
  }
  
  
  int lineStart = 160 * y;
  
  
  // fill pixel array with span
  pixel_t pixels[8];	// pixel 7 at the left, pixel 0 at the right
  pixel_t colors[8];
  int lowBits, highBits;
  if( vramBank == 0 )
  {
    lowBits = vram_bank_zero[vramAddr];
    highBits = vram_bank_zero[vramAddr + 1];
  }
  else
  {
    lowBits = vram_bank_one[vramAddr];
    highBits = vram_bank_one[vramAddr + 1];
  }

  int p;
  for( p=0; p<8; ++p )
  {
    int color;
    int mask;
    color = 0;
    mask = 1<<p;
    if( lowBits & mask )
      color = 1;
    if( highBits & mask )
      color += 2;
    colors[p] = color;
    pixels[p] = myPalette[color];
  }
  
  // Is the span partially offscreen?
  int spanStart = 0;
  int spanEnd = 7;
  // Partially off the left side of the screen
  if( x < 0 )
    spanStart = -x;
  // Partially off the right side of the screen
  if( x > (160-8) )
    spanEnd = 159 - x;
  
  // xFlip?
  if( xFlip ) {
  // Draw the span from right to left.
    for( p=spanStart; p<=spanEnd; ++p )
    {
      if( colors[p] != 0 )
	pixmem[ lineStart + x + p ] = pixels[ p ];
    }
  } else {
    // No
  // Draw the span from left to right.
    for( p=spanStart; p<=spanEnd; ++p )
    {
      if( colors[7-p] != 0 )
	pixmem[ lineStart + x + p ] = pixels[ 7-p ];
    }
  }
  
}

void vid_render_line()
{
  // If the LCD is off, then return.
  // We should probably blank the line instead...
  if( (state.lcdc & LCDC_LCD_ENABLE) == 0 )
    return;
  
  int backLineToRender = ((int)state.ly + (int)state.scy) % 256;
  int backTileRow = backLineToRender / 8;
  int backLineInTile = backLineToRender % 8;
  
  int i, tileNum, tileAddress, tileDataAddress, tile, spanAddress, vramBank;
  
  // Render the background.
  if( state.lcdc & LCDC_BG_ENABLE )
    for( i=-1; i<21; ++i )
    {
      tileNum = backTileRow*32 + (i + (state.scx >> 3))%32;
      
      
      if( state.lcdc & LCDC_BG_TILE_MAP_SELECT )
	tileAddress = 0x1C00 + tileNum;
      else
	tileAddress = 0x1800 + tileNum;
      
      
      if( state.lcdc & LCDC_TILE_DATA_SELECT )
      {
	tile = vram_bank_zero[tileAddress];
	tileDataAddress = 0x0000 + tile*16;
      }
      else
      {
	tile = vram_bank_zero[tileAddress];
	if(tile < 0x80)
	  tileDataAddress = 0x1000 + tile*16;
	else
	  tileDataAddress = tile*16;
      }
      
      spanAddress = tileDataAddress + (backLineInTile * 2);
      
      u8 attributes = vram_bank_one[tileAddress];
      
      // Set the VRAM bank.
      if( state.caps == 0x04 )
	vramBank = 0;
      else
	vramBank = (attributes & BG_VRAM_BANK)?1:0;
      
      // Set the palette.
      u8 pal;
      if( state.caps == 0x04 )
	pal = 0;	// dmg mode
      else
	pal = attributes & 0x07;	// cgb mode
      
      int xFlip = (attributes&BG_XFLIP)?1:0;
      vid_drawOpaqueSpan( pal, spanAddress, i*8 - (state.scx%8), state.ly, vramBank, xFlip );
    }
  
  // Render the window.
  int windowLineToRender = (int)state.ly - (int)state.wy;
  int windowTileRow = windowLineToRender / 8;
  int windowLineInTile = windowLineToRender % 8;
  if( (state.lcdc & LCDC_WINDOW_ENABLE) && (state.ly >= state.wy) )
  {
    for( i=0; i<20; ++i )
    {
      tileNum = windowTileRow*32 + i;
      
      if( state.lcdc & LCDC_WINDOW_TILE_MAP_SELECT )
	tileAddress = 0x1C00 + tileNum;
      else
	tileAddress = 0x1800 + tileNum;
      
      
      if( state.lcdc & LCDC_TILE_DATA_SELECT )
      {
	tile = vram_bank_zero[tileAddress];
	tileDataAddress = 0x0000 + tile*16;
      }
      else
      {
	tile = vram_bank_zero[tileAddress];
	if(tile < 0x80)
	  tileDataAddress = 0x1000 + tile*16;
	else
	  tileDataAddress = tile*16;
      }
      
      spanAddress = tileDataAddress + (windowLineInTile * 2);
      
      u8 attributes = vram_bank_one[tileAddress];
      
      // Set the VRAM bank.
      vramBank = 0;     // windows are always VRAM bank 0
      
      // Set the palette.
      u8 pal = 0;	// windows are always palette 0
      
      int xFlip = (attributes&BG_XFLIP)?1:0;
      vid_drawOpaqueSpan( pal, spanAddress, i*8 + state.wx - 7, state.ly, vramBank, xFlip );
    }
  }
  
  int objLineToRender = ((int)state.ly) % 256;
  
  // Render sprites.
  if( state.lcdc & LCDC_OBJ_DISPLAY )
    for( i=0; i<40; ++i )
    {
      u8 y          = oam[(i<<2)    ];
      u8 x          = oam[(i<<2) + 1];
      u8 tileNum    = oam[(i<<2) + 2];
      u8 attributes = oam[(i<<2) + 3];
      
      // Set object height.
      int objHeight;
      if( state.lcdc & LCDC_OBJ_SIZE )
	objHeight = 16;
      else
	objHeight = 8;
      
      // Is this sprite on this line? If not, let's skip it.
      int reject = 0;
      int lineOfSpriteToRender = objLineToRender - y + 16;
      if( attributes & SPRITE_YFLIP )
	lineOfSpriteToRender = objHeight - lineOfSpriteToRender - 1;
      if( lineOfSpriteToRender >= objHeight || lineOfSpriteToRender < 0  )
	reject = 1;
      if( reject == 1 )
	continue;
      
      // Set the palette.
      u8 pal;
      if( state.caps == 0x04 )
	if( attributes & SPRITE_DMG_PAL )	// dmg mode
	  pal = 1;
	else
	  pal = 0;
      else
	pal = attributes & 0x07;	// cgb mode
      
      // Set the VRAM bank.
      if( state.caps == 0x04 )
	vramBank = 0;	// dmg mode
      else
	vramBank = (attributes & SPRITE_VRAM_BANK)?1:0;	// cgb mode
      
      // Calculate the span address, relative to 0x8000.
      spanAddress = (tileNum * 16) + (lineOfSpriteToRender*2);
      
      int xFlip = attributes & SPRITE_XFLIP;
      vid_drawTransparentSpan( pal, spanAddress, x-8, state.ly, vramBank, xFlip );
    }
      
      
}

void vid_init()
{
#ifdef USE_SDL
  // init SDL
  if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    fprintf ( stderr, "Unable to init SDL: %s\n", SDL_GetError() );
    exit(1);
  }

  // set title bar
  SDL_WM_SetCaption( "cboy", "cboy" );
  
  // create window
  screen = SDL_SetVideoMode( 320, 288, 0, 0 );
#endif  // USE_SDL
  
//   int x,y;
//   u32 *surfPixels = screen->pixels;
//   for( y=0; y<screen->h; y++ )
//     for( x=0; x<screen->w; x++ )
//       surfPixels[y*screen->w + x] = SDL_MapRGB( screen->format, 127, 127, 127 );
}

void vid_waitForNextFrame()
{
#ifdef USE_SDL
#ifndef __APPLE__
  SDL_Delay(1000 / 60.0);
#endif
#endif
  return;
}

#ifdef USE_SDL
inline u32 rgb555_to_SDL( int color )
{
  int r = ((color & 0x001F) >> 0      ) << 3;
  int g = ((color & 0x03E0) >> 5      ) << 3;
  int b = ((color & 0x7C00) >> 10     ) << 3;
  return SDL_MapRGB( screen->format, r, g, b );
}
#endif  // USE_SDL

inline uint16_t rgb555_to_rgb565( pixel_t in )
{
  uint16_t out;
  const int r = ((in & 0x001F) >> 0      ) << 0;
  const int g = ((in & 0x03E0) >> 5      ) << 1;
  const int b = ((in & 0x7C00) >> 10     ) << 0;
  out = (r<<11) + (g<<5) + b;
  return out;
}

#ifdef USE_SDL
void vid_frame()
{
    // TODO
  int x,y;
  u32 *surfPixels = screen->pixels;
  int width = screen->w;
  int srcStartOfLine=0, dstStartOfLine=0;
  for( y=0; y<144; ++y )
  {
    for( x=0; x<160; ++x )
    {
		surfPixels[dstStartOfLine + 2*x + 0] = rgb555_to_SDL( pixmem[srcStartOfLine + x] );
		surfPixels[dstStartOfLine + 2*x + 1] = rgb555_to_SDL( pixmem[srcStartOfLine + x] );
		surfPixels[dstStartOfLine + 2*x + 0 + width] = rgb555_to_SDL( pixmem[srcStartOfLine + x] );
		surfPixels[dstStartOfLine + 2*x + 1 + width] = rgb555_to_SDL( pixmem[srcStartOfLine + x] );
    }
    srcStartOfLine += 160;
    dstStartOfLine += width*2;
  }
    
  SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
#endif // USE_SDL

#ifdef __ANDROID__
void vid_frame( AndroidBitmapInfo* info, void* pixels )
{
    // TODO
  int x,y;
  const int width = info->width;
  int srcStartOfLine=0, dstStartOfLine=0;
  for( y=0; y<144; ++y )
  {
    for( x=0; x<160; ++x )
    {
      ((uint16_t*)pixels)[dstStartOfLine + x] = rgb555_to_rgb565( pixmem[srcStartOfLine + x] );
    }
    srcStartOfLine += 160;
    dstStartOfLine += width;
  }
}
#endif // __ANDROID__

