#include "Game.h"
#include <windows.h>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

int main() {
  int screenWidth = 1280;
  int screenHeight = 720;
  LPCWSTR shaderPath0 = L"./Shaders/MainShader.hlsl";
  LPCWSTR shaderPath1 = L"./Shaders/TaskModifiedShader.hlsl";
  LPCWSTR shaderPath2 = L"./Shaders/TextureModifiedShader.hlsl";

  Game* game = Game::GetInstance();

  // Game init
  game->Initialize(
    screenWidth,
    screenHeight,
    //L"./Shaders/MainShader.hlsl"
    shaderPath0
  );

  // Init of examples and tasks:
  //game->InitTwoSquaresExample(shaderPath);
  //game->InitPongGame(shaderPath);
  //game->InitSolarSystem(shaderPath1);
  game->InitKatamari(shaderPath2);
  
  // Let the game begin!
  game->Run();

  return 0;
}