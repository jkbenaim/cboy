cboy
============

cboy is a Game Boy emulator written in C for GNU/Linux.

## Usage

```console
cboy run game.gb
```


## Install

You'll need a compiler and SDL2. On Debian, you can install all dependencies with
```console
sudo apt-get install build-essential libsdl2-dev
```

Then, to build cboy:
```console
cd src
make
```


## License

Most of cboy is GPLv3, however the gb\_apu library from Shay Green is LGPL.
