#pragma once

#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>

#include "Game.h"
#include "TriangleComponent.h"
#include "MeshCreator.h"

class Game;

// Objects of this game: players and projectile ball
struct PongObject {
	TriangleComponent* triangleMesh;
	DirectX::BoundingBox collision;
	Vector3 position;
};

class PongGame {
private:
	// Singleton Instance and Empty Constructor
	static PongGame* pongGameInstance;
	PongGame() {};

	// --- Gameplay values --- //
	// Scores of players
	// For player 1
	UINT player1Score = 0;
	// For player2
	UINT player2Score = 0;

	// Scores Increase Values
	// For player 1
	UINT player1ScoreIncrease = 1;
	// For player 2
	UINT plyer2ScoreIncrease = 1;

	// Ball Projectile Settings
	Vector4 ballProjectileVelocity;
	float ballProjectileSpeed = 1.0f;
	float ballProjectileSpeedGrow = 0.01f;

	// Create One Collision
	DirectX::BoundingBox CreateCollision(std::vector<DirectX::XMFLOAT4> points);

	// Level Collisions
	// Top
	DirectX::BoundingBox topLevelCollision;
	// Bottom
	DirectX::BoundingBox bottomLevelCollision;
	
	// Point triggers collisions
	// For player 1 side
	DirectX::BoundingBox player1SideCollision;
	// For player 2 side
	DirectX::BoundingBox player2SideCollision;

	// Update One Collision
	void UpdateCollision(
		std::vector<DirectX::XMFLOAT4> points,
		DirectX::BoundingBox* collision,
		bool isCenterNeeded = false,
		Vector3 scale = { 1.0f, 1.0f, 1.0f }
	);

	// Start the game
	void Start(UINT playerWinner);

	// Animation booleans
	bool player1Animation = false;
	bool player2Animation = false;

public:
	// Get instance of pong
	static PongGame* GetInstance() {
		if (pongGameInstance == nullptr) {
			pongGameInstance = new PongGame();
		}
		return pongGameInstance;
	}

	// Pong objects
	PongObject* player1PongObject;
	PongObject* player2PongObject;
	PongObject* ballPongObject;

	// Playable controlled objects
	PlayerControlComponent* player1Controled;
	PlayerControlComponent* player2Controled;

	// Net values
	//UINT netCount = 0;
	//float netUpdateTime = 0.0f;

	// Keys array for input commads
	// bool keys[256] = {};

	// All main methods
	void Initialize(LPCWSTR shaderPath);
	void UpdatePlayer(UINT paddle, UINT direction, float deltaTime);
	void UpdateBall(float deltaTime);
	void CheckAllCollisions();
	void Update();
	void UpdateInterval(float deltaTime);
};

