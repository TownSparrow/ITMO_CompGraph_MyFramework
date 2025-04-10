#pragma once

#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>
#include <vector>
#include <mutex>

#include <SimpleMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace DirectX::SimpleMath;

class ShadowMap
{
private:
	UINT width;
	UINT height;

	ID3D11Texture2D* renderTargetTexture;
	ID3D11Texture2D* depthStencilTexture;
	ID3D11RenderTargetView* rtv;

	ID3D11ShaderResourceView* shadowMapSRV;
	ID3D11DepthStencilView* shadowMapDSV;

	D3D11_VIEWPORT viewport;

public:
	ShadowMap();
	~ShadowMap();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, UINT width, UINT height);

	ID3D11ShaderResourceView* GetShadowMapDSV();

	void SetRenderTarget(ID3D11DeviceContext* context);
	void ClearRenderTarget(ID3D11DeviceContext* context, Vector4 color);
};

