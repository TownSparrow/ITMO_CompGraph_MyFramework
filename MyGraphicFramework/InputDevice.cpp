#include "InputDevice.h"
#include <iostream>
#include <SimpleMath.h>

#include "DisplayWin32.h"
#include "Game.h"

// --- Constructor --- //
InputDevice::InputDevice(Game* inGame) : game(inGame) {
	keys = new std::unordered_set<Keys>();

	RAWINPUTDEVICE Rid[2];

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
	Rid[0].hwndTarget = game->window->hWnd;

	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = 0;   // adds HID keyboard and also ignores legacy keyboard messages
	Rid[1].hwndTarget = game->window->hWnd;

	if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE) {
		auto errorCode = GetLastError();
		std::cout << "ERROR: " << errorCode << std::endl;
	}
}

// --- Destructor --- //
InputDevice::~InputDevice() {
	delete keys;
}

// --- Method on key down --- //
void InputDevice::OnKeyDown(KeyboardInputEventArgs args) {
	bool Break = args.Flags & 0x01;
	auto key = static_cast<Keys>(args.VKey);

	if (args.MakeCode == 42) key = Keys::LeftShift;
	if (args.MakeCode == 54) key = Keys::RightShift;

	// Check was the key activated or not
	if (Break) {
		if (keys->count(key))	
			RemovePressedKey(key);
	}
	else {
		if (!keys->count(key))
			AddPressedKey(key);
	}
}

// --- Method on mouse moving --- //
void InputDevice::OnMouseMove(RawMouseEventArgs args)
{
	// All common cases of using the mouse buttons
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonDown))
		AddPressedKey(Keys::LeftButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonUp))
		RemovePressedKey(Keys::LeftButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonDown))
		AddPressedKey(Keys::RightButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonUp))
		RemovePressedKey(Keys::RightButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonDown))
		AddPressedKey(Keys::MiddleButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonUp))
		RemovePressedKey(Keys::MiddleButton);

	// Cursor
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(game->window->hWnd, &p);

	// Mouse positions
	MousePosition = DirectX::SimpleMath::Vector2::Vector2(p.x, p.y);
	MouseOffset = DirectX::SimpleMath::Vector2::Vector2(args.X, args.Y);
	
	// Mouse wheel
	MouseWheelDelta = args.WheelDelta;

	const MouseMoveEventArgs moveArgs = { MousePosition, MouseOffset, MouseWheelDelta };

	//printf(" Mouse: posX=%04.4f posY:%04.4f offsetX:%04.4f offsetY:%04.4f, wheelDelta=%04d \n",
	//	MousePosition.x,
	//	MousePosition.y,
	//	MouseOffset.x,
	//	MouseOffset.y,
	//	MouseWheelDelta);

	MouseMove.Broadcast(moveArgs);
	Game::GetInstance()->MouseInputHandler(MouseOffset);
}

// --- Add Pressed Key --- //
void InputDevice::AddPressedKey(Keys key) {
	//if (!game->isActive) {
	//	return;
	//}
	keys->insert(key);
	Game::GetInstance()->KeyInputHadnler(keys);
}

// --- Remove Pressed Key --- //
void InputDevice::RemovePressedKey(Keys key) {
	keys->erase(key);
	Game::GetInstance()->KeyInputHadnler(keys);
}

// --- Boolean method for checking key pressing --- //
bool InputDevice::IsKeyDown(Keys key) {
	return keys->count(key);
}

// Refresh Kay States
void InputDevice::RefreshKeyStates() {
	// Например, для нужных клавиш
	if (GetAsyncKeyState(static_cast<int>(Keys::W)) & 0x8000)
		keys->insert(Keys::W);
	else
		keys->erase(Keys::W);
	if (GetAsyncKeyState(static_cast<int>(Keys::S)) & 0x8000)
		keys->insert(Keys::S);
	else
		keys->erase(Keys::S);
	if (GetAsyncKeyState(static_cast<int>(Keys::A)) & 0x8000)
		keys->insert(Keys::A);
	else
		keys->erase(Keys::A);
	if (GetAsyncKeyState(static_cast<int>(Keys::D)) & 0x8000)
		keys->insert(Keys::D);
	else
		keys->erase(Keys::D);
}
