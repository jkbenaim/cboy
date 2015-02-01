cboy
============

cboy is a Game Boy emulator written in C for Linux.

## Usage

```console
cboy bootrom.bin game.gb
```


## Install

You'll need a compiler and SDL. On Debian, you can install all dependencies with
```console
sudo apt-get install build-essential libsdl-dev
```

Then, to build cboy:
```console
cd source
make
```


## License

It's GPLv3:

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
