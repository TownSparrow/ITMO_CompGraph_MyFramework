#include "MeshCreator.h"

MeshCreator* MeshCreator::instance = nullptr;

// -------------------------------------------------------- //
// -- ADDITIONAL FUNCTIONS -- //


// --- Pick Color by Gradient Checking --- //
DirectX::XMFLOAT4 pickColor(
  bool isGradient,
  const std::vector<DirectX::XMFLOAT4>& colors,
  int i,
  int vertexCount
) {
  // If is not gradient
  if (!isGradient) {
    // Check array
    // If array is not empty - Set all points with only one color
    if (!colors.empty()) {
      return colors[0];
    }
    // Else set white color for all points
    else {
      return DirectX::XMFLOAT4(1, 1, 1, 1);
    }
  }
  // But if is the gradient
  else {
    // Check if array is less then points
    if (i < (int)colors.size()) {
      // For last verticles without setted colors take the last color from array
      return colors[i];
    }
    // Else just color each element with each color
    else if (!colors.empty()) {
      return colors.back();
    }
    // Or just set default color
    else {
      return DirectX::XMFLOAT4(1, 1, 1, 1);
    }
  }
}

// -------------------------------------------------------- //
// -- MAIN FUNCTIONS -- //

// --- Square --- //
Mesh MeshCreator::Square(
  DirectX::XMFLOAT2 center,
  float width,
  float height,
  bool isGradient,
  const std::vector<DirectX::XMFLOAT4>& colors
) {
  // Default coordinates in the start
  DirectX::XMFLOAT4 baseCoords[4] = {
      { +0.5f, +0.5f, 0.5f, 1.0f},
      { -0.5f, -0.5f, 0.5f, 1.0f},
      { +0.5f, -0.5f, 0.5f, 1.0f},
      { -0.5f, +0.5f, 0.5f, 1.0f},
  };

  // Setting points with colors (4 default verticles with each color: 4x2)
  std::vector<DirectX::XMFLOAT4> points;
  points.reserve(4 * 2);

  // Transform every verticle
  for (int i = 0; i < 4; i++) {
    // Scale every verticle by x and y axis
    float scaledX = baseCoords[i].x * width;
    float scaledY = baseCoords[i].y * height;

    // Set by distance of center
    float finalX = scaledX + center.x;
    float finalY = scaledY + center.y;

    // Set color
    DirectX::XMFLOAT4 c = pickColor(isGradient, colors, i, 4);

    // Push data about position and color
    points.push_back(DirectX::XMFLOAT4(finalX, finalY, baseCoords[i].z, baseCoords[i].w));
    points.push_back(c);
  }

  // Set the 2 triangles witdh indexes
  std::vector<int> indexes = { 0,1,2, 1,0,3 };

  // Result
  Mesh result = { points, indexes };
  return result;
}

// --- Star --- //
Mesh MeshCreator::Star(
  DirectX::XMFLOAT2 center,
  float width,
  float height,
  bool isGradient,
  const std::vector<DirectX::XMFLOAT4>& colors
) {
  // Default coordinates in the start
  DirectX::XMFLOAT4 baseCoords[10] = {
      {  0.0f,  0.7f, 1.0f, 1.0f},
      {  0.15f, 0.25f, 1.0f, 1.0f},
      {  0.6f,  0.25f, 1.0f, 1.0f},
      {  0.22f, 0.0f, 1.0f, 1.0f},
      {  0.365f, -0.45f, 1.0f, 1.0f},
      {  0.0f,  -0.15f, 1.0f, 1.0f},
      { -0.365f, -0.45f, 1.0f, 1.0f},
      { -0.22f,  0.0f, 1.0f, 1.0f},
      { -0.6f,   0.25f, 1.0f, 1.0f},
      { -0.15f,  0.25f, 1.0f, 1.0f},
  };

  // Indexes for the stars
  std::vector<int> indexes = {
      0,1,9,
      1,2,3,
      3,4,5,
      5,6,7,
      7,8,9,
      1,5,9,
      1,3,5,
      9,5,7
  };

  // Making points
  std::vector<DirectX::XMFLOAT4> points;
  points.reserve(10 * 2);

  // Transform every verticle
  for (int i = 0; i < 10; i++) {
    // Scale
    float scaledX = baseCoords[i].x * width;
    float scaledY = baseCoords[i].y * height;

    // Distance by center
    float finalX = scaledX + center.x;
    float finalY = scaledY + center.y;

    // Color
    DirectX::XMFLOAT4 c = pickColor(isGradient, colors, i, 10);

    // Pushing data
    points.push_back(DirectX::XMFLOAT4(finalX, finalY, baseCoords[i].z, baseCoords[i].w));
    points.push_back(c);
  }

  // Result
  Mesh result = { points, indexes };
  return result;
}

// --- Circle --- //
Mesh MeshCreator::Circle(DirectX::XMFLOAT2 center, float radius, int segments, bool isGradient, const std::vector<DirectX::XMFLOAT4>& colors) {
  // Segments amount
  const int segmentsAmount = segments;

  // Verticles amount: segments + 1 in center
  int totalVerts = segments + 1;

  // Making points
  std::vector<DirectX::XMFLOAT4> points;
  points.reserve(totalVerts * 2);

  // Center of circle
  DirectX::XMFLOAT4 centerPos(center.x, center.y, 0.5f, 1.0f);
  DirectX::XMFLOAT4 centerColor = pickColor(isGradient, colors, 0, totalVerts);
  points.push_back(centerPos);
  points.push_back(centerColor);

  // Calculate outter verticles
  //float doubledPi = 6.28318530718f;
  float doubledPi = DirectX::XM_PI * 2;
  for (int i = 0; i < segments; i++) {
    // Calculate positions
    float angle = (static_cast<float>(i) / segments) * doubledPi;
    float x = std::cos(angle) * radius;
    float y = std::sin(angle) * radius;
    float finalX = center.x + x;
    float finalY = center.y + y;
    DirectX::XMFLOAT4 pos(finalX, finalY, 0.5f, 1.0f);
    
    // Colors
    DirectX::XMFLOAT4 col = pickColor(isGradient, colors, i + 1, totalVerts);
    
    // Push
    points.push_back(pos);
    points.push_back(col);
  }

  // Indexes
  std::vector<int> indexes;
  indexes.reserve(segments * 3);

  // Triangle cycle: 0 - center, 1 - start outter point, 2 - next outter point
  for (int i = 1; i <= segments; i++) {
    indexes.push_back(0);
    indexes.push_back(i);
    indexes.push_back((i % segments) + 1);
  }

  // Result
  Mesh result = { points, indexes };
  return result;
}

Mesh MeshCreator::Oval(DirectX::XMFLOAT2 center, float width, float height, int segments, bool isGradient, const std::vector<DirectX::XMFLOAT4>& colors) {
  // Segments amount
  const int segmentsAmount = segments;

  // Verticles amount: segments + 1 in center
  int totalVerts = segments + 1;

  // Making points
  std::vector<DirectX::XMFLOAT4> points;
  points.reserve(totalVerts * 2);

  // Center
  DirectX::XMFLOAT4 centerPos(center.x, center.y, 0.5f, 1.0f);
  DirectX::XMFLOAT4 centerColor = pickColor(isGradient, colors, 0, totalVerts);
  points.push_back(centerPos);
  points.push_back(centerColor);

  // Calculate outter verticles
  //float doubledPi = 6.28318530718f;
  float doubledPi = DirectX::XM_PI * 2;

  for (int i = 0; i < segments; i++) {
    float angle = (static_cast<float>(i) / segments) * doubledPi;
    // Default positions
    float x = std::cos(angle) * 0.5f;
    float y = std::sin(angle) * 0.5f;
    // Scale positions by input parameters
    float finalX = x * width + center.x;
    float finalY = y * height + center.y;
    DirectX::XMFLOAT4 pos(finalX, finalY, 0.5f, 1.0f);
    // Colors
    DirectX::XMFLOAT4 col = pickColor(isGradient, colors, i + 1, totalVerts);
    points.push_back(pos);
    points.push_back(col);
  }

  // Indexes
  std::vector<int> indexes;
  indexes.reserve(segments * 3);

  // Triangle cycle: 0 - center, 1 - start outter point, 2 - next outter point
  for (int i = 1; i <= segments; i++) {
    indexes.push_back(0);
    indexes.push_back(i);
    indexes.push_back((i % segments) + 1);
  }

  // Result
  Mesh result = { points, indexes };
  return result;
}

// --- Cube --- //
Mesh MeshCreator::Cube(DirectX::XMFLOAT3 center, float size, bool isGradient, const std::vector<DirectX::XMFLOAT4>& colors) {
  float half = size * 0.5f;
  // Cube verticles
  DirectX::XMFLOAT4 baseCoords[8] = {
      { +half, +half, +half, 1.0f }, // v0
      { -half, +half, +half, 1.0f }, // v1
      { -half, -half, +half, 1.0f }, // v2
      { +half, -half, +half, 1.0f }, // v3
      { +half, +half, -half, 1.0f }, // v4
      { -half, +half, -half, 1.0f }, // v5
      { -half, -half, -half, 1.0f }, // v6
      { +half, -half, -half, 1.0f }  // v7
  };

  std::vector<DirectX::XMFLOAT4> points;
  // Adding color for each one
  for (int i = 0; i < 8; i++) {
    float finalX = baseCoords[i].x + center.x;
    float finalY = baseCoords[i].y + center.y;
    float finalZ = baseCoords[i].z + center.z;
    // If no gradient just pick the first color
    DirectX::XMFLOAT4 c = pickColor(isGradient, colors, i, 8);
    points.push_back(DirectX::XMFLOAT4(finalX, finalY, finalZ, 1.0f));
    points.push_back(c);
  }

  // Indexes for triangles
  std::vector<int> indexes = {
    // Front face
    0, 1, 2,   0, 2, 3,
    // Back face
    4, 7, 6,   4, 6, 5,
    // Left face
    1, 5, 6,   1, 6, 2,
    // Right face
    0, 3, 7,   0, 7, 4,
    // Top face
    0, 4, 5,   0, 5, 1,
    // Bottom face
    3, 2, 6,   3, 6, 7
  };

  Mesh result = { points, indexes };
  return result;
}

// --- Sphere --- //
Mesh MeshCreator::Sphere(
  DirectX::XMFLOAT3 center,
  float radius,
  int slices,
  int stacks,
  bool isGradient,
  const std::vector<DirectX::XMFLOAT4>& colors)
{
  std::vector<DirectX::XMFLOAT4> points;
  std::vector<int> indexes;

  // Generate the full net of sphere
  // For each stack and each slice i
  for (int stack = 0; stack <= stacks; stack++) {
    float theta = DirectX::XM_PI * static_cast<float>(stack) / stacks;
    float sinTheta = sinf(theta);
    float cosTheta = cosf(theta);

    for (int slice = 0; slice <= slices; slice++) {
      float phi = 2 * DirectX::XM_PI * static_cast<float>(slice) / slices;
      float sinPhi = sinf(phi);
      float cosPhi = cosf(phi);

      float x = sinTheta * cosPhi;
      float y = cosTheta;
      float z = sinTheta * sinPhi;

      float finalX = center.x + radius * x;
      float finalY = center.y + radius * y;
      float finalZ = center.z + radius * z;

      int vertIndex = stack * (slices + 1) + slice;
      DirectX::XMFLOAT4 c = pickColor(isGradient, colors, vertIndex, (stacks + 1) * (slices + 1));

      points.push_back(DirectX::XMFLOAT4(finalX, finalY, finalZ, 1.0f));
      points.push_back(c);
    }
  }

  for (int stack = 0; stack < stacks; stack++) {
    for (int slice = 0; slice < slices; slice++) {
      int first = stack * (slices + 1) + slice;
      int second = first + slices + 1;

      indexes.push_back(first);
      indexes.push_back(first + 1);
      indexes.push_back(second);

      indexes.push_back(second);
      indexes.push_back(first + 1);
      indexes.push_back(second + 1);
    }
  }

  Mesh result;
  result.points = points;
  result.indexes = indexes;
  return result;
}

// --- Grid XZ --- //
Mesh MeshCreator::GridXZCentered(
  float gridWidth,
  float gridDepth,
  int numDivisionsX,
  int numDivisionsZ,
  bool isGradient,
  const std::vector<DirectX::XMFLOAT4>& colors
) {
  std::vector<DirectX::XMFLOAT4> points;
  std::vector<int> indexes;

  float halfWidth = gridWidth * 0.5f;
  float halfDepth = gridDepth * 0.5f;

  float dx = gridWidth / numDivisionsX;
  float dz = gridDepth / numDivisionsZ;

  int vertexCount = 0;

  // Vertical lines
  for (int i = 0; i <= numDivisionsX; i++) {
    float x = -halfWidth + i * dx;
    DirectX::XMFLOAT4 posStart(x, 0.0f, -halfDepth, 1.0f);
    DirectX::XMFLOAT4 colStart = pickColor(isGradient, colors, i, numDivisionsX + 1);
    points.push_back(posStart);
    points.push_back(colStart);
    DirectX::XMFLOAT4 posEnd(x, 0.0f, halfDepth, 1.0f);
    DirectX::XMFLOAT4 colEnd = pickColor(isGradient, colors, i, numDivisionsX + 1);
    points.push_back(posEnd);
    points.push_back(colEnd);
    indexes.push_back(vertexCount);
    indexes.push_back(vertexCount + 1);
    vertexCount += 2;
  }

  // Horizontal
  for (int j = 0; j <= numDivisionsZ; j++) {
    float z = -halfDepth + j * dz;
    DirectX::XMFLOAT4 posStart(-halfWidth, 0.0f, z, 1.0f);
    DirectX::XMFLOAT4 colStart = pickColor(isGradient, colors, j, numDivisionsZ + 1);
    points.push_back(posStart);
    points.push_back(colStart);
    DirectX::XMFLOAT4 posEnd(halfWidth, 0.0f, z, 1.0f);
    DirectX::XMFLOAT4 colEnd = pickColor(isGradient, colors, j, numDivisionsZ + 1);
    points.push_back(posEnd);
    points.push_back(colEnd);
    indexes.push_back(vertexCount);
    indexes.push_back(vertexCount + 1);
    vertexCount += 2;
  }

  Mesh result;
  result.points = points;
  result.indexes = indexes;
  return result;
}

// --- Process Mesh --- //
// --- Process Mesh --- //
MeshWithTexture MeshCreator::ProcessMesh(
  aiMesh* mesh,
  const aiScene* scene,
  std::string dir
) {
  std::vector<Vertex> points;
  std::vector<int> indexes;

  // Перебор всех вершин
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    DirectX::XMFLOAT4 point;
    // Инвертируем Y для корректной ориентации
    point.x = mesh->mVertices[i].x;
    point.y = -mesh->mVertices[i].y;
    point.z = mesh->mVertices[i].z;
    point.w = 1.0f;

    // Текстурные координаты
    DirectX::XMFLOAT2 texCor(0.0f, 0.0f);
    if (mesh->mTextureCoords[0] != nullptr) {
      aiVector3D* pTexCoord = &(mesh->mTextureCoords[0][i]);
      texCor = DirectX::XMFLOAT2(pTexCoord->x, pTexCoord->y);
    }

    // Считываем нормали, которые были сгенерированы Assimp (или импортированы, если присутствуют)
    DirectX::XMFLOAT3 normal(0.0f, 0.0f, 0.0f);
    if (mesh->HasNormals()) {
      normal.x = mesh->mNormals[i].x;
      normal.y = mesh->mNormals[i].y;
      normal.z = mesh->mNormals[i].z;
    }

    // Формируем вершину, которая теперь содержит позицию, текстурные координаты и нормаль
    Vertex newPoint = { point, texCor, normal };
    points.push_back(newPoint);
  }

  // Обработка индексов
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    const aiFace& face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indexes.push_back(face.mIndices[j]);
    }
  }

  // Материал и текстура
  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  aiString path;
  if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
    material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
  }
  else {
    path.Set("");
  }
  std::string filename = dir + std::string(path.C_Str());
  std::wstring texturePath = std::wstring(filename.begin(), filename.end());

  MeshWithTexture result;
  result.points = points;
  result.indexes = indexes;
  result.texturePath = texturePath;
  return result;
}



// --- Load Model From File --- //
std::vector<MeshWithTexture> MeshCreator::MeshFromFile(const std::string& filepath) {
  Assimp::Importer importer;

  std::string directory = "";
  size_t lastSlash = filepath.find_last_of("/\\");
  directory = filepath.substr(0, lastSlash + 1);

  std::vector<MeshWithTexture> meshes;

  const aiScene* scene = importer.ReadFile(
    filepath,
    aiProcess_Triangulate
    | aiProcess_ConvertToLeftHanded
    | aiProcess_GenNormals
    | aiProcess_CalcTangentSpace
    | aiProcess_FlipUVs
  );
  if (scene == nullptr) {
    std::cout << importer.GetErrorString() << std::endl;
    return meshes;
  }

  aiNode* node = scene->mRootNode;
  for (UINT i = 0; i < scene->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[i];
    meshes.push_back(ProcessMesh(mesh, scene, directory));
  }

  return meshes;
}

std::vector<UINT> MeshCreator::GenerateAdjastencyIndices(const std::vector<Vertex>& vertices, const std::vector<int>& indices)
{
  using EdgeKey = std::pair<UINT, UINT>;

  auto makeEdge = [](UINT a, UINT b) {
    return std::make_pair(std::min(a, b), std::max(a, b));
    };

  std::map<EdgeKey, std::vector<UINT>> edgeMap;

  size_t triCount = indices.size() / 3;
  for (size_t i = 0; i < triCount; ++i) {
    UINT i0 = indices[i * 3 + 0];
    UINT i1 = indices[i * 3 + 1];
    UINT i2 = indices[i * 3 + 2];

    edgeMap[makeEdge(i0, i1)].push_back(i2);
    edgeMap[makeEdge(i1, i2)].push_back(i0);
    edgeMap[makeEdge(i2, i0)].push_back(i1);
  }

  std::vector<UINT> adjacencyIndices;

  for (size_t i = 0; i < triCount; ++i) {
    UINT i0 = indices[i * 3 + 0];
    UINT i1 = indices[i * 3 + 1];
    UINT i2 = indices[i * 3 + 2];

    auto getOpposite = [&](UINT a, UINT b) {
      auto key = makeEdge(a, b);
      const auto& vec = edgeMap[key];
      if (vec.size() == 2) {
        return (vec[0] != i0 && vec[0] != i1 && vec[0] != i2) ? vec[0] : vec[1];
      }
      return a;
      };

    UINT adj0 = getOpposite(i1, i2);
    UINT adj1 = getOpposite(i2, i0);
    UINT adj2 = getOpposite(i0, i1);

    adjacencyIndices.push_back(i0); adjacencyIndices.push_back(adj0);
    adjacencyIndices.push_back(i1); adjacencyIndices.push_back(adj1);
    adjacencyIndices.push_back(i2); adjacencyIndices.push_back(adj2);
  }

  return adjacencyIndices;
}