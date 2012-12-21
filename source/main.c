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

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#endif // __ANDROID__

#include <stdlib.h>
#include "types.h"
#include "input.h"
#include "cpu.h"
#include "memory.h"
#include "video.h"
#include "cart.h"
#include "main.h"

int stop = 0;

#ifdef __ANDROID__
#define  LOG_TAG    "libcboy"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif // __ANDROID__

#ifdef USE_SDL
int main ( int argc, char* argv[] ) {
  printf( " ----------------------- \n" );
  printf( " --- Welcome to cboy --- \n" );
  printf( " ---    (c) 2012     --- \n" );
  printf( " ----------------------- \n" );
  
  if(argc < 3)
  {
    fprintf(stderr, "Usage: cboy boot.bin game.gb\n");
    fprintf(stderr, "If you don't have a bootrom, use quickboot.bin.\n");
    exit(1);
  }
  
  mem_init();
  cpu_init();
  cart_init( argv[1], argv[2] );
  vid_init();
  input_init();
  while(!stop)
  {
    vid_waitForNextFrame();
    input_handle();
    cpu_do_one_frame();
    vid_frame();
  }
  
  cart_cleanup();
  
  return 0;
}
#endif // USE_SDL

#ifdef __ANDROID__
JNIEXPORT void JNICALL Java_org_trashfolder_cboy_CboyView_cboyFrame(JNIEnv * env, jobject  obj, jobject bitmap)
{
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static int         init;

    if (!init) {
        mem_init();
        cpu_init();
        cart_init( "/storage/emulated/0/roms/cgb_rom.bin", "/storage/emulated/0/roms/camera.gb" );
        vid_init();
        input_init();
        init = 1;
    }

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    input_handle();
    cpu_do_one_frame();
    vid_frame( &info, pixels );

    AndroidBitmap_unlockPixels(env, bitmap);
}
#endif // __ANDROID__