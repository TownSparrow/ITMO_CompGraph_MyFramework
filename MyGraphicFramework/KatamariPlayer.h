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
	//DirectX::BoundingBox collision;
	DirectX::BoundingOrientedBox collision;
	Vector3 position;
	Vector3 rotation;
};

using namespace DirectX::SimpleMath;

class KatamariPlayer {
private:
	Game* game;
	KatamariGame* katamariGame;

	//TriangleComponent* katamariMesh; 
	TriangleWithTextureComponent* katamariMesh;
	DirectX::BoundingSphere collision;

	OrbitalCamera* mainOrbit;

	Vector3 velocity = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	float collisionScale = 1.0f;

	float speed = 7.0f;
	float rotationSpeed = 7.0f;
	float radius = 0.5f;

	std::vector<Pickable*> collected;

	void MoveKatamari(float deltaTime);

public:
	KatamariPlayer(Game* gameInput);

	void CollisionCheck();
	void UpdateInterval(float deltaTime);
	void Update();

	OrbitalCamera* GetCamera();
};

