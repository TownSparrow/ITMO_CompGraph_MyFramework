#include "TriangleWithTextureComponent.h"
#include "Game.h"
#include <cmath>

using namespace DirectX;

// --- Create Shadow Shaders --- //
void TriangleWithTextureComponent::CreateShadowShaders() {
  ID3DBlob* errorVertexCode = nullptr;
  HRESULT res = D3DCompileFromFile(L"./Shaders/ShadowMapShader.hlsl",
    nullptr /*macros*/,
    nullptr /*include*/,
    "VSMain",
    "vs_5_0",
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
    0,
    &vertexByteCode_shadows,
    &errorVertexCode);
  game->device->CreateVertexShader(
    vertexByteCode_shadows->GetBufferPointer(),
    vertexByteCode_shadows->GetBufferSize(),
    nullptr, &vertexShader_shadows);


  ID3DBlob* errorPixelCode = nullptr;
  res = D3DCompileFromFile(L"./Shaders/ShadowMapShader.hlsl",
    nullptr /*macros*/,
    nullptr /*include*/,
    "PSMain",
    "ps_5_0",
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
    0,
    &pixelByteCode_shadows,
    &errorPixelCode);
  game->device->CreatePixelShader(
    pixelByteCode_shadows->GetBufferPointer(),
    pixelByteCode_shadows->GetBufferSize(),
    nullptr, &pixelShader_shadows);

  D3D11_SAMPLER_DESC shadowSamplerDesc;
  ZeroMemory(&shadowSamplerDesc, sizeof(shadowSamplerDesc));

  shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
  shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
  shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
  shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
  shadowSamplerDesc.BorderColor[0] = 0;
  shadowSamplerDesc.BorderColor[1] = 0;
  shadowSamplerDesc.BorderColor[2] = 0;
  shadowSamplerDesc.BorderColor[3] = 0;
  shadowSamplerDesc.MinLOD = 0;
  shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
  shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
  shadowSamplerDesc.MaxAnisotropy = 1;
  shadowSamplerDesc.MipLODBias = 0.0f;

  game->device->CreateSamplerState(&shadowSamplerDesc, &shadowSampler);

  CD3D11_RASTERIZER_DESC rastDesc = {};
  //rastDesc.CullMode = D3D11_CULL_BACK;
  rastDesc.CullMode = D3D11_CULL_BACK;
  rastDesc.FillMode = D3D11_FILL_SOLID /*D3D11_FILL_WIREFRAME*/;
  rastDesc.DepthBias = 10000;
  rastDesc.SlopeScaledDepthBias = 1.0f;
  rastDesc.DepthBiasClamp = 0.0f;

  res = game->device->CreateRasterizerState(&rastDesc, &rastState_shadows);
}

// --- Create Shadow Volumes (Stencil Shadow) --- //
void TriangleWithTextureComponent::CreateShadowVolumesShaders() {
  ID3DBlob* errorVertexCode = nullptr;
  HRESULT res = D3DCompileFromFile(L"./Shaders/ShadowVolumeShader.hlsl",
    nullptr /*macros*/,
    nullptr /*include*/,
    "VSMain",
    "vs_5_0",
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
    0,
    &vertexByteCode_shadowVolumes,
    &errorVertexCode);
  game->device->CreateVertexShader(
    vertexByteCode_shadowVolumes->GetBufferPointer(),
    vertexByteCode_shadowVolumes->GetBufferSize(),
    nullptr, &vertexShader_shadowVolumes);


  ID3DBlob* errorGeometryCode = nullptr;
  res = D3DCompileFromFile(L"./Shaders/ShadowVolumeShader.hlsl",
    nullptr /*macros*/,
    nullptr /*include*/,
    "GSMain",
    "gs_5_0",
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
    0,
    &geometryByteCode_shadowVolumes,
    &errorGeometryCode);
  game->device->CreateGeometryShader(
    geometryByteCode_shadowVolumes->GetBufferPointer(),
    geometryByteCode_shadowVolumes->GetBufferSize(),
    nullptr, &geometryShader_shadowVolumes);

  D3D11_BUFFER_DESC ibDesc = {};
  ibDesc.ByteWidth = sizeof(UINT) * indeces_with_adjastency.size();
  ibDesc.Usage = D3D11_USAGE_DEFAULT;
  ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

  D3D11_SUBRESOURCE_DATA initData = {};
  initData.pSysMem = &indeces_with_adjastency[0];

  game->device->CreateBuffer(&ibDesc, &initData, &pAdjacencyIB);

  D3D11_INPUT_ELEMENT_DESC layoutDescVol[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };
  game->device->CreateInputLayout(
    layoutDescVol, ARRAYSIZE(layoutDescVol),
    vertexByteCode_shadowVolumes->GetBufferPointer(),
    vertexByteCode_shadowVolumes->GetBufferSize(),
    &layoutShadowVolumes
  );
}

// --- Create Shadow Volumes --- //
void TriangleWithTextureComponent::CreateShadowVolumes() {
  game->context->RSSetState(rastState);

  game->context->IASetInputLayout(layout);
  game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ);
  game->context->IASetIndexBuffer(pAdjacencyIB, DXGI_FORMAT_R32_UINT, 0);

  game->context->VSSetConstantBuffers(0, 1, &constBuffer);
  game->context->GSSetConstantBuffers(0, 1, &constBuffer);
  game->context->GSSetConstantBuffers(1, 1, &lightBuffer);

  game->context->IASetVertexBuffers(0, 1, &vertexBuffer, strides.data(), offsets.data());
  game->context->VSSetShader(vertexShader_shadowVolumes, nullptr, 0);
  game->context->GSSetShader(geometryShader_shadowVolumes, nullptr, 0);
  game->context->PSSetShader(nullptr, nullptr, 0);

  game->context->DrawIndexed(indeces_with_adjastency.size(), 0, 0);
}

// --- Render Shadow Volumes --- //
void TriangleWithTextureComponent::RenderShadowVolume() {
  game->context->IASetInputLayout(layoutShadowVolumes);
  game->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ);
  game->context->IASetVertexBuffers(0, 1, &vertexBuffer, strides.data(), offsets.data());
  game->context->IASetIndexBuffer(pAdjacencyIB, DXGI_FORMAT_R32_UINT, 0);
  game->context->VSSetConstantBuffers(0, 1, &constBuffer);
  game->context->GSSetConstantBuffers(0, 1, &constBuffer);
  game->context->GSSetConstantBuffers(1, 1, &lightBuffer);
  game->context->VSSetShader(vertexShader_shadowVolumes, nullptr, 0);
  game->context->GSSetShader(geometryShader_shadowVolumes, nullptr, 0);
  game->context->PSSetShader(nullptr, nullptr, 0);
  game->context->DrawIndexed((UINT)indeces_with_adjastency.size(), 0, 0);
  game->context->GSSetShader(nullptr, nullptr, 0);
}

// --- Init --- //
void TriangleWithTextureComponent::Initialize(
  LPCWSTR shaderSource,
  std::vector<Vertex> pointsInput,
  std::vector<int> indexesInput,
  std::vector<UINT> stridesInput,
  std::vector<UINT> offsetsInput,
  std::wstring texturePath,
  Material* materialInput,
  bool isTransparentGot
) {

  points = pointsInput;
  indexes = indexesInput;
  isTransparent = isTransparentGot;
  
  // Check strides and set default values in empty case
  if (stridesInput.empty())
    strides = { 36 };
  else
    strides = stridesInput;
  if (offsetsInput.empty())
    offsets = { 0 };
  else
    offsets = offsetsInput;

  material = materialInput;

  // Find what the shader is (useful for additional tasks)
  if (shaderSource == L"./Shaders/TextureModifiedShader.hlsl") {
    shaderFileIndex = 2;
  }
  else if (shaderSource == L"./Shaders/TaskModifiedShader.hlsl") {
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
  if (FAILED(res)) {
    std::cout << "Compiling vertex shader error" << std::endl;
  }

  // Compiling pixel shader
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
  if (FAILED(res)) {
    std::cout << "Compiling pixel shader error" << std::endl;
  }

  // Creating shaders
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

  // Create input elements
  D3D11_INPUT_ELEMENT_DESC InputElements[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,                          D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };

  res = game->device->CreateInputLayout(
    InputElements,
    3,
    vertexByteCode->GetBufferPointer(),
    vertexByteCode->GetBufferSize(),
    &layout
  );

  // Vertex buffer
  D3D11_BUFFER_DESC vertexBufDesc = {};
  vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufDesc.CPUAccessFlags = 0;
  vertexBufDesc.MiscFlags = 0;
  vertexBufDesc.StructureByteStride = 0;
  vertexBufDesc.ByteWidth = sizeof(Vertex) * points.size();
  D3D11_SUBRESOURCE_DATA vertexData = {};
  vertexData.pSysMem = points.data();
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;
  res = game->device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

  // Index buffer
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
  res = game->device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

  // Rasterizer
  // For non-transparent
  //if (!isTransparent) {
  //  CD3D11_RASTERIZER_DESC rastDesc = {};
  //  rastDesc.CullMode = D3D11_CULL_NONE;
  //  rastDesc.FillMode = D3D11_FILL_SOLID;
  //  res = game->device->CreateRasterizerState(&rastDesc, &rastState);
  //}
  CD3D11_RASTERIZER_DESC rastDesc = {};
  //rastDesc.CullMode = D3D11_CULL_NONE;
  rastDesc.CullMode = D3D11_CULL_BACK;
  rastDesc.FillMode = D3D11_FILL_SOLID;
  res = game->device->CreateRasterizerState(&rastDesc, &rastState);
  
  // For transparent
  //if (isTransparent) {
  //  D3D11_RASTERIZER_DESC transparentRasterDesc = {};
  //  transparentRasterDesc.FillMode = D3D11_FILL_SOLID;
  //  transparentRasterDesc.CullMode = D3D11_CULL_NONE;
  //  transparentRasterDesc.DepthClipEnable = true;
  //  transparentRasterDesc.DepthBias = -1;
  //  transparentRasterDesc.DepthBiasClamp = 0.0f;
  //  transparentRasterDesc.SlopeScaledDepthBias = 0.0f;

  //  transparentRasterState = nullptr;
  //  res = game->device->CreateRasterizerState(&transparentRasterDesc, &transparentRasterState);
  //  if (FAILED(res)) {
  //    std::cout << "Rasterizing transparent objects error" << std::endl;
  //  }
  //}
  D3D11_RASTERIZER_DESC transparentRasterDesc = {};
  transparentRasterDesc.FillMode = D3D11_FILL_SOLID;
  transparentRasterDesc.CullMode = D3D11_CULL_NONE;
  transparentRasterDesc.DepthClipEnable = true;
  transparentRasterDesc.DepthBias = -1;
  transparentRasterDesc.DepthBiasClamp = 0.0f;
  transparentRasterDesc.SlopeScaledDepthBias = 0.0f;

  transparentRasterState = nullptr;
  res = game->device->CreateRasterizerState(&transparentRasterDesc, &transparentRasterState);
  if (FAILED(res)) {
    std::cout << "Rasterizing transparent objects error" << std::endl;
  }

  // Constant buffer
  D3D11_BUFFER_DESC constBufferDesc = {};
  constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  constBufferDesc.MiscFlags = 0;
  constBufferDesc.StructureByteStride = 0;
  //UINT bufferSize = ((sizeof(ConstData) + 15) / 16) * 16;
  UINT bufferSize = ceil(sizeof(ConstData) / 16) * 16;
  constBufferDesc.ByteWidth = bufferSize;
  res = game->device->CreateBuffer(&constBufferDesc, nullptr, &constBuffer);

  // Init transforms default values
  transforms = {};
  transforms.move = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
  transforms.rotate = Matrix::CreateRotationY(0.0f);
  transforms.scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);

  // Set values for constant buffer
  constData = {};
  constData.transformations = (transforms.scale * transforms.rotate * transforms.move).Transpose();
  constData.targetColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  constData.time = 0.0f;
  constData.amplitude = 0.1f;

  // Light buffer
  D3D11_BUFFER_DESC lightBufferDesc = {};
  lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  lightBufferDesc.MiscFlags = 0;
  lightBufferDesc.StructureByteStride = 0;
  lightBufferDesc.ByteWidth = sizeof(LightData);
  //UINT lightBufferSize = ((sizeof(LightData) + 15) / 16) * 16;
  //lightBufferDesc.ByteWidth = lightBufferSize;
  res = game->device->CreateBuffer(&lightBufferDesc, nullptr, &lightBuffer);

  lightData = {};
  //std::cout << "Size of LightData: " << sizeof(LightData) << " bytes" << std::endl;

  // Sampler
  D3D11_SAMPLER_DESC samplerDesc = {};
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
  samplerState = nullptr;
  res = game->device->CreateSamplerState(&samplerDesc, &samplerState);

  // Texture loading
  DirectX::ScratchImage image;
  DirectX::TexMetadata metadata;
  res = DirectX::LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_DEFAULT_SRGB, nullptr, image);
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

  // Shader Resource View for the textures
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = textureDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
  res = game->device->CreateShaderResourceView(texture2D, &srvDesc, &textureView);
  texture2D->Release();

  // Depth
  // For non-transparent
  D3D11_DEPTH_STENCIL_DESC normalDepthDesc = {};
  normalDepthDesc.DepthEnable = true;
  normalDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  normalDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
  normalDepthState = nullptr;
  game->device->CreateDepthStencilState(&normalDepthDesc, &normalDepthState);

  // For transparent
  D3D11_DEPTH_STENCIL_DESC transparentDepthDesc = {};
  transparentDepthDesc.DepthEnable = true;
  transparentDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  transparentDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
  
  transparentDepthState = nullptr;
  game->device->CreateDepthStencilState(&transparentDepthDesc, &transparentDepthState);

  // Blend state (for transparent materials)
  D3D11_BLEND_DESC blendDesc = {};
  blendDesc.AlphaToCoverageEnable = false;
  blendDesc.IndependentBlendEnable = false;
  blendDesc.RenderTarget[0].BlendEnable = TRUE;
  blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; // D3D11_BLEND_ZERO by default
  blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  blendState = nullptr;
  HRESULT hr = game->device->CreateBlendState(&blendDesc, &blendState);
  if (FAILED(hr)) {
    // Error
    std::cout << "Error of blend state" << std::endl;
  }

  //FOR SHADOW VOLUMES
  indeces_with_adjastency = MeshCreator::GetInstance()->GenerateAdjastencyIndices(points, indexes);
  res = D3DCompileFromFile(L"./Shaders/MeshesWithoutLightsShader.hlsl",
    nullptr,
    nullptr,
    "PSMain",
    "ps_5_0",
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
    0,
    &pixelByteCode_withoutLights,
    &errorPixelCode);

  game->device->CreatePixelShader(
    pixelByteCode_withoutLights->GetBufferPointer(),
    pixelByteCode_withoutLights->GetBufferSize(),
    nullptr, &pixelShader_withoutLights);
}

void TriangleWithTextureComponent::Draw() {
  float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
  game->context->OMSetBlendState(blendState, blendFactor, 0xffffffff);
  
  // Check is transparent for correct depth state
  if (isTransparent) {
    game->context->OMSetDepthStencilState(transparentDepthState, 0);
    game->context->RSSetState(transparentRasterState);
  }
  else {
    game->context->OMSetDepthStencilState(normalDepthState, 0);
    game->context->RSSetState(rastState);
  }
  
  game->context->IASetInputLayout(layout);
  game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  game->context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
  game->context->VSSetConstantBuffers(0, 1, &constBuffer);
  game->context->PSSetConstantBuffers(1, 1, &lightBuffer);
  game->context->PSSetShaderResources(0, 1, &textureView);
  game->context->PSSetSamplers(0, 1, &samplerState);
  if (!(game->isStencilShadowEnabled)) {
    shadowsResource = game->dirLightShadows->GetShadowMapDSV();
  }
  game->context->PSSetShaderResources(1, 1, &shadowsResource);
  game->context->PSSetSamplers(1, 1, &shadowSampler);
  game->context->IASetVertexBuffers(0, 1, &vertexBuffer, strides.data(), offsets.data());
  game->context->VSSetShader(vertexShader, nullptr, 0);
  game->context->GSSetShader(nullptr, nullptr, 0);
  game->context->PSSetShader(pixelShader, nullptr, 0);
  game->context->DrawIndexed(indexes.size(), 0, 0);
}

void TriangleWithTextureComponent::LightRender() {
  game->context->RSSetState(rastState_shadows);
  game->context->IASetInputLayout(layout);
  game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  game->context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
  game->context->VSSetConstantBuffers(0, 1, &constBuffer);
  game->context->IASetVertexBuffers(0, 1, &vertexBuffer, strides.data(), offsets.data());
  game->context->VSSetShader(vertexShader_shadows, nullptr, 0);
  game->context->PSSetShader(pixelShader_shadows, nullptr, 0);
  game->context->DrawIndexed(indexes.size(), 0, 0);
}

void TriangleWithTextureComponent::Update() {
  constData.transformations = (transforms.scale * transforms.rotate * transforms.move).Transpose();
  constData.view = game->activeCamera->cameraMatrix.view.Transpose();
  constData.projection = game->activeCamera->cameraMatrix.projection.Transpose();
  constData.time = game->totalTime;

  if (shaderFileIndex == 2) {
    constData.targetColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  if (shaderFileIndex == 1) {
    constData.targetColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  D3D11_MAPPED_SUBRESOURCE res = {};
  HRESULT hr = game->context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  if (SUCCEEDED(hr)) {
    memcpy(res.pData, &constData, sizeof(ConstData));
    game->context->Unmap(constBuffer, 0);
  }

  // Update light info
  if (game->directionalLight != nullptr)
    lightData.directional = *(game->directionalLight);
  
  //if (game->pointLight != nullptr)
  //  lightData.point = *(game->pointLight);
  
  //if (!game->pointLights.empty()) {
  //  lightData.numPointLights = static_cast<int>(game->pointLights.size());
  //  for (int i = 0; i < lightData.numPointLights && i < 8; i++) {
  //    lightData.pointLights[i] = *(game->pointLights[i]);
  //  }
  //}
  if (game->pointLights.size() != 0) {
    for (int i = 0; i < game->pointLights.size(); i++) {
      lightData.pointLights[i] = *(game->pointLights[i]);
    }
  }

  lightData.material = *material;
  lightData.spectator = Vector4(
    game->activeCamera->cameraPosition.x,
    game->activeCamera->cameraPosition.y,
    game->activeCamera->cameraPosition.z,
    1.0f
  );

  lightData.lightSpace = game->lightView * game->lightProjection;
  lightData.lightSpace = lightData.lightSpace.Transpose();

  D3D11_MAPPED_SUBRESOURCE resLight = {};
  game->context->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resLight);
  memcpy(resLight.pData, &lightData, sizeof(LightData));
  game->context->Unmap(lightBuffer, 0);

  // Debug
  if (shaderFileIndex == 1) {
    std::cout << constData.time << std::endl;
  }
}

void TriangleWithTextureComponent::LightUpdate() {
  constData.transformations = transforms.scale * transforms.rotate * transforms.move;
  constData.transformations = constData.transformations.Transpose();

  constData.view = game->lightView;
  constData.view = constData.view.Transpose();

  constData.projection = game->lightProjection;
  constData.projection = constData.projection.Transpose();

  D3D11_MAPPED_SUBRESOURCE res = {};
  game->context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  memcpy(res.pData, &constData, sizeof(ConstData));
  game->context->Unmap(constBuffer, 0);
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
  if (material) delete material;
}
