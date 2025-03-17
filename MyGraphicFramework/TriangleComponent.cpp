#include "TriangleComponent.h"
#include "Game.h"
#include <cmath>

void TriangleComponent::Initialize(
  LPCWSTR shaderSource,
  std::vector<DirectX::XMFLOAT4> pointsInput,
  std::vector<int> indexesInput,
  std::vector<UINT> stridesInput,
  std::vector<UINT> offsetsInput
) {
  points = pointsInput;
  indexes = indexesInput;
  strides = stridesInput;
  offsets = offsetsInput;

  // Compile vertex shader settings
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

  // Compile pixel shader settings
  D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

  ID3DBlob* errorPixelCode = nullptr;
  res = D3DCompileFromFile(
    shaderSource,
    //Shader_Macros,
    nullptr,
    nullptr,
    "PSMain",
    "ps_5_0",
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
    0,
    &pixelByteCode,
    &errorPixelCode
  );

  // Create shader
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

  // Describing elements positins and colors
  D3D11_INPUT_ELEMENT_DESC inputElements[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  game->device->CreateInputLayout(
    inputElements,
    2,
    vertexByteCode->GetBufferPointer(),
    vertexByteCode->GetBufferSize(),
    &layout
  );

  // Create vertex buffer
  D3D11_BUFFER_DESC vertexBufDesc = {};
  vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * points.size();
  D3D11_SUBRESOURCE_DATA vertexData = {};
  vertexData.pSysMem = points.data();
  game->device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

  // Create index buffer
  D3D11_BUFFER_DESC indexBufDesc = {};
  indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufDesc.ByteWidth = sizeof(int) * indexes.size();
  D3D11_SUBRESOURCE_DATA indexData = {};
  indexData.pSysMem = indexes.data();
  game->device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

  // Create rasterizer
  CD3D11_RASTERIZER_DESC rastDesc = {};
  rastDesc.CullMode = D3D11_CULL_FRONT;
  rastDesc.FillMode = D3D11_FILL_SOLID;
  res = game->device->CreateRasterizerState(&rastDesc, &rastState);

  // Create const buffer
  D3D11_BUFFER_DESC constBufferDesc = {};
  constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  UINT bufferSize = ((sizeof(ConstData) + 15) / 16) * 16;
  constBufferDesc.ByteWidth = bufferSize;
  game->device->CreateBuffer(&constBufferDesc, nullptr, &constBuffer);

  // Init default values for transforms
  transforms.move = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
  transforms.rotate = Matrix::CreateRotationY(0.0f);
  transforms.scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);

  // Init default values for constant buffer data
  constData.transformations = (transforms.scale * transforms.rotate * transforms.move).Transpose();
  constData.targetColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
  constData.time = 0.0f;
  constData.amplitude = 0.1f;
}

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

void TriangleComponent::Update() {
  constData.transformations = (transforms.scale * transforms.rotate * transforms.move).Transpose();
  constData.view = game->activeCamera->cameraMatrix.view.Transpose();
  constData.projection = game->activeCamera->cameraMatrix.projection.Transpose();
  constData.time = game->totalTime;
  constData.targetColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

  D3D11_MAPPED_SUBRESOURCE res = {};
  HRESULT hr = game->context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  if (SUCCEEDED(hr)) {
    memcpy(res.pData, &constData, sizeof(ConstData));
    game->context->Unmap(constBuffer, 0);
  }

  // Debug
  std::cout << constData.time << std::endl;
}

void TriangleComponent::DestroyResources() {
  if (layout) layout->Release();
  if (vertexShader) vertexShader->Release();
  if (vertexByteCode) vertexByteCode->Release();
  if (vertexBuffer) vertexBuffer->Release();
  if (pixelShader) pixelShader->Release();
  if (pixelByteCode) pixelByteCode->Release();
  if (indexBuffer) indexBuffer->Release();
  if (rastState) rastState->Release();
  if (constBuffer) constBuffer->Release();
}
