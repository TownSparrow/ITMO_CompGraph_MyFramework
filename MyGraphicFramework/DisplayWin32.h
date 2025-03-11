#pragma once

#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include "Keys.h"
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class Game;

class DisplayWin32 {
public:
	int ClientWidth;
	int ClientHeight;
	LPCWSTR applicationName;
	HINSTANCE hInstance;
	HMODULE hModule;
	WNDCLASSEX wc;

	HWND hWnd;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

	DisplayWin32(
		int clientWidthInput, 
		int clientHeightInput, 
		LPCWSTR applicationNameInput, 
		HMODULE hModuleInput = nullptr
	) {
		ClientWidth = clientWidthInput;
		ClientHeight = clientHeightInput;
		applicationName = applicationNameInput;
		hModule = hModuleInput;

		hInstance = GetModuleHandle(nullptr);

		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = applicationName;
		wc.cbSize = sizeof(WNDCLASSEX);

		RegisterClassEx(&wc);

		RECT windowRect = { 0, 0, static_cast<LONG>(ClientWidth), static_cast<LONG>(ClientHeight) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

		auto posX = (GetSystemMetrics(SM_CXSCREEN) - ClientWidth) / 2;
		auto posY = (GetSystemMetrics(SM_CYSCREEN) - ClientHeight) / 2;

		hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
			dwStyle,
			posX, posY,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr, nullptr, hInstance, nullptr);
	}

	~DisplayWin32() {
		if (hWnd) {
			DestroyWindow(hWnd);
			hWnd = nullptr;
		}

		if (wc.lpszClassName) {
			UnregisterClass(wc.lpszClassName, hInstance);
		}
	}

	void Display();
};