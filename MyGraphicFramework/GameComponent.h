#pragma once

#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//struct ConstData {
//	Matrix transformations;
//	Matrix view;
//	Matrix projection;
//	Vector4 color;
//};
//
//struct Transformations {
//	Matrix move;
//	Matrix rotate;
//	Matrix scale;
//};

class Game;

class GameComponent {
public:
	//Game* game;

	//Transformations transforms;
	//ConstData constData;

	//GameComponent(Game* gameInput) : game(gameInput) {}
	//~GameComponent() {};

	//void Initialize() {};

	//virtual void Draw(ConstData* data = nullptr) {};
	//virtual void Update(ConstData* data = nullptr) {};

	//void Reload() {};

	//virtual void DestroyResources() {};

	Game* game;

	GameComponent(Game* gameInput) : game(gameInput) {}
	~GameComponent() {};

	void Initialize() {};

	virtual void Draw() {};
	virtual void Update() {};

	void Reload() {};

	virtual void DestroyResources() {};

};