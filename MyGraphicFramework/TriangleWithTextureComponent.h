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
#include "ConstBufferStruct.h"
#include "VertexStruct.h"
#include <DirectXTex.h>

using namespace DirectX::SimpleMath;

class TriangleWithTextureComponent : public GameComponent {
private:
  int shaderFileIndex;

  ID3D11InputLayout* layout;
  ID3D11VertexShader* vertexShader;
  ID3DBlob* vertexByteCode;
  ID3D11Buffer* vertexBuffer;
  ID3D11PixelShader* pixelShader;
  ID3DBlob* pixelByteCode;
  ID3D11Buffer* indexBuffer;
  ID3D11RasterizerState* rastState;
  ID3D11Buffer* constBuffer;
  ID3D11Buffer* lightBuffer;

  std::vector<UINT> strides;
  std::vector<UINT> offsets;

  // For working with textures:
  //bool is2D;
  ID3D11ShaderResourceView* textureView;
  ID3D11SamplerState* samplerState;

  // Also materials
  Material* material;

public:
  //std::vector<DirectX::XMFLOAT4> points;
  std::vector<Vertex> points;
  std::vector<int> indexes;

  Transformations transforms;
  ConstData constData;
  LightData lightData;

  TriangleWithTextureComponent(Game* gameInput) : GameComponent(gameInput) {
    layout = nullptr;
    vertexShader = nullptr;
    vertexByteCode = nullptr;
    vertexBuffer = nullptr;
    pixelShader = nullptr;
    pixelByteCode = nullptr;
    indexBuffer = nullptr;
    rastState = nullptr;
    constBuffer = nullptr;
  }

  void Initialize(
    LPCWSTR shaderSource,
    //std::vector<DirectX::XMFLOAT4> pointsInput,
    std::vector<Vertex> pointsInput,
    std::vector<int> indexesInput,
    std::vector<UINT> stridesInput,
    std::vector<UINT> offsetsInput
    // For working with textures:
    ,
    //bool is2DInput,
    std::wstring texturePath,
    Material* materialInput
  );

  void Draw();
  void Update();
  void DestroyResources();
};
