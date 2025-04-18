#include "Game.h"
#include "DisplayWin32.h"
#include "TriangleComponent.h"
#include "TriangleWithTextureComponent.h"
#include "MeshCreator.h"

#include <random>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>

using namespace std;
using namespace DirectX::SimpleMath;

Game* Game::instance = nullptr;

// --- Init --- //
void Game::Initialize(
	int screenWidthInput,
	int screenHeightInput,
	LPCWSTR shaderPath
) {

	// --- Init: Window --- //
	screenWidth = screenWidthInput;
	screenHeight = screenHeightInput;

	window = new DisplayWin32(screenWidth, screenHeight, applicationName);
	window->Display();

	// --- Init: InputDevice --- //
	inputDevice = new InputDevice(GetInstance());

	// --- Init: Important Settings --- //
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = screenWidth;
	swapDesc.BufferDesc.Height = screenHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = window->hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	// --- Init: CreateDeviceAndSwapChain --- //
	auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context);

	if (FAILED(res))
	{
		// Well, that was unexpected
	}

	// --- Init: Create Back Buffer --- //
	CreateBackBuffer();

	// --- Init: Create Render Target View --- //
	res = device->CreateRenderTargetView(backBuffer, nullptr, &renderView);

	// --- Init: Create Depth Buffer --- //
	CreateDepthBuffer();
	
	CreateStencilStates();
	CreateRasterizerStates();
	CreateShadowQuadResources();

	// Adding Line Net
	//InitLineNet();

	// Init lights
	directionalLight = nullptr;
	//pointLight = nullptr;
	pointLights = {};
}

// --- Create Back Buffer --- //
void Game::CreateBackBuffer() {
	auto res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
}

// --- Create Depth Buffer --- //
void Game::CreateDepthBuffer() {
	/*D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	device->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	context->OMSetRenderTargets(1, &renderView, depthStencilView);

	ID3D11DepthStencilState* depthStencilState;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = FALSE;

	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	context->OMSetDepthStencilState(depthStencilState, 1);*/

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	device->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	context->OMSetRenderTargets(1, &renderView, depthStencilView);

	ID3D11DepthStencilState* depthStencilState;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = FALSE;

	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	context->OMSetDepthStencilState(depthStencilState, 1);
}

// --- Create Depth Stencil States --- //
void Game::CreateStencilStates() {
	// Back‑face pass: INCR
	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;
	ds.StencilReadMask = 0xFF;
	ds.StencilWriteMask = 0xFF;
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ds.BackFace = ds.FrontFace;
	//device->CreateDepthStencilState(&ds, &stencilBackState);

	// Front‑face pass: DECR
	//ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	ds.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	device->CreateDepthStencilState(&ds, &stencilFrontState);

	// Quad‑pass: stencil == 0
	D3D11_DEPTH_STENCIL_DESC ds2 = {};
	ds2.DepthEnable = FALSE;
	ds2.StencilEnable = TRUE;
	ds2.StencilReadMask = 0xFF;
	ds2.StencilWriteMask = 0;
	ds2.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	ds2.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds2.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds2.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ds2.BackFace = ds2.FrontFace;
	device->CreateDepthStencilState(&ds2, &noDoubleBlendState);
}

// --- Create Rasterizer States --- //
void Game::CreateRasterizerStates() {
	D3D11_RASTERIZER_DESC r = {};
	r.FillMode = D3D11_FILL_SOLID;
	r.CullMode = D3D11_CULL_NONE;
	r.DepthClipEnable = FALSE;
	device->CreateRasterizerState(&r, &rasterizerShadowBack);
	r.CullMode = D3D11_CULL_BACK;
	device->CreateRasterizerState(&r, &rasterizerShadowFront);
}

// --- Create Shadow Quad Resources --- //
void Game::CreateShadowQuadResources() {
	// Compile vertex buffer
	ID3DBlob* vsb = nullptr, * psb = nullptr, * err = nullptr;
	D3DCompileFromFile(L"./Shaders/ApplyShadowVolumeShader.hlsl",
		nullptr, nullptr,
		"VSMain", "vs_5_0",
		D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&vsb, &err);
	if (err) {
		OutputDebugStringA((char*)err->GetBufferPointer());
	}
	device->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(),
		nullptr, &quadVS);

	// InputLayout: POSITION, TEXCOORD
	D3D11_INPUT_ELEMENT_DESC ie[] = {
		D3D11_INPUT_ELEMENT_DESC { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		//D3D11_INPUT_ELEMENT_DESC { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	device->CreateInputLayout(ie, 2, vsb->GetBufferPointer(),
		vsb->GetBufferSize(), &quadIL);
	vsb->Release();

	// Compile pixel buffer
	D3DCompileFromFile(L"./Shaders/ApplyShadowVolumeShader.hlsl",
		nullptr, nullptr,
		"PSMain", "ps_5_0",
		D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&psb, &err);
	device->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(),
		nullptr, &quadPS);
	psb->Release();
}

// --- Draw shadow where stencil == 0 --- //
void Game::ApplyShadowStencil() {
	// 1) Creat vertex buffer for full-screen quad
	static ID3D11Buffer* quadVB = nullptr;
	if (!quadVB)
	{
		struct V { DirectX::XMFLOAT4 pos; DirectX::XMFLOAT2 uv; };
		V quad[4] = {
				{{-1,-1,0,1},{0,1}},
				{{-1, 1,0,1},{0,0}},
				{{ 1,-1,0,1},{1,1}},
				{{ 1, 1,0,1},{1,0}}
		};
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = sizeof(quad);
		D3D11_SUBRESOURCE_DATA sd = { quad };
		device->CreateBuffer(&bd, &sd, &quadVB);
	}

	// 2) Create alpha blend state
	static ID3D11BlendState* alphaBlendState = nullptr;
	if (!alphaBlendState)
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		device->CreateBlendState(&blendDesc, &alphaBlendState);
	}

	// 3) Reset viewport
	D3D11_VIEWPORT vp{ 0, 0, (float)screenWidth, (float)screenHeight, 0.f, 1.f };
	context->RSSetViewports(1, &vp);

	// 4) Depth Stencil Test
	context->OMSetDepthStencilState(noDoubleBlendState, /*ref=*/0);

	// 5) Reset rasterizer state
	context->RSSetState(nullptr);

	// 6) Turn on alpha-blending
	float blendFactor[4] = { 0,0,0,0 };
	context->OMSetBlendState(alphaBlendState, blendFactor, 0xFFFFFFFF);

	// 7) Connect shaders to quad-pass
	context->VSSetShader(quadVS, nullptr, 0);
	context->PSSetShader(quadPS, nullptr, 0);

	// 8) Connect IA to quad vertex buffer
	UINT stride = sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT2);
	UINT offset = 0;
	context->IASetInputLayout(quadIL);
	context->IASetVertexBuffers(0, 1, &quadVB, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 9) Draw
	context->Draw(4, 0);

	// 10) Reset states
	context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(nullptr, 0);

	// 11) Reset shaders
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}

// --- Render Shadow Map --- //
void Game::RenderShadowMap() {
	dirLightShadows->SetRenderTarget(context);
	dirLightShadows->ClearRenderTarget(context, Vector4(0.0f, 0.0f, 0.0f, 1.0f));

	for (TriangleWithTextureComponent* mesh : meshes) {
		mesh->LightUpdate();
		mesh->LightRender();
	}
}

// --- Create Shadow Volume Depth --- //
void Game::CreateShadowVolumeDepth() {
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};

	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dsDesc.StencilEnable = TRUE;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//HRESULT hr = device->CreateDepthStencilState(&dsDesc, &ShadowVolumeDSState);
	//context->OMSetDepthStencilState(ShadowVolumeDSState, 1);
}

// --- Render Color --- //
void Game::RenderColor() {
	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(renderView, color);
	context->OMSetRenderTargets(1, &renderView, depthStencilView);

	for (TriangleWithTextureComponent* mesh : meshes) {
		mesh->Draw();
	}
}

// --- Draw --- //
void Game::Draw() {
	// Set color: Shrek's Swamp Green
	//float color[] = { 0.1f, 0.2f, 0.0f, 1.0f };
	// Set color: Black
	//float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//context->ClearRenderTargetView(renderView, color);
	////context->OMSetRenderTargets(1, &renderView, nullptr);
	//context->OMSetRenderTargets(1, &renderView, depthStencilView);

	context->OMSetDepthStencilState(ShadowVolumeDSState, 0);

	// Draw every Game Component in the cycle
	for (GameComponent* component : components) {
		component->Draw();
	}

	// Clear context swapChain for next steps
	context->OMSetRenderTargets(0, nullptr, nullptr);
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

// --- Update --- //
void Game::Update() {
	//float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	//float orthoHeight = 2.0f;
	//float orthoWidth = orthoHeight * aspectRatio;
	if (isPong) {
		PongGame* pongGame = PongGame::GetInstance();
		pongGame->Update();
		//if (pongGame->netUpdateTime > 0.5f) {
		//	for (int i = 0; i < components.size() - 3; i++) {
		//		if (i == pongGame->netCount) components[i]->constData.color = Vector4(0.5f, 0.5f, 0.5f, 0.0f);
		//		else components[i]->constData.color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		//	}
		//	pongGame->netCount += 1;
		//	if (pongGame->netCount == 9) pongGame->netCount = 0;
		//	pongGame->netUpdateTime = 0.0f;
		//};
	};

	if (isSolarSystem) {
		SolarSystem* solarSystem = SolarSystem::GetInstance();
		solarSystem->Update();
	}

	if (isKatamari) {
		KatamariGame* katamari = KatamariGame::GetInstance();
		katamari->Update();
	}

	// Update every game component
	for (GameComponent* component : components) {
		component->Update();
	}
}

// --- Update Light --- //
void Game::UpdateLight() {
	Vector3 sceneCenter(0.0f, 0.0f, 0.0f);
	float distance = 50.0f;
	Vector3 lightCameraPos = sceneCenter - directionalLight->direction * distance;
	//lightView = Matrix::CreateLookAt(lightCameraPos, sceneCenter, Vector3::Up);
	lightView = Matrix::CreateLookAt(lightCameraPos, sceneCenter, Vector3(0,-1,0));

	float orthographicWidth = 128;
	float orthographicHeight = 128;
	float orthographicNearPlane = 0.1f;
	float orthographicFarPlane = 100.0f;
	lightProjection = Matrix::CreateOrthographic(
		orthographicWidth,
		orthographicHeight,
		orthographicNearPlane,
		orthographicFarPlane
	);
}

// --- End Frame --- //
void Game::EndFrame() {
	// Some kinda of end frame logic in the future...
	// std::cout << "EndFrame";
}

// --- Exit Gmame --- //
int Game::Exit() {
	// Delete classes's objects
	delete window;
	delete inputDevice;

	// Destroy every game component
	for (GameComponent* component : components) {
		component->DestroyResources();
		delete component;
	}

	// Release all important stuff
	context->Release();
	device->Release();
	swapChain->Release();
	backBuffer->Release();
	renderView->Release();

	// Release lights
	if (directionalLight != nullptr) delete directionalLight;
	//if (pointLight != nullptr) delete pointLight;
	//if (!pointLights.empty()) {
	//	pointLights.clear();
	//}
	if (pointLights.size() != 0) {
		for (int i = 0; i < pointLights.size(); i++) {
			delete pointLights[i];
		}
	}

	// Message for exit
	std::cout << "That's all, folks!\n";
	return 0;
}

// --- Prerape next Frame --- //
void Game::PrepareFrame() {
	// Clear state of context for the new data
	context->ClearState();
	if (depthStencilView) {
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// Set all default settings for viewport
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	// Apply new viewport
	context->RSSetViewports(1, &viewport);
}

// --- Update each interval --- //
void Game::UpdateInterval() {
	// Find difference between time and increase values
	auto curTime = std::chrono::steady_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;
	totalTime += deltaTime;
	timeForFPS += deltaTime;
	frameCount++;

	// Check time, calculate the FPS value and show in window
	if (timeForFPS > 1.0f) {
		float fps = frameCount / timeForFPS;
		timeForFPS -= 1.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(window->hWnd, text);

		frameCount = 0;
	}

	activeCamera->CameraMovement(deltaTime);

	// Check for pong
	if (isPong) PongGame::GetInstance()->UpdateInterval(deltaTime);

	// Check for Solar System
	if (isSolarSystem) SolarSystem::GetInstance()->UpdateInterval(deltaTime);

	// Check for Katamari
	if (isKatamari) KatamariGame::GetInstance()->UpdateInterval(deltaTime);

	// Important order of render stages!
	// Shadows work only for Katamari
	if (isKatamari && !isStencilShadowEnabled) {
		UpdateLight();
		RenderShadowMap();
	}
	PrepareFrame();
	Update();

	if (isStencilShadowEnabled) {
		RenderColor();

		// 1) Back‑face pass: Incr
		context->RSSetState(rasterizerShadowBack);
		//context->OMSetDepthStencilState(stencilBackState, 1);
		context->OMSetDepthStencilState(stencilFrontState, 1);
		for (auto* m : meshes)
			m->RenderShadowVolume();

		// 2) Front‑face pass: Decr
		context->RSSetState(rasterizerShadowFront);
		context->OMSetDepthStencilState(stencilFrontState, 1);
		for (auto* m : meshes)
			m->RenderShadowVolume();

		// 3) Reset
		context->RSSetState(nullptr);
		context->OMSetDepthStencilState(nullptr, 0);
		context->OMSetRenderTargets(1, &renderView, depthStencilView);

		// 4) Quad‑pass
		context->OMSetDepthStencilState(noDoubleBlendState, /*ref=*/0);
		ApplyShadowStencil();
	}

	Draw();
	EndFrame();
}

// --- Processing of messages --- //
void Game::MessageHandler() {
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// If windows signals to end the application then exit out.
	if (msg.message == WM_QUIT) {
		isExitRequested = true;
	}
}

//void Game::KeyInputHadnler(std::unordered_set<Keys>* keys)
//{
//	auto curTime = std::chrono::steady_clock::now();
//	float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
//	
//	this->inputDevice->RefreshKeyStates();
//	activeCamera->CameraMovement(keys, deltaTime);
//}

void Game::MouseInputHandler(Vector2 mouseInput)
{
	activeCamera->CameraRotation(mouseInput);
}

// --- Running the game --- //
void Game::Run() {
	// Set initial values for time and frames
	PrevTime = std::chrono::steady_clock::now();
	totalTime = 0;
	frameCount = 0;

	// While no exit command game works
	while (!isExitRequested) {
		MessageHandler();
		UpdateInterval();
	}
	Exit();
}

// --- EXAMPLE --- //
// Make 2 squares
// Make control for them: WASD and arrows
void Game::InitTwoSquaresExample(LPCWSTR shaderPath) {
	// example mesh with control 1
	std::vector<UINT> stridesPlayable1 = { 32 };
	std::vector<UINT> offsetsPlayable1 = { 0 };
	TriangleComponent* squarePlayable1 = new TriangleComponent(Game::GetInstance());
	Mesh squareMeshPlayable1 = MeshCreator::GetInstance()->Square(
		DirectX::XMFLOAT2(-0.5f, 0.0f),
		0.5f,
		0.5f,
		false,
		{ DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }
	);
	squarePlayable1->Initialize(shaderPath, squareMeshPlayable1.points, squareMeshPlayable1.indexes, stridesPlayable1, offsetsPlayable1);
	squarePlayable1->transforms.scale = Matrix::CreateScale(0.5f, 0.5f, 0.05f);
	PlayerControlComponent* controlPlayable1 = new PlayerControlComponent(Game::GetInstance(), squarePlayable1, 0);

	// example mesh with control 2
	std::vector<UINT> stridesPlayable2 = { 32 };
	std::vector<UINT> offsetsPlayable2 = { 0 };
	TriangleComponent* squarePlayable2 = new TriangleComponent(Game::GetInstance());
	Mesh squareMeshPlayable2 = MeshCreator::GetInstance()->Square(
		DirectX::XMFLOAT2(0.5f, 0.0f),
		0.3f,
		0.3f,
		false,
		{ DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
	);
	squarePlayable2->Initialize(shaderPath, squareMeshPlayable2.points, squareMeshPlayable2.indexes, stridesPlayable2, offsetsPlayable2);
	squarePlayable2->transforms.scale = Matrix::CreateScale(0.5f, 0.5f, 0.05f);
	PlayerControlComponent* controlPlayable2 = new PlayerControlComponent(Game::GetInstance(), squarePlayable2, 1);

	Game::GetInstance()->components.push_back(squarePlayable1);
	Game::GetInstance()->components.push_back(controlPlayable1);
	Game::GetInstance()->components.push_back(squarePlayable2);
	Game::GetInstance()->components.push_back(controlPlayable2);
}

// Make pong game
void Game::InitPongGame(LPCWSTR shaderPath) {
	isPong = true;
	
	PongGame* pongGame = PongGame::GetInstance();
	pongGame->Initialize(shaderPath);
}

// Make Solar System
void Game::InitSolarSystem(LPCWSTR shaderPath) {
	isSolarSystem = true;

	SolarSystem* solarSystem = SolarSystem::GetInstance();
	CreateDepthBuffer();
	solarSystem->Initialize(shaderPath);
}

void Game::InitKatamari(LPCWSTR shaderPath) {
	isKatamari = true;

	KatamariGame* katamari = KatamariGame::GetInstance();
	CreateDepthBuffer();
	//dirLightShadows = new ShadowMap();
	//dirLightShadows->Initialize(device, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	katamari->Initialize();
	for (TriangleWithTextureComponent* mesh : meshes) {
		//mesh->CreateShadowShaders();
		mesh->CreateShadowVolumesShaders();
	}
}

void Game::InitLineNet() {
	std::vector<UINT> strides = { 32 };
	std::vector<UINT> offsets = { 0 };

	std::vector<DirectX::XMFLOAT4> lines;

	
	for (int i = 0; i <= 400; i++) {
		lines.push_back(Vector4(-100.0f, 0.0f, -100.0f + i, 1.0f));
		lines.push_back(Vector4(0.2f, 0.2f, 0.2f, 0.2f));
		lines.push_back(Vector4(100.0f, 0.0f, -100.0f + i, 1.0f));
		lines.push_back(Vector4(0.2f, 0.2f, 0.2f, 0.2f));
	}

	//std::vector<int> indexes;

	LineComponent* linesComponent1 = new LineComponent(GetInstance());
	linesComponent1->Initialize(
		L"./Shaders/MainShader.hlsl",
		lines, 
		//indexes, 
		strides, 
		offsets
	);
	components.push_back(linesComponent1);

	LineComponent* linesComponent2 = new LineComponent(GetInstance());
	linesComponent2->Initialize(
		L"./Shaders/MainShader.hlsl",
		lines,
		//indexes, 
		strides,
		offsets
	);
	linesComponent2->transforms.rotate = Matrix::CreateRotationY(DirectX::XM_PIDIV2);
	components.push_back(linesComponent2);
}
