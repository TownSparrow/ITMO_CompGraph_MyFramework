#include "TriangleComponent.h"
#include "Game.h"

// --- Init --- //
void TriangleComponent::Initialize(
	LPCWSTR shaderSource,
	std::vector<DirectX::XMFLOAT4> pointsInput,
	std::vector<int> indexesInput,
	std::vector<UINT> stridesInput,
	std::vector<UINT> offsetsInput
){
	// Get all values from input info
	points = pointsInput;
	indexes = indexesInput;
	strides = stridesInput;
	offsets = offsetsInput;

	ID3DBlob* errorVertexCode = nullptr;
	
	// Compile result from vertex stage
	HRESULT res = D3DCompileFromFile(
		shaderSource,
		nullptr /*macros*/,
		nullptr /*include*/,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexByteCode,
		&errorVertexCode
	);

	// Default shader macros
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
	ID3DBlob* errorPixelCode = nullptr;

	// Compile result from pixel stage
	res = D3DCompileFromFile(
		shaderSource,
		Shader_Macros /*macros*/,
		nullptr /*include*/,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelByteCode,
		&errorPixelCode
	);

	// Create Vertex Shader
	game->device->CreateVertexShader(
		vertexByteCode->GetBufferPointer(),
		vertexByteCode->GetBufferSize(),
		nullptr, &vertexShader);

	// Create Pixel Shader
	game->device->CreatePixelShader(
		pixelByteCode->GetBufferPointer(),
		pixelByteCode->GetBufferSize(),
		nullptr, &pixelShader);

	// Get positions and colors as input elements
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
	D3D11_INPUT_ELEMENT_DESC {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0},
	D3D11_INPUT_ELEMENT_DESC {
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0}
	};

	// Create input layout with input elements
	game->device->CreateInputLayout(
		inputElements,
		2,
		vertexByteCode->GetBufferPointer(),
		vertexByteCode->GetBufferSize(),
		&layout);

	// Vertex buffer
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * points.size();

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	game->device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

	// Index buffer
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indexes);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indexes.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	game->device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

	// Rasterizer
	CD3D11_RASTERIZER_DESC rastDesc = {};
	//rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FillMode = D3D11_FILL_SOLID  /* D3D11_FILL_WIREFRAME*/;

	res = game->device->CreateRasterizerState(&rastDesc, &rastState);

	// Const buffer
	D3D11_BUFFER_DESC constBufferDesc = {};
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;
	constBufferDesc.ByteWidth = sizeof(ConstData);

	game->device->CreateBuffer(&constBufferDesc, nullptr, &constBuffer);

	// Set default transforms with matrix
	transforms = {};
	transforms.move = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	transforms.rotate = Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
	transforms.scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);

	// Put transformation and color data to const buffer
	constData = {};
	constData.transformations = transforms.scale * transforms.rotate * transforms.move;
	constData.color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

// --- Draw of triangle --- //
//void TriangleComponent::Draw(ConstData* data) {
void TriangleComponent::Draw() {
	game->context->RSSetState(rastState);
	game->context->IASetInputLayout(layout);
	game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	game->context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	game->context->VSSetConstantBuffers(0, 1, &constBuffer);
	game->context->IASetVertexBuffers(0, 1, &vertexBuffer, strides.data(), offsets.data());
	game->context->VSSetShader(vertexShader, nullptr, 0);
	game->context->PSSetShader(pixelShader, nullptr, 0);
	game->context->DrawIndexed(indexes.size(), 0, 0);
}

// --- Update of triangle --- //
//void TriangleComponent::Update(ConstData* data) {
void TriangleComponent::Update() {
	// Change data in constant buffer for new transformations
	constData.transformations = transforms.scale * transforms.rotate * transforms.move;
	constData.transformations = constData.transformations.Transpose();
	
	constData.view = game->activeCamera->cameraMatrix.view;
	constData.view = constData.view.Transpose();

	constData.projection = game->activeCamera->cameraMatrix.projection;
	constData.projection = constData.projection.Transpose();

	// Apply mapping of results
	D3D11_MAPPED_SUBRESOURCE res = {};
	game->context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	memcpy(res.pData, &constData, sizeof(ConstData));
	game->context->Unmap(constBuffer, 0);
}

// --- Destroy Resources --- //
void TriangleComponent::DestroyResources() {
	layout->Release();
	vertexShader->Release();
	vertexByteCode->Release();
	vertexBuffer->Release();
	pixelShader->Release();
	pixelByteCode->Release();
	indexBuffer->Release();
	rastState->Release();
}
