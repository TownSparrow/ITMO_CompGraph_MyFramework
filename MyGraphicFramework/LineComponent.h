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

using namespace DirectX::SimpleMath;

class LineComponent : public GameComponent {
private:
  int shaderFileIndex;

  ID3D11InputLayout* layout;
  ID3D11VertexShader* vertexShader;
  ID3DBlob* vertexByteCode;
  ID3D11Buffer* vertexBuffer;
  ID3D11PixelShader* pixelShader;
  ID3DBlob* pixelByteCode;
  //ID3D11Buffer* indexBuffer;
  ID3D11RasterizerState* rastState;
  ID3D11Buffer* constBuffer;

  std::vector<UINT> strides;
  std::vector<UINT> offsets;

public:
  std::vector<DirectX::XMFLOAT4> points;
  std::vector<int> indexes;

  Transformations transforms;
  ConstData constData;

  LineComponent(Game* gameInput) : GameComponent(gameInput) {
    layout = nullptr;
    vertexShader = nullptr;
    vertexByteCode = nullptr;
    vertexBuffer = nullptr;
    pixelShader = nullptr;
    pixelByteCode = nullptr;
    //indexBuffer = nullptr;
    rastState = nullptr;
    constBuffer = nullptr;
  }

  void Initialize(
    LPCWSTR shaderSource,
    std::vector<DirectX::XMFLOAT4> pointsInput,
    //std::vector<int> indexesInput,
    std::vector<UINT> stridesInput,
    std::vector<UINT> offsetsInput
  );

  void Draw();
  void Update();
  void DestroyResources();
};
