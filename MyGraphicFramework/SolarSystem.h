#pragma once

#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>

#include "TriangleComponent.h"
#include "MeshCreator.h"

// Cameras
#include "FirstPersonCamera.h"
#include "OrbitalCamera.h"

// Input Control
#include <unordered_set>
#include "Keys.h"

class Game;

// Planet Object
struct PlanetObject {
	// Mesh
	TriangleComponent* planetMesh;
	
	// Parameters for rotation
	float rotationAngle;
	float rotationSpeed;

	// Set additional parameters if orbital object
	float orbitalObjectAngle;
	float orbitalObjectSpeed;

	// Position of planet's center
	Vector3 centerPosition;

	// Position of orbit
	Vector3 orbitPosition;

	// Parent object (like Moon for the Earth)
	PlanetObject* parentObject;
};

// Orbital Camera Object
struct OrbitalCameraObject {
	// Object of camera
	OrbitalCamera* orbitalCamera;

	// Planet is the camera for
	PlanetObject* orbitalCameraPlanet;
};

class SolarSystem {
private:
	// Singleton and constructors
	static SolarSystem* solarSystemInstance;
	SolarSystem() {};

	// Game object
	Game* game;

	// Arrays of objects
	std::vector<PlanetObject*> objectsArray;

	// Arrays of cameras
	std::vector<OrbitalCameraObject*> camerasArray;
	
	// Main First Person Camera
	FirstPersonCamera* mainFirstPersonCamera;

	// Default Orbital Camera Position
	Vector3 orbitalCameraDefaultPosition = Vector3(0.0f, 0.0f, 2.0f);

public:
	// Instance of SolarSystem
	static SolarSystem* GetInstance() {
		if (solarSystemInstance == nullptr) {
			solarSystemInstance = new SolarSystem();
		}
		return solarSystemInstance;
	}

	// Main Methods
	void Initialize(LPCWSTR shaderPath);
	void ChangeCamera(int cameraID);
	void Update();
	void UpdateInterval(float deltaTime);
	void DestroyResources();
};