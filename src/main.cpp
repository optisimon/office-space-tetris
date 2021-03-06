#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_gfxPrimitives_font.h>
#include "sdl-related.hpp"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <atomic>
#include <sstream>
#include <thread>
#include <vector>
#include <fstream>

#include <stdio.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/input.h>


std::atomic<int> keyPresses(0);
std::atomic<bool> quitting(false);


#include "Tetris.hpp"

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
			keyPresses++;
		}

		if (event.value == 0)
		{
			std::cout << "Brake\n";
		}
	}
}

// This won't need root priviledges
void test_launching_xinput_test(int xinputId = 9)
{
	std::ostringstream cmd;
	cmd << "/usr/bin/xinput test " << xinputId;

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.str().c_str(), "r"), pclose);
	if (!pipe)
	{
		std::cout << "ERROR: unable to pipe\n";
		exit(1);
	}

	char buff[100] = {};

	// TODO: investigate select and non-blocking read (to be able to quit)
	struct pollfd fds;
	fds.fd = fileno(pipe.get());
	fds.events = POLLIN;

	while (!quitting)
	{
		int ret = poll(&fds, 1, /* timeout */ 1000);
		if (ret == -1)
		{
			std::cout << "ERROR: poll failed: " << strerror(errno) << "\n";
			quitting = true;
			break;
		}
		if (!ret)
		{
			std::cout << "INFO: poll timed out\n";
			continue;
		}

		if (!(fds.revents & POLLIN))
		{
			std::cout << "ERROR: should not happen (poll)\n";
			exit(1);
		}

		if(fgets(buff, sizeof(buff)-1, pipe.get()) == nullptr)
		{
			break;
		}

		buff[sizeof(buff)-1] = '\0';
		int bytes = strlen(buff);
		if (bytes >= 0)
		{
			buff[bytes] = 0;
			if (bytes > 0 && buff[bytes-1]=='\n')
			{
				buff[bytes-1] = 0; // strip one trailing newline
			}
			//std::cout << "INFO: got \"" << buff << "\"\n";
			if (strstr(buff, "press"))
			{
				keyPresses++;
				//std::cout << "Make\n";
			}
		}
		else
		{
			// TODO: use thread safe version of strerror?
			std::cout << "ERROR: parent unable to read: " << strerror(errno) << "\n";
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc != 2  || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		std::cout
		<<
		"Usage: office-space-tetris inputDeviceId\n"
		"\n"
		"Will launch a game of Tetris, making seemingly reasonable moves every time ANY key\n"
		"is pressed, regardless of which window is in focus\n"
		"\n"
		"A list of input device ID:s can be obtained by running\n"
		"  xinput list\n"
		"\n"
		"but a better alternative may be to run\n"
		"  xinput test-xi2\n"
		"and look for the number in paranthesis after the device entry for each key press.\n"
		"In my case, I got \"device: 3 (9)\", so 9 would be my inputDeviceId.\n";
		exit(0);
	}
	int inputDeviceId = atoi(argv[1]);

	std::thread t0(tetris);
	std::thread t1(test_launching_xinput_test, inputDeviceId);

	t0.join();
	quitting = true;
	t1.join();



//	std::thread t1(test_launching_xinput_test, 9);
//	t1.join();
//	tetris();

//	t1.join();
//	t0.join();

//	test_launching_xinput_test();
//	test_opening_event_device();
}
