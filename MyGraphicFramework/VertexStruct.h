#pragma once

#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

struct Vertex {
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT2 textureCordinates;
	DirectX::XMFLOAT3 normal;
};
