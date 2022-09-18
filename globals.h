#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "imgui/imgui.h"

#define ARGB(a, r, g, b) D3DCOLOR_ARGB(a, r, g, b)

struct GlobalVariables
{
	HWND hWnd;
	POINT WindowWH = { 0, 0 };
	bool menuActive = false;
	bool crosshairenabled = false;
	int crosshairtype = 0;
	int selectedcrosshair = 0;
	float crosshaircolor[3] = { 0.f, 1.f, 0.f };
	D3DCOLOR color;
};

extern GlobalVariables globals;