#pragma once

#include <wrl.h>
#include <iostream>
#include <algorithm>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "GameComponent.h"

//TEMP_ADD
struct ConstData {
	Matrix transformations;
	Matrix view;
	Matrix projection;
	Vector4 color;
};

struct Transformations {
	Matrix move;
	Matrix rotate;
	Matrix scale;
};

class TriangleComponent : public GameComponent {
private:
	ID3D11InputLayout* layout;
	ID3D11VertexShader* vertexShader;
	ID3DBlob* vertexByteCode;
	ID3D11Buffer* vertexBuffer;
	ID3D11PixelShader* pixelShader;
	ID3DBlob* pixelByteCode;
	ID3D11Buffer* indexBuffer;
	ID3D11RasterizerState* rastState;
	ID3D11Buffer* constBuffer;
	//ID3D11Buffer* constBuffer2;

	std::vector<UINT> strides;
	std::vector<UINT> offsets;

public:
	std::vector<DirectX::XMFLOAT4> points;
	std::vector<int> indexes;

	//TEMP_ADD
	Transformations transforms;
	ConstData constData;

	TriangleComponent(Game* gameInput) : GameComponent(gameInput) {
		layout = nullptr;
		vertexShader = nullptr;
		vertexByteCode = nullptr;
		vertexBuffer = nullptr;
		pixelShader = nullptr;
		pixelByteCode = nullptr;
		indexBuffer = nullptr;
		rastState = nullptr;
		constBuffer = nullptr;
	};

	void Initialize(
		LPCWSTR shaderSource,
		std::vector<DirectX::XMFLOAT4> pointsInput,
		std::vector<int> indexesInput,
		std::vector<UINT> stridesInput,
		std::vector<UINT> offsetsInput
	);

	//void Draw(ConstData* data = nullptr);
	//void Update(ConstData* data = nullptr);
	//void DestroyResources();
	void Draw();
	void Update();
	void DestroyResources();
};

