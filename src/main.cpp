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
#include "input.h"
#include "cpu.h"
#include "memory.h"
#include "audio.h"
#include "video.h"
#include "cart.h"
#include "main.h"
#include "audio.h"

int stop = 0;
int pause = 0;

struct command {
    const char *name;
    int (*func)(int, char**);
    const char *help_short;
    const char *help_long;
    int end;    // 0 (default) for commands, 1 for end-of-array sentinel
};
void print_usage();
int cmd_run(int, char**);
int cmd_help(int, char**);
int cmd_exit(int, char**);

struct command cmds[] = {
    {
        .name = "help",
        .func = cmd_help,
        .help_short = "Get help for a command.",
        .help_long = "help <command>\n\tget help for a command",
    },
    {
        .name = "run",
        .func = cmd_run,
        .help_short = "Run a Game Boy ROM in the emulator.",
        .help_long = "run <romfile>\n\trun romfile in the Game Boy emulator",
    },
    {
        .name = "",
        .func = cmd_exit,
        .help_short = "",
        .help_long = "",
        .end = 1, // end-of-array sentinel
    },
};

int get_cmd_index_for_name( char *needle )
{
    int cmds_index = 0;
    while( !cmds[cmds_index].end )
    {
        if( !strcmp( cmds[cmds_index].name, needle ) )
        {
            return cmds_index;
        }
        cmds_index++;
    }

    return -1;
}

int main( int argc, char *argv[] )
{
    if( argc < 2 )
    {
        print_usage();
        return 1;
    }

    char *exe_name = argv[0];
    char *command_name = argv[1];
    int cmd_index = get_cmd_index_for_name( command_name );
    if( cmd_index == -1 )
    {
        printf( "%s: '%s' is not a command.\n", exe_name, command_name );
        return 1;
    }
    else
    {
        return cmds[cmd_index].func( argc, argv );
    }
}

void print_usage()
{
    printf( "usage: cboy <command> <[args]>\n"
            "\n"
            "The available commands are:\n" );
    int cmds_index=0;
    while( !cmds[cmds_index].end )
    {
        printf( "\t%s\t%s\n", cmds[cmds_index].name, cmds[cmds_index].help_short);
        cmds_index++;
    }

    printf( "\n"
            "See 'cboy help <command>' to read about a specific command.\n" );
}

int cmd_help( int argc, char *argv[] )
{
    if( argc < 3 )
    {
        fprintf( stderr, "usage: cboy help <command>\n" );
        return 1;
    }

    char *command_name = argv[2];
    int cmds_index=0;
    while( !cmds[cmds_index].end )
    {
        if( !strcmp( cmds[cmds_index].name, command_name ) )
        {
            printf( "%s\n", cmds[cmds_index].help_long );
            return 0;
        }
        cmds_index++;
    }
    printf( "No command %s\n", command_name );
    return 1;
}

int cmd_run( int argc, char *argv[] )
{
    if( argc < 3 )
    {
        fprintf( stderr, "usage: cboy run <romfile>\n" );
        return 1;
    }

    mem_init();
    audio_init();
    if( !cpu_init() )
    {
        fprintf( stderr, "cpu init failed\n" );
        return 1;
    }
    cart_init( NULL, argv[2] );
    vid_init();
    input_init();
    while( !stop )
    {
        vid_waitForNextFrame();
        input_handle();
        if( !pause )
        {
            cpu_do_one_frame();
            vid_frame();
            audio_frame();
        }
    }

    cart_cleanup();
    audio_cleanup();

    return 0;
}

int cmd_exit( int argc, char *argv[] )
{
    return 0;
}
