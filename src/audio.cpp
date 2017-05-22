#include <SDL2/SDL.h>
#include "Basic_Gb_Apu.h"
#include "Sound_Queue.h"
#include "audio.h"

#define debugprintf printf
#define debugprintf(...)

Basic_Gb_Apu *apu;
Sound_Queue sound;

const int sample_rate = 44100;

int audio_init()
{
    debugprintf("audio was inited\n");
    apu = new Basic_Gb_Apu();
    apu->set_sample_rate( sample_rate );
    sound.start( sample_rate, 2 );
    return 0;
}

void audio_cleanup()
{
    debugprintf("audio was cleant up");
    sound.stop();
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
    const int buf_size = 20480;
    static blip_sample_t buf[buf_size];
    long count = apu->read_samples( buf, buf_size );
    sound.write( buf, count );
}

