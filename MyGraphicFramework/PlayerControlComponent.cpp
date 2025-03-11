#include "PlayerControlComponent.h"
#include "Game.h"
#include "TriangleComponent.h"

// --- Update of player control component --- //
//void PlayerControlComponent::Update(ConstData* data) {
void PlayerControlComponent::Update() {
  // Set the default value of delta time
  // 60FPS: ~0.016f
  float deltaTime = 0.016f;

  // Set the value of speed (1.0f is default modifier)
  float speed = 1.0f * deltaTime;

  // Default matrix values: as the game component's matrix
  Matrix translation = Matrix::Identity;

  // 0. WASD control
  if (controlMode == 0) {
    if (game->inputDevice->IsKeyDown(Keys::W))
      translation *= Matrix::CreateTranslation(0.0f, speed, 0.0f);
    if (game->inputDevice->IsKeyDown(Keys::S))
      translation *= Matrix::CreateTranslation(0.0f, -speed, 0.0f);
    if (game->inputDevice->IsKeyDown(Keys::A))
      translation *= Matrix::CreateTranslation(-speed, 0.0f, 0.0f);
    if (game->inputDevice->IsKeyDown(Keys::D))
      translation *= Matrix::CreateTranslation(speed, 0.0f, 0.0f);
  }
  // 1. Arrows control
  else if (controlMode == 1) {
    if (game->inputDevice->IsKeyDown(Keys::Up))
      translation *= Matrix::CreateTranslation(0.0f, speed, 0.0f);
    if (game->inputDevice->IsKeyDown(Keys::Down))
      translation *= Matrix::CreateTranslation(0.0f, -speed, 0.0f);
    if (game->inputDevice->IsKeyDown(Keys::Left))
      translation *= Matrix::CreateTranslation(-speed, 0.0f, 0.0f);
    if (game->inputDevice->IsKeyDown(Keys::Right))
      translation *= Matrix::CreateTranslation(speed, 0.0f, 0.0f);
  }

  // Update the matrix of controlled object (the position as for default)
  //controlledObject->transform.move = translation * controlledObject->transforms.move;
}
