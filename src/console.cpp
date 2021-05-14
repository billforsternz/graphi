#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <algorithm>
#include "console.h"

static const WORD MAX_CONSOLE_LINES = 500;      // maximum mumber of lines the output console should have

void RedirectIOToConsole()
{
    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    // allocate a console for this app
    AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(handle, &coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(handle, coninfo.dwSize);

	short height = 60;
	short width = 80;

	_SMALL_RECT rect;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = height - 1;
	rect.Right = width - 1;

	SetConsoleWindowInfo(handle, TRUE, &rect);            // Set Window Size

	// Redirect the CRT standard input, output, and error handles to the console
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//Clear the error state for each of the C++ standard stream objects. We need to do this, as
	//attempts to access the standard streams before they refer to a valid target will cause the
	//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
	//to always occur during startup regardless of whether anything has been read from or written to
	//the console or not.
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
}


