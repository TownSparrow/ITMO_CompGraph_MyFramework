#pragma once

#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>
#include <vector>
#include <mutex>

#include <SimpleMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "InputDevice.h"
#include "GameComponent.h"
#include "DisplayWin32.h"

// Include new tasks h-files:
#include "PlayerControlComponent.h"
#include "PongGame.h"
#include "SolarSystem.h"
#include "Camera.h"
#include "FirstPersonCamera.h"
#include "OrbitalCamera.h"
#include "KatamariGame.h"
#include "LineComponent.h"
#include "LightSystem.h"
#include "ShadowMap.h"

#include <unordered_set>
#include "Keys.h"

class DisplayWin32;

class Game {
private:
	Game() {};
	static Game* instance;
	
	int SHADOW_MAP_SIZE = 4096*2;

public:
	DisplayWin32* window;
	std::vector<GameComponent*> components;
	std::vector<TriangleWithTextureComponent*> meshes;
	InputDevice* inputDevice;
	MSG msg = {};
	bool isExitRequested = false;

	int screenWidth;
	int screenHeight;

	LPCWSTR applicationName = L"MyGraphicFramework";

	ID3D11Texture2D* backBuffer;
	ID3D11RenderTargetView* renderView;
	ID3D11DeviceContext* context;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	IDXGISwapChain* swapChain;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;

	//stencil shadows
	ID3D11DepthStencilState* ShadowVolumeDSState;
	// Stencil-states
	ID3D11DepthStencilState* stencilBackState = nullptr;
	ID3D11DepthStencilState* stencilFrontState = nullptr;
	ID3D11DepthStencilState* noDoubleBlendState = nullptr;
	// Rasterizer-states
	ID3D11RasterizerState* rasterizerShadowBack = nullptr;
	ID3D11RasterizerState* rasterizerShadowFront = nullptr;
	// Quad‑pass 
	ID3D11VertexShader* quadVS = nullptr;
	ID3D11PixelShader* quadPS = nullptr;
	ID3D11InputLayout* quadIL = nullptr;

	float totalTime = 0;
	float timeForFPS = totalTime;
	unsigned int frameCount = 0;
	std::chrono::time_point<std::chrono::steady_clock> PrevTime;

	Game(const Game& gameObject) = delete;

	static Game* GetInstance() {
		if (instance == nullptr) {
			instance = new Game();
		}
		return instance;
	}

	Camera* activeCamera;
	DirectionalLight* directionalLight;
	//PointLight* pointLight;
	std::vector<PointLight*> pointLights;

	// Shadows
	Matrix lightView;
	Matrix lightProjection;
	ShadowMap* dirLightShadows;

	// Is stencil shadow
	bool isStencilShadowEnabled = true;

	void Initialize(
		int screenWidthInput,
		int screenHeightInput,
		LPCWSTR shaderPath
	);
	void CreateBackBuffer();
	void CreateDepthBuffer();
	void CreateStencilStates();
	void CreateRasterizerStates();
	void CreateShadowQuadResources();
	void ApplyShadowStencil();
	void CreateShadowVolumeDepth();
	void RenderColor();
	//void CreateShadowVolumes();
	void Draw();
	void Update();
	void EndFrame();
	int Exit();
	void UpdateLight();
	void RenderShadowMap();
	void PrepareFrame();
	void UpdateInterval();
	void MessageHandler();
	//void KeyInputHadnler(std::unordered_set<Keys>* keys);
	void MouseInputHandler(Vector2 mouseInput);
	void Run();
	void InitLineNet();

	// --- Additional methods --- //
	// Task 1
	void InitTwoSquaresExample(LPCWSTR shaderPath);
	
	// Pong
	bool isPong = false;
	void InitPongGame(LPCWSTR shaderPath);

	// Solar Systen
	bool isSolarSystem = false;
	void InitSolarSystem(LPCWSTR shaderPath);

	// Katamari
	// Solar Systen
	bool isKatamari = false;
	void InitKatamari(LPCWSTR shaderPath);
};