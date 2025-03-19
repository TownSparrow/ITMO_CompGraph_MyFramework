#include "TriangleWithTextureComponent.h"
#include "Game.h"
#include <cmath>

using namespace DirectX;

void TriangleWithTextureComponent::Initialize(
  LPCWSTR shaderSource,
  //std::vector<DirectX::XMFLOAT4> pointsInput,
  std::vector<Vertex> pointsInput,
  std::vector<int> indexesInput,
  std::vector<UINT> stridesInput,
  std::vector<UINT> offsetsInput,
  //bool is2DInput,
  std::wstring texturePath
) {
  points = pointsInput;
  indexes = indexesInput;
  strides = stridesInput;
  offsets = offsetsInput;
  //is2D = is2DInput;

  // Check what the shader is:
  if (shaderSource == L"./Shaders/TextureModifiedShader.hlsl") {
    shaderFileIndex = 2;
  }
  else if (shaderSource == L"./Shaders/TaskModifiedShader.hlsl") {
    shaderFileIndex = 1;
  }
  else {
    shaderFileIndex = 0;
  }

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
  D3D11_INPUT_ELEMENT_DESC InputElements[] = {
  D3D11_INPUT_ELEMENT_DESC {
    "POSITION",
    0,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    0,
    0,
    D3D11_INPUT_PER_VERTEX_DATA,
    0},
  D3D11_INPUT_ELEMENT_DESC {
    "TEXCOORD",
    0,
    DXGI_FORMAT_R32G32_FLOAT,
    0,
    D3D11_APPEND_ALIGNED_ELEMENT,
    D3D11_INPUT_PER_VERTEX_DATA,
    0}
  };

  game->device->CreateInputLayout(
    InputElements,
    2,
    vertexByteCode->GetBufferPointer(),
    vertexByteCode->GetBufferSize(),
    &layout);

  // Create vertex buffer
  D3D11_BUFFER_DESC vertexBufDesc = {};
  vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufDesc.CPUAccessFlags = 0;
  vertexBufDesc.MiscFlags = 0;
  vertexBufDesc.StructureByteStride = 0;
  //vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * points.size();
  vertexBufDesc.ByteWidth = sizeof(Vertex) * points.size();
  D3D11_SUBRESOURCE_DATA vertexData = {};
  vertexData.pSysMem = points.data();
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;
  game->device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

  // Create index buffer
  D3D11_BUFFER_DESC indexBufDesc = {};
  indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufDesc.CPUAccessFlags = 0;
  indexBufDesc.MiscFlags = 0;
  indexBufDesc.StructureByteStride = 0;
  indexBufDesc.ByteWidth = sizeof(int) * indexes.size();
  D3D11_SUBRESOURCE_DATA indexData = {};
  indexData.pSysMem = indexes.data();
  indexData.SysMemPitch = 0;
  indexData.SysMemSlicePitch = 0;
  game->device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

  // Create rasterizer
  CD3D11_RASTERIZER_DESC rastDesc = {};
  rastDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_FRONT;
  rastDesc.FillMode = D3D11_FILL_SOLID;
  res = game->device->CreateRasterizerState(&rastDesc, &rastState);

  // Create const buffer
  D3D11_BUFFER_DESC constBufferDesc = {};
  constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  constBufferDesc.MiscFlags = 0;
  constBufferDesc.StructureByteStride = 0;
  // constBufferDesc.ByteWidth = sizeof(ConstData);
  UINT bufferSize = ((sizeof(ConstData) + 15) / 16) * 16;
  constBufferDesc.ByteWidth = bufferSize;
  game->device->CreateBuffer(&constBufferDesc, nullptr, &constBuffer);

  // Init default values for transforms
  transforms = {};
  transforms.move = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
  transforms.rotate = Matrix::CreateRotationY(0.0f);
  transforms.scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);

  // Init default values for constant buffer data
  constData = {};
  constData.transformations = (transforms.scale * transforms.rotate * transforms.move).Transpose();
  constData.targetColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
  constData.time = 0.0f;
  constData.amplitude = 0.1f;

  // Create sampler
  D3D11_SAMPLER_DESC samplerDesc = {};
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  samplerState = nullptr;
  game->device->CreateSamplerState(&samplerDesc, &samplerState);

  // Create texture2D
  DirectX::ScratchImage image;
  DirectX::TexMetadata metadata;

  DirectX::LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_DEFAULT_SRGB, nullptr, image);
  metadata = image.GetMetadata();

  D3D11_TEXTURE2D_DESC textureDesc = {};
  textureDesc.Width = static_cast<UINT>(metadata.width);
  textureDesc.Height = static_cast<UINT>(metadata.height);
  textureDesc.MipLevels = static_cast<UINT>(metadata.mipLevels);
  textureDesc.ArraySize = 1;
  textureDesc.Format = metadata.format;
  textureDesc.SampleDesc.Count = 1;
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

  ID3D11Texture2D* texture2D = nullptr;
  D3D11_SUBRESOURCE_DATA textureData = {};
  textureData.pSysMem = image.GetPixels();
  textureData.SysMemPitch = static_cast<UINT>(image.GetImages()->rowPitch);

  res = game->device->CreateTexture2D(&textureDesc, &textureData, &texture2D);

  // Create shader resource view
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = textureDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

  res = game->device->CreateShaderResourceView(texture2D, &srvDesc, &textureView);
  texture2D->Release();
}

void TriangleWithTextureComponent::Draw() {
  game->context->RSSetState(rastState);
  game->context->IASetInputLayout(layout);
  game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  game->context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
  game->context->VSSetConstantBuffers(0, 1, &constBuffer);
  game->context->PSSetShaderResources(0, 1, &textureView);
  game->context->PSGetSamplers(0, 1, &samplerState);
  game->context->IASetVertexBuffers(0, 1, &vertexBuffer, strides.data(), offsets.data());
  game->context->VSSetShader(vertexShader, nullptr, 0);
  game->context->PSSetShader(pixelShader, nullptr, 0);
  game->context->DrawIndexed(indexes.size(), 0, 0);
}

void TriangleWithTextureComponent::Update() {
  constData.transformations = (transforms.scale * transforms.rotate * transforms.move).Transpose();
  constData.view = game->activeCamera->cameraMatrix.view.Transpose();
  constData.projection = game->activeCamera->cameraMatrix.projection.Transpose();
  constData.time = game->totalTime;

  if (shaderFileIndex == 1) {
    constData.targetColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  D3D11_MAPPED_SUBRESOURCE res = {};
  HRESULT hr = game->context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  if (SUCCEEDED(hr)) {
    memcpy(res.pData, &constData, sizeof(ConstData));
    game->context->Unmap(constBuffer, 0);
  }

  // Debug
  if (shaderFileIndex == 1) {
    std::cout << constData.time << std::endl;
  }
}

void TriangleWithTextureComponent::DestroyResources() {
  if (layout) layout->Release();
  if (textureView) textureView->Release();
  if (samplerState) samplerState->Release();
  if (vertexShader) vertexShader->Release();
  if (vertexByteCode) vertexByteCode->Release();
  if (vertexBuffer) vertexBuffer->Release();
  if (pixelShader) pixelShader->Release();
  if (pixelByteCode) pixelByteCode->Release();
  if (indexBuffer) indexBuffer->Release();
  if (rastState) rastState->Release();
  if (constBuffer) constBuffer->Release();
}
