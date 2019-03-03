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
#include <vector>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/input.h>

using namespace std;

std::atomic<int> keyPresses(0);


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

/** @warning xinputId will vary for each setup. Could be
 *           determined by looking at output of xinput list.
 *               All possible candidates can be determined by
 *           xinput list | grep -Po 'id=\K\d+(?=.*slave\s*keyboard)'
 */
void test_launching_xinput_test(int xinputId = 14)
{
	int pipes[2] = {};
	if (-1 == pipe(pipes))
	{
		std::cout << "ERROR: unable to pipe: " << strerror(errno) << "\n";
	}

	const int fd_parent_read_stdout = pipes[0];
	const int fd_child_write_stdout = pipes[1];

	int pid = fork();
	if (pid)
	{
		if (-1 == dup2(fd_child_write_stdout, STDERR_FILENO))
		{
			std::cout << "ERROR: child proces dup2 \"" << strerror(errno) << "\"\n";
		}
		if (-1 == dup2(fd_child_write_stdout, STDOUT_FILENO))
		{
			std::cout << "ERROR: child proces dup2 \"" << strerror(errno) << "\"\n";
		}

		// close fd not needed by child
		close(fd_parent_read_stdout);
		close(fd_child_write_stdout);
		close(STDIN_FILENO); // TODO: just close, or add a dummy one???

		// child
		std::ostringstream id;
		id << xinputId;

		char * argv[] = {"/usr/bin/xinput", "test", 0, 0};
		argv[2] = const_cast<char*>(id.str().c_str());

		execv(argv[0], argv);

		std::cout << "ERROR: still alive after execv\n";
		exit(1);
	}
	else
	{
		// parent
		close(fd_child_write_stdout); // No use for childs write fd

		char buff[100] = {};
		while(true)
		{
			int bytes = read(fd_parent_read_stdout, buff, sizeof(buff)-1);
			if (bytes >= 0)
			{
				buff[bytes] = 0;
				if (bytes > 0 && buff[bytes-1]=='\n')
				{
					buff[bytes-1] = 0; // strip one trailing newline
				}
				std::cout << "INFO: got \"" << buff << "\"\n";
				if (strstr(buff, "press"))
				{
					keyPresses++;
					std::cout << "Make\n";
				}
			}
			else
			{
				// TODO: use thread safe version of strerror?
				std::cout << "ERROR: parent unable to read: " << strerror(errno) << "\n";
			}
		}
	}
}

int main(int argc, char* argv[])
{
	test_launching_xinput_test();
//	test_opening_event_device();
}
