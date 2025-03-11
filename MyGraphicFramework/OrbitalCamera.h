#pragma once

#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include <SimpleMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "Camera.h"

#include <unordered_set>
#include "Keys.h"

class OrbitalCamera : public Camera {
private:
	float cameraFOV;
	float aspectRatio;
	float nearToPlaneDistance;
	float farFromPlaneDistance;

	float cameraSensitivity = 0.005f;
	float cameraSpeed = 0.5f;

	float cameraYaw;
	float cameraPitch;

public:
	OrbitalCamera(Game* gameInput) : Camera(gameInput) {};

	Vector3 orbitVector;

	void Initialize(Vector3 orbitInput, Vector3 lookPointInput, Vector3 targetInput);
	void SetCameraPosition(Vector3 lookPointInput);
	void SetWatchTarget(Vector3 targetInput);
	void SetUpAxis(Vector3 upAxisInput);
	void CameraRotation(Vector2 mouseInput);
	void CameraMovement(std::unordered_set<Keys>* keys, float deltaTime);
	void Update();
	void DestroyResources();

};

