#include "FirstPersonCamera.h"
#include "Game.h"
#include <iostream>

#include <unordered_set>
#include "Keys.h"

// --- Init --- //
void FirstPersonCamera::Initialize() {
	// Vector values
	cameraPosition = Vector3(0.0f, 0.0f, -1.0f);
	cameraWatchTarget = Vector3(0.0f, 0.0f, 1.0f);
	cameraUpAxis = Vector3(0.0f, 1.0f, 0.0f);

	// Other default values
	cameraFOV = 30.0f;
	aspectRatio = static_cast<float>(game->screenWidth) / static_cast<float>(game->screenHeight);
	nearToPlaneDistance = 0.01f;
	farFromPlaneDistance = 100.0f;

	// Camera Matrix values
	cameraMatrix.projection = Matrix::CreatePerspectiveFieldOfView(cameraFOV, aspectRatio, nearToPlaneDistance, farFromPlaneDistance);
	cameraMatrix.view = Matrix::CreateLookAt(cameraPosition, cameraWatchTarget, cameraUpAxis);

	// Default velocity value
	velocity = Vector3(0.0f, 0.0f, 0.0f);

	// Yaw and Pitch values
	cameraYaw = atan2(cameraWatchTarget.z, cameraWatchTarget.x);
	cameraPitch = asin(cameraWatchTarget.y);

	// Calculate Forward Vector
	RecalculateForward();
}

// --- Set Camera Position --- //
void FirstPersonCamera::SetCameraPosition(Vector3 cameraPositionInput) {
	cameraPosition = cameraPositionInput;
}

// --- Set Watch Target --- //
void FirstPersonCamera::SetWatchTarget(Vector3 cameraWatchTargetInput) {
	cameraWatchTarget = cameraWatchTargetInput;
}

// --- Set Up-Axis value --- //
void FirstPersonCamera::SetUpAxis(Vector3 cameraUpAxisInput) {
	cameraUpAxis = cameraUpAxisInput;
}

// --- Recalculate vector for forward direction --- //
void FirstPersonCamera::RecalculateForward() {
	forwardDirection = cameraWatchTarget - cameraPosition;
	forwardDirection.Normalize();
}

// --- Calculate new camera rotation --- //
void FirstPersonCamera::CameraRotation(Vector2 mouseInput) {
	// Calculate Yaw and Pitch
	cameraYaw += mouseInput.x * cameraControlSensitivity;
	cameraPitch += mouseInput.y * cameraControlSensitivity;

	// Set the pitch limit
	float pitchLimit = DirectX::XM_PIDIV2 - 0.01f;
	if (cameraPitch < -pitchLimit) cameraPitch = -pitchLimit;
	if (cameraPitch > pitchLimit) cameraPitch = pitchLimit;

	// Calculate direction
	Vector3 direction;
	direction.x = -cosf(cameraYaw) * cosf(cameraPitch);
	direction.y = sinf(cameraPitch);
	direction.z = cosf(cameraPitch) * sinf(cameraYaw);
	direction.Normalize();

	// Apply new direction to watch target
	cameraWatchTarget = cameraPosition + direction;

	// Recalculate forward vector for new movement
	RecalculateForward();
	//std::cout << "forward Direction is: " << forwardDirection.x << "; " << forwardDirection.y << "; " << forwardDirection.z << std::endl;
}

// --- Calculate camera movement --- //
//void FirstPersonCamera::CameraMovement(std::unordered_set<Keys>* keys, float deltaTime) {
void FirstPersonCamera::CameraMovement(float deltaTime) {
	// Recalculate forward target for each new time (just re-checking)
	RecalculateForward();

	// Calculate right direction vector by using the forward
	Vector3 rightDirection = cameraUpAxis.Cross(forwardDirection);
	rightDirection.Normalize();

	// Set the new "empty" vector
	Vector3 direction(0.0f, 0.0f, 0.0f);

	// Horizontal Movement
	if (game->inputDevice->IsKeyDown(Keys::W))
		direction += forwardDirection;
	if (game->inputDevice->IsKeyDown(Keys::S))
		direction -= forwardDirection;
	if (game->inputDevice->IsKeyDown(Keys::A))
		direction -= rightDirection;
	if (game->inputDevice->IsKeyDown(Keys::D))
		direction += rightDirection;

	// Set velocity zero if no control
	if (!(game->inputDevice->IsKeyDown(Keys::W)) &&
		!(game->inputDevice->IsKeyDown(Keys::S)) &&
		!(game->inputDevice->IsKeyDown(Keys::A)) &&
		!(game->inputDevice->IsKeyDown(Keys::D)))
		direction = Vector3(0.0f, 0.0f, 0.0f);

	// Vertical Movement
	if (game->inputDevice->IsKeyDown(Keys::Space))
		direction -= cameraUpAxis;
	if (game->inputDevice->IsKeyDown(Keys::C))
		direction += cameraUpAxis;

	// If some keys are activated in one time - Normalize
	if (velocity.Length() > 0.0001f)
		velocity.Normalize();

	// Calculate speed
	float speed = 4.0f * deltaTime;
	//float speed = 4.0f;

	// Calculate velocity
	velocity = direction * speed;
}

// --- Update --- //
void FirstPersonCamera::Update() {
	// Recalculate forward target for each new time (just re-checking)
	RecalculateForward();
	
	cameraPosition += velocity * cameraControlSpeed;
	cameraWatchTarget += velocity * cameraControlSpeed;
	cameraMatrix.projection = Matrix::CreatePerspectiveFieldOfView(cameraFOV, aspectRatio, nearToPlaneDistance, farFromPlaneDistance);
	cameraMatrix.view = Matrix::CreateLookAt(cameraPosition, cameraWatchTarget, cameraUpAxis);
}

// --- Destroy Resources --- //
void FirstPersonCamera::DestroyResources() {
	// ???
}