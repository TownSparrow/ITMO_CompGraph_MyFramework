#include "PongGame.h"
#include <iostream>

//___________________________________
// PRIVATE METHODS

// Make the instance null before init
PongGame* PongGame::pongGameInstance = nullptr;

// --- CreateCollision --- //
DirectX::BoundingBox PongGame::CreateCollision(std::vector<DirectX::XMFLOAT4> points) {
  DirectX::BoundingBox collision;
  UpdateCollision(points, &collision, true);
  return collision;
}

// --- UpdateCollision --- //
void PongGame::UpdateCollision(
  std::vector<DirectX::XMFLOAT4> points,
  DirectX::BoundingBox* collision,
  bool isCenterNeeded,
  Vector3 scale
){
  // Limitations of points values
  Vector3 minPoint = { FLT_MAX, FLT_MAX, FLT_MAX };
  Vector3 maxPoint = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

  // Cycle checking of points
  for (int pointIndex = 0; pointIndex < points.size(); pointIndex++) {
    if (pointIndex % 2 == 0) {
      // Point Value
      Vector3 point = { points[pointIndex].x, points[pointIndex].y, points[pointIndex].z };

      // Limitation of minimal point
      minPoint.x = (((minPoint.x) < (point.x)) ? (minPoint.x) : (point.x));
      minPoint.y = (((minPoint.y) < (point.y)) ? (minPoint.y) : (point.y));
      minPoint.z = (((minPoint.z) < (point.z)) ? (minPoint.z) : (point.z));

      // Limitation of max point
      maxPoint.x = (((maxPoint.x) > (point.x)) ? (maxPoint.x) : (point.x));
      maxPoint.y = (((maxPoint.y) > (point.y)) ? (maxPoint.y) : (point.y));
      maxPoint.z = (((maxPoint.z) > (point.z)) ? (maxPoint.z) : (point.z));
    }
  }

  // Calculate halfs of sizes
  float halfWidth = ((maxPoint.x - minPoint.x) / 2.0f) * scale.x;
  float halfHeight = ((maxPoint.y - minPoint.y) / 2.0f) * scale.y;

  // Check to spawn collision in the center of display
  if (isCenterNeeded) 
    collision->Center = { 
    minPoint.x + halfWidth,
    minPoint.y + halfHeight,
    1.0f 
  };
  collision->Extents = { halfWidth , halfHeight, 1.0f };
}

// --- Start --- //
void PongGame::Start(UINT playerWinner) {
  // Deafult projectile position
  ballPongObject->position = Vector3(0.0f, 0.0f, 0.0f);

  // Check where to spawn the projectile
  if (playerWinner == 0) {
    ballProjectileVelocity = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  }
  else {
    ballProjectileVelocity = Vector4(-1.0f, 0.0f, 0.0f, 1.0f);
  }

  // Reset collision of projectile
  ballPongObject->collision.Center = {
    ballPongObject->position.x,
    ballPongObject->position.y,
    ballPongObject->position.z
  };

  // Set default speed of projectile
  ballProjectileSpeed = 1.0f;
}

//___________________________________
// PUBLIC METHODS

// --- Init --- //
void PongGame::Initialize(LPCWSTR shaderPath) {
  // Get instance of game
  Game* game = Game::GetInstance();

  // Init meshes
  // Strides and offsets
  std::vector<UINT> strides = { 32 };
  std::vector<UINT> offsets = { 0 };

  // Ball projectile mesh
  TriangleComponent* ballTriangleComponent = new TriangleComponent(game);
  //Mesh ballMesh = MeshCreator::GetInstance()->Circle
  //(
  //  DirectX::XMFLOAT2(0.0f, 0.0f),
  //  0.025f,
  //  8,
  //  false,
  //  { DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }
  //);
  Mesh ballMesh = MeshCreator::GetInstance()->Square
  (
    DirectX::XMFLOAT2(0.0f, 0.0f),
    0.05f,
    0.05f,
    false,
    { DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }
  );
  ballTriangleComponent->Initialize(shaderPath, ballMesh.points, ballMesh.indexes, strides, offsets);
  DirectX::BoundingBox ballCollision = CreateCollision(ballMesh.points);
  ballPongObject = new PongObject{
      ballTriangleComponent,
      ballCollision,
      Vector3(0.0f, 0.0f, 0.0f)
  };

  // Prepare the x-axis shift
  float xShift = static_cast<float>(game->screenWidth) / static_cast<float>(game->screenHeight);

  // Player 1 Mesh
  TriangleComponent* player1TriangleComponent = new TriangleComponent(game);
  Mesh player1Mesh = MeshCreator::GetInstance()->Square
  (
    DirectX::XMFLOAT2(0.0f, 0.0f),
    0.05f,
    0.5f,
    false,
    { DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
  );
  player1TriangleComponent->Initialize(shaderPath, player1Mesh.points, player1Mesh.indexes, strides, offsets);
  DirectX::BoundingBox player1Collision = CreateCollision(player1Mesh.points);
  Vector3 player1Position = Vector3(-xShift + 0.05f, 0.0f, 0.0f);
  player1TriangleComponent->transforms.move = Matrix::CreateTranslation(player1Position);
  player1Collision.Center = player1Position;

  player1PongObject = new PongObject{
      player1TriangleComponent,
      player1Collision,
      player1Position
  };
  //PlayerControlComponent* controlPlayable1 = new PlayerControlComponent(Game::GetInstance(), player1TriangleComponent, 0);

  // Player 2 Mesh
  TriangleComponent* player2TriangleComponent = new TriangleComponent(game);
  Mesh player2Mesh = MeshCreator::GetInstance()->Square
  (
    DirectX::XMFLOAT2(0.0f, 0.0f),
    0.05f,
    0.5f,
    false,
    { DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }
  );
  player2TriangleComponent->Initialize(shaderPath, player2Mesh.points, player2Mesh.indexes, strides, offsets);
  DirectX::BoundingBox player2Collision = CreateCollision(player2Mesh.points);
  Vector3 player2Position = Vector3(xShift - 0.05f, 0.0f, 0.0f);
  player2TriangleComponent->transforms.move = Matrix::CreateTranslation(player2Position);
  player2Collision.Center = player2Position;

  player2PongObject = new PongObject{
      player2TriangleComponent,
      player2Collision,
      player2Position
  };
  //PlayerControlComponent* controlPlayable2 = new PlayerControlComponent(Game::GetInstance(), player2TriangleComponent, 1);

  // Pushing all needed components
  Game::GetInstance()->components.push_back(ballTriangleComponent);
  Game::GetInstance()->components.push_back(player1TriangleComponent);
  //Game::GetInstance()->components.push_back(player1Controled);
  Game::GetInstance()->components.push_back(player2TriangleComponent);
  //Game::GetInstance()->components.push_back(player2Controled);

  // Set init positions of level collisions
  topLevelCollision.Center = { 0.0f, 1.0f, 1.0f };
  topLevelCollision.Extents = { xShift, 0.01f, 1.0f };
  bottomLevelCollision.Center = { 0.0f, -1.0f, 1.0f };
  bottomLevelCollision.Extents = { xShift, 0.01f, 1.0f };
  player1SideCollision.Center = { -xShift - 0.5f, 0.0f, 1.0f };
  player1SideCollision.Extents = { 0.5f, 1.0f, 1.0f };
  player2SideCollision.Center = { xShift + 0.5f, 0.0f, 1.0f };
  player2SideCollision.Extents = { 0.5f, 1.0f, 1.0f };

  // Let the game begin!
  Start(0);
}

// --- Update Player --- //
void PongGame::UpdatePlayer(UINT player, UINT direction, float deltaTime) {
  // Get game instance
  Game* game = Game::GetInstance();

  // Calculate the aspectRatio for the window screen
  float aspectRatio = static_cast<float>(game->screenWidth) / static_cast<float>(game->screenHeight);
  
  // Values of level limitations for the players
  const float limitTop = 0.75f;
  const float limitBottom = -0.75f;
  const float limitRight = aspectRatio - 0.05f;
  const float limitLeft = -aspectRatio + 0.05f;
  const float limitCenter = 0.0f;
  const float limitCenterMargin = 0.1f;

  // Calculate speed with modifire
  float speed = 1.0f * deltaTime;

  // Check player index and directions to set the moving logic
  
  // Player 1: Limit top
  if (player + direction == 0) {
    if (player1PongObject->position.y < limitTop) {
      player1PongObject->position.y += speed;
    }
  }

  // Player 1: Limit bottom
  if (player + direction == 1) {
    if (player1PongObject->position.y > limitBottom) {
      player1PongObject->position.y -= speed;
    }
  }

  // Player 2: Limit top
  if (player + direction == 2) {
    if (player2PongObject->position.y < limitTop) {
      player2PongObject->position.y += speed;
    }
  }
  // Player 2: Limit bottom
  if (player + direction == 3) {
    if (player2PongObject->position.y > limitBottom) {
      player2PongObject->position.y -= speed;
    }
  }

  // Player 2: Limit right
  if (player + direction == 4) {
    if (player2PongObject->position.x < limitRight)
      player2PongObject->position.x += speed;
  }

  // Player 2: Limit left
  if (player + direction == 5) {
    if (player2PongObject->position.x > limitCenter + limitCenterMargin)
      player2PongObject->position.x -= speed;
  }

  // Player 1: Limit right
  if (player + direction == 6) {
    if (player1PongObject->position.x < limitCenter - limitCenterMargin)
      player1PongObject->position.x += speed;
  }

  // Player 1: Limit left
  if (player + direction == 7) {
    if (player1PongObject->position.x > limitLeft)
      player1PongObject->position.x -= speed;
  }
}

// --- Update Ball --- //
void PongGame::UpdateBall(float deltaTime) {
  //  Values of level limitations for the ball projectile
  const float limitUp = 1.0f;
  const float limitDown = -1.0f;
  const float limitRight = 1.0f;
  const float limitLeft = -1.0f;

  // Change position of ball projectile
  ballPongObject->position.x += ballProjectileVelocity.x * ballProjectileSpeed * deltaTime;
  ballPongObject->position.y += ballProjectileVelocity.y * ballProjectileSpeed * deltaTime;
}

// --- Check all collisions -- //
void PongGame::CheckAllCollisions() {
  // Checking intersection of player 1 with projectile
  if ((ballPongObject->collision.Intersects(player1PongObject->collision)))
  {
    if (ballProjectileVelocity.x < 0) {
      float relativeIntersectY = (ballPongObject->collision.Center.y - player1PongObject->collision.Center.y) / player1PongObject->collision.Extents.y;

      if (relativeIntersectY < -1.0f) relativeIntersectY = -1.0f;
      if (relativeIntersectY > 1.0f) relativeIntersectY = 1.0f;

      ballProjectileVelocity.x *= -1.0f;
      ballProjectileVelocity.y = relativeIntersectY;
      ballProjectileSpeed += ballProjectileSpeedGrow;
    }
  }

  // Checking intersection of player 2 with projectile
  if ((ballPongObject->collision.Intersects(player2PongObject->collision)))
  {
    if (ballProjectileVelocity.x > 0) {
      float relativeIntersectY = (ballPongObject->collision.Center.y - player2PongObject->collision.Center.y) / player2PongObject->collision.Extents.y;

      if (relativeIntersectY < -1.0f) relativeIntersectY = -1.0f;
      if (relativeIntersectY > 1.0f) relativeIntersectY = 1.0f;

      ballProjectileVelocity.x *= -1.0f;
      ballProjectileVelocity.y = relativeIntersectY;
      ballProjectileSpeed += ballProjectileSpeedGrow;
    }
  }

  // Checking for goal of player 1
  if ((ballPongObject->collision.Intersects(player2SideCollision))) {
    player1Score += 1;
    std::cout << player1Score << " : " << player2Score << std::endl;
    Start(0);
  }

  // Checking for goal of player 2
  if ((ballPongObject->collision.Intersects(player1SideCollision))) {
    player2Score += 1;
    std::cout << player1Score << " : " << player2Score << std::endl;
    Start(1);
  }

  // Check if ball touches the up of level
  if ((ballPongObject->collision.Intersects(topLevelCollision)) && ballProjectileVelocity.y > 0) {
    ballProjectileVelocity.y *= -1.0f;
  }

  // Check if ball touches the bottom of level
  if ((ballPongObject->collision.Intersects(bottomLevelCollision)) && ballProjectileVelocity.y < 0) {
    ballProjectileVelocity.y *= -1.0f;
  }
}

// -- Update -- //
void PongGame::Update() {
  // Update each collision with matrix

  // Ball projectile
  ballPongObject->triangleMesh->transforms.move = Matrix::CreateTranslation(ballPongObject->position);
  ballPongObject->collision.Center = ballPongObject->position;

  // Player 1
  player1PongObject->triangleMesh->transforms.move = Matrix::CreateTranslation(player1PongObject->position);
  player1PongObject->collision.Center = player1PongObject->position;
  player1PongObject->triangleMesh->transforms.move = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
  player1PongObject->triangleMesh->Update();
  //player1PongObject->triangleMesh->transforms.scale = Matrix::CreateScale(1.0f + netUpdateTime * 0.2f, 1.0f + netUpdateTime * 0.2f, 1.0f);
  player1PongObject->triangleMesh->Update();
  player1PongObject->triangleMesh->transforms.move = Matrix::CreateTranslation(player1PongObject->position);
  UpdateCollision(
    player1PongObject->triangleMesh->points,
    &(player1PongObject->collision),
    false,
    Vector3(1.0f, 1.0f, 1.0f)
  );

  // Player 2
  player2PongObject->triangleMesh->transforms.move = Matrix::CreateTranslation(player2PongObject->position);
  player2PongObject->collision.Center = player2PongObject->position;
  player2PongObject->triangleMesh->transforms.move = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
  player2PongObject->triangleMesh->Update();
  //player2PongObject->triangleMesh->transforms.scale = Matrix::CreateScale(1.0f + netUpdateTime * 0.2f, 1.0f + netUpdateTime * 0.2f, 1.0f);
  player2PongObject->triangleMesh->Update();
  player2PongObject->triangleMesh->transforms.move = Matrix::CreateTranslation(player2PongObject->position);
  UpdateCollision(
    player2PongObject->triangleMesh->points,
    &(player2PongObject->collision),
    false,
    Vector3(1.0f, 1.0f, 1.0f)
  );
}

// --- Update Interval --- //
void PongGame::UpdateInterval(float deltaTime) {
  Game* game = Game::GetInstance();

  //netUpdateTime += deltaTime;

  // WASD:
  if (game->inputDevice->IsKeyDown(Keys::W))
    UpdatePlayer(0, 0, deltaTime);
  if (game->inputDevice->IsKeyDown(Keys::S))
    UpdatePlayer(0, 1, deltaTime);
  if (game->inputDevice->IsKeyDown(Keys::A))
    UpdatePlayer(0, 7, deltaTime);
  if (game->inputDevice->IsKeyDown(Keys::D))
    UpdatePlayer(0, 6, deltaTime);

  // Arrows
  if (game->inputDevice->IsKeyDown(Keys::Up))
    UpdatePlayer(1, 1, deltaTime);
  if (game->inputDevice->IsKeyDown(Keys::Down))
    UpdatePlayer(1, 2, deltaTime);
  if (game->inputDevice->IsKeyDown(Keys::Left))
    UpdatePlayer(1, 4, deltaTime);
  if (game->inputDevice->IsKeyDown(Keys::Right))
    UpdatePlayer(1, 3, deltaTime);

  // Another way to make the control system (with key array)
  //if (keys[87]) 
  // UpdatePaddle(0, 0, deltaTime);
  //if (keys[83]) 
  // UpdatePaddle(0, 1, deltaTime);
  //if (keys[37]) 
  // UpdatePaddle(1, 4, deltaTime);
  //if (keys[39]) 
  // UpdatePaddle(1, 3, deltaTime);
  //if (keys[38]) 
  // UpdatePaddle(1, 1, deltaTime);
  //if (keys[40]) 
  // UpdatePaddle(1, 2, deltaTime);
  //if (keys[65]) 
  // UpdatePaddle(0, 7, deltaTime);
  //if (keys[68]) 
  // UpdatePaddle(0, 6, deltaTime);

  UpdateBall(deltaTime);

  CheckAllCollisions();
  
  //if (netUpdateTime == 0.0f) {
  //  leftAnim = false;
  //  rightAnim = false;
  //}
}

