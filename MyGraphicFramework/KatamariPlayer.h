#pragma once

#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>

#include "MeshCreator.h"
#include "TriangleComponent.h"
#include "TriangleWithTextureComponent.h"

#include "OrbitalCamera.h"

class Game;
class KatamariGame;

struct Pickable {
	std::vector<TriangleWithTextureComponent*> mesh;
	DirectX::BoundingOrientedBox collision;
	Vector3 position;
	//Vector3 rotation;
	Quaternion rotation;
	bool isCollected;
	Vector3 localOffset;
	Quaternion localRotation;
};

using namespace DirectX::SimpleMath;

class KatamariPlayer {
private:
	Game* game;
	KatamariGame* katamariGame;

	TriangleWithTextureComponent* katamariMesh;
	DirectX::BoundingSphere collision;

	OrbitalCamera* mainOrbitalCam;
	Vector3 defaultCameraPosition = Vector3(4.0f, 4.0f, 4.0f);

	Vector3 velocity = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	//Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	Quaternion rotation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	Matrix localRotationMatrix = Matrix::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);

	float collisionScale = 1.0f;
	float currentSpeed = 0.0f;
	float growthDeltaSpeed = 0.05f;
	float maxSpeed = 7.0f;
	float rotationSpeed = 7.0f;
	float radius = 0.75f;
	bool isMovingForward = true;

	std::vector<Pickable*> collectedPickableObjects;

	float Inertia(float currentSpeed, bool isMoving);
	void Move(float deltaTime);

public:
	KatamariPlayer(Game* gameInput);

	void CheckCollision();
	void UpdateInterval(float deltaTime);
	void UpdatePlayer(Pickable* object);
	void Update();

	OrbitalCamera* GetCamera();
};

