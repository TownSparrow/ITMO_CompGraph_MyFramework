#include "KatamariPlayer.h"

#include "Game.h"
#include "KatamariGame.h"

void KatamariPlayer::MoveKatamari(float deltaTime)
{
	Vector3 forward = mainOrbit->cameraWatchTarget - mainOrbit->cameraPosition;
	forward.y = 0.0f;
	forward.Normalize();

	Vector3 right = mainOrbit->cameraUpAxis.Cross(forward);
	right.y = 0.0f;
	right.Normalize();

	Vector3 moveDirection = Vector3(0.0f, 0.0f, 0.0f);

	float rotationSpeed = speed / radius;
	float rotationAngle = rotationSpeed * deltaTime;

	if (game->inputDevice->IsKeyDown(Keys::W)) {
		moveDirection += forward;
		rotation.x -= rotationAngle;
	}
	if (game->inputDevice->IsKeyDown(Keys::S)) {
		moveDirection -= forward;
		rotation.x += rotationAngle;
	}
	if (game->inputDevice->IsKeyDown(Keys::A)) {
		moveDirection -= right;
		rotation.z += rotationAngle;
	}
	if (game->inputDevice->IsKeyDown(Keys::D)) {
		moveDirection += right;
		rotation.z -= rotationAngle;
	}

	if ((moveDirection.x != 0) || (moveDirection.y != 0) || (moveDirection.z != 0)) {
		moveDirection.Normalize();
		position += moveDirection * speed * deltaTime;
	};

	katamariMesh->Update();
	rotation.y = atan2(forward.x, forward.z);

}

KatamariPlayer::KatamariPlayer(Game* gameInput)
{
	game = gameInput;
	katamariGame = KatamariGame::getInstance();

	std::vector<UINT> strides = { 24 };
	std::vector<UINT> offsets = { 0 };

	/*katamariMesh = new TriangleComponent(game);
	Mesh ballSphere = MeshGenerator::getInstance()->getSphere(1.0f, 24, 24, Vector4(0.8f, 0.05f, 0.1f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	katamariMesh->Initialize(L"./Shaders/MyVeryFirstShader.hlsl", ballSphere.points, ballSphere.indeces, strides, offsets, false);
	game->components.push_back(katamariMesh);*/

	katamariMesh = new TriangleWithTextureComponent(game);
	std::vector<MeshWithTexture> ballSphere = MeshCreator::GetInstance()->MeshFromFile("./Models/TrashCan/Trash.obj");
	katamariMesh->Initialize(L"./Shaders/TextureModifiedShader.hlsl", ballSphere[0].points, ballSphere[0].indexes, strides, offsets, L"./Models/TrashCan/Trash.jpg");
	game->components.push_back(katamariMesh);

	DirectX::BoundingSphere collision = DirectX::BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 1.0f);

	Vector3 orbit = Vector3(2.0f, 2.0f, 2.0f);
	mainOrbit = new OrbitalCamera(game);
	mainOrbit->Initialize(orbit, position - orbit, position);
	game->activeCamera = mainOrbit;
	game->components.push_back(mainOrbit);
}

void KatamariPlayer::CollisionCheck()
{

	for (Pickable* object : katamariGame->pickables) {
		if (collision.Intersects(object->collision)) {
			Vector3 vectorToObject = collision.Center - object->collision.Center;
			//vectorToObject.Normalize();
			//vectorToObject *= radius;
			//object->position = vectorToObject;
			object->position = vectorToObject;
			collected.push_back(object);
		}
	}
}

void KatamariPlayer::UpdateInterval(float deltaTime)
{
	MoveKatamari(deltaTime);
	CollisionCheck();
}

void KatamariPlayer::Update()
{
	collision.Center = position;
	katamariMesh->transforms.move = Matrix::CreateTranslation(position);
	katamariMesh->transforms.rotate = Matrix::CreateFromYawPitchRoll(rotation);
	mainOrbit->SetWatchTarget(position);
	mainOrbit->SetCameraPosition(position + Vector3::Transform(mainOrbit->orbitVector, Matrix::CreateRotationY(0.0f)));

	for (Pickable* object : collected) {
		for (TriangleWithTextureComponent* part : object->mesh) {
			Vector3 newObjectPosition = Vector3::Transform(object->position, Matrix::CreateFromYawPitchRoll(rotation));
			part->transforms.move = Matrix::CreateTranslation(position + newObjectPosition);
		}
	}
}


OrbitalCamera* KatamariPlayer::getCamera()
{
	return mainOrbit;
}
