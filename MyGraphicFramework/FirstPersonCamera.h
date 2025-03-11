#pragma once

#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

// Base Camera
#include "Camera.h"

// Input Control
#include <unordered_set>
#include "Keys.h"

class FirstPersonCamera : public Camera {
private:
	// Camera's Field of View
	float cameraFOV;
	// Camera's Aspect Ratio
	float aspectRatio;
	// The destince near to plane
	float nearToPlaneDistance;
	// The distance far from plane
	float farFromPlaneDistance;

	// Camera control sensitivitiy
	float cameraControlSensitivity = 0.002f;
	// Camera Control Speed
	float cameraControlSpeed = 0.5f;

	// Camera Velocity Vector
	Vector3 velocity;
	// Camera Forward Vector
	Vector3 forwardDirection;

	// Camera Yaw Value
	float cameraYaw;
	// Camera Pitch Value
	float cameraPitch;

public:
	// Base constructor
	FirstPersonCamera(Game* game) : Camera(game) {};

	// Main methods
	void Initialize();
	void SetCameraPosition(Vector3 cameraPositionVector);
	void SetWatchTarget(Vector3 targetVector);
	void SetUpAxis(Vector3 upAxisVector);
	void RecalculateForward();
	void CameraRotation(Vector2 mouseInput);
	void CameraMovement(std::unordered_set<Keys>* keys, float deltaTime);
	void Update();
	void DestroyResources();

};

