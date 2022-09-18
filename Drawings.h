#pragma once
#include "overlay.h"

#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color);
void DrawCircle(int x, int y, float radius, float width, float rez, D3DCOLOR color);