#include "KatamariGame.h"
#include "Game.h"

KatamariGame* KatamariGame::katamariInstance = nullptr;

using namespace std;

void KatamariGame::Initialize()
{
	game = Game::GetInstance();

	std::vector<UINT> strides = { 32 };
	std::vector<UINT> offsets = { 0 };

	RandomObjectGeneration();

	ball = new KatamariPlayer(game);
	mainOrbit = ball->getCamera();
}

void KatamariGame::RandomObjectGeneration()
{
	random_device rd;
	mt19937 gen(rd());

	uniform_real_distribution<> distX(ldMapCorner.x, ruMapCorner.x);
	uniform_real_distribution<> distZ(ldMapCorner.z, ruMapCorner.z);
	uniform_real_distribution<> rotY(0, DirectX::XM_2PI);

	vector<LPCSTR> models;
	models.push_back("./Models/Rose/Red_rose_SF.obj");
	//models.push_back("./Models/SlothSword/sword.obj");

	uniform_int_distribution<> modelDist(0, models.size() - 1);

	std::vector<UINT> strides = { 24 };
	std::vector<UINT> offsets = { 0 };

	for (int i = 0; i < objectsCount; i++) {
		Vector3 position = Vector3(distX(gen), 0.0f, distZ(gen));
		float rotationY = rotY(gen);
		std::cout << rotationY << std::endl;

		std::vector<MeshWithTexture> meshes = MeshCreator::GetInstance()->MeshFromFile(models.at(modelDist(gen)));
		std::vector<TriangleWithTextureComponent*> modelParts;

		for (MeshWithTexture mesh : meshes) {
			TriangleWithTextureComponent* modelPart = new TriangleWithTextureComponent(game);
			modelPart->Initialize(L"./Shaders/TextureModifiedShader.hlsl", mesh.points, mesh.indexes, strides, offsets, mesh.texturePath);
			modelPart->transforms.rotate = Matrix::CreateFromYawPitchRoll(Vector3(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, rotationY));
			modelPart->transforms.move = Matrix::CreateTranslation(position);
			game->components.push_back(modelPart);
			modelParts.push_back(modelPart);
		}

		DirectX::BoundingOrientedBox collision;
		collision.Extents = Vector3(0.1f, 1.1f, 0.1f);
		;		collision.Orientation = Quaternion::CreateFromYawPitchRoll(Vector3(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, rotationY));
		collision.Center = Vector3(position.x, position.y, position.z);

		Pickable* object = new Pickable{
			modelParts,
			collision,
			position,
			Vector3(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, rotationY),
		};

		pickables.push_back(object);
	}
}

void KatamariGame::Update()
{
	ball->Update();
}

void KatamariGame::UpdateInterval(float deltaTime)
{
	ball->UpdateInterval(deltaTime);
}
