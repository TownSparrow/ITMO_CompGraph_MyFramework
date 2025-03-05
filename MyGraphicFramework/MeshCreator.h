#pragma once

#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <vector>

struct Mesh {
  std::vector<DirectX::XMFLOAT4> points;
  std::vector<int> indexes;
};

class MeshCreator {
private:
  static MeshCreator* instance;
  MeshCreator() {};

public:
  static MeshCreator* GetInstance() {
    if (instance == nullptr) {
      instance = new MeshCreator();
    }
    return instance;
  }

  // All important setting of each mesh:
  // 1) center - Main center pivot of mesh
  // 2) width - Width of mesh
  // 3) height - Height of mesh
  // 4) isGradient - Checks is there gradient color in the mesh
  // 5) colors - Array of colors (has to contain 1 element at least!)

  // Square mesh
  Mesh Square(
    DirectX::XMFLOAT2 center,                     
    float width,                                  
    float height,                                 
    bool isGradient,                             
    const std::vector<DirectX::XMFLOAT4>& colors  
  );

  // Star mesh
  Mesh Star(
    DirectX::XMFLOAT2 center,
    float width,
    float height,
    bool isGradient,
    const std::vector<DirectX::XMFLOAT4>& colors
  );

  // Circle
  Mesh Circle(
    DirectX::XMFLOAT2 center,
    float radius,
    int segments,
    bool isGradient,
    const std::vector<DirectX::XMFLOAT4>& colors
  );

  // Oval (modified circle)
  Mesh Oval(
    DirectX::XMFLOAT2 center,
    float width,
    float height,
    int segments,
    bool isGradient,
    const std::vector<DirectX::XMFLOAT4>& colors
  );
};
