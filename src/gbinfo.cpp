/*************************************************************************
 *   Cboy, a Game Boy emulator
 *   Copyright (C) 2014 jkbenaim
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "types.h"
#include "cartdesc.h"

struct header_s {
	/* 0x100 */ uint32_t entry;
	/* 0x104 */ uint8_t logo[48];
	/* 0x134 */ char title[16];
	//     /* 0x13f */ char manufacturer[4];
	//     /* 0x143 */ uint8_t cgb_flag;
	/* 0x144 */ char new_licensee_code[2];
	/* 0x146 */ uint8_t sgb_flag;
	/* 0x147 */ uint8_t cartridge_type;
	/* 0x148 */ uint8_t rom_size;
	/* 0x149 */ uint8_t ram_size;
	/* 0x14a */ uint8_t destination;
	/* 0x14b */ uint8_t old_licensee_code;
	/* 0x14c */ uint8_t rom_version;
	/* 0x14d */ uint8_t header_checksum;
	/* 0x14e */ uint16_t rom_checksum;
};

const uint8_t logo[48] = {
	0xce, 0xed, 0x66, 0x66,
	0xcc, 0x0d, 0x00, 0x0b,
	0x03, 0x73, 0x00, 0x83,
	0x00, 0x0c, 0x00, 0x0d,
	0x00, 0x08, 0x11, 0x1f,
	0x88, 0x89, 0x00, 0x0e,
	0xdc, 0xcc, 0x6e, 0xe6,
	0xdd, 0xdd, 0xd9, 0x99,
	0xbb, 0xbb, 0x67, 0x63,
	0x6e, 0x0e, 0xec, 0xcc,
	0xdd, 0xdc, 0x99, 0x9f,
	0xbb, 0xb9, 0x33, 0x3e
};

#define MAX_ROM_SIZE (8*1024*1024)

int cmd_info_impl(int argc, char *argv[])
{
	uint32_t rom_size = 0;
	uint8_t *rom;
	FILE *fd;
	int i = 0;
	uint16_t my_checksum = 0;
	uint8_t my_header_checksum = 0;

	if (argc < 3) {
		fprintf(stderr, "need an argument\n");
		return 1;
	}

	struct stat s;
	if (stat(argv[2], &s) != 0) {
		fprintf(stderr, "Couldn't stat file: %s\n", argv[2]);
		return 1;
	}

	if (s.st_size > MAX_ROM_SIZE)
		rom_size = MAX_ROM_SIZE;
	else
		rom_size = s.st_size;

	// Allocate memory for the cartrom.
	if ((rom = (uint8_t *) malloc(rom_size)) == NULL) {
		fprintf(stderr, "Cart rom malloc failed.\n");
		return 1;
	}
	// Read the cartrom.
	fd = fopen(argv[2], "r");
	if (fread(rom, rom_size, 1, fd) != 1) {
		fprintf(stderr, "Reading cart rom failed.\n");
		return 1;
	}
	fclose(fd);

	struct header_s header;
	memcpy(&header, rom + 0x100, 0x50);

	//   /* 0x100 */ uint8_t entry[4];
	printf("Entry: ");
	uint32_t temp = ntohl(header.entry);
	if (temp == 0x00c35001)
		printf("normal\n");
	else
		printf("ABONRMAL: %08X\n", temp);

	//     /* 0x104 */ uint8_t logo[48];
	printf("Logo: ");
	if (memcmp(header.logo, logo, 48))
		printf("BAD\n");
	else
		printf("Nintendo (OK)\n");

	//     /* 0x134 */ char title[16];
	//     /* 0x13f */ char manufacturer[4];
	//     /* 0x143 */ uint8_t cgb_flag;
	// TODO: clean up title printing
	printf("Title: %.16s\n", header.title);

	//     /* 0x144 */ char new_licensee_code[2];
	printf("New licensee code: %.2s\n", header.new_licensee_code);

	//     /* 0x146 */ uint8_t sgb_flag;
	printf("SGB flag: %02X", header.sgb_flag);
	if (header.sgb_flag == 0x03)
		printf(" - SGB supported\n");
	else
		printf(" - no SGB support\n");

	//     /* 0x147 */ uint8_t cartridge_type;
	printf("Cartridge type: %02X - %s\n", header.cartridge_type,
	       cartdesc_carttype[header.cartridge_type]);

	//     /* 0x148 */ uint8_t rom_size;
	printf("ROM size: %02X - %s\n", header.rom_size,
	       cartdesc_romsize[header.rom_size]);

	//     /* 0x149 */ uint8_t ram_size;
	printf("SRAM size: %02X - %s\n", header.ram_size,
	       cartdesc_ramsize[header.ram_size]);

	//     /* 0x14a */ uint8_t destination;
	printf("Destination: %02X - %s\n", header.destination,
	       header.destination ? "World" : "Japan");

	//     /* 0x14b */ uint8_t old_licensee_code;
	printf("Old licensee code: %02X\n", header.old_licensee_code);

	//     /* 0x14c */ uint8_t rom_version;
	printf("Mask ROM revision: %02X\n", header.rom_version);

	//     /* 0x14d */ uint8_t header_checksum;
	printf("Header checksum: %02X ", header.header_checksum);
	my_header_checksum = 0;
	for (i = 0x134; i <= 0x14c; ++i)
		my_header_checksum = my_header_checksum - rom[i] - 1;
	if (my_header_checksum == header.header_checksum)
		printf(" - OK\n");
	else
		printf(" - BAD, should be %02X\n", my_header_checksum);

	//     /* 0x14e */ uint16_t rom_checksum;
	printf("ROM checksum: %04X", ntohs(header.rom_checksum));
	my_checksum = 0;
	size_t index;
	for (index = 0; index < rom_size; index++)
		my_checksum += rom[index];
	my_checksum -= rom[0x14e];
	my_checksum -= rom[0x14f];
	if (my_checksum == ntohs(header.rom_checksum))
		printf(" - OK\n");
	else
		printf(" - BAD, should be %X\n", my_checksum);

	free(rom);
	return 0;
}
