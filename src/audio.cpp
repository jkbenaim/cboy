#include <SDL.h>
#include "Basic_Gb_Apu.h"
#include "audio.h"
#include <stdio.h>
// #define debugprintf printf
#define debugprintf(...)

Basic_Gb_Apu *apu;

const int sample_rate = 48000;
void fill_audio( void *userdata, uint8_t *stream, int len );

int audio_init()
{
    debugprintf("audio was inited\n");
    apu = new Basic_Gb_Apu();
    apu->set_sample_rate( sample_rate );

    SDL_AudioSpec wanted;
    wanted.freq = sample_rate;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 2;
    wanted.samples = 1024;
    wanted.callback = fill_audio;
    wanted.userdata = NULL;

    if( SDL_OpenAudio( &wanted, NULL ) < 0 )
    {
        fprintf( stderr, "Couldn't open audio: %s\n", SDL_GetError() );
        return 1;
    }
    SDL_PauseAudio(0);
    return 0;
}

void fill_audio( void *userdata, uint8_t *stream, int len )
{
    debugprintf( "fill_audio( %p, %p, %d );\n", userdata, stream, len );
    memset( stream, 0, len );
    long count = apu->read_samples( (blip_sample_t *)stream, len/2 );
    debugprintf( "%d\n", count );
}

void audio_cleanup()
{
    debugprintf("audio was cleant up");
    SDL_CloseAudio();
}

uint8_t audio_read( uint16_t address )
{
    return apu->read_register( address );
}

void audio_write( uint16_t address, uint8_t data )
{
    apu->write_register( address, data );
}

void audio_frame()
{
    apu->end_frame();
}

