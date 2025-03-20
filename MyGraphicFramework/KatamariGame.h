#pragma once

#include <iostream>
#include <random>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>

#include "TriangleComponent.h"
#include "TriangleWithTextureComponent.h"
#include "MeshCreator.h"

#include "OrbitalCamera.h"
#include "FirstPersonCamera.h"

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

	int objectsCount = 3;
	Vector3 ruMapCorner = Vector3(20, 0, 20);
	Vector3 ldMapCorner = Vector3(0, 0, 0);

	KatamariPlayer* ball;

public:
	std::vector<Pickable*> pickables;

	KatamariGame(const KatamariGame& katamari) = delete;

	static KatamariGame* GetInstance() {
		if (katamariInstance == nullptr)
			katamariInstance = new KatamariGame();
		return katamariInstance;
	}

	void Initialize();
	void RandomObjectGeneration();
	void Update();
	void UpdateInterval(float deltaTime);

};

