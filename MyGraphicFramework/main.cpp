#include "Game.h"
#include <windows.h>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")


int main() {
  int screenWidth = 800;
  int screenHeight = 800;
  LPCWSTR shaderPath = L"./Shaders/MainShader.hlsl";

  Game* game = Game::GetInstance();

  // Game init
  game->Initialize(
    screenWidth,
    screenHeight,
    //L"./Shaders/MainShader.hlsl"
    shaderPath
  );

  // Init of examples and tasks:
  //game->InitTwoSquaresExample(shaderPath);
  game->InitPongGame(shaderPath);

  // Let the game begin!
  game->Run();

  return 0;
}