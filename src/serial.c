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
#include "memory.h"
#include "cpu.h"
#include "serial.h"

int serial_receive_bit( int in, int reset )
{
  return 1;
}

// int serial_receive_bit( int in, int reset )
// {
//   static int bits_received_count = 0;
//   static int inbuffer = 0, outbuffer = 0;
//   int out;
//   static int nextOut = 0;
//   static int myReset = 0;
//   
//   if( reset )
//   {
//     
//     printf( "RESETRESETRESETRESETRESETRESET\n" );
//     // it's been >250ms since last bit was received. let's reset.
//     printf( "serial bit inbuffer reset\n" );
//     inbuffer = 0;
//     bits_received_count = 0;
//     state.serialTimeoutClock = 0;
//     myReset = 1;  // passed on to the serial_receive_byte function
//   }
//   
//   inbuffer = (inbuffer << 1) + in;
// //   printf( "received bit: %02X (%d)\n", in, bits_received_count );
//   bits_received_count++;
//   if( bits_received_count == 8 )
//   {
//     outbuffer = serial_receive_byte( inbuffer, myReset );
//     myReset=0;
//     inbuffer = 0;
//     bits_received_count = 0;
//   }
//   
//   out = nextOut;
//   nextOut = (outbuffer&0x80)?1:0;
//   outbuffer = (outbuffer << 1) & 0xFF;
//   
//   return out;
// }
// 
// int serial_receive_byte( int in, int reset )
// {
//   static int bytes_received = 0;
//   static int inbuffer[0x1000];
//   static int status = 0;
//   int out = 0x00;
//   
//   if( reset )
//   {
//     printf( "serial byte inbuffer reset\n" );
//     bytes_received = 0;
//   }
//   
//   if( bytes_received < 0x1000 )
//   {
//     inbuffer[bytes_received++] = in;
//     printf( "bytes received: %d\n", bytes_received );
//   }
//   
//   if( bytes_received == 3 )
//   {
//     // verify magic values
//     if( inbuffer[0] == 0x88 && inbuffer[1] == 0x33 )
//       switch( inbuffer[2] )
//       {
//         case 1:
//           printf( "command: init\n");
//           break;
//         case 2:
//           printf( "command: print\n");
//           break;
//         case 4:
//           printf( "command: fill buffer\n");
//           break;
//         case 0xF:
//           printf( "command: read status\n");
//           break;
//         default:
//           printf( "command: UNKNOWN\n" );
//           break;
//       }
//     else
//       printf( "ruh-roh\n" );
//   }
//   
//   static int data_length = 0;
//   if( (inbuffer[0] == 0x88 && inbuffer[1] == 0x33) && bytes_received == 6 )
//   {
//     // look at data length
//     data_length = inbuffer[4] + inbuffer[5]*256;
//     printf( "data length: %d\n", data_length );
//   }
//   
//   if( (inbuffer[0] == 0x88 && inbuffer[1] == 0x33) && bytes_received == (8 + data_length) )
//     out = 0x81;
//   
//   if( (inbuffer[0] == 0x88 && inbuffer[1] == 0x33) && bytes_received == (9 + data_length) )
//     out = status;
//   
//   printf( "printer received byte: %02X\n", in  );
//   printf( "printer sending  byte: %02X\n", out );
//   
//   return out;
// }
