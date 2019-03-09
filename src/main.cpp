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

void test_launching_xinput_test_NEWER(int xinputId = 9)
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

		if (!fds.revents & POLLIN)
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




/** @warning xinputId will vary for each setup. Could be
 *           determined by looking at output of xinput list.
 *               All possible candidates can be determined by
 *           xinput list | grep -Po 'id=\K\d+(?=.*slave\s*keyboard)'
 */
void test_launching_xinput_test(int xinputId = 9)
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
		while (true)
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
	std::thread t1(test_launching_xinput_test_NEWER, 9);
	t1.join();
//	test_launching_xinput_test();
//	test_opening_event_device();
}
