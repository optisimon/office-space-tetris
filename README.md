# office-space-tetris
A version of tetris which don't care which keys you are pressing, but continues solving the puzzle as long as keys are pressed on the keyboard.

**This will only work on UNIX / Linux systems still using an x-server without GUI level security separation of applications.**

It's using the input event devices which all running programs have access to as long as you are running an x.org server (and not wayland etc).

# Usage
It's using xinput under the hood for reading key presses, and for ubuntu 18.04,
that require installation of the package "xinput"

Running office-space-tetris without any arguments (or with only -h) provides basic instructions:

```bash
Usage: office-space-tetris inputDeviceId

Will launch a game of Tetris, making seemingly reasonable moves every time ANY key
is pressed, regardless of which window is in focus

A list of input device ID:s can be obtained by running
  xinput list

but a better alternative may be to run
  xinput test-xi2
and look for the number in paranthesis after the device entry for each key press.
In my case, I got "device: 3 (9)", so 9 would be my inputDeviceId.
```

# Figuring out your input device
```bash
# The ID after each "Virtual core keyboard", as well as each
# "device" field when you are pressing keys shows the number to
# append to /dev/input/event to get the correct device for your keyboard
xinput test-xi2

# You can also get a list of all devices from, but since my keyboard presents
# itself as two input devices here with only one sending regular keys, and the
# other sending multimedia keys, it can be hard to determine the device ID from this:
xinput list 
```
