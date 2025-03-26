#pragma once

#include <iostream>
#include <random>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>
#include <vector>

#include "TriangleComponent.h"
#include "TriangleWithTextureComponent.h"
#include "MeshCreator.h"

#include "OrbitalCamera.h"
#include "FirstPersonCamera.h"
#include "LightSystem.h"

#include "KatamariPlayer.h"

class Game;

using namespace DirectX::SimpleMath;

class KatamariGame {
private:
	static KatamariGame* katamariInstance;
	KatamariGame() {};

	OrbitalCamera* mainOrbitalCamera;
	FirstPersonCamera* mainFirstPersonCamera;
	Game* game;

	// Previous default values for base realisation
	// Now it is in local groups
	int objectsCount = 100;
	Vector3 rightUpMapCorner = Vector3(20, 0, 20);
	Vector3 leftDownMapCorner = Vector3(-20, 0, -20);

	KatamariPlayer* player;

	// Light sources
	PointLight* pointLight;
	DirectionalLight* directionalLight;

public:
	std::vector<Pickable*> pickables;

	KatamariGame(const KatamariGame& katamari) = delete;

	static KatamariGame* GetInstance() {
		if (katamariInstance == nullptr)
			katamariInstance = new KatamariGame();
		return katamariInstance;
	}

	void Initialize();
	void SpawnGround();
	void SpawnLittleObjectsGroup();
	void SpawnMediumObjectsGroup();
	void SpawnBigObjectsGroup();
	void SpawnRandomObjects(
		std::vector<LPCSTR> models,
		Vector3 leftDownMaxCorner,
		Vector3 rightDownMaxCorner,
		Vector3 leftDownMinCorner,
		Vector3 rightDownMinCorner,
		int objectsAmount
	);
	void Update();
	void UpdateInterval(float deltaTime);

};

