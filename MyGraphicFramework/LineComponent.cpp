#include "LineComponent.h"
#include "Game.h"

// --- Init --- //
void LineComponent::Initialize(
  LPCWSTR shaderSource,
  std::vector<DirectX::XMFLOAT4> pointsInput,
  //std::vector<int> indexesInput,
  std::vector<UINT> stridesInput,
  std::vector<UINT> offsetsInput
) {
  points = pointsInput;
  //indexes = indexesInput;
  strides = stridesInput;
  offsets = offsetsInput;

  if (shaderSource == L"./Shaders/TaskModifiedShader.hlsl") {
    shaderFileIndex = 1;
  }
  else {
    shaderFileIndex = 0;
  }

  // Compiling vertex shader
  ID3DBlob* errorVertexCode = nullptr;
  HRESULT res = D3DCompileFromFile(
    shaderSource,
    nullptr,
    nullptr,
    "VSMain",
    "vs_5_0",
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
    0,
    &vertexByteCode,
    &errorVertexCode
  );

  // Compliling pixel shader
  D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
  ID3DBlob* errorPixelCode = nullptr;
  res = D3DCompileFromFile(
    shaderSource,
    nullptr,
    nullptr,
    "PSMain",
    "ps_5_0",
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
    0,
    &pixelByteCode,
    &errorPixelCode
  );

  // Create Shaders
  game->device->CreateVertexShader(
    vertexByteCode->GetBufferPointer(),
    vertexByteCode->GetBufferSize(),
    nullptr,
    &vertexShader
  );
  game->device->CreatePixelShader(
    pixelByteCode->GetBufferPointer(),
    pixelByteCode->GetBufferSize(),
    nullptr,
    &pixelShader
  );

  // Input Layout
  D3D11_INPUT_ELEMENT_DESC inputElements[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,                           D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  game->device->CreateInputLayout(
    inputElements,
    2,
    vertexByteCode->GetBufferPointer(),
    vertexByteCode->GetBufferSize(),
    &layout
  );

  // Vertex buffer
  D3D11_BUFFER_DESC vertexBufDesc = {};
  vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * points.size();
  D3D11_SUBRESOURCE_DATA vertexData = {};
  vertexData.pSysMem = points.data();
  game->device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

  // If using indexes:
  //if (!indexes.empty()) {
  //  D3D11_BUFFER_DESC indexBufDesc = {};
  //  indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
  //  indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  //  indexBufDesc.ByteWidth = sizeof(int) * indexes.size();
  //  D3D11_SUBRESOURCE_DATA indexData = {};
  //  indexData.pSysMem = indexes.data();
  //  //game->device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);
  //  game->device->CreateBuffer(&indexBufDesc, &indexData, nullptr);
  //}

  // Rasterizer
  CD3D11_RASTERIZER_DESC rastDesc = {};
  rastDesc.CullMode = D3D11_CULL_FRONT;
  rastDesc.FillMode = D3D11_FILL_SOLID;
  res = game->device->CreateRasterizerState(&rastDesc, &rastState);

  // Constant buffer
  D3D11_BUFFER_DESC constBufferDesc = {};
  constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  UINT bufferSize = ((sizeof(ConstData) + 15) / 16) * 16;
  constBufferDesc.ByteWidth = bufferSize;
  game->device->CreateBuffer(&constBufferDesc, nullptr, &constBuffer);

  // Default values for matrix
  transforms.move = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
  transforms.rotate = Matrix::CreateRotationY(0.0f);
  transforms.scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);

  // Init default values for constant buffer
  constData.transformations = (transforms.scale * transforms.rotate * transforms.move).Transpose();
  if (shaderFileIndex == 0) {
    constData.targetColor = Vector4(0.2f, 0.2f, 0.2f, 0.2f);
  }
  else {
    constData.targetColor = Vector4(1.0f, 0.2f, 0.2f, 0.5f);
  }
  constData.time = 0.0f;
  constData.amplitude = 0.1f;
}

// --- Draw --- //
void LineComponent::Draw() {
  game->context->RSSetState(rastState);
  game->context->IASetInputLayout(layout);
  game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
  game->context->VSSetConstantBuffers(0, 1, &constBuffer);
  game->context->IASetVertexBuffers(0, 1, &vertexBuffer, strides.data(), offsets.data());
  game->context->VSSetShader(vertexShader, nullptr, 0);
  game->context->PSSetShader(pixelShader, nullptr, 0);
  game->context->Draw(static_cast<UINT>(points.size() / 2), 0);
}

// --- Update --- //
void LineComponent::Update() {
  constData.transformations = (transforms.scale * transforms.rotate * transforms.move).Transpose();
  constData.view = game->activeCamera->cameraMatrix.view.Transpose();
  constData.projection = game->activeCamera->cameraMatrix.projection.Transpose();
  constData.time = game->totalTime;

  if (shaderFileIndex == 1) {
    constData.targetColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  }

  D3D11_MAPPED_SUBRESOURCE mappedResource = {};
  HRESULT hr = game->context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (SUCCEEDED(hr)) {
    memcpy(mappedResource.pData, &constData, sizeof(ConstData));
    game->context->Unmap(constBuffer, 0);
  }
}

// --- Destroy Resources --- //
void LineComponent::DestroyResources() {
  if (layout) layout->Release();
  if (vertexShader) vertexShader->Release();
  if (vertexByteCode) vertexByteCode->Release();
  if (vertexBuffer) vertexBuffer->Release();
  if (pixelShader) pixelShader->Release();
  if (pixelByteCode) pixelByteCode->Release();
  //if (indexBuffer) indexBuffer->Release();
  if (rastState) rastState->Release();
  if (constBuffer) constBuffer->Release();
}
