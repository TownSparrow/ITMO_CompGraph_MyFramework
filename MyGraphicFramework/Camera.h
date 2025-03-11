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

#include "GameComponent.h"

#include <unordered_set>
#include "Keys.h"

// Camera Matrix
struct CameraMatrix {
	Matrix view;
	Matrix projection;
};

class Camera : public GameComponent {
public:
	// Main Constructor
	Camera(Game* game) : GameComponent(game) {}
	
	// Camera Matrix
	CameraMatrix cameraMatrix;

	// Main parametres of camera: 
	// Camera position
	Vector3 cameraPosition;
	// Where camera has to look
	Vector3 cameraWatchTarget;
	// Camera's up-axis
	Vector3 cameraUpAxis;

	// Main methods
	void Initialize() {};
	virtual void CameraRotation(Vector2 mouseInput) {};
	virtual void CameraMovement(std::unordered_set<Keys>* keys, float deltaTime) {};
	virtual void Draw() {};
	virtual void Update() {};
	virtual void DestroyResources() {};
};