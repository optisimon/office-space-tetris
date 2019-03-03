#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_gfxPrimitives_font.h>
#include "sdl-related.hpp"

#include <assert.h>

#include <unistd.h>

#include <vector>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/input.h>

using namespace std;


// This would require root priviledges
void test_opening_event_device()
{
	const char* device = "/dev/input/event3";
	struct input_event event;
	int fd = open(device, O_RDONLY);
	if (fd == -1)
	{
		std::cout << "ERROR: unable to open \"" << device << "\"\n";
	}
	
	while (read(fd, &event, sizeof(event)) > 0)
	{
		if (event.type != EV_KEY)
		{
			continue;
		}

		if (event.value == 2)
		{
			std::cout << "Repeat\n";
		}

		if (event.value == 1)
		{
			std::cout << "Make\n";
		}

		if (event.value == 0)
		{
			std::cout << "Brake\n";
		}
	}
}


int main(int argc, char* argv[])
{
	test_opening_event_device();
}
