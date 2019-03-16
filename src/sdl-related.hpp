#pragma once

#include <SDL/SDL.h>

void Slock(SDL_Surface *screen);

void Sulock(SDL_Surface *screen);

void safeDrawPixel(SDL_Surface *screen, int x, int y,
		Uint8 R, Uint8 G, Uint8 B);

void DrawPixel(SDL_Surface *screen, int x, int y,
                                    Uint8 R, Uint8 G, Uint8 B);

SDL_Surface* setup_sdl(int w, int h);
