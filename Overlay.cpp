#include "overlay.h"
#include <string>
#include <filesystem>
#include <thread>

HWND OverlayHWND;
IDirect3D9Ex* p_Object;
IDirect3DDevice9Ex* p_Device;
D3DPRESENT_PARAMETERS p_Params;
ID3DXLine* p_Line;
ID3DXFont* p_Font;
ID3DXFont* pModuleFont;
RECT rc;
MARGINS margin = { -1 };
ImFont* titleFont;
ImFont* bigFont;
MSG message;
POINT WindowWH = { 0, 0 };
bool menuActive = false;
bool crosshairenabled = false;
int crosshairtype = 0;
int selectedcrosshair = 0;

std::wstring lClassName = L"ExoCross";
std::wstring lWindowName = L"ExoCross v1";

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;

	case WM_DESTROY:
		Overlay::DirectXShutdown();
		PostQuitMessage(0);
		return 0;
		break;

	default:
		return DefWindowProcW(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void HandleMessages()
{
	OverlayHWND = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED, lClassName.c_str(), lWindowName.c_str(), WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, 0, 0);
	DwmExtendFrameIntoClientArea(OverlayHWND, &margin);
	ShowWindow(OverlayHWND, SW_SHOW);
	UpdateWindow(OverlayHWND);


	while (true)
	{
		if (PeekMessageW(&message, OverlayHWND, 0, 0, PM_REMOVE))
		{
			DispatchMessageW(&message);
			TranslateMessage(&message);
		}

		Sleep(1);
	}
}

bool Overlay::Init() {

	WNDCLASSEXW wClass;
	RtlZeroMemory(&wClass, sizeof(wClass));
	wClass.cbSize = sizeof(WNDCLASSEXW);
	wClass.lpfnWndProc = WinProc;
	wClass.lpszMenuName = lWindowName.c_str();
	wClass.lpszClassName = lClassName.c_str();
	if (!RegisterClassExW(&wClass))
		return false;

	std::thread msgThread(HandleMessages);
	msgThread.detach();

	while (!OverlayHWND) {
		Sleep(1);
	}
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		return false;

	globals.WindowWH = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	RtlZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.hDeviceWindow = OverlayHWND;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.BackBufferWidth = globals.WindowWH.x;
	p_Params.BackBufferHeight = globals.WindowWH.y;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.Windowed = TRUE;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayHWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
		return false;

	D3DXCreateLine(p_Device, &p_Line);
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(OverlayHWND);
	ImGui_ImplDX9_Init(p_Device);
	return true;
}

void InputHandler() {
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;
	int button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) button = 0;
	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
}

void DrawImGui()
{
	InputHandler();
	if (globals.menuActive) {
		SetWindowLongA(OverlayHWND, GWL_EXSTYLE, (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));
	}
	else{
		SetWindowLongA(OverlayHWND, GWL_EXSTYLE, (WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));
	}

	if (globals.menuActive)
	{
		ImGui::SetNextWindowSize(ImVec2(350, 150));
		ImGui::Begin("ExoCross v1", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Checkbox("Enable Crosshair", &globals.crosshairenabled);
		static const char* crosshairtype[]{ ".", "x", "+" };
		if (globals.crosshairenabled) {
			ImGui::Combo("Crosshair Type", &globals.selectedcrosshair, crosshairtype, IM_ARRAYSIZE(crosshairtype));
			if (globals.selectedcrosshair == 0) {
				globals.crosshairtype = 0;
			}
			if (globals.selectedcrosshair == 1) {
				globals.crosshairtype = 1;
			}
			if (globals.selectedcrosshair == 2) {
				globals.crosshairtype = 2;
			}
			ImGui::ColorEdit3("Crosshair Color", globals.crosshaircolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions);
		}
		std::string fps = std::to_string(ImGui::GetIO().Framerate);
		fps.pop_back(); fps.pop_back(); fps.pop_back(); fps.pop_back(); fps.pop_back(); fps.pop_back(); fps.pop_back();
		fps.append(" FPS");
		ImGui::Text(fps.c_str());
		ImGui::End();
	}
}

void Overlay::Render()
{
	static ImDrawList* drawList;

	while (!GetAsyncKeyState(VK_END))
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		DrawImGui();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::Begin("ExoCross Main", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
		drawList = ImGui::GetWindowDrawList();
		p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.f, 0);
		p_Device->BeginScene();

		//render
		globals.color = ARGB(255, (int)(globals.crosshaircolor[0] * 255), (int)(globals.crosshaircolor[1] * 255), (int)(globals.crosshaircolor[2] * 255));
		if (globals.crosshairenabled) {
			if (globals.crosshairtype == 0) {
				DrawCircle(globals.WindowWH.x / 2 - 1, globals.WindowWH.y / 2 - 1, 1, 1.f, 20.f, globals.color);

			}
			if (globals.crosshairtype == 1) {
				DrawLine((globals.WindowWH.x / 2) - 10, (globals.WindowWH.y / 2) - 10, (globals.WindowWH.x / 2) + 10, (globals.WindowWH.y / 2) + 10, 2, globals.color);
				DrawLine((globals.WindowWH.x / 2) - 10, (globals.WindowWH.y / 2) + 10, (globals.WindowWH.x / 2) + 10, (globals.WindowWH.y / 2) - 10, 2, globals.color);
			}
			if (globals.crosshairtype == 2) {
				DrawLine((globals.WindowWH.x / 2) - 10, globals.WindowWH.y / 2, (globals.WindowWH.x / 2) + 10, globals.WindowWH.y / 2, 2, globals.color);
				DrawLine(globals.WindowWH.x / 2, (globals.WindowWH.y / 2) - 10, globals.WindowWH.x / 2, (globals.WindowWH.y / 2) + 10, 2, globals.color);
			}

		}

		drawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
		p_Device->PresentEx(0, 0, 0, 0, 0);
		Sleep(10);
	}
}

void Overlay::DirectXShutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	p_Object->Release();
	p_Device->Release();
	p_Line->Release();
	DestroyWindow(OverlayHWND);
}