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

using namespace DirectX::SimpleMath;

struct ConstData {
  Matrix transformations;  // итоговая матрица world (транспонированная)
  Matrix view;             // матрица вида (транспонированная)
  Matrix projection;       // матрица проекции (транспонированная)
  Vector4 targetColor; // базовый цвет (например, для смешивания)
  float time;              // общее время, используемое для пульсации
  float amplitude;         // амплитуда пульсации
};

struct Transformations {
  Matrix move;
  Matrix rotate;
  Matrix scale;
};

class TriangleComponent : public GameComponent {
private:
  ID3D11InputLayout* layout;
  ID3D11VertexShader* vertexShader;
  ID3DBlob* vertexByteCode;
  ID3D11Buffer* vertexBuffer;
  ID3D11PixelShader* pixelShader;
  ID3DBlob* pixelByteCode;
  ID3D11Buffer* indexBuffer;
  ID3D11RasterizerState* rastState;
  ID3D11Buffer* constBuffer; // константный буфер для объекта

  std::vector<UINT> strides;
  std::vector<UINT> offsets;

public:
  std::vector<DirectX::XMFLOAT4> points;
  std::vector<int> indexes;

  Transformations transforms;
  ConstData constData;

  TriangleComponent(Game* gameInput) : GameComponent(gameInput) {
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

  // Инициализация с загрузкой шейдеров, вершинного и индексного буфера
  void Initialize(
    LPCWSTR shaderSource,
    std::vector<DirectX::XMFLOAT4> pointsInput,
    std::vector<int> indexesInput,
    std::vector<UINT> stridesInput,
    std::vector<UINT> offsetsInput
  );

  // Отрисовка объекта
  void Draw();

  // Обновление константного буфера – сюда включён и параметр времени для пульсации
  void Update();

  // Освобождение ресурсов
  void DestroyResources();
};
