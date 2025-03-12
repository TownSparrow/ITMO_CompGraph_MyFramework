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

  // Cube
  Mesh Cube(
    DirectX::XMFLOAT3 center,
    float size,
    bool isGradient,
    const std::vector<DirectX::XMFLOAT4>& colors
  );

  // Sphere
  Mesh Sphere(
    DirectX::XMFLOAT3 center,
    float radius,
    int slices,
    int stacks,
    bool isGradient,
    const std::vector<DirectX::XMFLOAT4>& colors
  );

  // XZ-Grid
  Mesh GridXZCentered(
    float gridWidth,              // Полная ширина сетки (например, 20)
    float gridDepth,              // Полная глубина сетки (например, 20)
    int numDivisionsX,            // Число делений по оси X (вертикальные линии: numDivisionsX+1)
    int numDivisionsZ,            // Число делений по оси Z (горизонтальные линии: numDivisionsZ+1)
    bool isGradient,              // Если true – использовать градиент, иначе один цвет
    const std::vector<DirectX::XMFLOAT4>& colors   // Массив цветов (минимум 1 элемент)
  );
};
