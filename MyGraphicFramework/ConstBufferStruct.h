#pragma once

#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include <SimpleMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "LightSystem.h"

using namespace DirectX::SimpleMath;

struct ConstData {
  Matrix transformations;
  Matrix view;
  Matrix projection;
  Vector4 targetColor;
  float time;
  float amplitude;
};

struct Transformations {
  Matrix move;
  Matrix rotate;
  Matrix scale;
};

struct Material {
  Vector4 ambient;
  Vector4 diffuse;
  Vector4 specular;
};

struct LightData {
  Material material;
  DirectionalLight directional;
  PointLight point;
  Vector4 spectator;
};