#include "sdl-related.hpp"
#include <iostream>
#include <stdio.h>

void Slock(SDL_Surface *screen)
{
	if ( SDL_MUSTLOCK(screen) )
	{
		if ( SDL_LockSurface(screen) < 0 )
		{
			return;
		}
	}
}

void Sulock(SDL_Surface *screen)
{
	if ( SDL_MUSTLOCK(screen) )
	{
		SDL_UnlockSurface(screen);
	}
}
/*
void safeDrawPixel(SDL_Surface *screen, int x, int y,
		Uint8 R, Uint8 G, Uint8 B)
{
	if( x >= 0 && x < w_ && y >= 0 && y < h_)
	{
		DrawPixel(screen, x, y, R, G, B);
	}
}
*/
void DrawPixel(SDL_Surface *screen, int x, int y,
		Uint8 R, Uint8 G, Uint8 B)
{
	Uint32 color = SDL_MapRGB(screen->format, R, G, B);
	switch (screen->format->BytesPerPixel)
	{
	case 1: // Assuming 8-bpp
	{
		Uint8 *bufp;
		bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
		*bufp = color;
	}
	break;
	case 2: // Probably 15-bpp or 16-bpp
	{
		Uint16 *bufp;
		bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
		*bufp = color;
	}
	break;
	case 3: // Slow 24-bpp mode, usually not used
	{
		Uint8 *bufp;
		bufp = (Uint8 *)screen->pixels + y*screen->pitch + x * 3;
		if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
		{
			bufp[0] = color;
			bufp[1] = color >> 8;
			bufp[2] = color >> 16;
		} else {
			bufp[2] = color;
			bufp[1] = color >> 8;
			bufp[0] = color >> 16;
		}
	}
	break;
	case 4: // Probably 32-bpp
	{
		Uint32 *bufp;
		bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
		*bufp = color;
	}
	break;
	}
}



SDL_Joystick * setup_joystick(){
	SDL_Joystick * joy = 0;

	int n = SDL_NumJoysticks();
	if(n<=0){
		return 0;
	}else{
		int choice=0;
		if(n==1){
			std::cout << "Using the only available joystick \""
			<< SDL_JoystickName(0) << "\"\n";
		}
		else
		{
			do{
				std::cout << "Select joystick device:\n";
				for(int i=0; i<n; i++){
					std::cout << "  " << i << SDL_JoystickName(i);
				}
				std::cout << "\n:";
			}while( (std::cin >> choice) && (choice<0 || choice>=n) );
		}
		joy = SDL_JoystickOpen(choice);

		std::cout << "Number of general axis controls: " << SDL_JoystickNumAxes(joy) << "\n"
		<< "Number of trackballs on a joystick (Joystick trackballs have only relative motion\n"
		<< "events associated with them and their state cannot be polled): " 
		<< SDL_JoystickNumBalls(joy) << "\n" 
		<< "number of POV hats: " << SDL_JoystickNumHats(joy) <<"\n"
		<< "number of buttons on a joystick:" << SDL_JoystickNumButtons(joy) << "\n"
		<< "\n";

	}
	return joy;
}


SDL_Surface* setup_sdl(int w, int h)
{

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);
	SDL_WM_SetCaption("Work Progress Monitor", "Progress");//Sets the window tile and icon name


	SDL_Surface *screen;
	screen=SDL_SetVideoMode(w,h,32,SDL_SWSURFACE|SDL_DOUBLEBUF);
	if ( screen == NULL )
	{
		printf("Unable to set %dx%d video: %s\n", w, h, SDL_GetError());
		exit(1);
	}
	return screen;
}
