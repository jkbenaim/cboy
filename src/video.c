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
#include <SDL.h>

static pixel_t pixmem[160*144];
static pixel_t colormem[160*144];

char inval_palette = 1;
static pixel_t myCachedPalettes[8][8];
static pixel_t pixels[8];
static pixel_t colors[8];

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *gametex;

static int vid_drawSpanCommon(pixel_t *palette, int vramAddr, int x, int y, int vramBank,
        int xFlip, int *lineStart, int *spanStart, int *spanEnd) {

    // Is this span off the left of the screen?
    if(x<-7)
        return 1;

    // Is this span off the top of the screen?
    if(y<0)
        return 1;

    if((x-8)>160)
        return 1;

    *lineStart = 160 * y;
    //   uint32_t *pixmem = (uint32_t*) screen->pixels;

    // fill pixel array with span
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

    // pixel 7 at the left, pixel 0 at the right
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
        pixels[p] = palette[color];
        colors[p] = color;
    }

    // Is the span partially offscreen?
    *spanStart = 0;
    *spanEnd = 7;
    // Partially off the left side of the screen
    if( x < 0 )
        *spanStart = -x;
    // Partially off the right side of the screen
    if( x > (160-8) )
        *spanEnd = 159 - x;

    // xFlip?
    if( xFlip )
    {
        // Flip the span.
        pixel_t temp_pixels[8];
        pixel_t temp_colors[8];

        int i;
        for(i=0; i<8; ++i)
        {
            // Copy pixels to a temporary place, in reverse order.
            temp_pixels[i] = pixels[7-i];
            temp_colors[i] = colors[7-i];
        }
        for(i=0; i<8; ++i)
        {
            pixels[i] = temp_pixels[i];
            colors[i] = temp_colors[i];
        }
    }

    return 0;
}


static void vid_drawOpaqueSpan( uint8_t pal, uint16_t vramAddr, int x, int y, int vramBank, int xFlip, int updateColormem ) {
    int lineStart, spanStart, spanEnd;
    if (vid_drawSpanCommon(&myCachedPalettes[pal][0], vramAddr, x, y, vramBank, xFlip, &lineStart, &spanStart, &spanEnd))
      return;

    // Draw the span from left to right.
    unsigned didx = lineStart + x + spanStart;
    unsigned sidx = 7 - spanStart;

    for( int p=spanStart; p<=spanEnd; ++p )
    {
        if( updateColormem ) {
          colormem[ didx ] = colors[ sidx ];
        }

        pixmem[ didx++ ] = pixels[ sidx-- ];
    }
}

static void vid_drawTransparentSpan( uint8_t pal, uint16_t vramAddr, int x, int y, int vramBank, int xFlip, int priority ) {
    int lineStart, spanStart, spanEnd;
    if (vid_drawSpanCommon(&myCachedPalettes[pal][4], vramAddr, x, y, vramBank, xFlip, &lineStart, &spanStart, &spanEnd))
      return;

    // Draw the span from left to right.
    int p;
    for( p=spanStart; p<=spanEnd; ++p )
    {
        if( colors[7-p] != 0 )
        {
            if( priority )
            {
                if( colormem[ lineStart + x + p ] == 0 )
                {
                    pixmem[ lineStart + x + p ] = pixels[ 7-p ];
                } else {
                    // Uncomment the next line to highlight the
                    // sprite-behind-background case in bright red.
                    //           pixmem[ lineStart + x + p ] = 0x001f; // red
                }
            } else {
                pixmem[ lineStart + x + p ] = pixels[ 7-p ];
            }
        }
    }
}

void vid_render_line()
{
    // If the LCD is off, then return.
    // We should probably blank the line instead...
    if( (state.lcdc & LCDC_LCD_ENABLE) == 0 )
        return;

    // Set up cached palettes.
    if (inval_palette) {
      if( state.caps & 0x04 )
      {
          pixel_t tempPalette[4];
          // DMG mode
          // colors need to be translated through BOTH the DMG and CGB palettes
          for (int i = 0; i < 8; i++) {
            tempPalette[0] = state.bgpd[(i*8)+0] + (state.bgpd[(i*8)+1]<<8);
            tempPalette[1] = state.bgpd[(i*8)+2] + (state.bgpd[(i*8)+3]<<8);
            tempPalette[2] = state.bgpd[(i*8)+4] + (state.bgpd[(i*8)+5]<<8);
            tempPalette[3] = state.bgpd[(i*8)+6] + (state.bgpd[(i*8)+7]<<8);

            myCachedPalettes[i][0] = tempPalette[((state.bgp)      & 0x3) ];
            myCachedPalettes[i][1] = tempPalette[((state.bgp >> 2) & 0x3) ];
            myCachedPalettes[i][2] = tempPalette[((state.bgp >> 4) & 0x3) ];
            myCachedPalettes[i][3] = tempPalette[((state.bgp >> 6) & 0x3) ];

            tempPalette[0] = state.obpd[(i*8)+0] + (state.obpd[(i*8)+1]<<8);
            tempPalette[1] = state.obpd[(i*8)+2] + (state.obpd[(i*8)+3]<<8);
            tempPalette[2] = state.obpd[(i*8)+4] + (state.obpd[(i*8)+5]<<8);
            tempPalette[3] = state.obpd[(i*8)+6] + (state.obpd[(i*8)+7]<<8);

            int dmgPalette = (i==0) ? state.obp0 : state.obp1;

            myCachedPalettes[i][4] = tempPalette[((dmgPalette)      & 0x3) ];
            myCachedPalettes[i][5] = tempPalette[((dmgPalette >> 2) & 0x3) ];
            myCachedPalettes[i][6] = tempPalette[((dmgPalette >> 4) & 0x3) ];
            myCachedPalettes[i][7] = tempPalette[((dmgPalette >> 6) & 0x3) ];
          }
      } else {
          // CGB mode
          for (int i = 0; i < 8; i++) {
            myCachedPalettes[i][0] = state.bgpd[(i*8)+0] + (state.bgpd[(i*8)+1]<<8);
            myCachedPalettes[i][1] = state.bgpd[(i*8)+2] + (state.bgpd[(i*8)+3]<<8);
            myCachedPalettes[i][2] = state.bgpd[(i*8)+4] + (state.bgpd[(i*8)+5]<<8);
            myCachedPalettes[i][3] = state.bgpd[(i*8)+6] + (state.bgpd[(i*8)+7]<<8);
            myCachedPalettes[i][4] = state.obpd[(i*8)+0] + (state.obpd[(i*8)+1]<<8);
            myCachedPalettes[i][5] = state.obpd[(i*8)+2] + (state.obpd[(i*8)+3]<<8);
            myCachedPalettes[i][6] = state.obpd[(i*8)+4] + (state.obpd[(i*8)+5]<<8);
            myCachedPalettes[i][7] = state.obpd[(i*8)+6] + (state.obpd[(i*8)+7]<<8);
          }
      }

      inval_palette = 0;
    }

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


            uint8_t attributes = vram_bank_one[tileAddress];
            int xFlip = (attributes&BG_XFLIP)?1:0;
            int yFlip = (attributes&BG_YFLIP)?1:0;

            if( yFlip )
                spanAddress = tileDataAddress + ((7-backLineInTile) * 2);
            else
                spanAddress = tileDataAddress + (backLineInTile * 2);

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;
            else
                vramBank = (attributes & BG_VRAM_BANK)?1:0;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                pal = 0;    // dmg mode
            else
                pal = attributes & 0x07;    // cgb mode

            vid_drawOpaqueSpan( pal, spanAddress, i*8 - (state.scx%8), state.ly, vramBank, xFlip, 1 );
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

            uint8_t attributes = vram_bank_one[tileAddress];
            int xFlip = (attributes&BG_XFLIP)?1:0;
            int yFlip = (attributes&BG_YFLIP)?1:0;

            if( yFlip )
                spanAddress = tileDataAddress + ((7-windowLineInTile) * 2);
            else
                spanAddress = tileDataAddress + (windowLineInTile * 2);

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;
            else
                vramBank = (attributes & BG_VRAM_BANK)?1:0;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                pal = 0;        // dmg mode
            else
                pal = attributes & 0x07;        // cgb mode

            vid_drawOpaqueSpan( pal, spanAddress, i*8 + state.wx - 7, state.ly, vramBank, xFlip, 1 );
        }
    }

    int objLineToRender = ((int)state.ly) % 256;

    // Render sprites.
    if( state.lcdc & LCDC_OBJ_DISPLAY )
        for( i=0; i<40; ++i )
        {
            uint8_t y          = oam[(i<<2)    ];
            uint8_t x          = oam[(i<<2) + 1];
            uint8_t tileNum    = oam[(i<<2) + 2];
            uint8_t attributes = oam[(i<<2) + 3];

            // Set object height.
            int objHeight;
            if( state.lcdc & LCDC_OBJ_SIZE )
                objHeight = 16;
            else
                objHeight = 8;

            if(objHeight == 16)
            {
                tileNum &= 0xfe;
            }

            // Is this sprite on this line? If not, let's skip it.
            int reject = 0;
            int lineOfSpriteToRender = objLineToRender - y + 16;
            // yFlip is handled here
            if( attributes & SPRITE_YFLIP )
                lineOfSpriteToRender = objHeight - lineOfSpriteToRender - 1;
            if( lineOfSpriteToRender >= objHeight || lineOfSpriteToRender < 0  )
                reject = 1;
            if( reject == 1 )
                continue;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                if( attributes & SPRITE_DMG_PAL )   // dmg mode
                    pal = 1;
                else
                    pal = 0;
            else
                pal = attributes & 0x07;    // cgb mode

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;   // dmg mode
            else
                vramBank = (attributes & SPRITE_VRAM_BANK)?1:0; // cgb mode

            // Set the priority. 0=sprite above background, 1=sprite behind bg color 1-3
            int priority = attributes & SPRITE_PRIORITY;

            // Calculate the span address, relative to 0x8000.
            spanAddress = (tileNum * 16) + (lineOfSpriteToRender*2);

            int xFlip = attributes & SPRITE_XFLIP;
            vid_drawTransparentSpan( pal, spanAddress, x-8, state.ly, vramBank, xFlip, priority );
        }


}

inline uint32_t rgb555_to_SDL( int color )
{
    int r = ((color & 0x001F) >> 0      ) << 3;
    int g = ((color & 0x03E0) >> 5      ) << 3;
    int b = ((color & 0x7C00) >> 10     ) << 3;
    //return SDL_MapRGB( window->format, r, g, b );
    // TODO
    return g+b+r;
}

inline uint32_t rgb555_to_rgb888( int color )
{
    uint32_t out = 0;
    int r = ((color & 0x001F) >> 0      ) << 3;
    r += (color & 0x001C)>>2;
    int g = ((color & 0x03E0) >> 5      ) << 3;
    g += (color & 0x0380) >> 7;
    int b = ((color & 0x7C00) >> 10     ) << 3;
    b+= (color & 0x7000) >> 12;
    out = (r<<16) + (g<<8) + b;
    return out;
}

inline uint16_t rgb555_to_rgb565( pixel_t in )
{
    uint16_t out;
    const int r = ((in & 0x001F) >> 0      ) << 0;
    const int g = ((in & 0x03E0) >> 5      ) << 1;
    const int b = ((in & 0x7C00) >> 10     ) << 0;
    out = (r<<11) + (g<<5) + b;
    return out;
}

void vid_init()
{
    // set up window and renderer
    const int scale = 2;
    window = SDL_CreateWindow( "cboy",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               160*scale,
                               144*scale,
                               0 );
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_PRESENTVSYNC );
    SDL_RenderSetLogicalSize( renderer, 160, 144 );

    // clear trash out of window by drawing black
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // set up game texture
    gametex = SDL_CreateTexture( renderer,
                                 SDL_PIXELFORMAT_BGR555,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 160,
                                 144 );
}

void vid_waitForNextFrame()
{
}

void vid_frame()
{
    if( state.ly != 0 )
        printf("warning: in vid_frame and ly is %d instead of 0\n", state.ly);

    SDL_UpdateTexture( gametex, NULL, pixmem, 160 * sizeof(pixel_t) );
    //SDL_RenderClear( renderer );
    SDL_RenderCopy( renderer, gametex, NULL, NULL );
    SDL_RenderPresent( renderer ); 
}
