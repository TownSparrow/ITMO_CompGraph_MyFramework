#include "DisplayWin32.h"
#include "Game.h"

// --- Window Process --- //
LRESULT DisplayWin32::WndProc(
	HWND hwnd,
	UINT umessage,
	WPARAM wparam,
	LPARAM lparam
){
	Game* game = Game::GetInstance();

	switch (umessage){
	case WM_KEYDOWN:{
		// If a key is pressed send it to the input object so it can record that state.
		// std::cout << "Key value: " << static_cast<unsigned int>(wparam) << std::endl;

		//if (game->isPong) {
		//	if (wparam < 256) PongGame::GetInstance()->keys[wparam] = true;
		//}

		if (static_cast<unsigned int>(wparam) == 27) {
		// But in future it could be like this:
		// if (game->inputDevice->IsKeyDown(Keys::Escape))
			PostQuitMessage(0);
			return 0;
		}
		break;
	}

	case WM_KEYUP:
	{
		//if (game->isPong) {
		//	if (wparam < 256) PongGame::GetInstance()->keys[wparam] = false;
		//}
	}

	case WM_INPUT: {
		UINT dwSize = 0;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == nullptr) {
			return 0;
		}

		if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			//printf(" Kbd: make=%04i Flags:%04i Reserved:%04i ExtraInformation:%08i, msg=%04i VK=%i \n",
			//	raw->data.keyboard.MakeCode,
			//	raw->data.keyboard.Flags,
			//	raw->data.keyboard.Reserved,
			//	raw->data.keyboard.ExtraInformation,
			//	raw->data.keyboard.Message,
			//	raw->data.keyboard.VKey);

			game->inputDevice->OnKeyDown({
				raw->data.keyboard.MakeCode,
				raw->data.keyboard.Flags,
				raw->data.keyboard.VKey,
				raw->data.keyboard.Message
				});
		}
		else if (raw->header.dwType == RIM_TYPEMOUSE) {
			//printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
			game->inputDevice->OnMouseMove({
				raw->data.mouse.usFlags,
				raw->data.mouse.usButtonFlags,
				static_cast<int>(raw->data.mouse.ulExtraInformation),
				static_cast<int>(raw->data.mouse.ulRawButtons),
				static_cast<short>(raw->data.mouse.usButtonData),
				raw->data.mouse.lLastX,
				raw->data.mouse.lLastY
				});
		}

		delete[] lpb;
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}

	default: {
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}

// --- Display --- //
void DisplayWin32::Display() {
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ShowCursor(true);
}
