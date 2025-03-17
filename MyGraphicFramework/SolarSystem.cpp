#include "SolarSystem.h"
#include "Game.h"

#include <iostream>

// Instance
SolarSystem* SolarSystem::solarSystemInstance = nullptr;

// --- Init --- //
void SolarSystem::Initialize(LPCWSTR shaderPath) {
	// Getting instance of game
	game = Game::GetInstance();
	
	// Default strides and offsets
	std::vector<UINT> strides = { 32 };
	std::vector<UINT> offsets = { 0 };

	//Solar System center
	PlanetObject* solarSystemCenter = new PlanetObject{
		nullptr,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f,0.0f, 0.0f),
		nullptr
	};

	//MESHES Init

	// Test template
	//TriangleComponent* testMesh = new TriangleComponent(game);
	//Mesh testSphere = MeshCreator::GetInstance()->Sphere(
	//	Vector3(0, 0, 0),
	//	0.2,
	//	32,
	//	16,
	//	true,
	//	{ Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.8f, 0.0f, 1.0f) }
	//);
	//testMesh->Initialize(
	//	shaderPath,
	//	testSphere.points,
	//	testSphere.indexes,
	//	strides,
	//	offsets
	//);
	//game->components.push_back(testMesh);

	//PlanetObject* test = new PlanetObject{
	//	testMesh,
	//	0.0f,
	//	0.6f,
	//	0.0f,
	//	0.1f,
	//	Vector3(2.0f, 0.0f, 0.0f),
	//	Vector3(2.0f, 0.0f, 0.0f),
	//	sun
	//};
	//objectsArray.push_back(test);
	//test->planetMesh->transforms.move = Matrix::CreateTranslation(test->centerPosition);

	//OrbitalCamera* testCam = new OrbitalCamera(game);
	//testCam->Initialize(orbitalCameraDefaultPosition, test->centerPosition - orbitalCameraDefaultPosition, test->centerPosition);
	//OrbitalCameraObject* testCamObject = new OrbitalCameraObject{ testCam, test };
	//camerasArray.push_back(testCamObject);
	//game->components.push_back(testCam);

	//Sun
	TriangleComponent* sunMesh = new TriangleComponent(game);
	//Mesh sunCube = MeshCreator::GetInstance()->Cube(
	//	Vector3(0,0,0),
	//	0.5, 
	//	false,
	//	{ Vector4(1.0f, 0.8f, 0.0f, 1.0f) }
	//);
	Mesh sunSphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.5,
		32,
		16,
		false,
		{ Vector4(1.0f, 1.0f, 0.0f, 1.0f) }
	);
	sunMesh->Initialize(
		shaderPath,
		sunSphere.points, 
		sunSphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(sunMesh);

	PlanetObject* sun = new PlanetObject{
		sunMesh,
		0.0f,
		0.1f,
		0.0f,
		0.0f,
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, 0.0f, 0.0f),
		solarSystemCenter
	};
	objectsArray.push_back(sun);

	OrbitalCamera* sunCam = new OrbitalCamera(game);
	sunCam->Initialize(orbitalCameraDefaultPosition, sun->centerPosition - orbitalCameraDefaultPosition, sun->centerPosition);
	OrbitalCameraObject* sunCamObject = new OrbitalCameraObject{ sunCam, sun };
	camerasArray.push_back(sunCamObject);
	game->components.push_back(sunCam);

	// Mercury
	TriangleComponent* mercuryMesh = new TriangleComponent(game);
	Mesh mercurySphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.2,
		32,
		16,
		false,
		{ Vector4(1.0f, 0.5f, 0.0f, 1.0f) }
	);
	mercuryMesh->Initialize(
		shaderPath,
		mercurySphere.points,
		mercurySphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(mercuryMesh);

	PlanetObject* mercury = new PlanetObject{
		mercuryMesh,
		0.0f,
		1.6f,
		0.0f,
		0.2f,
		Vector3(2.0f, 0.0f, 1.0f),
		Vector3(2.0f, 0.0f, 1.0f),
		sun
	};
	objectsArray.push_back(mercury);
	mercury->planetMesh->transforms.move = Matrix::CreateTranslation(mercury->centerPosition);

	OrbitalCamera* mercuryCam = new OrbitalCamera(game);
	mercuryCam->Initialize(orbitalCameraDefaultPosition, mercury->centerPosition - orbitalCameraDefaultPosition, mercury->centerPosition);
	OrbitalCameraObject* mercuryCamObject = new OrbitalCameraObject { mercuryCam, mercury };
	camerasArray.push_back(mercuryCamObject);
	game->components.push_back(mercuryCam);

	// Venus
	TriangleComponent* venusMesh = new TriangleComponent(game);
	Mesh venusSphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.225,
		32,
		16,
		false,
		{ Vector4(0.2f, 0.2f, 0.0f, 1.0f) }
	);
	venusMesh->Initialize(
		shaderPath,
		venusSphere.points,
		venusSphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(venusMesh);

	PlanetObject* venus = new PlanetObject{
		venusMesh,
		0.0f,
		1.8f,
		0.4f,
		0.4f,
		Vector3(3.0f, 0.0f, -0.5f),
		Vector3(3.0f, 0.0f, -0.5f),
		sun
	};
	objectsArray.push_back(venus);
	venus->planetMesh->transforms.move = Matrix::CreateTranslation(venus->centerPosition);

	OrbitalCamera* venusCam = new OrbitalCamera(game);
	venusCam->Initialize(orbitalCameraDefaultPosition, venus->centerPosition - orbitalCameraDefaultPosition, venus->centerPosition);
	OrbitalCameraObject* venusCamObject = new OrbitalCameraObject{ venusCam, venus };
	camerasArray.push_back(venusCamObject);
	game->components.push_back(venusCam);

	// Earth
	TriangleComponent* earthMesh = new TriangleComponent(game);
	Mesh earthSphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.25,
		32,
		16,
		false,
		{ Vector4(0.0f, 0.1f, 8.0f, 1.0f) }
	);
	earthMesh->Initialize(
		shaderPath,
		earthSphere.points,
		earthSphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(earthMesh);

	PlanetObject* earth = new PlanetObject{
		earthMesh,
		0.0f,
		0.6f,
		0.0f,
		0.1f,
		Vector3(4.0f, 0.0f, 0.0f),
		Vector3(4.0f, 0.0f, 0.0f),
		sun
	};
	objectsArray.push_back(earth);
	earth->planetMesh->transforms.move = Matrix::CreateTranslation(earth->centerPosition);

	OrbitalCamera* earthCam = new OrbitalCamera(game);
	earthCam->Initialize(orbitalCameraDefaultPosition, earth->centerPosition - orbitalCameraDefaultPosition, earth->centerPosition);
	OrbitalCameraObject* earthCamObject = new OrbitalCameraObject{ earthCam, earth };
	camerasArray.push_back(earthCamObject);
	game->components.push_back(earthCam);

	//Moon
	TriangleComponent* moonMesh = new TriangleComponent(game);
	Mesh moonCube = MeshCreator::GetInstance()->Cube(
		Vector3(0, 0, 0),
		0.1,
		true,
		{ Vector4(0.8f, 0.8f, 0.8f, 1.0f), Vector4(0.3f, 0.3f, 0.3f, 1.0f) }
	);
	moonMesh->Initialize(
		shaderPath,
		moonCube.points,
		moonCube.indexes,
		strides,
		offsets
	);
	game->components.push_back(moonMesh);

	PlanetObject* moon = new PlanetObject{
		moonMesh,
		0.0f,
		0.3f,
		0.0f,
		0.8f,
		Vector3(0.5f, 0.0f, 0.0f),
		Vector3(0.5f, 0.0f, 0.0f),
		earth
	};
	objectsArray.push_back(moon);
	moon->planetMesh->transforms.move = Matrix::CreateTranslation(moon->centerPosition);

	//OrbitalCamera* moonCam = new OrbitalCamera(game);
	//moonCam->Initialize(orbitalCameraDefaultPosition, moon->centerPosition - orbitalCameraDefaultPosition, moon->centerPosition);
	//OrbitalCameraObject* moonCamObject = new OrbitalCameraObject{ moonCam, moon };
	//camerasArray.push_back(moonCamObject);
	//game->components.push_back(moonCam);

	//Mars
	TriangleComponent* marsMesh = new TriangleComponent(game);
	Mesh marsSphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.15,
		32,
		16,
		false,
		{ Vector4(1.0f, 0.2f, 0.0f, 1.0f) }
	);
	marsMesh->Initialize(
		shaderPath,
		marsSphere.points,
		marsSphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(marsMesh);

	PlanetObject* mars = new PlanetObject{
		marsMesh,
		0.0f,
		0.3f,
		0.0f,
		0.3f,
		Vector3(5.0f, 0.0f, -2.0f),
		Vector3(5.0f, 0.0f, -2.0f),
		sun
	};
	objectsArray.push_back(mars);
	mars->planetMesh->transforms.move = Matrix::CreateTranslation(mars->centerPosition);

	OrbitalCamera* marsCam = new OrbitalCamera(game);
	marsCam->Initialize(orbitalCameraDefaultPosition, mars->centerPosition - orbitalCameraDefaultPosition, mars->centerPosition);
	OrbitalCameraObject* marsCamObject = new OrbitalCameraObject{ marsCam, mars };
	camerasArray.push_back(marsCamObject);
	game->components.push_back(marsCam);

	// Fobos
	TriangleComponent* fobosMesh = new TriangleComponent(game);
	Mesh fobosCube = MeshCreator::GetInstance()->Cube(
		Vector3(0, 0, 0),
		0.05,
		true,
		{ Vector4(0.8f, 0.8f, 0.8f, 1.0f), Vector4(0.1f, 0.2f, 0.3f, 1.0f) }
	);
	fobosMesh->Initialize(
		shaderPath,
		fobosCube.points,
		fobosCube.indexes,
		strides,
		offsets
	);
	game->components.push_back(fobosMesh);

	PlanetObject* fobos = new PlanetObject{
		fobosMesh,
		0.0f,
		1.5f,
		0.0f,
		0.8f,
		Vector3(0.25f, 0.0f, -0.25f),
		Vector3(0.25f, 0.0f, -0.25f),
		mars
	};
	objectsArray.push_back(fobos);
	fobos->planetMesh->transforms.move = Matrix::CreateTranslation(fobos->centerPosition);

	//Deimos
	TriangleComponent* deimosMesh = new TriangleComponent(game);
	Mesh deimosCube = MeshCreator::GetInstance()->Cube(
		Vector3(0, 0, 0),
		0.08,
		true,
		{ Vector4(0.3f, 0.5f, 0.5f, 1.0f), Vector4(0.2f, 0.3f, 0.3f, 1.0f) }
	);
	deimosMesh->Initialize(
		shaderPath,
		deimosCube.points,
		deimosCube.indexes,
		strides,
		offsets
	);
	game->components.push_back(deimosMesh);

	PlanetObject* deimos = new PlanetObject{
		deimosMesh,
		0.0f,
		0.6f,
		0.0f,
		0.8f,
		Vector3(0.5f, 0.0f, 0.5f),
		Vector3(0.5f, 0.0f, 0.5f),
		mars
	};
	objectsArray.push_back(deimos);
	deimos->planetMesh->transforms.move = Matrix::CreateTranslation(deimos->centerPosition);
	
	//Jupiter
	TriangleComponent* jupiterMesh = new TriangleComponent(game);
	Mesh jupiterSphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.4,
		32,
		16,
		false,
		{ Vector4(0.4f, 0.4f, 0.0f, 1.0f) }
	);
	jupiterMesh->Initialize(
		shaderPath,
		jupiterSphere.points,
		jupiterSphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(jupiterMesh);

	PlanetObject* jupiter = new PlanetObject{
		jupiterMesh,
		0.0f,
		0.5f,
		0.0f,
		0.6f,
		Vector3(8.0f, 0.0f, -2.0f),
		Vector3(8.0f, 0.0f, -2.0f),
		sun
	};
	objectsArray.push_back(jupiter);
	jupiter->planetMesh->transforms.move = Matrix::CreateTranslation(jupiter->centerPosition);

	OrbitalCamera* jupiterCam = new OrbitalCamera(game);
	jupiterCam->Initialize(orbitalCameraDefaultPosition, jupiter->centerPosition - orbitalCameraDefaultPosition, jupiter->centerPosition);
	OrbitalCameraObject* jupiterCamObject = new OrbitalCameraObject{ jupiterCam, jupiter };
	camerasArray.push_back(jupiterCamObject);
	game->components.push_back(jupiterCam);

	//Saturn
	TriangleComponent* saturnMesh = new TriangleComponent(game);
	Mesh saturnSphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.5,
		32,
		16,
		false,
		{ Vector4(0.3f, 0.5f, 0.0f, 1.0f) }
	);
	saturnMesh->Initialize(
		shaderPath,
		saturnSphere.points,
		saturnSphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(saturnMesh);

	PlanetObject* saturn = new PlanetObject{
		saturnMesh,
		0.0f,
		0.5f,
		0.0f,
		0.5f,
		Vector3(10.0f, 0.0f, 1.0f),
		Vector3(10.0f, 0.0f, 1.0f),
		sun
	};
	objectsArray.push_back(saturn);
	saturn->planetMesh->transforms.move = Matrix::CreateTranslation(saturn->centerPosition);

	OrbitalCamera* saturnCam = new OrbitalCamera(game);
	saturnCam->Initialize(orbitalCameraDefaultPosition, saturn->centerPosition - orbitalCameraDefaultPosition, saturn->centerPosition);
	OrbitalCameraObject* testCamObject = new OrbitalCameraObject{ saturnCam, saturn };
	camerasArray.push_back(testCamObject);
	game->components.push_back(saturnCam);

	//Uranium
	TriangleComponent* uraniumMesh = new TriangleComponent(game);
	Mesh testSphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.2,
		32,
		16,
		false,
		{ Vector4(0.0f, 0.4f, 0.9f, 1.0f) }
	);
	uraniumMesh->Initialize(
		shaderPath,
		testSphere.points,
		testSphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(uraniumMesh);

	PlanetObject* uranium = new PlanetObject{
		uraniumMesh,
		0.0f,
		1.8f,
		0.0f,
		1.0f,
		Vector3(12.0f, 0.0f, -2.0f),
		Vector3(12.0f, 0.0f, -2.0f),
		sun
	};
	objectsArray.push_back(uranium);
	uranium->planetMesh->transforms.move = Matrix::CreateTranslation(uranium->centerPosition);

	OrbitalCamera* uraniumCam = new OrbitalCamera(game);
	uraniumCam->Initialize(orbitalCameraDefaultPosition, uranium->centerPosition - orbitalCameraDefaultPosition, uranium->centerPosition);
	OrbitalCameraObject* uraniumCamObject = new OrbitalCameraObject{ uraniumCam, uranium };
	camerasArray.push_back(testCamObject);
	game->components.push_back(uraniumCam);

	//Neptune
	TriangleComponent* neptuneMesh = new TriangleComponent(game);
	Mesh neptuneSphere = MeshCreator::GetInstance()->Sphere(
		Vector3(0, 0, 0),
		0.2,
		32,
		16,
		false,
		{ Vector4(0.0f, 0.2f, 0.9f, 1.0f) }
	);
	neptuneMesh->Initialize(
		shaderPath,
		testSphere.points,
		testSphere.indexes,
		strides,
		offsets
	);
	game->components.push_back(neptuneMesh);

	PlanetObject* neptune = new PlanetObject{
		neptuneMesh,
		0.0f,
		0.6f,
		0.0f,
		0.6f,
		Vector3(14.0f, 0.0f, -4.0f),
		Vector3(14.0f, 0.0f, -4.0f),
		sun
	};
	objectsArray.push_back(neptune);
	neptune->planetMesh->transforms.move = Matrix::CreateTranslation(neptune->centerPosition);

	OrbitalCamera* neptuneCam = new OrbitalCamera(game);
	neptuneCam->Initialize(orbitalCameraDefaultPosition, neptune->centerPosition - orbitalCameraDefaultPosition, neptune->centerPosition);
	OrbitalCameraObject* neptuneCamObject = new OrbitalCameraObject{ neptuneCam, neptune };
	camerasArray.push_back(testCamObject);
	game->components.push_back(neptuneCam);

	// XZ-grid
	//TriangleComponent* gridMesh = new TriangleComponent(game);
	//Mesh gridGeometry = MeshCreator::GetInstance()->GridXZCentered(
	//	2000.0f,
	//	2000.0f,
	//	200000,
	//	200000,
	//	false,
	//	{ DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 0.1f) }
	//);

	//gridMesh->Initialize(
	//	shaderPath,
	//	gridGeometry.points,
	//	gridGeometry.indexes,
	//	strides,
	//	offsets
	//);
	//game->components.push_back(gridMesh);

	//Main First Person Camera Init
	mainFirstPersonCamera = new FirstPersonCamera(game);
	mainFirstPersonCamera->Initialize();
	mainFirstPersonCamera->SetCameraPosition(Vector3(1.0f, 0.0f, 0.0f));
	game->activeCamera = mainFirstPersonCamera;
	game->components.push_back(mainFirstPersonCamera);
}

// --- Change Camera --- //
void SolarSystem::ChangeCamera(int cameraID) {
	if (cameraID == -1) Game::GetInstance()->activeCamera = mainFirstPersonCamera;
	else Game::GetInstance()->activeCamera = camerasArray[cameraID]->orbitalCamera;
}

// --- Update --- //
void SolarSystem::Update() {
	// ???
}

// --- Update every interval --- //
void SolarSystem::UpdateInterval(float deltaTime) {
	for (PlanetObject* object : objectsArray) {
		object->planetMesh->transforms.move = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
		object->planetMesh->Update();

		object->rotationAngle += object->rotationSpeed * deltaTime;
		object->planetMesh->transforms.rotate = Matrix::CreateRotationY(object->rotationAngle);
		object->planetMesh->Update();

		object->planetMesh->transforms.move = Matrix::CreateTranslation(object->centerPosition);
		object->planetMesh->Update();

		object->orbitalObjectAngle += object->orbitalObjectSpeed * deltaTime;
		Vector3 newObjectPosition = Vector3::Transform(object->orbitPosition, Matrix::CreateRotationY(object->orbitalObjectAngle));
		newObjectPosition += object->parentObject->centerPosition;
		object->centerPosition = newObjectPosition;
		object->planetMesh->transforms.move = Matrix::CreateTranslation(newObjectPosition);
	}

	for (OrbitalCameraObject* cam : camerasArray) {
		cam->orbitalCamera->SetWatchTarget(cam->orbitalCameraPlanet->centerPosition);
		//cam->camera->SetLookPoint(cam->cameraOwner->position + cam->camera->orbit);
		cam->orbitalCamera->SetCameraPosition(cam->orbitalCameraPlanet->centerPosition + Vector3::Transform(cam->orbitalCamera->orbitVector, Matrix::CreateRotationY(cam->orbitalCameraPlanet->orbitalObjectAngle)));
	}

	for (int i = 0; i <= camerasArray.size(); i++) {
		Keys key = static_cast<Keys>(static_cast<int>(Keys::D0) + i);
		if (game->inputDevice->IsKeyDown(key)) {
			ChangeCamera(i - 1);
		}
	}
}

// --- Destroy Resources --- //
void SolarSystem::DestroyResources() {
	// ???
}