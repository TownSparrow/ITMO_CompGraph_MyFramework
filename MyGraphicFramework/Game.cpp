#include "Game.h"
#include "DisplayWin32.h"
#include "TriangleComponent.h"
#include "MeshCreator.h"

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
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	depthStencilBuffer = nullptr;
	device->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	context->OMSetRenderTargets(1, &renderView, depthStencilView);

	ID3D11DepthStencilState* depthStencilState;

	//if (isPong) {
	//	D3D11_DEPTH_STENCIL_DESC depthDisabledDesc = {};

	//	depthDisabledDesc.DepthEnable = FALSE;
	//	depthDisabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	//	depthDisabledDesc.StencilEnable = FALSE;

	//	device->CreateDepthStencilState(&depthDisabledDesc, &depthStencilState);
	//	std::cout << "dfgdfg";
	//}
	//else {
	//	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	//	depthStencilDesc.DepthEnable = TRUE;
	//	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	//	depthStencilDesc.StencilEnable = FALSE;

	//	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	//};
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = FALSE;

	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	context->OMSetDepthStencilState(depthStencilState, 1);

	// --- Init: Set meshes to draw --- //
	// background circle
	//std::vector<UINT> stridesCircle = { 32 };
	//std::vector<UINT> offsetsCircle = { 0 };
	//TriangleComponent* circle = new TriangleComponent(Game::GetInstance());
	//Mesh circleMeshBackground = MeshCreator::GetInstance()->Circle(
	//	DirectX::XMFLOAT2(0.0f, 0.0f),
	//	0.5f,
	//	32,
	//	false,
	//	{ DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
	//);
	//circle->Initialize(shaderPath, circleMeshBackground.points, circleMeshBackground.indexes, stridesCircle, offsetsCircle);
	//Game::GetInstance()->components.push_back(circle);
}

// --- Create Back Buffer --- //
void Game::CreateBackBuffer() {
	auto res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
}

// --- Draw --- //
void Game::Draw() {
	// Set color: Shrek's Swamp Green
	//float color[] = { 0.1f, 0.2f, 0.0f, 1.0f };
	// Set color: Black
	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(renderView, color);
	//context->OMSetRenderTargets(1, &renderView, nullptr);
	context->OMSetRenderTargets(1, &renderView, depthStencilView);

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

	// Update every game component
	for (GameComponent* component : components) {
		component->Update();
	}
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

	// Message for exit
	std::cout << "That's all, folks!\n";
	return 0;
}

// --- Prerape next Frame --- //
void Game::PrepareFrame() {
	// Clear state of context for the new data
	context->ClearState();
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

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
	frameCount++;

	// Check time, calculate the FPS value and show in window
	if (totalTime > 1.0f) {
		float fps = frameCount / totalTime;
		totalTime -= 1.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(window->hWnd, text);

		frameCount = 0;
	}

	// Check for pong
	if (isPong) PongGame::GetInstance()->UpdateInterval(deltaTime);

	// Check for Solar System
	if (isSolarSystem) SolarSystem::GetInstance()->UpdateInterval(deltaTime);

	// Important order of render stages!
	PrepareFrame();
	Update();
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

void Game::KeyInputHadnler(std::unordered_set<Keys>* keys)
{
	auto curTime = std::chrono::steady_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	
	//this->inputDevice->RefreshKeyStates();
	activeCamera->CameraMovement(keys, deltaTime);
}

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
	solarSystem->Initialize(shaderPath);
}