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
  float doubledPi = 6.28318530718f;
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
  float doubledPi = 6.28318530718f;
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