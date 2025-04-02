#include "KatamariGame.h"
#include "Game.h"

KatamariGame* KatamariGame::katamariInstance = nullptr;

using namespace std;

// --- Init --- //
void KatamariGame::Initialize() {
	game = Game::GetInstance();

	//std::vector<UINT> strides = { 32 };
	//std::vector<UINT> offsets = { 0 };

	//SpawnRandomObjects();
  SpawnGround();

  player = new KatamariPlayer(game);
	mainOrbitalCamera = player->GetCamera();
  
  SpawnLittleObjectsGroup();
  SpawnMediumObjectsGroup();
  SpawnBigObjectsGroup();

  // Lighting
  // Directional Light
  directionalLight = new DirectionalLight{
    // Ambient
    Vector4(1.0f, 1.0f, 1.0f, 1.0f),
    // Diffuse
    Vector4(0.2f, 0.2f, 0.8f, 1.0f),
    // Specular
    Vector4(0.2f, 0.2f, 0.2, 0.2f),
    // Direction
    Vector4(0.0f, 1.0f, 1.0f, 1.0f)
  };
  game->directionalLight = directionalLight;

  // Point Light
  //pointLight = new PointLight{
  //  // Ambient
  //  Vector4(1.5f, 1.5f, 0.3f, 1.0f),
  //  // Diffuse
  //  Vector4(2.0f, 2.0f, 0.8f, 1.0f),
  //  // Specular
  //  Vector4(1.5f, 1.5f, 1.2f, 1.0f),
  //  // Position
  //  Vector3(0.0f, 1.0f, 0.0f),
  //  // Radius
  //  15.0f,
  //  // Attenuation
  //  Vector4(0.2f, 0.02f, 0.0f, 1.0f)
  //};
  //game->pointLights.push_back(pointLight);
}

// --- Spawn ground --- //
void KatamariGame::SpawnGround() {
  UINT stride = sizeof(Vertex);
  std::vector<UINT> strides = { stride };
  //std::vector<UINT> strides = { 24 };
  std::vector<UINT> offsets = { 0 };

  Material* material = new Material{
    Vector4(0.23f, 0.23f, 0.23f, 1.00f),
    Vector4(0.28f, 0.28f, 0.28f, 1.00f),
    Vector4(0.77f, 0.77f, 0.77f, 5.9f)
  };

  TriangleWithTextureComponent* roadTriangleComponent = new TriangleWithTextureComponent(game);
  std::vector<MeshWithTexture> roadMesh = MeshCreator::GetInstance()->MeshFromFile("./Models/RoadPlane/RoadPlane.obj");
  roadTriangleComponent->Initialize(L"./Shaders/TextureModifiedShader.hlsl", roadMesh[0].points, roadMesh[0].indexes, strides, offsets, roadMesh[0].texturePath, material, false);
  game->components.push_back(roadTriangleComponent);
}

// --- Spawn group of little objects --- //
void KatamariGame::SpawnLittleObjectsGroup() {
  vector<LPCSTR> models;
  models.push_back("./Models/TrashCan/Trash.obj");
  models.push_back("./Models/FireHydrant/Fire_Hydrant.obj");
  models.push_back("./Models/TrafficCone/TrafficCone.obj");
  //models.push_back("./Models/Rose/Red_rose_SF.obj");
  //models.push_back("./Models/Coca-Cola/Coca-Cola.obj");

  float sizeMin = 10;
  Vector3 leftDownMapMinCorner = Vector3(-sizeMin, 0, -sizeMin);
  Vector3 rightUpMapMinCorner = Vector3(sizeMin, 0, sizeMin);
  
  float sizeMax = 20;
  Vector3 leftDownMapMaxCorner = Vector3(-sizeMax, 0, -sizeMax);
  Vector3 rightUpMapMaxCorner = Vector3(sizeMax, 0, sizeMax);

  int objectsAmount = 10;

  SpawnRandomObjects(
    models,
    leftDownMapMinCorner,
    rightUpMapMinCorner,
    leftDownMapMaxCorner,
    rightUpMapMaxCorner,
    objectsAmount,
    false
  );
}

// --- Spawn group of medium objects --- //
void KatamariGame::SpawnMediumObjectsGroup() {
  vector<LPCSTR> models;
  //models.push_back("./Models/TrashCan/Trash.obj");
  models.push_back("./Models/ConcreteBarrier/ConcreteBarrier.obj");
  //models.push_back("./Models/Rose/Red_rose_SF.obj");
  //models.push_back("./Models/Coca-Cola/Coca-Cola.obj");
  models.push_back("./Models/RomanColumn/RomanColumn.obj");

  float sizeMin = 20;
  Vector3 leftDownMapMinCorner = Vector3(-sizeMin, 0, -sizeMin);
  Vector3 rightUpMapMinCorner = Vector3(sizeMin, 0, sizeMin);

  float sizeMax = 40;
  Vector3 leftDownMapMaxCorner = Vector3(-sizeMax, 0, -sizeMax);
  Vector3 rightUpMapMaxCorner = Vector3(sizeMax, 0, sizeMax);

  int objectsAmount = 10;

  SpawnRandomObjects(
    models,
    leftDownMapMinCorner,
    rightUpMapMinCorner,
    leftDownMapMaxCorner,
    rightUpMapMaxCorner,
    objectsAmount,
    false
  );
}

// --- Spawn group of big objects --- //
void KatamariGame::SpawnBigObjectsGroup() {
  vector<LPCSTR> models;
  //models.push_back("./Models/TrashCan/Trash.obj");
  //models.push_back("./Models/Rose/Red_rose_SF.obj");
  models.push_back("./Models/Coca-Cola/Coca-Cola.obj");
  //models.push_back("./Models/BigRat/BigRat.obj");

  float sizeMin = 5;
  Vector3 leftDownMapMinCorner = Vector3(-sizeMin, 0, -sizeMin);
  Vector3 rightUpMapMinCorner = Vector3(sizeMin, 0, sizeMin);

  float sizeMax = 10;
  Vector3 leftDownMapMaxCorner = Vector3(-sizeMax, 0, -sizeMax);
  Vector3 rightUpMapMaxCorner = Vector3(sizeMax, 0, sizeMax);

  int objectsAmount = 30;

  SpawnRandomObjects(
    models,
    leftDownMapMinCorner,
    rightUpMapMinCorner,
    leftDownMapMaxCorner,
    rightUpMapMaxCorner,
    objectsAmount,
    true
  );
}

// Previous Base realization
//void KatamariGame::SpawnRandomObjects() {
//  random_device rd;
//  mt19937 gen(rd());
//
//  uniform_real_distribution<> distX(ldMapCorner.x, ruMapCorner.x);
//  uniform_real_distribution<> distZ(ldMapCorner.z, ruMapCorner.z);
//  uniform_real_distribution<> rotY(0, DirectX::XM_2PI);
//
//  vector<LPCSTR> models;
//  models.push_back("./Models/TrashCan/Trash.obj");
//  models.push_back("./Models/Rose/Red_rose_SF.obj");
//  models.push_back("./Models/Coca-Cola/Coca-Cola.obj");
//
//  uniform_int_distribution<> modelDist(0, models.size() - 1);
//
//  std::vector<UINT> strides = { 24 };
//  std::vector<UINT> offsets = { 0 };
//
//  for (int i = 0; i < objectsCount; i++) {
//    Vector3 position = Vector3(distX(gen), 0.0f, distZ(gen));
//    float rotationY = rotY(gen);
//    std::cout << rotationY << std::endl;
//
//    std::vector<MeshWithTexture> meshes = MeshCreator::GetInstance()->MeshFromFile(models.at(modelDist(gen)));
//    std::vector<TriangleWithTextureComponent*> modelParts;
//
//    // Init overall values
//    Vector3 overallMin(FLT_MAX, FLT_MAX, FLT_MAX);
//    Vector3 overallMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
//
//    // Calculate overall values for each mesh
//    for (const auto& mesh : meshes) {
//      for (const auto& vertex : mesh.points) {
//        // Преобразуем Vertex::position (XMFLOAT4) в Vector3, игнорируя w
//        Vector3 pt(vertex.position.x, vertex.position.y, vertex.position.z);
//        overallMin = Vector3::Min(overallMin, pt);
//        overallMax = Vector3::Max(overallMax, pt);
//      }
//    }
//
//    // Calculate center and extents
//    Vector3 localCenter = (overallMin + overallMax) * 0.5f;
//    Vector3 localExtents = (overallMax - overallMin) * 0.5f;
//
//    // Debug
//    float length = overallMax.x - overallMin.x;
//    float height = overallMax.y - overallMin.y;
//    float width = overallMax.z - overallMin.z;
//    std::cout << "Length: " << length << ", Height: " << height << ", Width: " << width << std::endl;
//
//    
//    Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(Vector3(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, rotationY));
//
//    // Init model components
//    for (MeshWithTexture mesh : meshes) {
//      TriangleWithTextureComponent* modelPart = new TriangleWithTextureComponent(game);
//      modelPart->Initialize(L"./Shaders/TextureModifiedShader.hlsl", mesh.points, mesh.indexes, strides, offsets, mesh.texturePath);
//      modelPart->transforms.rotate = rotationMatrix;
//      modelPart->transforms.move = Matrix::CreateTranslation(position);
//      game->components.push_back(modelPart);
//      modelParts.push_back(modelPart);
//    }
//
//    // Calculate world center and orientation
//    Vector3 worldCenter = Vector3::Transform(localCenter, rotationMatrix) + position;
//    Quaternion orientation = Quaternion::CreateFromYawPitchRoll(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, rotationY);
//
//    // Init collision with calculated values
//    DirectX::BoundingOrientedBox collision;
//    collision.Center = worldCenter;
//    collision.Extents = localExtents;
//    collision.Orientation = orientation;
//
//    // Create pickable object
//    Pickable* object = new Pickable{
//        modelParts,
//        collision,
//        position,
//        orientation,
//        false
//    };
//    pickables.push_back(object);
//  }
//}

// --- Main Spawn Random Object Algorithm --- //
void KatamariGame::SpawnRandomObjects(
  std::vector<LPCSTR> models,
  Vector3 leftDownMaxCorner,
  Vector3 rightDownMaxCorner,
  Vector3 leftDownMinCorner,
  Vector3 rightDownMinCorner,
  int objectsAmount,
  bool isTransparentGot
) {
  // random values generator
  random_device rd;
  mt19937 gen(rd());

  bool isTransparent = isTransparentGot;

  // Calculate zone of spawn
  Vector3 minCorner(
    std::min(leftDownMinCorner.x, rightDownMinCorner.x),
    std::min(leftDownMinCorner.y, rightDownMinCorner.y),
    std::min(leftDownMinCorner.z, rightDownMinCorner.z)
  );
  Vector3 maxCorner(
    std::max(leftDownMaxCorner.x, rightDownMaxCorner.x),
    std::max(leftDownMaxCorner.y, rightDownMaxCorner.y),
    std::max(leftDownMaxCorner.z, rightDownMaxCorner.z)
  );

  // Rechecking and Compensation of values problems
  if (fabs(maxCorner.x - minCorner.x) < 0.001f)
    maxCorner.x = minCorner.x + 0.001f;
  if (fabs(maxCorner.z - minCorner.z) < 0.001f)
    maxCorner.z = minCorner.z + 0.001f;

  // Create distibutions
  uniform_real_distribution<> distX(minCorner.x, maxCorner.x);
  uniform_real_distribution<> distZ(minCorner.z, maxCorner.z);
  uniform_real_distribution<> rotY(0, DirectX::XM_2PI);

  // Models array
  uniform_int_distribution<> modelDist(0, models.size() - 1);

  UINT stride = sizeof(Vertex);
  std::vector<UINT> strides = { stride };
  //std::vector<UINT> strides = { 32 };
  std::vector<UINT> offsets = { 0 };

  for (int i = 0; i < objectsAmount; i++) {
    // Choose random position
    Vector3 position = Vector3(distX(gen), 0.0f, distZ(gen));
    float rotationY = rotY(gen);
    std::cout << "RotationY: " << rotationY << std::endl;

    // Loading random mesh model
    std::vector<MeshWithTexture> meshes = MeshCreator::GetInstance()->MeshFromFile(models.at(modelDist(gen)));
    std::vector<TriangleWithTextureComponent*> modelParts;

    // Init overall
    Vector3 overallMin(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 overallMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    // Analyse mesh for calculation collision
    for (const auto& mesh : meshes) {
      for (const auto& vertex : mesh.points) {
        // Convert Vertex::position (XMFLOAT4) to Vector3 (ignoring w)
        Vector3 pt(vertex.position.x, vertex.position.y, vertex.position.z);
        overallMin = Vector3::Min(overallMin, pt);
        overallMax = Vector3::Max(overallMax, pt);
      }
    }

    // Calculate locan center and extents
    Vector3 localCenter = (overallMin + overallMax) * 0.5f;
    Vector3 localExtents = (overallMax - overallMin) * 0.5f;

    // Debug output
    float length = overallMax.x - overallMin.x;
    float height = overallMax.y - overallMin.y;
    float width = overallMax.z - overallMin.z;
    std::cout << "Length: " << length << ", Height: " << height << ", Width: " << width << std::endl;

    // Create rotation matrix
    //Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(Vector3(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, rotationY));
    Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(rotationY, 0, 0);

    // Init models components
    for (MeshWithTexture mesh : meshes) {
      TriangleWithTextureComponent* modelPart = new TriangleWithTextureComponent(game);
      Material* material = new Material{
        Vector4(0.2f, 0.2f, 0.2f, 0.2f),
        Vector4(0.2f, 0.2f, 0.2f, 0.2f),
        Vector4(0.55f, 0.55f, 0.55f, 1.00f)
      };
      modelPart->Initialize(L"./Shaders/TextureModifiedShader.hlsl", mesh.points, mesh.indexes, strides, offsets, mesh.texturePath, material, isTransparent);
      modelPart->transforms.rotate = rotationMatrix;
      modelPart->transforms.move = Matrix::CreateTranslation(position);
      game->components.push_back(modelPart);
      modelParts.push_back(modelPart);
    }

    // Calculate world position of mofel
    Vector3 worldCenter = Vector3::Transform(localCenter, rotationMatrix) + position;
    //Quaternion orientation = Quaternion::CreateFromYawPitchRoll(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, rotationY);
    Quaternion orientation = Quaternion::CreateFromYawPitchRoll(rotationY, 0, 0);

    // Create collision with calculated values
    DirectX::BoundingOrientedBox collision;
    collision.Center = worldCenter;
    collision.Extents = localExtents;
    collision.Orientation = orientation;

    // Creating Pickable-object
    Pickable* object = new Pickable{
        modelParts,
        collision,
        position,
        orientation,
        false
    };
    pickables.push_back(object);
  }
}

// --- Update --- //
void KatamariGame::Update() {
  player->Update();
}

// --- Update Interval --- //
void KatamariGame::UpdateInterval(float deltaTime) {
	player->UpdateInterval(deltaTime);
}
