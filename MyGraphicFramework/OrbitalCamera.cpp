#include "OrbitalCamera.h"
#include "Game.h"

#include <unordered_set>
#include "Keys.h"

#include "iostream"

// --- Init --- //
void OrbitalCamera::Initialize(Vector3 orbitInput, Vector3 lookPointInput, Vector3 targetInput) {
	orbitVector = orbitInput;

	cameraPosition = lookPointInput;
	cameraWatchTarget = targetInput;
	cameraUpAxis = Vector3(0.0f, 1.0f, 0.0f);

	cameraFOV = 30.0f;
	aspectRatio = static_cast<float>(game->screenWidth) / static_cast<float>(game->screenHeight);
	nearToPlaneDistance = 0.01f;
	farFromPlaneDistance = 100.0f;

	cameraMatrix.projection = Matrix::CreatePerspectiveFieldOfView(cameraFOV, aspectRatio, nearToPlaneDistance, farFromPlaneDistance);
	cameraMatrix.view = Matrix::CreateLookAt(cameraPosition, cameraWatchTarget, cameraUpAxis);

	cameraYaw = 0.0f;
	cameraPitch = 0.0f;

	Vector3 camPosition = Vector3(Vector3::Distance(cameraWatchTarget, cameraPosition), 0.0f, 0.0f);
	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(cameraYaw, 0, cameraPitch);
	orbitVector = Vector3::Transform(camPosition, rotationMatrix);
}

// --- Set Camera Position --- //
void OrbitalCamera::SetCameraPosition(Vector3 lookPointInput) { 
	cameraPosition = lookPointInput; 
}

// --- Set Watch Target --- //
void OrbitalCamera::SetWatchTarget(Vector3 targetInput) { 
	cameraWatchTarget = targetInput; 
}

// --- Set Up Axis --- //
void OrbitalCamera::SetUpAxis(Vector3 upAxisInput) { 
	cameraUpAxis = upAxisInput; 
}

// --- Camera Rotation --- //
void OrbitalCamera::CameraRotation(Vector2 mouseInput) {
	Vector3 camPosition = Vector3(Vector3::Distance(cameraWatchTarget, cameraPosition), 0.0f, 0.0f);

	cameraYaw += mouseInput.x * cameraSensitivity;
	cameraPitch -= mouseInput.y * cameraSensitivity;

	float pitchLimit = DirectX::XM_PIDIV2 - 0.01f;
	if (cameraPitch < -pitchLimit) cameraPitch = -pitchLimit;
	//if (cameraPitch > pitchLimit) cameraPitch = pitchLimit;
	if (cameraPitch > 0) cameraPitch = 0;

	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(cameraYaw, 0, cameraPitch);

	orbitVector = Vector3::Transform(camPosition, rotationMatrix);
}

// --- Camera Movement --- //
// There is no movement, actually... Just changing FOV
void OrbitalCamera::CameraMovement(std::unordered_set<Keys>* keys, float deltaTime) {
	//if (game->inputDevice->IsKeyDown(Keys::W)) {
	//	if (cameraFOV < 31) {
	//		cameraFOV += 0.1;
	//		std::cout << "Current FOV : " << cameraFOV << std::endl;
	//	}
	//}
	//if (game->inputDevice->IsKeyDown(Keys::S)) {
	//	if (cameraFOV > 30) {
	//		cameraFOV -= 0.1;
	//		std::cout << "Current FOV : " << cameraFOV << std::endl;
	//	}
	//}
}

// --- Update --- //
void OrbitalCamera::Update() {
	cameraMatrix.projection = Matrix::CreatePerspectiveFieldOfView(cameraFOV, aspectRatio, nearToPlaneDistance, farFromPlaneDistance);
	cameraMatrix.view = Matrix::CreateLookAt(cameraPosition, cameraWatchTarget, cameraUpAxis);

	if (game->inputDevice->IsKeyDown(Keys::Up)) {
		if (cameraFOV < 31) {
			cameraFOV += 0.01;
			std::cout << "Current FOV : " << cameraFOV << std::endl;
		}
	}
	if (game->inputDevice->IsKeyDown(Keys::Down)) {
		if (cameraFOV > 30) {
			cameraFOV -= 0.01;
			std::cout << "Current FOV : " << cameraFOV << std::endl;
		}
	}
}

// --- Destroy Resources --- //
void OrbitalCamera::DestroyResources() {
	// ???
}
