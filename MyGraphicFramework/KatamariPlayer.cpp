#include "KatamariPlayer.h"

#include "Game.h"
#include "KatamariGame.h"

// --- Inertia --- //
float KatamariPlayer::Inertia(
	float currentSpeed,
	bool isMoving
) {
	// Getting current speed as a calculation value begin
	float newSpeed = currentSpeed;

	// Rules for inertias
	if (isMoving == true) {
		if (newSpeed < maxSpeed) {
			newSpeed += growthDeltaSpeed;
		}
		else {
			newSpeed = maxSpeed;
		}
	}
	else {
		if (newSpeed > 0.0f) {
			newSpeed -= growthDeltaSpeed;
		}
		else {
			newSpeed = 0.0f;
		}
	}

	// Return results
	return newSpeed;
}

// --- Move --- //
void KatamariPlayer::Move(float deltaTime) {
	Vector3 forward = mainOrbitalCam->cameraWatchTarget - mainOrbitalCam->cameraPosition;
	forward.y = 0.0f;
	forward.Normalize();

	//Vector3 right = mainOrbit->cameraUpAxis.Cross(forward);
  //right.y = 0.0f;
	Vector3 right = forward.Cross(mainOrbitalCam->cameraUpAxis);
	right.Normalize();

	Vector3 moveDirection = Vector3(0.0f, 0.0f, 0.0f);

	//float rotationSpeed = maxSpeed / radius;
	//float rotationAngle = rotationSpeed * deltaTime;

	if (game->inputDevice->IsKeyDown(Keys::W)) {
		moveDirection += forward;
		//rotation.x -= rotationAngle;
		isMovingForward = true;
	}
	if (game->inputDevice->IsKeyDown(Keys::S)) {
		moveDirection -= forward;
		//rotation.x += rotationAngle;
		isMovingForward = false;
	}
	//if (game->inputDevice->IsKeyDown(Keys::A)) {
	//	moveDirection -= right;
	//	rotation.z += rotationAngle;
	//}
	//if (game->inputDevice->IsKeyDown(Keys::D)) {
	//	moveDirection += right;
	//	rotation.z -= rotationAngle;
	//}

	if ((moveDirection.x != 0) || (moveDirection.y != 0) || (moveDirection.z != 0)) {
		moveDirection.Normalize();
		velocity = moveDirection;
		currentSpeed = Inertia(currentSpeed, true);
		position += velocity * currentSpeed * deltaTime;
	}
	else {
		currentSpeed = Inertia(currentSpeed, false);
		position += velocity * currentSpeed * deltaTime;
	}

	//katamariMesh->Update();
	//rotation.y = atan2(forward.x, forward.z);

	float rotationAngle = currentSpeed * deltaTime / radius;
	Quaternion currentDelta;

	if (isMovingForward) { 
		currentDelta = Quaternion::CreateFromAxisAngle(right, rotationAngle);
	}
	else { 
		currentDelta = Quaternion::CreateFromAxisAngle(right, -rotationAngle);
	}

	localRotationMatrix = Matrix::CreateFromQuaternion(currentDelta);

	Quaternion newRotation = rotation * currentDelta;
	newRotation.Normalize();
	rotation = newRotation;
}

// --- Constructor --- //
KatamariPlayer::KatamariPlayer(Game* gameInput) {
	game = gameInput;
	katamariGame = KatamariGame::GetInstance();

	UINT stride = sizeof(Vertex);
	std::vector<UINT> strides = { stride };
	//std::vector<UINT> strides = { 24 };
	std::vector<UINT> offsets = { 0 };

	/*katamariMesh = new TriangleComponent(game);
	Mesh ballSphere = MeshGenerator::getInstance()->getSphere(1.0f, 24, 24, Vector4(0.8f, 0.05f, 0.1f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	katamariMesh->Initialize(L"./Shaders/MyVeryFirstShader.hlsl", ballSphere.points, ballSphere.indeces, strides, offsets, false);
	game->components.push_back(katamariMesh);*/

	Material* material = new Material{
		Vector4(1.0f, 1.0f, 1.0f, 1.00f),
		Vector4(0.28f, 0.28f, 0.28f, 1.00f),
		Vector4(0.77f, 0.77f, 0.77f, 5.9f)
	};

	katamariMesh = new TriangleWithTextureComponent(game);
	std::vector<MeshWithTexture> ballSphere = MeshCreator::GetInstance()->MeshFromFile("./Models/Katamari/katamari_ball.obj");
	katamariMesh->Initialize(L"./Shaders/TextureModifiedShader.hlsl", ballSphere[0].points, ballSphere[0].indexes, strides, offsets, L"./Models/Katamari/katamari_texture02.jpg", material, false);
	game->components.push_back(katamariMesh);

	position = Vector3(0.0f, -radius, 0.0f);

	//collision = DirectX::BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), radius);
	collision = DirectX::BoundingSphere(position, radius);

	Vector3 orbitalCamera = Vector3(4.0f, 4.0f, 4.0f);
	mainOrbitalCam = new OrbitalCamera(game);
	mainOrbitalCam->Initialize(orbitalCamera, position - orbitalCamera, position);
	game->activeCamera = mainOrbitalCam;
	game->components.push_back(mainOrbitalCam);

	lightOrbitAngle = 0.0f;
	lightOrbitSpeed = DirectX::XM_PI * 0.5f;

	SpawnLights();
}

// --- Spawn Lights --- //
void KatamariPlayer::SpawnLights() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distTheta(0, 2 * DirectX::XM_PI);
	std::uniform_real_distribution<> distPhi(0, DirectX::XM_PI);
	std::uniform_real_distribution<> distColor(1.0f, 10.0f);

	// Clear data before spawn
	orbitalPointLigthsDirections.clear();
	debugSpheres.clear();
	game->pointLights.clear();

	for (int i = 0; i < numLights; ++i) {
		// Calculate direction on the player's mesh
		float theta = static_cast<float>(distTheta(gen));
		float phi = static_cast<float>(distPhi(gen));
		Vector3 dir = Vector3(
			sin(phi) * cos(theta),
			sin(phi) * sin(theta),
			cos(phi)
		);
		dir.Normalize();
		orbitalPointLigthsDirections.push_back(dir);

		// Calculate local position
		Vector3 localPos = dir * (radius + offsetFromSurface);
		//Vector3 localPos = Vector3(radius * 0.5f, 0.0f, 0.0f);

		// Generate color
		float colorR = static_cast<float>(distColor(gen));
		float colorG = static_cast<float>(distColor(gen));
		float colorB = static_cast<float>(distColor(gen));
		float coefAmbient = 0.2;
		float coefDiffuse = 2.0;
		float coefSpecular = 2.0;
		Vector4 lightAmbient = Vector4(colorR * coefAmbient, colorG * coefAmbient, colorB * coefAmbient, 1.0f);
		Vector4 lightDiffuse = Vector4(colorR * coefDiffuse, colorG * coefDiffuse, colorB * coefDiffuse, 1.0f);
		Vector4 lightSpecular = Vector4(colorR * coefSpecular, colorG * coefSpecular, colorB * coefSpecular, 1.0f);

		// Create point light
		PointLight* point = new PointLight{
				lightAmbient,
				lightDiffuse,
				lightSpecular,
				Vector3(0, 0, 0),
				1.5f,
				Vector4(1.5f, 1.2f, 1.0f, 1.0f)
		};
		game->pointLights.push_back(point);

		// Create debug sphere
		Mesh debugSphereMesh = MeshCreator::GetInstance()->Sphere(
			Vector3(0, 0, 0),
			0.25f,
			8,
			8,
			false,
			{ Vector4(1.0f, 1.0f, 0.0f, 1.0f) }
		);
		TriangleComponent* debugSphere = new TriangleComponent(game);
		std::vector<UINT> debugStrides = { 32 };
		std::vector<UINT> debugOffsets = { 0 };
		debugSphere->Initialize(
			L"./Shaders/TaskModifiedShader.hlsl",
			debugSphereMesh.points,
			debugSphereMesh.indexes,
			debugStrides,
			debugOffsets
		);
		debugSphere->transforms.move = Matrix::CreateTranslation(
			position + Vector3::Transform(localPos, rotation)
		);
		debugSphere->transforms.rotate = Matrix::CreateFromQuaternion(rotation);
		debugSpheres.push_back(debugSphere);
		game->components.push_back(debugSphere);
	}
}

// --- Check collisions --- //
void KatamariPlayer::CheckCollision() {
	for (Pickable* object : katamariGame->pickables) {
		// Calculate collision size of pickable mesh for checking
		float objectEffectiveSize = std::max(
			object->collision.Extents.x,
			std::max(object->collision.Extents.y, object->collision.Extents.z)
		);

		float sizeFactor = 1.0f; // size-factor

		// Checking
		if (collision.Intersects(object->collision)
			&& radius >= objectEffectiveSize * sizeFactor
			&& !object->isCollected) {

			collectedPickableObjects.push_back(object);
			object->isCollected = true;

			// Rotation calculate
			Quaternion rotationInverted;
			rotation.Inverse(rotationInverted);
			Quaternion localRotation = object->rotation* rotationInverted;
			object->rotation = localRotation;

			// Position calculate
			object->position = Vector3::Transform(object->position - position, rotationInverted);

			std::cout << "I've become bigger boi..." << std::endl;

			UpdatePlayer(object);
		}
	}
}

// Previous realization of Update Player
//void KatamariPlayer::UpdatePlayer(Pickable* object) {
//	float size = object->mesh.size();
//
//	radius += size * 0.5;
//	collision.Radius = radius;
//	std::cout << collision.Radius << std::endl;
//
//	Vector3 orbitOffset = mainOrbit->cameraPosition - position;
//	float currentDistance = orbitOffset.Length();
//	orbitOffset.Normalize();
//	orbitOffset *= (currentDistance + size);
//
//	mainOrbit->orbitVector = orbitOffset;
//	mainOrbit->SetWatchTarget(position);
//	mainOrbit->SetCameraPosition(position + orbitOffset);
//}

// --- Update Player --- //
void KatamariPlayer::UpdatePlayer(Pickable* object) {
	// Calculate new radius with pickable size
	float colliderSize = std::max({ object->collision.Extents.x, object->collision.Extents.y, object->collision.Extents.z });
	
	float growthCoefficient = 0.05f; // growth factor
	
	radius += colliderSize * growthCoefficient;
	collision.Radius = radius;
	std::cout << "Collision Radius: " << collision.Radius << std::endl;

	// Calculate camera position for new offset
	Vector3 offset = mainOrbitalCam->cameraPosition - position;
	if (offset.LengthSquared() < 0.001f) {
		offset = Vector3(0.0f, 0.0f, -1.0f);
	}
	else {
		offset.Normalize();
	}

	// Setting default values
	float baseDistance = 10.0f;
	float cameraGrowthFactor = 0.5f;
	float desiredDistance = baseDistance + (radius * cameraGrowthFactor);

	// Calculate new target position
	Vector3 targetCameraPosition = position + offset * desiredDistance;

	// If need vertical offset
	//float verticalOffset = 10.0f;
	//targetCameraPosition.y += verticalOffset;

	// Changing camera position with lerp
	float lerpFactor = 0.1f;
	auto Lerp = [](const Vector3& start, const Vector3& end, float t) -> Vector3 {
		return start + (end - start) * t;
		};
	Vector3 newCameraPosition = Lerp(mainOrbitalCam->cameraPosition, targetCameraPosition, lerpFactor);
	mainOrbitalCam->SetCameraPosition(newCameraPosition);

	// Update camera
	mainOrbitalCam->orbitVector = newCameraPosition - position;
	mainOrbitalCam->SetWatchTarget(position);

	std::cout << "New Camera Position: " << mainOrbitalCam->cameraPosition.x << std::endl;
}

// --- Update with interval --- //
void KatamariPlayer::UpdateInterval(float deltaTime) {
	Move(deltaTime);
	CheckCollision();
	
	// If want to rotate every interval of time the sphere:
	RotateLightsWithInterval(deltaTime);
}

// --- Rotate spawned light with interval --- //
void KatamariPlayer::RotateLightsWithInterval(float deltaTime) {
	lightOrbitAngle += deltaTime * lightOrbitSpeed;

	for (int i = 0; i < game->pointLights.size(); i++) {
		Vector3 baseDir = orbitalPointLigthsDirections[i];
		Matrix orbitRotation = Matrix::CreateRotationY(lightOrbitAngle);
		Vector3 rotatedDir = Vector3::Transform(baseDir, orbitRotation);
		Vector3 localPos = rotatedDir * (radius + offsetFromSurface);
		Vector3 worldLightPos = position + Vector3::Transform(localPos, rotation);

		game->pointLights[i]->position = worldLightPos;
		debugSpheres[i]->transforms.move = Matrix::CreateTranslation(worldLightPos);
		debugSpheres[i]->transforms.rotate = Matrix::CreateFromQuaternion(rotation);

		std::cout << "UpdateInterval: Light #" << i + 1 << " position: ("
			<< worldLightPos.x << "; "
			<< worldLightPos.y << "; "
			<< worldLightPos.z << ")" << std::endl;
	}
}


// --- Update --- //
void KatamariPlayer::Update() {
	collision.Center = position;
	katamariMesh->transforms.move = Matrix::CreateTranslation(position);
	//katamariMesh->transforms.rotate = Matrix::CreateFromYawPitchRoll(rotation);
	katamariMesh->transforms.rotate = Matrix::CreateFromQuaternion(rotation);
	mainOrbitalCam->SetWatchTarget(position);
	mainOrbitalCam->SetCameraPosition(position + Vector3::Transform(mainOrbitalCam->orbitVector, Matrix::CreateRotationY(0.0f)));

	for (Pickable* object : collectedPickableObjects) {
		
		Vector3 newPosition = Vector3::Transform(object->position, rotation);

		for (TriangleWithTextureComponent* part : object->mesh) {
			//Vector3 newObjectPosition = Vector3::Transform(object->position, Matrix::CreateFromYawPitchRoll(rotation));
			//part->transforms.move = Matrix::CreateTranslation(position + newObjectPosition);
			part->transforms.move = Matrix::CreateTranslation(newPosition + position);
			part->transforms.rotate = Matrix::CreateFromQuaternion(object->rotation * rotation);
		}
	}
	
	//// Update lights (if don't need update every time of interval)
	//for (int i = 0; i < game->pointLights.size(); i++) {
	//	Vector3 localPos = orbitalPointLigthsDirections[i] * (radius + offsetFromSurface);
	//	Vector3 worldLightPos = position + Vector3::Transform(localPos, rotation);
	//	game->pointLights[i]->position = worldLightPos;
	//	debugSpheres[i]->transforms.move = Matrix::CreateTranslation(worldLightPos);
	//	debugSpheres[i]->transforms.rotate = Matrix::CreateFromQuaternion(rotation);

	//	// Debug output
	//	std::cout << "Light #" << i+1 << " position: ("
	//		<< worldLightPos.x << "; "
	//		<< worldLightPos.y << "; "
	//		<< worldLightPos.z << ")" << std::endl;
	//}
}

// --- Get Camera --- //
OrbitalCamera* KatamariPlayer::GetCamera() {
	return mainOrbitalCam;
}
