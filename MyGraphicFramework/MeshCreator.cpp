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
Mesh MeshCreator::Sphere(DirectX::XMFLOAT3 center, float radius, int slices, int stacks, bool isGradient, const std::vector<DirectX::XMFLOAT4>& colors) {
  std::vector<DirectX::XMFLOAT4> points;
  std::vector<int> indexes;

  // Generate the full net of sphere
  // For each stack and each slice i
  for (int stack = 0; stack <= stacks; stack++) {
    float theta = DirectX::XM_PI * static_cast<float>(stack) / stacks; // угол от 0 до PI
    float sinTheta = sinf(theta);
    float cosTheta = cosf(theta);

    for (int slice = 0; slice <= slices; slice++) {
      float phi = 2 * DirectX::XM_PI * static_cast<float>(slice) / slices; // угол от 0 до 2PI
      float sinPhi = sinf(phi);
      float cosPhi = cosf(phi);

      float x = sinTheta * cosPhi;
      float y = cosTheta;
      float z = sinTheta * sinPhi;

      // Apply radius and center shift
      float finalX = center.x + radius * x;
      float finalY = center.y + radius * y;
      float finalZ = center.z + radius * z;
      
      // Color for each verticle
      // Index calculating:
      int index = stack * (slices + 1) + slice;
      DirectX::XMFLOAT4 c = pickColor(isGradient, colors, index, (stacks + 1) * (slices + 1));
      points.push_back(DirectX::XMFLOAT4(finalX, finalY, finalZ, 1.0f));
      points.push_back(c);
    }
  }

  // Indexes for triangles
  for (int stack = 0; stack < stacks; stack++) {
    for (int slice = 0; slice < slices; slice++) {
      int first = stack * (slices + 1) + slice;
      int second = first + slices + 1;

      indexes.push_back(first);
      indexes.push_back(second);
      indexes.push_back(first + 1);

      indexes.push_back(second);
      indexes.push_back(second + 1);
      indexes.push_back(first + 1);
    }
  }

  Mesh result = { points, indexes };
  return result;
}