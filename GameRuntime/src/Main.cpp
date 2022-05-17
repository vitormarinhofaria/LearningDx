#include "Game.h"
#include "Renderer.h"
#include "DirectXTK/GamePad.h"
#include "DirectXTK/Mouse.h"
#include "DirectXTK/Keyboard.h"
#include "entt/entt.hpp"

LRESULT CALLBACK WindowProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
bool running = false;

//typedef  Engine::Game** RegisterGameClass(void* window, size_t* game);
extern "C" typedef Engine::Game* (*RegisterGameClass)(void* window);
int main()
{
	WNDCLASSEX wnd_class{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_VREDRAW | CS_HREDRAW,
		.lpfnWndProc = WindowProc,
		.hInstance = GetModuleHandle(nullptr),
		.lpszClassName = L"MainWindowClass",
	};
	RegisterClassEx(&wnd_class);

	HWND window = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, wnd_class.lpszClassName, L"Main Window",
		WS_OVERLAPPEDWINDOW, 0, 0, 1280, 720, nullptr, nullptr, wnd_class.hInstance, nullptr);

	if (window == nullptr)
	{
		fmt::print("Failed to create a window\n");
		return EXIT_FAILURE;
	}
	ShowWindow(window, SHOW_OPENWINDOW);

	std::unique_ptr<DirectX::Keyboard> dxKeyboard = std::make_unique<DirectX::Keyboard>();
	std::unique_ptr<DirectX::Mouse> dxMouse = std::make_unique<DirectX::Mouse>();
	std::unique_ptr<DirectX::GamePad> dxGamePad = std::make_unique<DirectX::GamePad>();

	auto lib = LoadLibrary(L"GameProj.dll");
	auto* func = (RegisterGameClass)GetProcAddress(lib, "RegisterGameClass");
	Engine::Game* game = func(window);
	game->Init();
	running = true;
	while (running) {
		MSG msg{};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		};
		if (msg.message == WM_QUIT) {
			running = false;
			break;
		}
		if (DirectX::Keyboard::Get().GetState().Escape) {
			running = false;
			break;
		}
		game->Update(0);
		game->mRenderer.Render();
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT return_result = 0;
	switch (msg)
	{
	case WM_ACTIVATE:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_DESTROY:
	{
		PostQuitMessage(EXIT_SUCCESS);
		running = false;
		break;
	}
	default:
		return_result = DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	return return_result;
}