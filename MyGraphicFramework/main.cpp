#include "Game.h"
#include <windows.h>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

int main() {
  int screenWidth = 1920;
  int screenHeight = 1080;
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
  //game->InitPongGame(shaderPath);
  game->InitSolarSystem(shaderPath);
  // Let the game begin!
  game->Run();

  return 0;
}