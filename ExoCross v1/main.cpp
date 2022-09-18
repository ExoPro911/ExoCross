#include <thread>
#include "globals.h"
#include "overlay.h"
#include <iostream>

int lastkey = 0;

int main(int argC, char** argV)
{
	if (!Overlay::Init())
	{
		std::cout << "Failed to create Window and initialize DirectX\n";
		Sleep(5000);
		return -1;
	}
	std::thread(Overlay::Render).detach();
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	while (!GetAsyncKeyState(VK_END))
	{
		while (GetAsyncKeyState(VK_F11))
		{
			lastkey = VK_F11;
		}

		if (lastkey == VK_F11)
		{
			lastkey = 0;
			globals.menuActive = !globals.menuActive;
		}
		Sleep(1);
	}
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	std::thread(Overlay::Render).join();
	Overlay::DirectXShutdown();
	return 0;
}