#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#include "audio.h"

snd_pcm_t *playback_handle;
#define AUDIO_BUF_FRAMES 44100
short audio_buf[AUDIO_BUF_FRAMES * 2];

int audio_init()
{
  printf(" ==audio init start== \n");
  int err;
  snd_pcm_hw_params_t *hw_params;
  
  err = snd_pcm_open( &playback_handle,
                      "default:CARD=Headset", 
                      SND_PCM_STREAM_PLAYBACK, 
                      0);
  printf("snd_pcm_open: %d\n", err);
  
  
  err = snd_pcm_hw_params_malloc( &hw_params );
  printf("snd_pcm_hw_params_malloc: %d\n", err);
  
  
  err = snd_pcm_hw_params_any( playback_handle, 
                               hw_params );
  printf("snd_pcm_hw_params_any: %d\n", err);
  
  
  err = snd_pcm_hw_params_set_access( playback_handle, 
                                      hw_params,
                                      SND_PCM_ACCESS_RW_INTERLEAVED);
  printf("snd_pcm_hw_params_set_access: %d\n", err);
  
  
  err = snd_pcm_hw_params_set_format( playback_handle, 
                                      hw_params,
                                      SND_PCM_FORMAT_S16_LE);
  printf("snd_pcm_hw_params_set_format: %d\n", err);
  
  
  unsigned int samplerate = 44100;
  err = snd_pcm_hw_params_set_rate_near( playback_handle, 
                                         hw_params,
                                         &samplerate,
                                         0);
  printf("snd_pcm_hw_params_set_rate_near: %d\n", err);
  
  
  err = snd_pcm_hw_params_set_channels( playback_handle, 
                                        hw_params,
                                        2);
  printf("snd_pcm_hw_params_set_channels: %d\n", err);
  
  
  err = snd_pcm_hw_params( playback_handle, 
                           hw_params);
  printf("snd_pcm_hw_params: %d\n", err);
  
  
  snd_pcm_hw_params_free( hw_params );
  
  
  err = snd_pcm_prepare( playback_handle );
  printf("snd_pcm_prepare: %d\n", err);
  
//   short buf[512];
//   short val=0x1000;
//   int i;
//   
//   while(1)
//   {
//     for(i=0; i<512; ++i)
//     {
//       buf[i] = val;
//     }
//     
//     err = snd_pcm_writei( playback_handle, buf, 256 );
//     printf("snd_pcm_writei: %d\n", err);
//     
//     val = -val;
//     
//   }
  
  // clear the buffer
  int i;
  for(i=0; i<AUDIO_BUF_FRAMES * 2; ++i)
  {
    audio_buf[i] = 0;
  }
  
  
  printf(" ==audio init end  == \n");
  return 0;
}

void audio_cleanup()
{
  snd_pcm_close( playback_handle );
}