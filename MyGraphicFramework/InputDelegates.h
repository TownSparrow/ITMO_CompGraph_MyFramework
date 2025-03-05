// InputDelegates.h
#pragma once
#include "Delegates.h"
#include <DirectXMath.h>

struct MouseMoveEventArgs {
  DirectX::XMFLOAT2 Position;
  DirectX::XMFLOAT2 Offset;
  int WheelDelta;
};

typedef MulticastDelegate<const MouseMoveEventArgs&> MouseMoveDelegate;